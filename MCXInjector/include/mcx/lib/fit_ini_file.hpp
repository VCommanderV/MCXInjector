#pragma once
#include "file.hpp"
#include "mcx_inject.hpp"
#include <cstddef>
#include <cstdint>

#ifndef ENABLE_LOG_FITINIFILE
#define ENABLE_LOG_FITINIFILE 0
#endif

#ifndef NO_ERR
#define NO_ERR	0
#endif

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

    FitIniFile();

    long readIdLong(char* key, long* out_value);

    int open(File* file, unsigned long param_2, long param_3);

    // protected: long __thiscall FitIniFile::findNextBlockStart(char *,unsigned long)
    long findNextBlockStart(char* line, unsigned long line_length);
};


# pragma region Funcs VAs
static constexpr uintptr_t va_FitIniFile_vftable = 0x0077c328;
static constexpr uintptr_t va_FitIniFile_ctor = 0x00649090;
// virtual long __thiscall FitIniFile::open(File*,unsigned long,long) */
static constexpr uintptr_t va_FitIniFile_open = 0x00649f10;

// virtual long __thiscall FitIniFile::open(char*,FileMode,long)
static constexpr uintptr_t va_FitIniFile_open2 = 0x00649ed0;

static constexpr uintptr_t va_FitIniFile_readIdLong = 0x0064a190;

static constexpr uintptr_t va_FitIniFile_afterOpen = 0x00649250;
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