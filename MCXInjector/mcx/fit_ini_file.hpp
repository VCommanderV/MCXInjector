#pragma once
#include "File.hpp"
#include "mcx_inject.hpp"
#include <cstddef>
#include <cstdint>

# pragma region Funcs VAs
static constexpr uintptr_t va_FitIniFile_vftable = 0x0077c328;
static constexpr uintptr_t va_FitIniFile_ctor = 0x00649090;
static constexpr uintptr_t va_FitIniFile_open = 0x00649f10;
static constexpr uintptr_t va_FitIniFile_readIdLong = 0x0064a190;
# pragma endregion

class FitIniFile : public File {
# pragma region Fields
public:
    uint32_t field_0x4c;
    uint32_t field_0x50;
    uint32_t field_0x54;
    uint32_t field_0x58;
    uint32_t field_0x5c;
#pragma endregion

public:
    static FitIniFile* fromPtr(void* p) noexcept {
        return reinterpret_cast<FitIniFile*>(p);
    }
};

static_assert(sizeof(FitIniFile) == 0x60, "FitIniFile size mismatch");
static_assert(alignof(FitIniFile) == 0x4, "FitIniFile align mismatch");

static_assert(offsetof(FitIniFile, vtable) == 0x0, "FitIniFile.vtable offset mismatch");
static_assert(offsetof(FitIniFile, field_0x4c) == 0x4c, "FitIniFile.field_0x4C offset mismatch");
static_assert(offsetof(FitIniFile, field_0x50) == 0x50, "FitIniFile.field_0x50 offset mismatch");
static_assert(offsetof(FitIniFile, field_0x54) == 0x54, "FitIniFile.field_0x54 offset mismatch");
static_assert(offsetof(FitIniFile, field_0x5c) == 0x5c, "FitIniFile.field_0x5c offset mismatch");