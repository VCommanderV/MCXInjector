#pragma once
#include "file.hpp"
#include "mcx_inject.hpp"
#include <cstddef>
#include <cstdint>

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

    long readIdLong(char* key, long* out_value);

    int open(char* file, int flags, int extra);
};


# pragma region Funcs VAs
static constexpr uintptr_t va_FitIniFile_vftable = 0x0077c328;
static constexpr uintptr_t va_FitIniFile_ctor = 0x00649090;
static constexpr uintptr_t va_FitIniFile_open = 0x00649f10;
static constexpr uintptr_t va_FitIniFile_readIdLong = 0x0064a190;
# pragma endregion

#pragma region Layout asserts
ASSERT_SIZE(FitIniFile, 0x60);
ASSERT_ALIGN(FitIniFile, 0x4);

ASSERT_FIELD_OFFSET(FitIniFile, vtable, 0x00);
ASSERT_FIELD_OFFSET(FitIniFile, field_0x4c, 0x4C);
ASSERT_FIELD_OFFSET(FitIniFile, field_0x50, 0x50);
ASSERT_FIELD_OFFSET(FitIniFile, field_0x54, 0x54);
ASSERT_FIELD_OFFSET(FitIniFile, field_0x5c, 0x5C);
# pragma endregion