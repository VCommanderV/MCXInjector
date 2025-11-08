#pragma once
#include <windows.h>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <utility>
#include <initializer_list>
#include <detours.h>

#include "absl/base/log_severity.h"
#include "absl/base/macros.h"
#include "absl/log/globals.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include <absl/strings/str_format.h>

#define LOG_DEBUG DLOG(INFO)
#define LOGF_DEBUG(fmt, ...) DLOG(INFO) << absl::StrFormat(fmt, __VA_ARGS__)

static constexpr uintptr_t MCX_IMAGE_BASE = 0x00600000;

// honours __thiscall ctors convetions to put `this` in EAX before returning.
#define MOV_THIS_TO_EAX(this_ptr) __asm { mov eax, this_ptr }

void LogDebug(const char* fmt, const char* tag = "", ...);
void SetModuleBaseOnce(HMODULE module_handle) noexcept;
bool PatchVTableSlotInPlace(void** vtable, size_t slot_index, void* new_func);
bool RestoreVTableSlotInPlace(void** vtable, size_t slot_index);
void* GetPatchedOriginal(void** vtable, size_t slo_index);
void* GetPatchedFuncOrigFromVA(void* target_virtual_addr);
uintptr_t GetModuleBaseOnce() noexcept;
uintptr_t GetRuntimeAddr(uintptr_t virtual_addr, uintptr_t img_base_addr = MCX_IMAGE_BASE) noexcept;
bool PatchFunction(void* target, void* hook);
bool RestoreFunction(void* target, void* hook);

// Gets the trampoline to the patched function from its VA.
template <typename Fn>
inline Fn GetTrampoline(uintptr_t virtual_addr) {
    static Fn fn = reinterpret_cast<Fn>(
        GetPatchedFuncOrigFromVA(reinterpret_cast<void*>(virtual_addr))
        );
    return fn;
}

#define MCX_RUNTIME_ADDR(va) (GetRuntimeAddr(static_cast<uintptr_t>(va)))


struct VTableKey {
    void** vtable;
    size_t slot_index;
    bool operator==(VTableKey const& o) const noexcept;
};

struct VTableKeyHash {
    size_t operator()(VTableKey const& k) const noexcept;
};

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

struct PatchEntry {
    void* target;
    void* hook;
};

class FuncPatchSet {
public:
    FuncPatchSet(const char* name,
        std::initializer_list<PatchEntry> items)
        : name(name)
    {
        for (auto& e : items)
            patches.emplace_back(e.target, e.hook);
    }

    // your existing ApplyAll(), RestoreAll(), etc.
public:
    bool ApplyAll() const;
    void RestoreAll() const;
private:
    std::string name;
    std::vector<std::pair<void*, void*>> patches;
};

struct PatchRegistration {
    std::string name;
    VTablePatchSet* vtable_patch = nullptr;
    FuncPatchSet* func_patch = nullptr;
};

class PatchRegistry {
public:
    static PatchRegistry* instance();

    void Add(PatchRegistration reg);
    void ApplyAll();
    void RestoreAll();
    size_t Size() const;
private:
    PatchRegistry() = default;
    std::vector<PatchRegistration> entries;
};

struct AutoFuncPatchRegister {
    AutoFuncPatchRegister(
        const char* name,
        std::initializer_list<PatchEntry> items
    ) {
        PatchRegistry::instance()->Add({
            name,
            nullptr,
            new FuncPatchSet(name, items)
        });
    }
};

// From detours: "C++ does not support converting a pointer to a member
// function to an arbitrary pointer.  To get a raw pointer, the address of
// the member function must be moved into a temporary member-function
// pointer, then passed by taking it's address, then de-referencing it.
// 
// This is some MSVC-specific evil to do what's explained above.
// Obtain the raw pointer-to-member-function that MSVC stores in a struct.
// struct {
//   void* func;     // actual function address
//   int   adjust;   // this-pointer adjustment
//   // other field?
//};
template <typename Class, typename Method>
void* GetMemberPtr(Method Class::* pmf) {
    void* out;
    memcpy(&out, &pmf, sizeof(out));
    return out;
}
