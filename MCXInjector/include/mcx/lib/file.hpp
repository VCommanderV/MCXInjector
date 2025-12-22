#pragma once
#include <cstddef>
#include <cstdint>
#include "mcx/lib/fast_file.hpp"
#include "mcx_inject.hpp"

#ifndef ENABLE_LOG_BASEFILE
#define ENABLE_LOG_BASEFILE 0
#endif

constexpr auto FILE_ERR_NO_SPACE_FOR_FILENAME = 0xBADF000D;
constexpr auto FILE_ERR_NO_SPACE_FOR_CHILD_LIST = 0xBADF0012;
constexpr auto FILE_ERR_NO_SPACE_LEFT_FOR_CHILD = 0xBADF000F;
constexpr auto FILE_ERR_BAD_PARENT = 0xBADF000E;
constexpr auto FILE_ERR_PARENT_READONLY = 0xBADF0011;
constexpr auto FILE_ERR_WRITE_FAIL = 0xBADF000A;

constexpr auto FILE_ERR_FILE_NOT_FOUND = 2;
constexpr long FILE_DEFAULT_MAX_CHILDREN = 32;

using FileSize = uint32_t;
using FileOffset = uint32_t;

class File;

enum FileMode
{
    NOMODE = 0,
    READ = 1,
    CREATE = 2,
    WRITE = 3,
    RDWRITE = 4
};

enum FileClass
{
    BASEFILEE = 0,
    INIFILE = 1,
    PACKETFILE = 2,
    CSVFILE = 3
};


struct File_vtable
{
    long(__thiscall* open)(File*, File*, unsigned long, long);
    long(__thiscall* open2)(File*, char*, FileMode, long);
    long(__thiscall* create)(File*, char*);
    void(__thiscall* close)(File*);
    FileClass(__thiscall* getFileClass)(File*);
};

// 76 bytes, 4 bytes alignment.
class File {
# pragma region Fields
public:
    // 0x00
    File_vtable* vtable;

    // 0x04
    char* file_name;

    // 0x08
    FileMode file_mode;

    // 0x0C
    HANDLE file_handle;

    // 0x10
    FastFile* fastfile;

    // 0x14
    long fastfile_handle;

    // 0x18
    FileSize length;

    // 0x1C
    FileOffset pos;

    // 0x20
    uint32_t buf_result;

    // 0x24
    File** children;

    // 0x28
    uint32_t num_children;

    // 0x2C
    uint32_t max_children;

    // 0x30
    File* parent;

    // 0x34
    FileOffset parent_offset;

    // 0x38
    FileSize phys_length;

    // 0x3C
    uint32_t field_0x3C;

    // 0x40
    uint32_t field_0x40;

    // 0x44
    BOOL in_ram;

    // 0x48
    void* fastfile_buf;
#pragma endregion
public:
    static BOOL logFileTraffic;
    static DWORD lastError;

    static BOOL fileExists(char* filepath)
    {
        DWORD file_attrs = GetFileAttributesA(filepath);
        return file_attrs != INVALID_FILE_ATTRIBUTES;
    }
public:
    static File* fromPtr(void* p) noexcept {
        return reinterpret_cast<File*>(p);
    }

    static void* operator new(size_t size);

    static void* operator new(size_t size, void* ptr);

    static void operator delete(void* ptr);

    File();

    // public: virtual long __thiscall File::create(char *)
    long create(char* filename);

    // public: virtual long __thiscall File::open(char *,enum FileMode,long)
    long open(char* filename, FileMode mode, long max_children);

    // public: virtual long __thiscall File::open(class File *,unsigned long,long)
    long open2(File* file, FileSize file_size, long num_children);

    // public: virtual void __thiscall File::close(void)
    void close();

    // public: int __thiscall File::isOpen(void)
    BOOL isOpen();

    // public: unsigned long __thiscall File::fileSize(void)
    FileSize fileSize();

    // public: long __thiscall File::addChild(class File *)
    long addChild(File*);

    // public: void __thiscall File::removeChild(class File *)
    void removeChild(File*);

    // public: int __thiscall File::eof(void
    BOOL eof();

    // public: virtual enum FileClass __thiscall File::getFileClass(void) */
    FileClass getFileClass();

    // public: char * __thiscall File::getFilename(void)
    char* getFilename();

    // public: unsigned long __thiscall File::getLength(void
    FileSize getLength();

    // public: long __thiscall File::seek(long,long)
    long seek(long offset, long seek_mode);

    // public: void __thiscall File::skip(long
    void skip(long bytes);

    // public: long __thiscall File::read(unsigned long,unsigned char *,long)
    // INLINED
    long read(FileOffset position, unsigned char* dst_buf, long bytes);

    // public: long __thiscall File::read(unsigned char *,long)
    long read2(unsigned char* dst_buf, long bytes);

    // public: unsigned char __thiscall File::readByte(void)
    BYTE readByte();

    // public: unsigned long __thiscall File::getNumLines(void)
    uint32_t getNumLines();

    // public: long __thiscall File::readLine(unsigned char *,long)
    long readLine(unsigned char* dst_buf, long buf_size);

    // public: short __thiscall File::readWord(void)
    short readWord(void);

    // public: short __thiscall File::readShort(void)
    short readShort(void);

    // public: long __thiscall File::readString(unsigned char *)
    long readString(unsigned char* dst_buf);

    // public: long __thiscall File::write(unsigned long,unsigned char *,long)
    long write(FileOffset position, unsigned char* src_buf, long bytes);

    // public: long __thiscall File::write(unsigned char *,long)
    long write2(unsigned char* src_buf, long bytes);

    // public: long __thiscall File::writeByte(unsigned char)
    long writeByte(BYTE b);

    // public: long __thiscall File::writeFloat(float)
    long writeFloat(float f);

    // public: long __thiscall File::writeLong(long)
    long writeLong(long l);

    // public: long __thiscall File::writeShort(short)
    long writeShort(short short_);

    // public: long __thiscall File::writeWord(short)
    long writeWord(short word_);

    // public: long __thiscall File::writeString(char *)
    long writeString(char* string);

    // public: long FUN_00645f10()
    long readLong();

protected:
    template<typename T>
    inline long writeT(T t);
};

#pragma region Methods VAs
static constexpr uintptr_t va_File_vftable = 0x0077c30c;
static constexpr uintptr_t va_File_ctor = 0x00645400;
static constexpr uintptr_t va_operator_new = 0x006453A0;
static constexpr uintptr_t va_operator_delete = 0x006453D0;

static constexpr uintptr_t va_File_getFileClass = 0x00645440;
static constexpr uintptr_t va_File_dtor = 0x00645450;
static constexpr uintptr_t va_File_eof = 0x00645460;
// virtual long __thiscall File::open(char *,enum FileMode,long)
static constexpr uintptr_t va_File_open = 0x00645480;
// virtual long __thiscall File::open(class File *,unsigned long,long)
static constexpr uintptr_t va_File_open2 = 0x00645780;
static constexpr uintptr_t va_File_create = 0x00645940;
static constexpr uintptr_t va_File_addChild = 0x00645960;
static constexpr uintptr_t va_File_removeChild = 0x006459A0;
static constexpr uintptr_t va_File_close = 0x006459E0;
static constexpr uintptr_t va_File_deleteFile = 0x00645AE0;
static constexpr uintptr_t va_File_seek = 0x00645B00;
static constexpr uintptr_t va_File_read = 0x00645CB0;
static constexpr uintptr_t va_File_readByte = 0x00645DA0;
static constexpr uintptr_t va_File_readWord = 0x00645E50;
static constexpr uintptr_t va_File_readShort = 0x00645F00;
static constexpr uintptr_t va_File_readString = 0x00645FC0;
static constexpr uintptr_t va_File_read2 = 0x00646010;
static constexpr uintptr_t va_File_readLine = 0x006460E0;
static constexpr uintptr_t va_File_readLineEx = 0x00646290;
static constexpr uintptr_t va_File_write = 0x00646410;
static constexpr uintptr_t va_File_write2 = 0x006467A0;
static constexpr uintptr_t va_File_writeByte = 0x006464A0;
static constexpr uintptr_t va_File_writeWord = 0x00646520;
static constexpr uintptr_t va_File_writeShort = 0x006465A0;
static constexpr uintptr_t va_File_writeLong = 0x006465B0;
static constexpr uintptr_t va_File_writeFloat = 0x00646630;
static constexpr uintptr_t va_File_writeString = 0x006466B0;
static constexpr uintptr_t va_File_writeLine = 0x00646720;
static constexpr uintptr_t va_File_isOpen = 0x00646830;
static constexpr uintptr_t va_File_getFilename = 0x00646850;
static constexpr uintptr_t va_File_getLength = 0x00646860;
static constexpr uintptr_t va_File_fileSize = 0x006468C0;
static constexpr uintptr_t va_File_getNumLines = 0x006468D0;
static constexpr uintptr_t va_File_skip = 0x00646940;

static constexpr uintptr_t va_File_logFileTraffic = 0x007d8fa0;
static constexpr uintptr_t va_File_fileTrafficLog = 0x007d8fa4;
static constexpr uintptr_t va_File_createTrafficLog = 0x006452f0;
static constexpr uintptr_t va_File_newPosition = 0x007d8fa8;
#pragma endregion

#pragma region Globals
OBJ_REF(long, g_newPosition, va_File_newPosition);
OBJ_REF(BOOL, g_logFileTraffic, va_File_logFileTraffic);
PTR_REF(File*, g_fileTrafficLog_ptr, va_File_fileTrafficLog);
inline auto createTrafficLog = FUN_PTR<void(__cdecl*)(void)>(va_File_createTrafficLog);

#define LOG_FILE_TRAFFIC(fmt, ...) \
if (g_logFileTraffic != FALSE) \
{ \
    if (*g_fileTrafficLog_ptr == nullptr) \
    { \
        createTrafficLog(); \
    } \
    char msg[300]; \
    sprintf(msg, fmt, __VA_ARGS__); \
    auto writeLine = (long(__thiscall*)(File*, char*))(MCX_RUNTIME_ADDR(va_File_writeLine)); \
    writeLine(*g_fileTrafficLog_ptr, msg); \
}

#pragma endregion

#pragma region Layout asserts
ASSERT_SIZE(File, 0x4C);
ASSERT_ALIGN(File, 0x4);
ASSERT_FIELD_OFFSET(File, vtable, 0x00);
ASSERT_FIELD_OFFSET(File, file_name, 0x04);
ASSERT_FIELD_OFFSET(File, file_mode, 0x08);
ASSERT_FIELD_OFFSET(File, file_handle, 0x0C);
ASSERT_FIELD_OFFSET(File, fastfile, 0x10);
ASSERT_FIELD_OFFSET(File, fastfile_handle, 0x14);
ASSERT_FIELD_OFFSET(File, length, 0x18);
ASSERT_FIELD_OFFSET(File, pos, 0x1C);
ASSERT_FIELD_OFFSET(File, buf_result, 0x20);
ASSERT_FIELD_OFFSET(File, children, 0x24);
ASSERT_FIELD_OFFSET(File, num_children, 0x28);
ASSERT_FIELD_OFFSET(File, max_children, 0x2C);
ASSERT_FIELD_OFFSET(File, parent, 0x30);
ASSERT_FIELD_OFFSET(File, parent_offset, 0x34);
ASSERT_FIELD_OFFSET(File, phys_length, 0x38);
ASSERT_FIELD_OFFSET(File, field_0x3C, 0x3C);
ASSERT_FIELD_OFFSET(File, field_0x40, 0x40);
ASSERT_FIELD_OFFSET(File, in_ram, 0x44);
ASSERT_FIELD_OFFSET(File, fastfile_buf, 0x48);
#pragma endregion