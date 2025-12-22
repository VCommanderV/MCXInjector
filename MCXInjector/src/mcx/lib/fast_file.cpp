#include "mcx_inject.hpp"
#include "mcx/lib/fast_file.hpp"
#include "mcx/lib/lz_decomp.hpp"

MCXI_CTOR_HOOK Hook_FastFile_ctor(FastFile* this_ptr, void* unused_edx)
{
    this_ptr->entries = (FastFileEntry*)0x0;
    this_ptr->num_entries = 0;
    this_ptr->filename = (char*)0x0;
    this_ptr->file_handle = INVALID_HANDLE_VALUE;
    this_ptr->file_size = 0;
    this_ptr->file_cursor = 0;
    MOV_THIS_TO_EAX(this_ptr);
}

#pragma warning(disable:26495, justification : "Initialization happens in Hook_FastFile_ctor" )
FastFile::FastFile()
{
    Hook_FastFile_ctor(this, nullptr);
}
#pragma warning(default:26495)

int FastFile::open(char* filename)
{
    HOOK_DLOG(FASTFILE, INFO) << "Opening file " << filename;

    FileSize len = strlen(filename) + 1;

    this->filename = new char[len];

    strncpy(this->filename, filename, len);

    HANDLE handle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    this->file_handle = handle;

    // update file size
    if (handle != INVALID_HANDLE_VALUE) 
    {
        this->file_cursor = 0;

        if (this->file_size == 0)
        {
            this->file_size = GetFileSize(handle, NULL);
        }
    }

    HOOK_DLOGF(FASTFILE, INFO, "FastFile(%s) handle=%p size=%d", filename, handle, this->file_size);

    // seek the file to the beginning
    DWORD new_file_ptr_low = SetFilePointer(handle, 0, NULL, FILE_BEGIN);
    this->file_cursor = new_file_ptr_low;

    // Read 4 bytes header that contains file size
    DWORD read_bytes = 0;
    BOOL read_result = ReadFile(handle, &this->num_entries, 4, &read_bytes, (LPOVERLAPPED)nullptr);

    HOOK_DLOGF(FASTFILE, INFO, "FastFile(%s) handle=%p size=%d entries=%d",
        filename,
        handle,
        this->file_size,
        this->num_entries
    );

    this->file_cursor += 4;

    if (read_result != FALSE) {
        int num_entries = this->num_entries;
        auto buf = (FastFileEntry*)malloc(num_entries * sizeof(FastFileEntry));
        this->entries = buf;
        
        for (int i = 0; i < num_entries; i++)
        {
            auto entry = (FastFileEntry*)((char*)this->entries + i * sizeof(FastFileEntry));

            if (entry == nullptr) {
                return -1;
            }

            // Allocate and zero a 0x106-byte block
            entry->data = (FastFileEntryHeader*)malloc(sizeof(FastFileEntryHeader));

            if (entry->data == nullptr) {
                return -1;
            }

            memset(entry->data, 0, sizeof(FastFileEntryHeader));

            // Read the record from file
            read_result = ReadFile(handle, entry->data, sizeof(FastFileEntryHeader), &read_bytes, (LPOVERLAPPED)nullptr);

            if (read_result == FALSE) {
                return GetLastError();
            }

            // Initialize metadata
            entry->has_valid_data = 0;
            entry->offset_in_file = 0;
        }
        return 0;
    }

    return GetLastError();
}

long FastFile::lzSizeFast(long entry_idx)
{
    if (entry_idx >= 0 &&
        entry_idx < this->num_entries &&
        this->entries[entry_idx].has_valid_data != 0)
    {
        long size = this->entries[entry_idx].data->uncompressed_size;

        HOOK_DLOGF(FASTFILE, INFO, "lzSizeFast(%s) => %d", this->filename, size);

        return size;
    }

    return -1;
}

long FastFile::openFast(char* filename)
{
    auto num_entries = this->num_entries;

    if (num_entries <= 0) {
        return -1;
    }

    FastFileEntry* entries = this->entries;

    for (auto i = 0; i < num_entries; i++) {
        FastFileEntry& entry = entries[i];

        if (_stricmp(entry.data->name, filename) == 0)
        {
            entry.has_valid_data = 1;
            entry.offset_in_file = 0;

            HOOK_DLOGF(FASTFILE, INFO, "FastFile::fastOpen entry[%d] %s", i, entry.data->name);

            return i;
        }
    }

    return -1;
}

long FastFile::readFast(long entry_idx, void* out_buffer, long offset)
{
    if (entry_idx < 0 ||
        entry_idx >= this->num_entries ||
        !entries[entry_idx].has_valid_data)
    {
        return ERR_BADF_INVALID_ENTRY;
    }

    FastFileEntry entry = this->entries[entry_idx];
    FastFileEntryHeader* header = entry.data;

    uint32_t payload_offset = entry.offset_in_file + header->size;
    uint32_t uncompressed_size = header->uncompressed_size;
    uint32_t compressed_size = header->compressed_size;

    HANDLE handle = this->file_handle;
    
    DWORD new_pointer = SetFilePointer(handle, payload_offset, (PLONG)0x0, 0);
    this->file_cursor = new_pointer;

    DWORD read_bytes = 0;

    if (uncompressed_size == compressed_size) // NOT compressed, read directly.
    {
        this->file_cursor = new_pointer + offset;

        BOOL read_result = ReadFile(handle, out_buffer, offset, &read_bytes, (LPOVERLAPPED)0x0);

        if (read_result != FALSE) {
            return read_bytes;
        }

        return GetLastError();
    }

    // Decompress LZ
    if (g_LZPacketBuffer == nullptr) 
    {
        g_LZPacketBuffer = (unsigned char*)malloc(LZPacketBufferSize);
        if (g_LZPacketBuffer == nullptr)
        {
            return 0;
        }
    }

    if (LZPacketBufferSize < uncompressed_size)
    {
        free(g_LZPacketBuffer);
        g_LZPacketBuffer = (unsigned char*)malloc(uncompressed_size);
        if (g_LZPacketBuffer == nullptr)
        {
            return 0;
        }
    }
    else if (g_LZPacketBuffer == nullptr)
    {
        return 0;
    }

    this->file_cursor = new_pointer + uncompressed_size;

    BOOL read_result = ReadFile(handle, g_LZPacketBuffer, uncompressed_size, &read_bytes, (LPOVERLAPPED)0x0);

    if (read_result == FALSE) 
    {
        return GetLastError();
    }

    auto x = LZDecomp((unsigned char*)out_buffer, g_LZPacketBuffer, uncompressed_size);

    HOOK_DLOGF(FASTFILE, INFO, "FastFile::fastRead decompressed '%s'.'%s' %d => %d (%d)", this->filename, entry.data->name, compressed_size, uncompressed_size, x);

    return x & (x != compressed_size) - 1;
}

void FastFile::close()
{
    delete this->filename;
    this->filename = nullptr;
    this->file_size = 0;

    if (this->file_handle != INVALID_HANDLE_VALUE) {
        CloseHandle(this->file_handle);
        this->file_handle = INVALID_HANDLE_VALUE;
    }

    for (int i = 0; i < this->num_entries; i++) {
        if (this->entries[i].data) {
            free(this->entries[i].data);
            this->entries[i].data = nullptr;
        }
    }

    free(this->entries);
    this->entries = nullptr;
    this->num_entries = 0;
}

void FastFile::closeFast(long entry_idx)
{
    if (entry_idx >= 0 &&
        entry_idx < this->num_entries &&
        this->entries[entry_idx].has_valid_data != 0)
    {
        FastFileEntry entry = this->entries[entry_idx];
        entry.has_valid_data = 0;
        entry.offset_in_file = 0;
    }
}

long FastFile::seekFast(long entry_idx, long offset, long seek_orig)
{
    if (entry_idx < 0 || entry_idx >= this->num_entries)
    {
        return ERR_BADF_INVALID_ENTRY;
    }

    FastFileEntry* entry = &entries[entry_idx];

    if (!entry->has_valid_data) {
        return ERR_BADF_INVALID_ENTRY;
    }

    long new_offset = 0;
    long data_size = entry->data->size;

    switch (seek_orig) {
        case SEEK_SET:
            new_offset = offset;
            if (offset > data_size) {
                return ERR_BADF_SEEK_OUT_OF_BOUNDS;
            }
            break;
        case SEEK_CUR:
            new_offset = entry->offset_in_file + offset;
            if (new_offset > data_size) {
                return ERR_BADF_SEEK_OUT_OF_BOUNDS;
            }
            break;
        case SEEK_END:
            new_offset = data_size + offset;
            if (abs(offset) > data_size || offset > 0) {
                return ERR_BADF_SEEK_OUT_OF_BOUNDS;
            }
            break;
    }

    if (new_offset == -1) {
        return ERR_BADF_INVALID_OFFSET;
    }

    entry->offset_in_file = new_offset;

    DWORD new_file_pos = SetFilePointer(
        this->file_handle,
        entry->data->size + new_offset,
        NULL,
        FILE_BEGIN
    );

    this->file_cursor = new_file_pos;

    return new_offset;
}

long FastFile::sizeFast(long entry_idx)
{
    if (entry_idx >= 0 &&
        entry_idx < this->num_entries)
    {
        FastFileEntry* entry = &this->entries[entry_idx];

        if (entry->has_valid_data != 0) {
            return entry->data->uncompressed_size;
        }
    }
    return -1;
}

long FastFile::tellFast(long entry_idx)
{
    if (entry_idx >= 0 &&
        entry_idx < this->num_entries)
    {
        FastFileEntry* entry = &this->entries[entry_idx];

        if (entry->has_valid_data != 0) {
            return entry->offset_in_file;
        }
    }
    return -1;
}

FastFile::~FastFile()
{
    this->close();
}

FastFile* FastFile::FastFileFind(char* filename)
{
    //FastFile*** p_fastFiles = reinterpret_cast<FastFile***>(MCX_RUNTIME_ADDR(va_FastFile_fastFiles));
    //FastFile** fastFiles = *p_fastFiles;
    FastFile** fastFiles = g_fastFiles;
    long numFastFiles = g_numFastFiles;
    HOOK_DLOGF(FASTFILE, INFO, "FastFile::FastFileFind(%s) numFastFiles=%d ff=%p", filename, numFastFiles, fastFiles);

    if (fastFiles == nullptr || numFastFiles <= 0)
    {
        return nullptr;
    }

    for (int i = 0; i < numFastFiles; i++)
    {
        FastFile* ff = fastFiles[i];
        long result = ff->openFast(filename);

        if (result != -1)
        {
            return ff;
        }
    }

    return nullptr;
}

static AutoFuncPatchRegister reg_FastFile{
    "FastFile",
    {
        { (void*)va_FastFile_ctor, (void*)Hook_FastFile_ctor},
        { (void*)va_FastFile_open, GetMemberPtr(&FastFile::open)},
        { (void*)va_FastFile_lzSizeFast, GetMemberPtr(&FastFile::lzSizeFast)},
        { (void*)va_FastFile_openFast, GetMemberPtr(&FastFile::openFast)},
        { (void*)va_FastFile_close, GetMemberPtr(&FastFile::close)},
        { (void*)va_FastFile_closeFast, GetMemberPtr(&FastFile::closeFast)},
        { (void*)va_FastFile_readFast, GetMemberPtr(&FastFile::readFast)},
        { (void*)va_FastFile_seekFast, GetMemberPtr(&FastFile::seekFast)},
        { (void*)va_FastFile_sizeFast, GetMemberPtr(&FastFile::sizeFast)},
        { (void*)va_FastFile_tellFast, GetMemberPtr(&FastFile::tellFast)},
        { (void*)va_FastFile_dtor, GetMemberPtr(&FastFile::close)},
    }
};