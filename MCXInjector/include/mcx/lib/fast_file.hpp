#pragma once
#include <cstddef>
#include <cstdint>
#include "mcx_inject.hpp"

#ifndef ENABLE_LOG_FASTFILE
#define ENABLE_LOG_FASTFILE 0
#endif

using FileSize = uint32_t;

constexpr auto ERR_BADF_INVALID_ENTRY = 0xBADF0010;   // -0x4520fff0: Invalid entry index or no valid data;
constexpr auto ERR_BADF_SEEK_OUT_OF_BOUNDS = 0xBADF0008;    // -0x4520fff8: Seek position out of bounds;
constexpr auto ERR_BADF_INVALID_OFFSET = 0xBADF0009;  // -0x4520fff7: Invalid offset (-1);

constexpr auto MAX_FILENAME_SIZE = 250;

#pragma pack(push, 1)
struct FastFileEntryHeader
{
    uint32_t size;                    // 0x00
    uint32_t compressed_size;         // 0x04
    uint32_t uncompressed_size;       // 0x08
    char name[MAX_FILENAME_SIZE]; // 0x0C: 250 bytes for name
};
#pragma pack(pop)

struct FastFileEntry {
    uint32_t has_valid_data;     // 0x0: non-zero means this entry contains valid data
    uint32_t offset_in_file;     // 0x4: unknown purpose, always initialized to 0
    FastFileEntryHeader* data;
};

class FastFile {
# pragma region Fields
public:
    // 0x00
    int32_t num_entries;

    // 0x04
    FastFileEntry* entries;

    // 0x08
    char* filename;

    // 0xc
    HANDLE file_handle;

    // 0x10
    int32_t file_size;

    // 0x14
    int32_t file_cursor;
# pragma endregion
public:
    // __thiscall FastFile::FastFile(void)
    FastFile();

    // int __thiscall FastFile::open(char*)
    int open(char* filename);

    // long __thiscall FastFile::lzSizeFast(long)
    long lzSizeFast(long entry_index); // NOTE: Unused?

    // long __thiscall FastFile::openFast(char*)
    long openFast(char* filename);

    // long __thiscall FastFile::readFast(long,void *,long)
    long readFast(long entry_idx, void* buf, long offset);

    // void __thiscall FastFile::close()
    void close();

    // void __thiscall FastFile::closeFast(long) 
    void closeFast(long entry_idx);

    // long __thiscall FastFile::seekFast(long, long, long)
    long seekFast(long entry_idx, long offset, long seek_orig);

    // long __thiscall FastFile::closeFast(long) 
    long sizeFast(long entry_idx);

    // long __thiscall FastFile::tellFast(long) 
    long tellFast(long entry_idx);

    // __thiscall FastFile::~FastFile(void)
    ~FastFile();
    
    static FastFile* FastFileFind(char* filename);
};


#pragma region Methods VAs
static constexpr uintptr_t va_FastFile_operator_new = 0x00644d10;
static constexpr uintptr_t va_FastFile_operator_delete = 0x00644d30;

static constexpr uintptr_t va_FastFile_ctor = 0x00644d50;
static constexpr uintptr_t va_FastFile_dtor = 0x00644d70;

static constexpr uintptr_t va_FastFile_open = 0x00644d80;
static constexpr uintptr_t va_FastFile_close = 0x00644ed0;

static constexpr uintptr_t va_FastFile_openFast = 0x00644f40;
static constexpr uintptr_t va_FastFile_closeFast = 0x00644fb0;
static constexpr uintptr_t va_FastFile_seekFast = 0x00644fe0;
static constexpr uintptr_t va_FastFile_readFast = 0x006450f0;

static constexpr uintptr_t va_FastFile_tellFast = 0x00645260;
static constexpr uintptr_t va_FastFile_sizeFast = 0x00645290;
static constexpr uintptr_t va_FastFile_lzSizeFast = 0x006452c0;

static constexpr uintptr_t va_FastFile_fastFiles = 0x0080a854;
static constexpr uintptr_t va_FastFile_numFastFiles = 0x0080a858;
# pragma endregion

# pragma region Globals
static uint32_t* g_numFastFiles = reinterpret_cast<uint32_t*>(MCX_RUNTIME_ADDR(va_FastFile_numFastFiles));
static FastFile** g_fastFiles = reinterpret_cast<FastFile**>(MCX_RUNTIME_ADDR(va_FastFile_fastFiles));
# pragma endregion

#pragma region Layout asserts
ASSERT_SIZE(FastFileEntry, 0x0C);
ASSERT_ALIGN(FastFileEntry, 0x4);
ASSERT_FIELD_OFFSET(FastFileEntry, has_valid_data, 0x00);
ASSERT_FIELD_OFFSET(FastFileEntry, offset_in_file, 0x04);
ASSERT_FIELD_OFFSET(FastFileEntry, data, 0x8);

ASSERT_SIZE(FastFileEntryHeader, 0x106);
ASSERT_ALIGN(FastFileEntryHeader, 0x1);
ASSERT_FIELD_OFFSET(FastFileEntryHeader, size, 0x00);
ASSERT_FIELD_OFFSET(FastFileEntryHeader, compressed_size, 0x04);
ASSERT_FIELD_OFFSET(FastFileEntryHeader, uncompressed_size, 0x08);
ASSERT_FIELD_OFFSET(FastFileEntryHeader, name, 0x0c);

ASSERT_SIZE(FastFile, 0x18);
ASSERT_ALIGN(FastFile, 0x4);
ASSERT_FIELD_OFFSET(FastFile, num_entries, 0x00);
ASSERT_FIELD_OFFSET(FastFile, entries, 0x04);
ASSERT_FIELD_OFFSET(FastFile, filename, 0x08);
ASSERT_FIELD_OFFSET(FastFile, file_handle, 0x0C);
ASSERT_FIELD_OFFSET(FastFile, file_size, 0x10);
ASSERT_FIELD_OFFSET(FastFile, file_cursor, 0x14);
# pragma endregion