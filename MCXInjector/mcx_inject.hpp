#pragma once
#include <windows.h>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <utility>
#include <initializer_list>
#include <detours.h>

static constexpr uintptr_t MCX_IMAGE_BASE = 0x00600000;

// honours __thiscall ctors convetions to put `this` in EAX before returning.
#define MOV_THIS_TO_EAX(this_ptr) __asm { mov eax, this_ptr }

struct VTableKey {
    void** vtable;
    size_t slot_index;
    bool operator==(VTableKey const& o) const noexcept;
};

struct VTableKeyHash {
    size_t operator()(VTableKey const& k) const noexcept;
};

void LogDebug(const char* fmt, const char* tag = "", ...);
void SetModuleBaseOnce(HMODULE module_handle) noexcept;
bool PatchVTableSlotInPlace(void** vtable, size_t slot_index, void* new_func);
bool RestoreVTableSlotInPlace(void** vtable, size_t slot_index);
void* GetPatchedOriginal(void** vtable, size_t slo_index);
void* GetPatchedFuncOrigFromVA(void* target_virtual_addr);
uintptr_t GetModuleBaseOnce() noexcept;
uintptr_t GetRuntimeAddr(uintptr_t virtual_addr, uintptr_t img_base_addr = MCX_IMAGE_BASE) noexcept;

// Gets the trampoline to the patched function from its VA.
template <typename Fn>
inline Fn GetTrampoline(uintptr_t virtual_addr) {
    static Fn fn = reinterpret_cast<Fn>(
        GetPatchedFuncOrigFromVA(reinterpret_cast<void*>(virtual_addr))
        );
    return fn;
}

#define MCX_RUNTIME_ADDR(va) (GetRuntimeAddr(static_cast<uintptr_t>(va)))

struct VTablePatchSet {
    const char* name;
    uintptr_t vtable_va;
    void** vtable;
    std::unordered_map<size_t, void*> patches;

    VTablePatchSet(const char* n, uintptr_t va,
        std::initializer_list<std::pair<const size_t, void*>> init);

    bool ApplyAll() const;
    void RestoreAll() const;
};

VTablePatchSet MakeVTablePatchSet(const char* name, uintptr_t va,
    std::initializer_list<std::pair<const size_t, void*>> init);


struct FuncPatchKey {
    void* target;
    bool operator==(const FuncPatchKey& other) const noexcept;
    FuncPatchKey() : target(nullptr) {}
    explicit FuncPatchKey(void* t) : target(t) {}
};

namespace std {
    template <>
    struct hash<FuncPatchKey> {
        size_t operator()(const FuncPatchKey& k) const noexcept {
            return std::hash<void*>()(k.target);
        }
    };
}

bool PatchFunction(void* target, void* hook);
bool RestoreFunction(void* target, void* hook);

struct FuncPatchSet {
    const char* name;
    std::unordered_map<void*, void*> patches;

    FuncPatchSet(const char* n, const std::initializer_list<std::pair<void*, void*>>& init);

    bool ApplyAll() const;
    void RestoreAll() const;
};

struct PatchRegistration {
    std::string name;
    VTablePatchSet* vtable_patch = nullptr;
    FuncPatchSet* func_patch = nullptr;
};

class PatchRegistry {
public:
    // singleton
    static PatchRegistry& instance() {
        static PatchRegistry inst;
        return inst;
    }

    void Add(PatchRegistration reg) {
        entries.push_back(std::move(reg));
    }

    void ApplyAll() {
        for (auto& e : entries) {
            if (e.vtable_patch) {
                LogDebug("Hook", "Applying VTablePatch: %s", e.name.c_str());
                e.vtable_patch->ApplyAll();
            }
            if (e.func_patch) {
                LogDebug("Hook", "Applying FuncPatch: %s", e.name.c_str());
                e.func_patch->ApplyAll();
            }
        }
    }

    void RestoreAll() {
        for (auto& e : entries) {
            if (e.vtable_patch) {
                LogDebug("Hook", "Restoring VTablePatch: %s", e.name.c_str());
                e.vtable_patch->RestoreAll();
            }
            if (e.func_patch) {
                LogDebug("Hook", "Restoring FuncPatch: %s", e.name.c_str());
                e.func_patch->RestoreAll();
            }
        }
    }

    size_t Size() const { return entries.size(); }

private:
    PatchRegistry() = default;
    std::vector<PatchRegistration> entries;
};

#define CAT2(a,b) a##b
#define CAT(a,b)  CAT2(a,b)

struct AutoFuncPatchRegister {
    AutoFuncPatchRegister(const char* name, std::initializer_list<std::pair<void*, void*>> init);
};

struct AutoVTablePatchRegister {
    AutoVTablePatchRegister(const char* name,
        uintptr_t vtableVA,
        std::initializer_list<std::pair<const size_t, void*>> init);
};