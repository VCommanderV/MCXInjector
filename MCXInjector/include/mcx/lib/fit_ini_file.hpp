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

static constexpr auto INI_ERR_BUFFER_TOO_SMALL = 0xFADA0003;
static constexpr auto INI_ERR_NOT_A_FITINI_FILE = 0xFADA0004;
static constexpr auto INI_ERR_BLOCKS_MALLOC_FAILED = 0xFADA0005;
static constexpr auto INI_ERR_END_OF_BLOCKS = 0xFADA0006;
static constexpr auto INI_ERR_BLOCKS_OVERRUN = 0xFADA0007;
static constexpr auto INI_ERR_UNEXPECTED_BLOCKS_NUM = 0xFADA0008;
static constexpr auto INI_ERR_MALFORMED_BLOCK= 0xFADA000A;
static constexpr char FITINI_HEADER[7] = "FITini";
static constexpr char FITINI_FOOTER[7] = "FITend";
static constexpr long FITINI_BLOCK_ID_MAX_SIZE = 50;

#pragma pack(push, 4)
struct FitIniBlock
{
    char block_id[50];
    FileOffset block_offset;
}; 
#pragma pack(pop)

class FitIniFile : public File {
# pragma region Fields
public:
    uint32_t num_blocks;
    FitIniBlock* blocks;
    uint32_t field_0x54;
    uint32_t field_0x58;
    uint32_t field_0x5c;
#pragma endregion

public:
    static FitIniFile* fromPtr(void* p) noexcept {
        return reinterpret_cast<FitIniFile*>(p);
    }

    FitIniFile();

    // __thiscall FitIniFile::open(File*, int mode, int extra)
    int open(File* file, unsigned long param_2, long param_3);

    // protected: long __thiscall FitIniFile::afterOpen(void)
    long afterOpen();

    // public: virtual void __thiscall FitIniFile::close(void)
    void close();

    // protected: void __thiscall FitIniFile::atClose(void
    void atClose();

    // public: virtual long __thiscall getFileClass(void)
    FileClass getFileClass();

    // protected: long __thiscall FitIniFile::textToLong(char *)
    long textToLong(char* line);

    // protected: long __thiscall FitIniFile::findNextBlockStart(char *,unsigned long)
    long findNextBlockStart(char* line, unsigned long line_length);

    // protected: long __thiscall FitIniFile::countBlocks(void)
    long countBlocks();

    // protected: long __thiscall FitIniFile::booleanToLong(char *)
    long booleanToLong(char* line);

    // protected: long __thiscall FitIniFile::byteToTextDec(char *,unsigned char,unsigned long)
    long byteToTextDec(char* text_buf, unsigned char value, unsigned long buf_length);

    // protected: long __thiscall FitIniFile::byteToTextHex(char *,unsigned char,unsigned long)
    long byteToTextHex(char* text_buf, unsigned char value, unsigned long buf_length);

    // protected: long __thiscall FitIniFile::floatToText(char *,float,unsigned long)
    long floatToText(char* text_buf, float value, unsigned long buf_length);

    // protected: long __thiscall FitIniFile::longToTextDec(char *,long,unsigned long)
    long longToTextDec(char* text_buf, long value, unsigned long buf_length);

    // protected: long __thiscall FitIniFile::longToTextHex(char *,long,unsigned long)
    long longToTextHex(char* text_buf, long value, unsigned long buf_length);

    // protected: long __thiscall FitIniFile::shortToTextDec(char *,short,unsigned long)
    long shortToTextDec(char* text_buf, short value, unsigned long buf_length);

    // protected: long __thiscall FitIniFile::longToTextHex(char *,short,unsigned long)
    long shortToTextHex(char* text_buf, short value, unsigned long buf_length);

    // protected: long __thiscall FitIniFile::copyString(char *,char *,unsigned long)
    long copyString(char* src, char* dst, unsigned long length);

    long readIdLong(char* key, long* out_value);

protected:
    template<typename T>
    inline long NumberToText(const char* fmt, char* buf, T value, unsigned long buf_length)
    {
        char int_str[252];

        sprintf(int_str, fmt, value);

        size_t int_str_len = strlen(int_str);

        if (buf_length <= buf_length)
        {
            return INI_ERR_BUFFER_TOO_SMALL;
        }

        strncpy(buf, int_str, int_str_len);

        buf[int_str_len] = '\0';

        return NO_ERR;
    }
};


# pragma region Funcs VAs
static constexpr uintptr_t va_FitIniFile_vftable = 0x0077c328;
static constexpr uintptr_t va_FitIniFile_ctor = 0x00649090;
static constexpr uintptr_t va_FitIniFile_dtor = 0x006490d0;
// virtual long __thiscall FitIniFile::open(File*,unsigned long,long) */
static constexpr uintptr_t va_FitIniFile_open = 0x00649f10;
// virtual long __thiscall FitIniFile::open(char*,FileMode,long)
static constexpr uintptr_t va_FitIniFile_open2 = 0x00649ed0;
static constexpr uintptr_t va_FitIniFile_writeIdUShortArray = 0x0064c350;
static constexpr uintptr_t va_FitIniFile_writeIdUShort = 0x0064c240;
static constexpr uintptr_t va_FitIniFile_writeIdULong = 0x0064c1f0;
static constexpr uintptr_t va_FitIniFile_writeIdUCharArray = 0x0064c640;
static constexpr uintptr_t va_FitIniFile_writeIdUChar = 0x0064c2a0;
static constexpr uintptr_t va_FitIniFile_writeIdString = 0x0064c300;
static constexpr uintptr_t va_FitIniFile_writeIdShort = 0x0064c130;
static constexpr uintptr_t va_FitIniFile_writeIdLongArray = 0x0064c450;
static constexpr uintptr_t va_FitIniFile_writeIdLong = 0x0064c0e0;
static constexpr uintptr_t va_FitIniFile_writeIdFloatArray = 0x0064c540;
static constexpr uintptr_t va_FitIniFile_writeIdFloat = 0x0064c010;
static constexpr uintptr_t va_FitIniFile_writeIdChar = 0x0064c190;
static constexpr uintptr_t va_FitIniFile_writeIdBoolean = 0x0064c070;
static constexpr uintptr_t va_FitIniFile_writeBlock = 0x0064bfc0;
static constexpr uintptr_t va_FitIniFile_textToUShort = 0x006498f0;
static constexpr uintptr_t va_FitIniFile_textToULong = 0x006497d0;
static constexpr uintptr_t va_FitIniFile_textToUChar = 0x00649a00;
static constexpr uintptr_t va_FitIniFile_textToShort = 0x006495d0;
static constexpr uintptr_t va_FitIniFile_textToLong = 0x006494b0;
static constexpr uintptr_t va_FitIniFile_textToFloat = 0x00649490;
static constexpr uintptr_t va_FitIniFile_textToChar = 0x006496e0;
static constexpr uintptr_t va_FitIniFile_shortToTextHex = 0x00649d80;
static constexpr uintptr_t va_FitIniFile_shortToTextDec = 0x00649d10;
static constexpr uintptr_t va_FitIniFile_readIdUShortArray = 0x0064b3c0;
static constexpr uintptr_t va_FitIniFile_readIdUShort = 0x0064a770;
static constexpr uintptr_t va_FitIniFile_readIdULongArray = 0x0064b010;
static constexpr uintptr_t va_FitIniFile_readIdULong = 0x0064a640;
static constexpr uintptr_t va_FitIniFile_readIdUChar = 0x0064a8a0;
static constexpr uintptr_t va_FitIniFile_readIdString = 0x0064aa30;
static constexpr uintptr_t va_FitIniFile_readIdShortArray = 0x0064b1e0;
static constexpr uintptr_t va_FitIniFile_readIdShort = 0x0064a3e0;
static constexpr uintptr_t va_FitIniFile_readIdLongArray = 0x0064ae40;
static constexpr uintptr_t va_FitIniFile_readIdLong = 0x0064a190;
static constexpr uintptr_t va_FitIniFile_readIdFloatArray = 0x0064ac70;
static constexpr uintptr_t va_FitIniFile_readIdFloat = 0x0064a050;
static constexpr uintptr_t va_FitIniFile_readIdCharArray = 0x0064b590;
static constexpr uintptr_t va_FitIniFile_readIdChar = 0x0064a510;
static constexpr uintptr_t va_FitIniFile_readIdBoolean = 0x0064a2c0;
static constexpr uintptr_t va_FitIniFile_mathToUShort = 0x00649b90;
static constexpr uintptr_t va_FitIniFile_mathToULong = 0x00649b70;
static constexpr uintptr_t va_FitIniFile_mathToUChar = 0x00649bb0;
static constexpr uintptr_t va_FitIniFile_mathToShort = 0x00649b80;
static constexpr uintptr_t va_FitIniFile_mathToLong = 0x00649b60;
static constexpr uintptr_t va_FitIniFile_mathToFloat = 0x00649b50;
static constexpr uintptr_t va_FitIniFile_mathToChar = 0x00649ba0;
static constexpr uintptr_t va_FitIniFile_longToTextHex = 0x00649ca0;
static constexpr uintptr_t va_FitIniFile_longToTextDec = 0x00649c30;
static constexpr uintptr_t va_FitIniFile_getNextWord = 0x006491a0;
static constexpr uintptr_t va_FitIniFile_getIdUShortArrayElements = 0x0064bcf0;
static constexpr uintptr_t va_FitIniFile_getIdULongArrayElements = 0x0064bb10;
static constexpr uintptr_t va_FitIniFile_getIdUCharArrayElements = 0x0064bed0;
static constexpr uintptr_t va_FitIniFile_getIdStringLength = 0x0064ab50;
static constexpr uintptr_t va_FitIniFile_getIdShortArrayElements = 0x0064bc00;
static constexpr uintptr_t va_FitIniFile_getIdLongArrayElements = 0x0064ba20;
static constexpr uintptr_t va_FitIniFile_getIdFloatArrayElements = 0x0064b930;
static constexpr uintptr_t va_FitIniFile_getIdCharArrayElements = 0x0064bde0;
static constexpr uintptr_t va_FitIniFile_getFileClass = 0x006490c0;
static constexpr uintptr_t va_FitIniFile_floatToText = 0x00649bc0;
static constexpr uintptr_t va_FitIniFile_findNextBlockStart = 0x006490f0;
static constexpr uintptr_t va_FitIniFile_create = 0x00649f40;
static constexpr uintptr_t va_FitIniFile_countBlocks = 0x00649160;
static constexpr uintptr_t va_FitIniFile_copyString = 0x0064a9d0;
static constexpr uintptr_t va_FitIniFile_close = 0x00649f50;
static constexpr uintptr_t va_FitIniFile_byteToTextHex = 0x00649e60;
static constexpr uintptr_t va_FitIniFile_byteToTextDec = 0x00649df0;
static constexpr uintptr_t va_FitIniFile_booleanToLong = 0x00649af0;
static constexpr uintptr_t va_FitIniFile_atClose = 0x00649400;
static constexpr uintptr_t va_FitIniFile_afterOpen = 0x00649250;
# pragma endregion

#pragma region Layout asserts
ASSERT_SIZE(FitIniFile, 0x60);
ASSERT_ALIGN(FitIniFile, 0x4);

ASSERT_FIELD_OFFSET(FitIniFile, vtable, 0x00);
ASSERT_FIELD_OFFSET(FitIniFile, num_blocks, 0x4C);
ASSERT_FIELD_OFFSET(FitIniFile, blocks, 0x50);
ASSERT_FIELD_OFFSET(FitIniFile, field_0x54, 0x54);
ASSERT_FIELD_OFFSET(FitIniFile, field_0x5c, 0x5C);
# pragma endregion