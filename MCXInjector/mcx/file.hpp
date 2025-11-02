#pragma once
#include <cstddef>
#include <cstdint>
#include "mcx_inject.hpp"

static constexpr uintptr_t va_File_vftable = 0x0077c30c;
static constexpr uintptr_t va_File_ctor = 0x00645400;
static constexpr uintptr_t va_operator_new = 0x006453A0;
static constexpr uintptr_t va_operator_delete = 0x006453D0;

static constexpr uintptr_t va_File_getFileClass = 0x00645440;
static constexpr uintptr_t va_File_dtor = 0x00645450;
static constexpr uintptr_t va_File_eof = 0x00645460;
static constexpr uintptr_t va_File_open = 0x00645480;
static constexpr uintptr_t va_File_open_alt = 0x00645780;
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
static constexpr uintptr_t va_File_read_alt = 0x00646010;
static constexpr uintptr_t va_File_readLine = 0x006460E0;
static constexpr uintptr_t va_File_readLineEx = 0x00646290;
static constexpr uintptr_t va_File_write = 0x00646410;
static constexpr uintptr_t va_File_writeByte = 0x006464A0;
static constexpr uintptr_t va_File_writeWord = 0x00646520;
static constexpr uintptr_t va_File_writeShort = 0x006465A0;
static constexpr uintptr_t va_File_writeLong = 0x006465B0;
static constexpr uintptr_t va_File_writeFloat = 0x00646630;
static constexpr uintptr_t va_File_writeString = 0x006466B0;
static constexpr uintptr_t va_File_writeLine = 0x00646720;
static constexpr uintptr_t va_File_write_alt = 0x006467A0;
static constexpr uintptr_t va_File_isOpen = 0x00646830;
static constexpr uintptr_t va_File_getFilename = 0x00646850;
static constexpr uintptr_t va_File_getLength = 0x00646860;
static constexpr uintptr_t va_File_fileSize = 0x006468C0;
static constexpr uintptr_t va_File_getNumLines = 0x006468D0;
static constexpr uintptr_t va_File_skip = 0x00646940;

// 76 bytes, 4 bytes alignment.
class File {
# pragma region Fields
public:
    // 0x00
    void* vtable;

    // 0x04
    char* filename_0x04;

    // 0x08
    int32_t open_flags_0x08;

    // 0x0C
    HANDLE filehandle_0xc;

    // 0x10
    void* p_fast_file_ptr_0x10;

    // 0x14
    int32_t fastfile_handle;

    // 0x18
    uint32_t file_size_0x18;

    // 0x1C
    uint32_t field_0x1c;

    // 0x20
    uint32_t field_0x20;

    // 0x24
    uint32_t* children_0x24;

    // 0x28
    int32_t children_count;

    // 0x2C
    int32_t field_0x2c;

    // 0x30
    uint32_t field_0x30;

    // 0x34
    uint32_t field_0x34;

    // 0x38
    uint32_t field_0x38;

    // 0x3C
    uint32_t field_0x3C;

    // 0x40
    uint32_t field_0x40;

    // 0x44
    uint32_t flags_0x44;

    // 0x48
    void* fastfile_buffer_0x48;
#pragma endregion

public:
    static File* fromPtr(void* p) noexcept {
        return reinterpret_cast<File*>(p);
    }
};

static_assert(sizeof(File) == 0x4C, "File size mismatch");
static_assert(alignof(File) == 4, "File alignment mismatch");

static_assert(offsetof(File, vtable) == 0x00, "File.vtable offset mismatch");
static_assert(offsetof(File, filename_0x04) == 0x04, "File.filename_0x04 offset mismatch");
static_assert(offsetof(File, open_flags_0x08) == 0x08, "File.open_flags_0x08 offset mismatch");
static_assert(offsetof(File, filehandle_0xc) == 0x0C, "File.filehandle_0xc offset mismatch");
static_assert(offsetof(File, p_fast_file_ptr_0x10) == 0x10, "File.p_fast_file_ptr_0x10 offset mismatch");
static_assert(offsetof(File, fastfile_handle) == 0x14, "File.fastfile_handle offset mismatch");
static_assert(offsetof(File, file_size_0x18) == 0x18, "File.file_size_0x18 offset mismatch");
static_assert(offsetof(File, field_0x1c) == 0x1C, "File.field_0x1c offset mismatch");
static_assert(offsetof(File, field_0x20) == 0x20, "File.field_0x20 offset mismatch");
static_assert(offsetof(File, children_0x24) == 0x24, "File.children_0x24 offset mismatch");
static_assert(offsetof(File, children_count) == 0x28, "File.children_count offset mismatch");
static_assert(offsetof(File, field_0x2c) == 0x2C, "File.field_0x2c offset mismatch");
static_assert(offsetof(File, field_0x30) == 0x30, "File.field_0x30 offset mismatch");
static_assert(offsetof(File, field_0x34) == 0x34, "File.field_0x34 offset mismatch");
static_assert(offsetof(File, field_0x38) == 0x38, "File.field_0x38 offset mismatch");
static_assert(offsetof(File, field_0x3C) == 0x3C, "File.field_0x3C offset mismatch");
static_assert(offsetof(File, field_0x40) == 0x40, "File.field_0x40 offset mismatch");
static_assert(offsetof(File, flags_0x44) == 0x44, "File.flags_0x44 offset mismatch");
static_assert(offsetof(File, fastfile_buffer_0x48) == 0x48, "File.fastfile_buffer_0x48 offset mismatch");
