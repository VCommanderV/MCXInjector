#pragma once
#include <cstddef>
#include <cstdint>
#include "mcx_inject.hpp"

constexpr auto ERR_BADF_INVALID_ENTRY = 0xBADF0010;   // -0x4520fff0: Invalid entry index or no valid data;
constexpr auto ERR_BADF_SEEK_OUT_OF_BOUNDS = 0xBADF0008;    // -0x4520fff8: Seek position out of bounds;
constexpr auto ERR_BADF_INVALID_OFFSET = 0xBADF0009;  // -0x4520fff7: Invalid offset (-1);

#pragma pack(push, 1)
struct FastFileEntryHeader
{
    uint32_t size;                 // +0x00, not compressed size
    uint32_t compressed_size;      // +0x04
    uint32_t uncompressed_size;    // +0x08
    char name[0xFE - 4];           // +0x0C, 250 bytes for name
};
#pragma pack(pop)

struct FastFileEntry {
    uint32_t has_valid_data;     // 0x0: non-zero means this entry contains valid data
    uint32_t offset_in_file;     // 0x4: unknown purpose, always initialized to 0
    FastFileEntryHeader* data;   // points to 0x106 bytes:
};

enum FastFileSeekOrigin : long {
    SeekSet = 0,
    SeekCur = 1,
    SeekEnd = 2
};

/*
[4 bytes] number_of_entries
repeat number_of_entries times:
    [0x106 bytes] raw entry data
*/
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
    long seekFast(long entry_idx, long offset, FastFileSeekOrigin seek_orig);

    // long __thiscall FastFile::closeFast(long) 
    long sizeFast(long entry_idx);

    // long __thiscall FastFile::tellFast(long) 
    long tellFast(long entry_idx);

    // __thiscall FastFile::~FastFile(void)
    ~FastFile();
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
# pragma endregion

#pragma region Layout asserts
static_assert(sizeof(FastFileEntry) == 0xc, "FastFileEntry size mismatch");
static_assert(sizeof(FastFileEntryHeader) == 0x106, "LzHeader size mismatch");

static_assert(sizeof(FastFile) == 0x18, "FastFile size mismatch");
static_assert(alignof(FastFile) == 4, "FastFile alignment mismatch");

static_assert(offsetof(FastFile, num_entries) == 0x00, "FastFile.num_entries offset mismatch");
static_assert(offsetof(FastFile, entries) == 0x04, "FastFile.entries offset mismatch");
static_assert(offsetof(FastFile, filename) == 0x08, "FastFile.filename offset mismatch");
static_assert(offsetof(FastFile, file_handle) == 0x0C, "FastFile.file_handle offset mismatch");
static_assert(offsetof(FastFile, file_size) == 0x10, "FastFile.file_size offset mismatch");
static_assert(offsetof(FastFile, file_cursor) == 0x14, "FastFile.file_cursor offset mismatch");
# pragma endregion