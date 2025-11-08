#include "mcx_inject.hpp"
#include <stdio.h>
#include <mutex>
#include <windows.h>
#include <detours.h>
#include <chrono>
#include <ctime>
#include <cstdarg>
#include <cstdio>
#include <absl/strings/str_format.h>

using namespace std::chrono;

uintptr_t GetModuleBaseOnce() noexcept {
    static uintptr_t mb = []()->uintptr_t {
        HMODULE h = GetModuleHandleA(NULL);
        return h ? reinterpret_cast<uintptr_t>(h) : 0;
        }();
        return mb;
}

uintptr_t GetRuntimeAddr(uintptr_t virtualAddr, uintptr_t imageBaseAddr) noexcept {
    uintptr_t mb = GetModuleBaseOnce();
    if (!mb) return 0;
    uintptr_t rva = virtualAddr - MCX_IMAGE_BASE;
    return mb + rva;
}

void LogDebug(const char* tag, const char* fmt, ...) {
# if DEBUG
    char buf[256];
    va_list ap;
    va_start(ap, fmt);
    _vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, ap);
    va_end(ap);
    OutputDebugStringA(buf);
# else
    auto now = system_clock::now();
    auto now_ms = time_point_cast<milliseconds>(now);
    int ms = (int)(now_ms.time_since_epoch().count() % 1000);

    std::time_t t = system_clock::to_time_t(now);
    struct tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    printf("%02d:%02d:%02d.%03d [%s] - ",
        tm.tm_hour,
        tm.tm_min,
        tm.tm_sec,
        ms, 
        tag
    );
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
# endif
}

void SetModuleBaseOnce(HMODULE hMod) noexcept {
    static uintptr_t& mb = *new uintptr_t{ reinterpret_cast<uintptr_t>(hMod) };
    (void)mb;
}

static inline void** RuntimeVTableFromVA(uintptr_t vTableVa)
{
    HMODULE h = GetModuleHandleA(NULL);
    if (!h) return nullptr;
    uintptr_t moduleBase = (uintptr_t)h;
    return reinterpret_cast<void**>(MCX_RUNTIME_ADDR(vTableVa));
}

static std::unordered_map<VTableKey, void*, VTableKeyHash> g_originals;

bool VTableKey::operator==(VTableKey const& o) const noexcept {
    return vtable == o.vtable && slot_index == o.slot_index;
}

size_t VTableKeyHash::operator()(VTableKey const& k) const noexcept {
    return std::hash<void*>()((void*)k.vtable) ^ (k.slot_index * 0x9e3779b9);
}

inline bool PatchVTableSlotInPlace(void** vtable, size_t slot_index, void* new_func)
{
    if (!vtable || !new_func) return false;

    VTableKey key{ vtable, slot_index };
    if (g_originals.find(key) != g_originals.end()) {
        // already patched
        return false;
    }

    // compute address of the slot
    void** slot_addr = vtable + slot_index;

    // Make the page writable (store old protection to restore later)
    DWORD old_protect = 0;
    if (!VirtualProtect(slot_addr, sizeof(void*), PAGE_EXECUTE_READWRITE, &old_protect)) {
        return false;
    }

    // Atomically swap the function pointer (InterlockedExchangePointer returns the previous value)
    void* orig = InterlockedExchangePointer((PVOID*)slot_addr, new_func);

    // Flush instruction cache so other threads see new pointer/code
    FlushInstructionCache(GetCurrentProcess(), slot_addr, sizeof(void*));

    // Store original in map
    g_originals.emplace(key, orig);

    // Restore page protection (optional; keep as RX to be safe)
    DWORD tmp;
    VirtualProtect(slot_addr, sizeof(void*), old_protect, &tmp);

    return true;
}

// Restore a previously patched slot
inline bool RestoreVTableSlotInPlace(void** vtable, size_t slot_index)
{
    if (!vtable) return false;

    VTableKey key{ vtable, slot_index };
    auto it = g_originals.find(key);
    if (it == g_originals.end()) {
        return false; // not found
    }

    void* orig = it->second;
    void** slotAddr = vtable + slot_index;

    DWORD oldProtect = 0;
    if (!VirtualProtect(slotAddr, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        return false;
    }

    // Atomically restore original pointer
    InterlockedExchangePointer((PVOID*)slotAddr, orig);
    FlushInstructionCache(GetCurrentProcess(), slotAddr, sizeof(void*));

    DWORD tmp;
    VirtualProtect(slotAddr, sizeof(void*), oldProtect, &tmp);

    // remove from map
    g_originals.erase(it);

    return true;
}

// Get stored original pointer (or nullptr if not patched)
inline void* GetPatchedOriginal(void** vtable, size_t slot_index)
{
    VTableKey key{ vtable, slot_index };
    auto it = g_originals.find(key);
    void* orig = (it == g_originals.end()) ? nullptr : it->second;
    return orig;
}

VTablePatchSet::VTablePatchSet(const char* n, uintptr_t va,
    std::initializer_list<std::pair<const size_t, void*>> init)
    : name(n), vtable_va(va), vtable(RuntimeVTableFromVA(va)), patches(init) {}

bool VTablePatchSet::ApplyAll() const {
    if (!vtable) {
        LOG_DEBUG << name << " Invalid vtable (nullptr)";
        return false;
    }
    bool all_ok = true;

    for (auto it = patches.begin(); it != patches.end(); ++it) {
        size_t slot = it->first;
        void* fn = it->second;

        if (fn == nullptr) {
            continue;
        }

        if (!PatchVTableSlotInPlace(vtable, slot, fn)) {
            LOG_DEBUG << absl::StrFormat("%s vtbl % p[% zu]: failed or already patched", name, vtable, slot);
            all_ok = false;
        }
        else {
            LOG_DEBUG << absl::StrFormat("%s vtbl %p[%zu] -> %p", name, vtable, slot, fn);
        }
    }

    return all_ok;
}

void VTablePatchSet::RestoreAll() const {
    if (!vtable) return;
    
    for (auto it = patches.begin(); it != patches.end(); ++it) {
        size_t slot = it->first;
        if (RestoreVTableSlotInPlace(vtable, slot)) {
            LOG_DEBUG << absl::StrFormat("[%s] Restored %p[%zu]", name, vtable, slot);
        }
    }
}

VTablePatchSet MakeVTablePatchSet(const char* name, uintptr_t va,
    std::initializer_list<std::pair<const size_t, void*>> init) {
    return VTablePatchSet(name, va, init);
}

static std::unordered_map<FuncPatchKey, void*> g_funcOriginals;

bool FuncPatchKey::operator==(const FuncPatchKey& other) const noexcept {
    return target == other.target;
}

bool PatchFunction(void* target, void* hook) {
    if (!target || !hook) return false;
        
    LOG_DEBUG << absl::StrFormat("Start PatchFunc target = % p hook = % p", target, hook);
        
    auto runtime_addr = (void*)MCX_RUNTIME_ADDR(reinterpret_cast<uintptr_t>(target));
        
    FuncPatchKey key{ target };
        
    LOG_DEBUG << "Start PatchFunc key=" << key.target;
        
    if (g_funcOriginals.find(key) != g_funcOriginals.end()) {
        // already patched
        LOG_DEBUG << absl::StrFormat("Hook", "PatchFunc target=%p hook=%p: already patched.", runtime_addr, hook);
        return false;
    }
        
    void* orig = runtime_addr;
        
    // Read first few bytes of the target function
    BYTE prologue[8] = { 0 };
    SIZE_T read = 0;
    if (ReadProcessMemory(GetCurrentProcess(), runtime_addr, prologue, sizeof(prologue), &read) && read >= 4) {
        LOG_DEBUG << absl::StrFormat("Prologue at %p: %02X %02X %02X %02X",
            runtime_addr, prologue[0], prologue[1], prologue[2], prologue[3]);
    }
    else {
        LOG_DEBUG << absl::StrFormat("Could not read prologue bytes at %p", runtime_addr);
    }
        
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    LONG rc = DetourAttach(&(PVOID&)orig, hook);
    if (rc != NO_ERROR) {
        DetourTransactionAbort();
        LOG_DEBUG << absl::StrFormat("DetourAttach failed rc=%ld for %p", rc, runtime_addr);
        return false;
    }
        
    rc = DetourTransactionCommit();
    if (rc != NO_ERROR) {
        LOG_DEBUG << absl::StrFormat("DetourTransactionCommit failed rc=%ld for %p", rc, runtime_addr);
        return false;
    }

    g_funcOriginals.insert(std::make_pair(key, orig));
        
    LOG_DEBUG << absl::StrFormat("Patched func %p -> %p (orig=%p)", runtime_addr, hook, orig);
    return true;
}

bool RestoreFunction(void* target, void* hook) {
    if (!target)
        return false;

    auto runtime_addr = (void*)MCX_RUNTIME_ADDR(reinterpret_cast<uintptr_t>(target));

    FuncPatchKey key{ runtime_addr };
    auto it = g_funcOriginals.find(key);
    if (it == g_funcOriginals.end()) {
        return false;
    }

    void* orig = it->second;

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    LONG rc = DetourDetach(&(PVOID&)orig, hook);
    if (rc != NO_ERROR) {
        DetourTransactionAbort();
        LOG_DEBUG << absl::StrFormat("DetourDetach failed rc=%ld for %p", rc, target);
        return false;
    }

    rc = DetourTransactionCommit();
    if (rc != NO_ERROR) {
        LOG_DEBUG << absl::StrFormat("Detour commit failed rc=%ld for %p", rc, target);
        return false;
    }

    g_funcOriginals.erase(it);
    LOG_DEBUG << absl::StrFormat("Restored func %p", target);

    return true;
}

void* GetPatchedFuncOrigFromVA(void* target_virtual_addr) {
    FuncPatchKey key{ target_virtual_addr };
    auto it = g_funcOriginals.find(key);
    return (it == g_funcOriginals.end()) ? nullptr : it->second;
}

bool FuncPatchSet::ApplyAll() const {
    LOG(INFO) << "Applying " << patches.size() << " patches.";
    for (auto& kv : patches) {
        LOG(INFO) << "Applying " << kv.first << " -> " << kv.second;
        PatchFunction(kv.first, kv.second);
    }
    return true;
}

void FuncPatchSet::RestoreAll() const {
    for (auto& kv : patches) {
        RestoreFunction(kv.first, kv.second);
    }
}

PatchRegistry* PatchRegistry::instance() {
    static PatchRegistry inst;
    LOG(INFO) << "Created patch registry " << &inst;
    return &inst;
}

void PatchRegistry::Add(PatchRegistration reg) {
    entries.emplace_back(std::move(reg));
}

void PatchRegistry::ApplyAll() {
    for (auto& e : entries) {
        if (e.vtable_patch) {
            LOG_DEBUG << "Applying VTablePatch: " << e.name;
            e.vtable_patch->ApplyAll();
        }
        if (e.func_patch) {
            LOG_DEBUG << "Applying FuncPatch: " << e.name;
            e.func_patch->ApplyAll();
        }
    }
}

void PatchRegistry::RestoreAll() {
    for (auto& e : entries) {
        if (e.vtable_patch) {
            LOG_DEBUG << "Restoring VTablePatch: " << e.name;
            e.vtable_patch->RestoreAll();
        }
        if (e.func_patch) {
            LOG_DEBUG << "Restoring FuncPatch: " << e.name;
            e.func_patch->RestoreAll();
        }
    }
}

size_t PatchRegistry::Size() const { return entries.size(); }

//AutoFuncPatchRegister::AutoFuncPatchRegister(const char* name,
//    std::initializer_list<std::pair<void*, void*>> init)
//{
//    static std::vector<FuncPatchSet> s_storage;
//    s_storage.emplace_back(name, init);
//    FuncPatchSet* p = &s_storage.back();
//
//    PatchRegistry::instance().Add({ std::string(name), nullptr, p });
//}
//
//AutoVTablePatchRegister::AutoVTablePatchRegister(
//    const char* name,
//    uintptr_t vtableVA,
//    std::initializer_list<std::pair<const size_t, void*>> init)
//{
//    // Keep persistent storage to avoid dangling)
//    static std::vector<VTablePatchSet> s_vtStorage;
//    s_vtStorage.emplace_back(name, vtableVA, init);
//    VTablePatchSet* p = &s_vtStorage.back();
//    PatchRegistry::instance().Add({ std::string(name), p, nullptr });
//}
