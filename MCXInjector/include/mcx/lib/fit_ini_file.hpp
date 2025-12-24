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
static constexpr auto INI_ERR_VAR_NOT_FOUND = 0xFADA0009;
static constexpr auto INI_ERR_MALFORMED_BLOCK = 0xFADA000A;
static constexpr auto INI_ERR_ARRAY_NOT_ENOUGH_ITEMS = 0xFADA000B;
static constexpr auto INI_ERR_END_OF_LINE = 0xFADA000C;
static constexpr auto INI_ERR_ARRAY_TOO_SMALL = 0xFADA000D;
static constexpr auto INI_ERR_ARRAY_TO_MANY_ITEMS = 0xFADA000E;

static constexpr char FITINI_HEADER[7] = "FITini";
static constexpr char FITINI_FOOTER[7] = "FITend";
static constexpr long FITINI_BLOCK_ID_MAX_SIZE = 50;
static constexpr long FITINI_ARRAY_MAX_ITEMS = 9;

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
    uint32_t num_blocks;        // 0x4x
    FitIniBlock* blocks;        // 0x50
    char* curr_block_id;        // 0x54
    uint32_t curr_block_offset; // 0x58
    uint32_t curr_block_size;   // 0x5c
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

    // protected: unsigned long __thiscall FitIniFile::textToULong(char *)
    unsigned long textToULong(char* line);

    // protected: char __thiscall FitIniFile::textToChar(char*)
    char textToChar(char* line);

    // protected: unsigned char __thiscall FitIniFile::textToUChar(char*)
    unsigned char textToUChar(char* line);

    // protected: short __thiscall FitIniFile::textToShort(char *)
    short textToShort(char* line);

    // protected: unsigned short __thiscall FitIniFile::textToUShort(char *)
    short textToUShort(char* line);

    // protected: float __thiscall FitIniFile::textToFloat(char *)
    float textToFloat(char* line);

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

    // protected: long __thiscall FitIniFile::getNextWord(char * &,char *,unsigned long)
    long getNextWord(char** line, char* buf, unsigned long buf_length);

    // protected: char __thiscall FitIniFile::mathToChar(char *)
    char mathToChar(char*);

    // protected: float __thiscall FitIniFile::mathToFloat(char *)
    float mathToFloat(char*);

    // protected: long __thiscall FitIniFile::mathToLong(char *)
    long mathToLong(char*);

    // protected: short __thiscall FitIniFile::mathToShort(char *)
    short mathToShort(char*);

    // protected: uchar __thiscall FitIniFile::mathToUChar(char *)
    unsigned char mathToUChar(char*);

    // protected: ulong __thiscall FitIniFile::mathToULong(char *)
    long mathToULong(char*);

    // protected: ushort __thiscall FitIniFile::mathToUShort(char *)
    unsigned short mathToUShort(char*);

    // public: long __thiscall FitIniFile::readIdBoolean(char *,int &)
    long readIdBoolean(char* line, int* result);

    // public: long __thiscall FitIniFile::readIdLong(char *,long &)
    long readIdLong(char* key, long* result);

    // public: long __thiscall FitIniFile::readIdChar(char *,char &)
    long readIdChar(char* key, char* result);

    // public: long __thiscall FitIniFile::readIdShort(char *,short &)
    long readIdShort(char* key, short* result);

    // public: long __thiscall FitIniFile::readIdFloat(char *,float &)
    long readIdFloat(char* key, float* result);

    // public: long __thiscall FitIniFile::readIdString(char *,char *,unsigned long)
    long readIdString(char* key, char* buf, unsigned long buf_length);

    // public: long __thiscall FitIniFile::readIdUChar(char *,unsigned char &)
    long readIdUChar(char* key, unsigned char* result);

    // public: long __thiscall FitIniFile::readIdLong(char *,unsigned long &)
    long readIdULong(char* key, unsigned long* result);

    // public: long __thiscall FitIniFile::readIdUShort(char *,unsigned short &)
    long readIdUShort(char* key, unsigned short* result);

    // public: long __thiscall FitIniFile::readIdLongArray(char *,long *,unsigned long) */
    long readIdLongArray(char* key, long* buf, unsigned long buf_length);

    // public: long __thiscall FitIniFile::writeBlock(char *)
    long writeBlock(char* key);

    // public: long __thiscall FitIniFile::writeIdLong(char *,long)
    long writeIdLong(char* key, long value);

    long writeIdShort(char* key, short value);

    long writeIdUShort(char* key, unsigned short value);

    long writeIdULong(char* key, unsigned long value);

    long writeIdFloat(char* key, float value);

    long writeIdString(char* key, char* value);

    long writeIdBoolean(char* key, BOOL value);

    long writeIdLongArray(char* key, long* values, unsigned long length);

    long writeIdUShortArray(char* key, unsigned short* values, unsigned long length);

    long writeIdUCharArray(char* key, unsigned char* values, unsigned long length);
private:
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

    long __forceinline find_key_value(const char* prefix, char* id, unsigned char line_buf[256], char** value_start_out)
    {
        char key[256];

        this->seek(this->curr_block_offset, SEEK_SET);
        uint32_t block_end = this->curr_block_offset + this->curr_block_size;

        // "b keyName"
        sprintf((char*)key, "%s %s", prefix, id);
        size_t key_len = strlen((char*)key);


        // scan the file to find the line that starts with the key
        while (this->pos < block_end)
        {
            this->readLine(line_buf, 254);

            // found key
            if (strnicmp(key, (char*)line_buf, key_len) == 0)
            {
                char* value_start = (char*)line_buf + key_len;

                // Strip leading whitespace
                while (isspace(*value_start))
                {
                    value_start += 1;
                }

                if (*value_start == '=')
                {
                    break;
                }
            }
        }

        if (this->pos >= block_end)
        {
            *value_start_out = nullptr;
            return INI_ERR_VAR_NOT_FOUND;
        }

        char* equals_ptr = strstr((char*)line_buf, "=");

        if (strstr((char*)line_buf, "=") == nullptr)
        {
            *value_start_out = nullptr;
            return INI_ERR_MALFORMED_BLOCK;
        }

        *value_start_out = equals_ptr + 1; // skip =
        // HOOK_DLOGF(FITINIFILE, INFO, "Found key line: %s", (char*)line_buf);

        return NO_ERR;
    }

    long __forceinline find_array_key_value(
        const char* prefix, 
        char* id, 
        unsigned char line_buf[256], 
        char** value_start_out,
        unsigned long buf_length,
        uint32_t* block_end_out,
        unsigned long *items_count_out
    )
    {
        char key[256];
        char prefix_buf[12];

        this->seek(this->curr_block_offset, SEEK_SET);
        uint32_t block_end = this->curr_block_offset + this->curr_block_size;
        *block_end_out = block_end;

        // "b keyName"
        sprintf((char*)key, "] %s", id);
        size_t key_len = strlen((char*)key);

        sprintf((char*)prefix_buf, "%s[", prefix);
        size_t prefix_len = strlen((char*)prefix_buf);

        char* prefix_ptr = nullptr;
        char* key_ptr = nullptr;

        // scan the file to find the line that starts with the key
        while (this->pos < block_end)
        {
            this->readLine(line_buf, 254);

            // found key
            prefix_ptr = (char*)strstr((char*)line_buf, prefix_buf);
            key_ptr = (char*)strstr((char*)line_buf, key);

            if (prefix_ptr != nullptr && key_ptr != nullptr)
            {
                break;
            }
        }

        if (this->pos >= block_end)
        {
            *value_start_out = nullptr;
            return INI_ERR_VAR_NOT_FOUND;
        }

        int items_count_digits = (int)key_ptr - (int)(prefix_ptr + 2);

        if (items_count_digits >= FITINI_ARRAY_MAX_ITEMS)
        {
            return INI_ERR_ARRAY_TO_MANY_ITEMS;
        }

        char buf[12];
        strncpy(buf, prefix_ptr + 2, items_count_digits);
        buf[items_count_digits] = '\0';

        unsigned long len = this->textToULong(buf);
        *items_count_out = len;
        if (buf_length < len)
        {
            return INI_ERR_ARRAY_TOO_SMALL;
        }

        char* equals_ptr = strstr((char*)line_buf, "=");

        if (equals_ptr == nullptr)
        {
            *value_start_out = nullptr;
            return INI_ERR_MALFORMED_BLOCK;
        }

        equals_ptr += 1;

        while (isspace(*equals_ptr))
        {
            equals_ptr += 1;
        }

        *value_start_out = equals_ptr; // skip =

        return NO_ERR;
    }

    template<typename T>
    inline long WriteInteger(const char* fmt, char* key, T value)
    {
        unsigned char key_buf[256];

        sprintf((char*)key_buf, "%s %s=%d\r\n", fmt, key, value);
        size_t key_len = strlen((char*)key_buf);

        HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::writeN('%s', '%s')", this->file_name, (char*)key_buf);

        return this->write2(key_buf, key_len);
    }

    template<typename T>
    inline long WriteIntegerArray(const char* fmt, char* key, T* values, unsigned long length)
    {
        unsigned char key_buf[256];
        long total_written = 0;

        sprintf((char*)key_buf, "%s[%d] %s=%d,", fmt, length, key, *values);
        size_t key_len = strlen((char*)key_buf);
        total_written += this->write2(key_buf, key_len);

        HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::writeIdNArray('%s','%s') => %s", this->file_name, key, (char*)key_buf);

        if (length > 1)
        {
            length -= 1;
            values += 1;

            while (length != 0)
            {
                sprintf((char*)key_buf, "%ld,", *values);
                key_len = strlen((char*)key_buf);
                total_written += this->write2(key_buf, key_len);

                HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::writeIdNArray('%s','%s') => buf[%d]=%d", this->file_name, key_buf, length, *values);

                length -= 1;
                values += 1;
            }
        }

        sprintf((char*)key_buf, "\r\n");
        key_len = strlen((char*)key_buf);
        total_written += this->write2(key_buf, key_len);

        return total_written;
    }
};


# pragma region Funcs VAs
VADDR(va_FitIniFile_vftable, 0x0077c328);
VADDR(va_FitIniFile_ctor, 0x00649090);
VADDR(va_FitIniFile_dtor, 0x006490d0);
// virtual long __thiscall FitIniFile::open(File*,unsigned long,long) */
VADDR(va_FitIniFile_open, 0x00649f10);
// virtual long __thiscall FitIniFile::open(char*,FileMode,long)
VADDR(va_FitIniFile_open2, 0x00649ed0);
VADDR(va_FitIniFile_writeIdUShortArray, 0x0064c350);
VADDR(va_FitIniFile_writeIdUShort, 0x0064c240);
VADDR(va_FitIniFile_writeIdULong, 0x0064c1f0);
VADDR(va_FitIniFile_writeIdUCharArray, 0x0064c640);
VADDR(va_FitIniFile_writeIdUChar, 0x0064c2a0);
VADDR(va_FitIniFile_writeIdString, 0x0064c300);
VADDR(va_FitIniFile_writeIdShort, 0x0064c130);
VADDR(va_FitIniFile_writeIdLongArray, 0x0064c450);
VADDR(va_FitIniFile_writeIdLong, 0x0064c0e0);
VADDR(va_FitIniFile_writeIdFloatArray, 0x0064c540);
VADDR(va_FitIniFile_writeIdFloat, 0x0064c010);
VADDR(va_FitIniFile_writeIdChar, 0x0064c190);
VADDR(va_FitIniFile_writeIdBoolean, 0x0064c070);
VADDR(va_FitIniFile_writeBlock, 0x0064bfc0);
VADDR(va_FitIniFile_textToUShort, 0x006498f0);
VADDR(va_FitIniFile_textToULong, 0x006497d0);
VADDR(va_FitIniFile_textToUChar, 0x00649a00);
VADDR(va_FitIniFile_textToShort, 0x006495d0);
VADDR(va_FitIniFile_textToLong, 0x006494b0);
VADDR(va_FitIniFile_textToFloat, 0x00649490);
VADDR(va_FitIniFile_textToChar, 0x006496e0);
VADDR(va_FitIniFile_shortToTextHex, 0x00649d80);
VADDR(va_FitIniFile_shortToTextDec, 0x00649d10);
VADDR(va_FitIniFile_readIdUShortArray, 0x0064b3c0);
VADDR(va_FitIniFile_readIdUShort, 0x0064a770);
VADDR(va_FitIniFile_readIdULongArray, 0x0064b010);
VADDR(va_FitIniFile_readIdULong, 0x0064a640);
VADDR(va_FitIniFile_readIdUChar, 0x0064a8a0);
VADDR(va_FitIniFile_readIdString, 0x0064aa30);
VADDR(va_FitIniFile_readIdShortArray, 0x0064b1e0);
VADDR(va_FitIniFile_readIdShort, 0x0064a3e0);
VADDR(va_FitIniFile_readIdLongArray, 0x0064ae40);
VADDR(va_FitIniFile_readIdLong, 0x0064a190);
VADDR(va_FitIniFile_readIdFloatArray, 0x0064ac70);
VADDR(va_FitIniFile_readIdFloat, 0x0064a050);
VADDR(va_FitIniFile_readIdCharArray, 0x0064b590);
VADDR(va_FitIniFile_readIdChar, 0x0064a510);
VADDR(va_FitIniFile_readIdBoolean, 0x0064a2c0);
VADDR(va_FitIniFile_mathToUShort, 0x00649b90);
VADDR(va_FitIniFile_mathToULong, 0x00649b70);
VADDR(va_FitIniFile_mathToUChar, 0x00649bb0);
VADDR(va_FitIniFile_mathToShort, 0x00649b80);
VADDR(va_FitIniFile_mathToLong, 0x00649b60);
VADDR(va_FitIniFile_mathToFloat, 0x00649b50);
VADDR(va_FitIniFile_mathToChar, 0x00649ba0);
VADDR(va_FitIniFile_longToTextHex, 0x00649ca0);
VADDR(va_FitIniFile_longToTextDec, 0x00649c30);
VADDR(va_FitIniFile_getNextWord, 0x006491a0);
VADDR(va_FitIniFile_getIdUShortArrayElements, 0x0064bcf0);
VADDR(va_FitIniFile_getIdULongArrayElements, 0x0064bb10);
VADDR(va_FitIniFile_getIdUCharArrayElements, 0x0064bed0);
VADDR(va_FitIniFile_getIdStringLength, 0x0064ab50);
VADDR(va_FitIniFile_getIdShortArrayElements, 0x0064bc00);
VADDR(va_FitIniFile_getIdLongArrayElements, 0x0064ba20);
VADDR(va_FitIniFile_getIdFloatArrayElements, 0x0064b930);
VADDR(va_FitIniFile_getIdCharArrayElements, 0x0064bde0);
VADDR(va_FitIniFile_getFileClass, 0x006490c0);
VADDR(va_FitIniFile_floatToText, 0x00649bc0);
VADDR(va_FitIniFile_findNextBlockStart, 0x006490f0);
VADDR(va_FitIniFile_create, 0x00649f40);
VADDR(va_FitIniFile_countBlocks, 0x00649160);
VADDR(va_FitIniFile_copyString, 0x0064a9d0);
VADDR(va_FitIniFile_close, 0x00649f50);
VADDR(va_FitIniFile_byteToTextHex, 0x00649e60);
VADDR(va_FitIniFile_byteToTextDec, 0x00649df0);
VADDR(va_FitIniFile_booleanToLong, 0x00649af0);
VADDR(va_FitIniFile_atClose, 0x00649400);
VADDR(va_FitIniFile_afterOpen, 0x00649250);
# pragma endregion

#pragma region Layout asserts
ASSERT_SIZE(FitIniFile, 0x60);
ASSERT_ALIGN(FitIniFile, 0x4);

ASSERT_FIELD_OFFSET(FitIniFile, vtable, 0x00);
ASSERT_FIELD_OFFSET(FitIniFile, num_blocks, 0x4C);
ASSERT_FIELD_OFFSET(FitIniFile, blocks, 0x50);
ASSERT_FIELD_OFFSET(FitIniFile, curr_block_id, 0x54);
ASSERT_FIELD_OFFSET(FitIniFile, curr_block_offset, 0x58);
ASSERT_FIELD_OFFSET(FitIniFile, curr_block_size, 0x5C);
# pragma endregion