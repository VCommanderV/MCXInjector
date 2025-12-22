#include "mcx_inject.hpp"
#include "mcx/lib/error.hpp"
#include "mcx/lib/heap.hpp"
#include "mcx/lib/fast_file.hpp"
#include "mcx/lib/file.hpp"
#include <mcx/lib/packet_file.hpp>

DWORD File::lastError = NO_ERR;
BOOL File::logFileTraffic = FALSE;
File* fileTrafficLog = NULL;

MCXI_CTOR_HOOK Hook_File_ctor(File* this_ptr, void* unused_edx)
{
    this_ptr->vtable = (File_vtable*)MCX_RUNTIME_ADDR(va_File_vftable);
    this_ptr->file_name = (char*)0x0;
    this_ptr->file_mode = (FileMode)0;
    this_ptr->file_handle = INVALID_HANDLE_VALUE;
    this_ptr->length = 0;
    this_ptr->pos = 0;
    this_ptr->buf_result = 0;
    this_ptr->parent = nullptr;
    this_ptr->parent_offset = 0;
    this_ptr->phys_length = 0;
    this_ptr->children = 0;
    this_ptr->num_children = 0;
    this_ptr->in_ram = 0;
    this_ptr->fastfile_buf = 0;
    this_ptr->fastfile = (FastFile*)0x0;
    MOV_THIS_TO_EAX(this_ptr);
}

static __forceinline void* __cdecl Hook_File_new(size_t size)
{
    return mcx::heap::adaptive_malloc<void>(g_systemHeap, size);
}

static void __forceinline __cdecl Hook_File_delete(void* ptr)
{
    mcx::heap::adaptive_free<void>(g_systemHeap, ptr);
}

#pragma warning(disable:26495, justification : "Initialization happens in Hook_File_ctor" )
void* File::operator new(size_t size)
{
    return Hook_File_new(size);
}

void* File::operator new(size_t size, void* ptr) noexcept
{
    return ptr;
}

void File::operator delete(void* ptr)
{
    Hook_File_delete(ptr);
}

File::File()
{
    Hook_File_ctor(this, nullptr);
}
#pragma warning(default:26495)


long File::open(char* filename, FileMode mode, long max_children)
{
    HOOK_DLOGF(BASEFILE, INFO, "File::open(%s, %d, %d)", filename, mode, max_children);

    UserHeap* system_heap = g_systemHeap;

    auto filename_len = strnlen_s(filename, MAX_FILENAME_SIZE) + 1;

    this->file_name = mcx::heap::adaptive_malloc<char>(system_heap, filename_len);

    if (this->file_name == nullptr)
    {
        return FILE_ERR_NO_SPACE_FOR_FILENAME;
    }

    strcpy(this->file_name, filename);

    this->file_mode = mode;
    DWORD desired_access = 0;

    switch (mode) 
    {
        case READ:
            desired_access = GENERIC_READ;
            break;
        case CREATE:
        case WRITE:
        case RDWRITE:
        case RDWRITE | READ:
            desired_access = GENERIC_READ | GENERIC_WRITE;
            break;
    }

    if (mode == CREATE)
    {
        HANDLE handle = CreateFileA(
            filename,
            desired_access,
            FILE_SHARE_READ,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        this->file_handle = handle;

        if (handle == INVALID_HANDLE_VALUE)
        {
            DWORD last_err = GetLastError();
            SetGlobalLastError(last_err);
            return last_err;
        }
    }
    else
    {
        HANDLE handle = CreateFileA(
            filename,
            desired_access,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        this->file_handle = handle;

        // Nope, try to see if the file is actually FastFile.
        if (handle == INVALID_HANDLE_VALUE)
        {
            FastFile* ff = FastFile::FastFileFind(filename);
            
            this->fastfile = ff;
            
            if (ff == nullptr)
            {
                mcx::heap::adaptive_free(system_heap, this->file_name);
                this->file_name = nullptr;
                LOG_FILE_TRAFFIC("FNF    Length: 0000000000  File: %s", filename);
                return FILE_ERR_FILE_NOT_FOUND;
            }
            
            long ff_handle = ff->openFast(filename);
            this->fastfile_handle = ff_handle;

            FileSize file_size = this->fileSize();
            LOG_FILE_TRAFFIC("FASTF    Length: %010d   File: %s", file_size, filename);

            this->in_ram = TRUE;
            this->fastfile_buf = system_heap->malloc(file_size);
            
            if (this->fastfile_buf != nullptr)
            {
                ff->readFast(ff_handle, this->fastfile_buf, file_size);
                ff->closeFast(ff_handle);
                this->fastfile_handle = -1;
                this->fastfile = nullptr;
                this->pos = 0;
                return NO_ERR;
            }
        }
        else 
        {
            FileSize file_size = this->fileSize();
            LOG_FILE_TRAFFIC("CFHandle    Length: %010d    File: %s", file_size, filename);
            this->pos = 0;
            if (this->isOpen() == FALSE)
            {
                this->length = 0;
            }
            else 
            {
                this->length = file_size;
            }

            this->parent = (File*)0x0;
            this->parent_offset = 0;
            this->phys_length = this->length;
            this->children = (File**)0x0;
            this->num_children = 0;
            this->max_children = max_children;

            this->children = mcx::heap::adaptive_malloc<File*>(system_heap, max_children * sizeof(File*));

            if (this->children == nullptr)
            {
                return FILE_ERR_NO_SPACE_FOR_CHILD_LIST;
            }

            this->num_children = 0;

            for (int i = 0; i < max_children; i++)
            {
                this->children[i] = NULL;
            }
        }
    }

    return NO_ERR;
}

long File::open2(File* new_parent, FileSize file_size, long children_num)
{
    if (new_parent == nullptr || new_parent->fastfile != nullptr)
    {
        return FILE_ERR_BAD_PARENT;
    }

    this->parent = new_parent;
    
    FileMode parent_mode = parent->file_mode;

    if (parent_mode != FileMode::READ)
    {
        return FILE_ERR_PARENT_READONLY;
    }

    this->phys_length = file_size;
    this->parent_offset = parent->pos;
    this->pos = 0;
    this->file_name = new_parent->file_name;
    this->file_mode = parent_mode;
    this->file_handle = parent->file_handle;

    LOG_FILE_TRAFFIC("CHILD     Length: %010d    File: %s", file_size, parent->file_name);

    long res = new_parent->addChild(this);

    if (res != NO_ERR)
    {
        return res;
    }

    UserHeap* system_heap = g_systemHeap;

    if (children_num == -1)
    {
        this->max_children = 0;
        this->in_ram = TRUE;
        new_parent = nullptr;
        void* buf = system_heap->malloc(file_size);
        this->fastfile_buf = buf;
        
        if (buf == nullptr)
        {
            this->in_ram = FALSE;
        }
        
        FileClass parent_class = this->parent->vtable->getFileClass(this->parent);

        if (parent_class == FileClass::PACKETFILE)
        {
            PacketFile* pkt_file = reinterpret_cast<PacketFile*>(this->parent);
            children_num = pkt_file->current_packet;
            auto fun = reinterpret_cast<unsigned int(__thiscall*)(PacketFile*, uint32_t, void*)>(
                MCX_RUNTIME_ADDR(va_PacketFile_0064d250)
            );
            unsigned int x = fun(pkt_file, children_num, buf);
            // handle PacketFile
            return NO_ERR;
        }

        DWORD bytes_read = 0;
        BOOL read_res = ReadFile(
            this->file_handle,
            buf,
            file_size,
            &bytes_read,
            NULL
        );

        if (read_res == FALSE)
        {
            SetGlobalLastError(GetLastError());
        }

        return NO_ERR;
    }
    else 
    {
        this->max_children = children_num;
        this->children = mcx::heap::adaptive_malloc<File*>(system_heap, max_children * sizeof(File*));

        if (this->children == nullptr)
        {
            return FILE_ERR_NO_SPACE_FOR_CHILD_LIST;
        }

        this->num_children = 0;

        for (int i = 0; i < children_num; i++)
        {
            this->children[i] = NULL;
        }
        return NO_ERR;
    }

    return NO_ERR;  
}

void File::close()
{
    HOOK_DLOGF(BASEFILE, INFO, "File::close() this=%p filename=%s max_children=%d", this, this->file_name, this->max_children);

    UserHeap* system_heap = g_systemHeap;

    if (this->parent == nullptr)
    {
        mcx::heap::adaptive_free(system_heap, this->file_name);
    }

    this->file_name = nullptr;
    this->length = 0;

    if (this->isOpen() != FALSE)
    {
        if (this->parent == nullptr && this->file_handle != INVALID_HANDLE_VALUE)
        {
            if (this->file_handle != NULL)
            {
                BOOL handle_closed = CloseHandle(this->file_handle);
            }
        }

        this->file_handle = INVALID_HANDLE_VALUE;

        if (this->fastfile != nullptr)
        {
            this->fastfile->closeFast(this->fastfile_handle);
        }

        this->fastfile = nullptr;
        this->fastfile_handle = -1;
    }

    // Free children
    uint32_t max_children = this->max_children;

    if (max_children > 0 && this->children != nullptr)
    {
        for (size_t i = 0; i < this->max_children; i++)
        {
            File* child = this->children[i];
            if (child == nullptr) continue;
            child->vtable->close(child); // TODO: virtual
        }
        mcx::heap::adaptive_free(system_heap, this->children);
    }

    // Remove ourselves from parent, if any
    if (this->parent != nullptr)
    {
        auto removeChild = reinterpret_cast<void(__thiscall*)(File*, File*)>(MCX_RUNTIME_ADDR(va_File_removeChild));
        removeChild(this->parent, this);
    }

    this->children = nullptr;
    this->num_children = 0;

    if (this->in_ram != FALSE)
    {
        system_heap->free(this->fastfile_buf);
        this->fastfile_buf = nullptr;
        this->in_ram = FALSE;
    }
}

long File::create(char* filename)
{
    return this->vtable->open2(this, filename, FileMode::CREATE, 0x32);
}

BOOL File::isOpen()
{
    if (this->file_handle == INVALID_HANDLE_VALUE && this->fastfile_buf == nullptr)
    {
        return FALSE;
    }

    return TRUE;
}

FileSize __forceinline File::fileSize()
{
    //HOOK_DLOGF(BASEFILE, INFO, "File::fileSize(%s)", this->file_name);

    // Backed by a FastFile.
    if (this->fastfile != nullptr && this->length == 0)
    {
        FileSize file_size = this->fastfile->sizeFast(this->fastfile_handle);
        this->length = file_size;
        //HOOK_DLOGF(BASEFILE, INFO, "File::fileSize(%s), fastfile => %d", this->file_name, file_size);
        return file_size;
    }
    
    // Parent physical file
    if (this->parent != nullptr)
    {
        this->length = this->phys_length;
        //HOOK_DLOGF(BASEFILE, INFO, "File::fileSize(%s), parent => %d", this->file_name, this->phys_length);
        return this->phys_length;
    }

    BOOL is_open = this->isOpen();
    FileSize file_size = this->length;

    if (is_open == TRUE)
    {

        if (file_size == 0 || this->file_mode > FileMode::CREATE)
        {
            this->length = GetFileSize(this->file_handle, 0);
        }

        file_size = this->length;
    }

    //HOOK_DLOGF(BASEFILE, INFO, "File::fileSize(%s) => %d", this->file_name, file_size);
    return file_size;
}

long File::addChild(File* child)
{
    if (this->max_children > 0)
    {
        for (size_t i = 0; i < max_children; i++)
        {
            File* child_slot = this->children[i];

            if (child_slot == nullptr)
            {
                this->children[i] = child;
                HOOK_DLOGF(BASEFILE, INFO, "File::addChild(%s) children[%d]=%s", this->file_name, i, child->file_name);
                return NO_ERR;
            }
        }
    }

    return FILE_ERR_NO_SPACE_LEFT_FOR_CHILD;
}

void File::removeChild(File* child)
{
    if (this->max_children <= 0 || this->children == nullptr)
    {
        return;
    }

    for (size_t i = 0; i < this->max_children; i++)
    {
        if (this->children[i] == child)
        {
            this->children[i] = nullptr;
            return;
        }
    }
}

char* File::getFilename()
{
    return this->file_name;
}

FileClass File::getFileClass()
{
    return FileClass::BASEFILEE;
}

FileSize File::getLength()
{
    return this->fileSize();
}

long File::seek(long offset, long seek_orig)
{
    HOOK_DLOGF(BASEFILE, INFO, "File::seek(%s, %d, %d)", this->file_name, offset, seek_orig);

    FileSize length = this->getLength();

    switch (seek_orig) 
    {
        case SEEK_SET:
            if (length < offset) {
                return ERR_BADF_SEEK_OUT_OF_BOUNDS;
            }
            break;
        case SEEK_CUR:
            if (length < this->pos + offset) {
                return ERR_BADF_SEEK_OUT_OF_BOUNDS;
            }
            break;
        case SEEK_END:
            if (offset < 0 || length < std::abs(offset))
            {
                return ERR_BADF_SEEK_OUT_OF_BOUNDS;
            }
            break;
    }

    if (this->in_ram == FALSE || this->fastfile_buf == nullptr)
    {
        if (this->fastfile == nullptr)
        {
            if (this->parent == nullptr)
            {
                g_newPosition = SetFilePointer(this->file_handle, offset, (PLONG)0x0, seek_orig);
            }
            else if (seek_orig == SEEK_SET) {
                DWORD new_pos = SetFilePointer(this->file_handle, this->parent_offset + offset, (PLONG)0x0, 0);
                g_newPosition = new_pos - this->parent_offset;
            }
            else if (seek_orig == SEEK_CUR) {
                DWORD new_pos = SetFilePointer(this->file_handle, offset, (PLONG)0x0, 1);
                g_newPosition = new_pos - this->parent_offset;
            }
            else if (seek_orig == SEEK_END) {
                PLONG dst_to_move_high = (PLONG)0x0;
                FileSize length = this->getLength();
                g_newPosition = SetFilePointer(this->file_handle, length + this->parent_offset, dst_to_move_high, seek_orig);
                DWORD new_pos = SetFilePointer(this->file_handle, offset, (PLONG)0x0, 1);
                g_newPosition = new_pos - this->parent_offset;
            }
            else {
                g_newPosition = g_newPosition - this->parent_offset;
            }
            if (g_newPosition == 0xffffffff) {
                return ERR_BADF_INVALID_OFFSET;
            }
        }
        else 
        {
            g_newPosition = this->fastfile->seekFast(this->fastfile_handle, offset, seek_orig);
            this->pos = g_newPosition;
            return NO_ERR;
        }
    }

    if (this->parent == nullptr)
    {
        switch (seek_orig)
        {
        case SEEK_SET:
            g_newPosition = offset;
            break;
        case SEEK_CUR:
            g_newPosition = g_newPosition + offset;
            break;
        case SEEK_END:
            g_newPosition = this->getLength() + offset;
            break;
        }
    }
    else if (seek_orig == SEEK_SET) {
        g_newPosition = offset;
    }
    else if (seek_orig == SEEK_CUR) {
        g_newPosition = g_newPosition + offset;
    }
    else if (seek_orig == SEEK_END) {
        g_newPosition = this->getLength() + this->parent_offset + offset;
    }
    if (g_newPosition == -1) {
        return ERR_BADF_INVALID_OFFSET;
    }

    this->pos = g_newPosition;
    return NO_ERR;
}

void File::skip(long bytes)
{
    if (bytes != 0)
    {
        this->seek(this->pos + bytes, SEEK_SET);
    }
}

long File::read(FileOffset offset, unsigned char* dst_buf, long bytes)
{
    HOOK_DLOGF(BASEFILE, INFO, "File::read(%s, %d, %p, %d)", this->file_name, offset, dst_buf, bytes);

    // In RAM
    if (this->in_ram != FALSE && this->fastfile_buf != nullptr)
    {
        void* src = static_cast<char*>(this->fastfile_buf) + offset;
        std::memcpy(dst_buf, src, bytes);
        return bytes;
    }

    // FastFile
    FastFile* ff = this->fastfile;
    if (ff != nullptr)
    {
        if (this->pos != offset)
        {
            ff->seekFast(this->fastfile_handle, offset, SEEK_SET);
        }
        return ff->readFast(this->fastfile_handle, dst_buf, bytes);
    }


    if (this->isOpen() == FALSE)
    {
        SetGlobalLastError(ERR_BADF_INVALID_ENTRY);
        return 0;
    }

    if (this->pos != offset) {
        this->seek(offset, 0);
    }
    DWORD bytes_read = 0;
    BOOL read_res = ReadFile(this->file_handle, dst_buf, bytes, &bytes_read, 0x0);

    if (read_res == FALSE)
    {
        SetGlobalLastError(GetLastError());
        return bytes_read;
    }

    return bytes_read;
}

long File::read2(unsigned char* dst_buf, long bytes)
{
    HOOK_DLOGF(BASEFILE, INFO, "File::read(%s, %p, %d)", this->file_name, dst_buf, bytes);

    // In RAM
    if (this->in_ram != FALSE && this->fastfile_buf != nullptr)
    {
        void* src = static_cast<char*>(this->fastfile_buf) + this->pos;
        std::memcpy(dst_buf, src, bytes);
        this->pos += bytes;
        return bytes;
    }

    // FastFile
    if (this->fastfile != nullptr)
    {
        long bytes_read = this->fastfile->readFast(
            this->fastfile_handle,
            dst_buf,
            bytes
        );
        this->pos += bytes_read;
        return bytes_read;
    }

    // Regulare file handle
    if (this->isOpen() == FALSE)
    {
        SetGlobalLastError(ERR_BADF_INVALID_ENTRY);
        return 0;
    }

    DWORD bytes_read = 0;

    BOOL read_res = ReadFile(
        this->file_handle,
        dst_buf,
        bytes,
        &bytes_read,
        NULL
    );

    if (read_res == FALSE)
    {
        SetGlobalLastError(GetLastError());
        return bytes_read;
    }

    this->pos += bytes_read;
    return bytes_read;
}

BYTE File::readByte()
{
    BYTE byte = 0;
    long bytes_read = this->read2(&byte, 1);
    HOOK_DLOGF(BASEFILE, INFO, "File::readByte(%s) => %d (%c)", this->file_name, byte, byte);
    return byte;
}

short File::readWord(void)
{
    short s = 0;
    long bytes_read = this->read2((unsigned char*)s, 2);
    return s;
}

long File::readString(unsigned char* dst_buf)
{
    if (this->isOpen() == FALSE)
    {
        return ERR_BADF_INVALID_ENTRY;
    }

    unsigned char ch = this->readByte();
    *dst_buf = ch;
    if (ch != '\0')
    {
        size_t i = 0;
        do
        {
            i += 1;
            ch = this->readByte();
            dst_buf[i] = ch;
        } while (ch != '\0');
        return i;
    }

    return 0;
}

short File::readShort(void)
{
    return this->readWord();
}

uint32_t File::getNumLines()
{
    uint32_t num_lines = 0;

    FileOffset old_pos = this->pos;

    this->seek(0, SEEK_SET);

    if (length != 0)
    {
        size_t i = 0;

        do
        {
            BYTE ch = this->readByte();

            if (ch == '\n')
            {
                num_lines += 1;
            }

            i += 1;
        } while (i < this->getLength());
    }

    this->seek(old_pos, SEEK_SET);
    return num_lines;
}

long File::readLine(unsigned char* dst_buf, long buf_size)
{
    long bytes_read = 0;
    long line_length = 0;
    unsigned char* search_buf = nullptr;

    // In RAM
    if (this->in_ram != FALSE && this->fastfile_buf != nullptr)
    {
        // Search for \r in the buffer
        search_buf = static_cast<unsigned char*>(this->fastfile_buf) + this->pos;

        // Find \r or end of buffer
        for (line_length = 0; line_length < buf_size; line_length++)
        {
            if (search_buf[line_length] == '\r') {
                break;
            }
        }

        // copy to output buffer
        std::memcpy(dst_buf, search_buf, line_length);
        dst_buf[line_length] = '\0';

        // advance position past \r
        this->pos += line_length + 1;

        // is this CRLF?
        if (search_buf[line_length + 1] == '\n') {
            this->pos += 1;  // consume \n too
        }

        HOOK_DLOGF(BASEFILE, INFO, "File::readLine(%s, '%s') [RAM]", this->file_name, (char*)dst_buf);

        return line_length + 1;
    }

    // FastFile
    if (this->fastfile != nullptr)
    {
        bytes_read = this->fastfile->readFast(
            this->fastfile_handle,
            dst_buf,
            buf_size
        );

        if (bytes_read < buf_size) {
            buf_size = bytes_read;
        }

        // Same as before
        for (line_length = 0; line_length < buf_size; line_length++)
        {
            if (dst_buf[line_length] == '\r') {
                break;
            }
        }

        dst_buf[line_length] = '\0';

        this->pos += line_length + 1;

        if (dst_buf[line_length + 1] == '\n') {
            this->pos += 1;
        }

        // Seek to new position
        this->fastfile->seekFast(
            this->fastfile_handle,
            this->pos,
            0
        );

        HOOK_DLOGF(BASEFILE, INFO, "File::readLine(%s, %s, %d) [FF]", this->file_name, (char*)dst_buf, buf_size);

        return line_length + 1;
    }

    // Regular File
    if (this->isOpen() == FALSE)
    {
        SetGlobalLastError(ERR_BADF_INVALID_ENTRY);
        return 0;
    }

    DWORD bytes_read_dw = 0;
    BOOL read_result = ReadFile(
        this->file_handle,
        dst_buf,
        buf_size,
        &bytes_read_dw,
        NULL
    );

    if (read_result == FALSE)
    {
        SetGlobalLastError(GetLastError());
        return bytes_read_dw;
    }

    bytes_read = bytes_read_dw;

    // Same again
    if (bytes_read < buf_size) {
        buf_size = bytes_read;
    }

    for (line_length = 0; line_length < buf_size; line_length++)
    {
        if (dst_buf[line_length] == '\r') {
            break;
        }
    }

    dst_buf[line_length] = '\0';

    this->pos += line_length + 1;

    if (dst_buf[line_length + 1] == '\n') {
        this->pos += 1;
    }

    this->seek(this->pos, SEEK_SET);

    HOOK_DLOGF(BASEFILE, INFO, "File::readLine(%s, %s, %d) [FH]", this->file_name, (char*)dst_buf, buf_size);

    return line_length + 1;
}

long File::write(FileOffset position, unsigned char* src_buf, long bytes)
{
    if (this->parent != nullptr)
    {
        SetGlobalLastError(FILE_ERR_PARENT_READONLY);
        return 0;
    }

    if (this->isOpen() != FALSE)
    {
        if (this->pos != position) {
            this->seek(position, SEEK_SET);
        }

        DWORD bytes_written = 0;
        BOOL write_res = WriteFile(this->file_handle, src_buf, bytes, &bytes_written, 0);
        if (write_res == FALSE)
        {
            SetGlobalLastError(GetLastError());
        }
        return bytes_written;
    }

    SetGlobalLastError(ERR_BADF_INVALID_ENTRY);
    return 0;
}

long File::write2(unsigned char* src_buf, long bytes)
{
    if (this->parent != nullptr)
    {
        SetGlobalLastError(FILE_ERR_PARENT_READONLY);
        return 0;
    }

    if (this->isOpen() != FALSE)
    {
        DWORD bytes_written = 0;
        BOOL write_res = WriteFile(this->file_handle, src_buf, bytes, &bytes_written, 0);
        if (write_res == FALSE)
        {
            SetGlobalLastError(GetLastError());
        }
        this->pos += bytes_written;
        return bytes_written;
    }

    SetGlobalLastError(ERR_BADF_INVALID_ENTRY);
    return 0;
}

template<typename T>
inline long File::writeT(T t)
{
    if (this->parent != nullptr)
    {
        SetGlobalLastError(FILE_ERR_PARENT_READONLY);
        return 0;
    }

    if (this->isOpen() != FALSE)
    {
        DWORD bytes_written = 0;
        BOOL write_res = WriteFile(this->file_handle, &t, sizeof(T), &bytes_written, 0);
        if (write_res == FALSE)
        {
            SetGlobalLastError(GetLastError());
        }

        if (bytes_written == sizeof(T))
        {
            this->pos += sizeof(T);
            return 0;
        }

        return FILE_ERR_WRITE_FAIL;
    }

    SetGlobalLastError(ERR_BADF_INVALID_ENTRY);
    return 0;
}

long File::writeByte(BYTE byte_)
{
    return this->writeT<BYTE>(byte_);
}

long File::writeFloat(float float_)
{
    return this->writeT<float>(float_);
}

long File::writeLong(long long_)
{
    return this->writeT<long>(long_);
}

long File::writeShort(short short_)
{
    return this->writeT<short>(short_);
}

long File::writeString(char* string)
{
    if (this->parent != nullptr)
    {
        SetGlobalLastError(FILE_ERR_PARENT_READONLY);
        return -1;
    }

    if (this->isOpen() == FALSE)
    {
        SetGlobalLastError(ERR_BADF_INVALID_ENTRY);
        return -1;
    }

    char* ptr = string;
    while (*ptr != '\0')
    {
        this->writeByte(*ptr);
        ptr++;
    }

    return ptr - string;
}

long File::readLong()
{
    long result = 0;

    // in RAM
    if (this->in_ram != FALSE && this->fastfile_buf != nullptr)
    {
        // readLong?
        result = *(long*)(this->pos + (int)this->fastfile_buf);
        this->pos += 4;
        return result;
    }

    // FastFile
    if (this->fastfile != nullptr)
    {
        this->fastfile->readFast(this->fastfile_handle, &result, 4);
        this->pos += 4;
        return result;
    }

    // Regular file
    if (this->isOpen() == FALSE)
    {
        // not open?    
        SetGlobalLastError(ERR_BADF_INVALID_ENTRY);
        return 0;
    }

    this->pos += 4;
    DWORD bytes_read = 0;
    BOOL read_res = ReadFile(this->file_handle, &result, 4, &bytes_read, nullptr);
    if (read_res == FALSE)
    {
        SetGlobalLastError(GetLastError());
        return result;
    }

    return result;
}

long File::writeWord(short word_)
{
    return this->writeT<short>(word_);
}

BOOL File::eof()
{
    return this->pos >= this->getLength();
}

static AutoFuncPatchRegister reg_File{
    "File",
    {
        { (void*)va_File_ctor, (void*)Hook_File_ctor },
        { (void*)va_FastFile_operator_new, (void*)Hook_File_new},
        { (void*)va_FastFile_operator_delete, (void*)Hook_File_delete},
        { (void*)va_File_isOpen, GetMemberPtr(&File::isOpen)},
        { (void*)va_File_eof, GetMemberPtr(&File::eof)},
        { (void*)va_File_fileSize, GetMemberPtr(&File::fileSize)},
        { (void*)va_File_open, GetMemberPtr(&File::open)},
        { (void*)va_File_open2, GetMemberPtr(&File::open2)},
        { (void*)va_File_close, GetMemberPtr(&File::close)},
        { (void*)va_File_addChild, GetMemberPtr(&File::addChild)},
        { (void*)va_File_removeChild, GetMemberPtr(&File::removeChild)},
        { (void*)va_File_seek, GetMemberPtr(&File::seek)},
        { (void*)va_File_read, GetMemberPtr(&File::read)},
        { (void*)va_File_read2, GetMemberPtr(&File::read2)},
        { (void*)va_File_getNumLines, GetMemberPtr(&File::getNumLines)},
        { (void*)va_File_readByte, GetMemberPtr(&File::readByte)},
        { (void*)va_File_readLine, GetMemberPtr(&File::readLine)},
        { (void*)va_File_readShort, GetMemberPtr(&File::readShort)},
        { (void*)va_File_readString, GetMemberPtr(&File::readString)},
        { (void*)va_File_skip, GetMemberPtr(&File::skip)},
        { (void*)va_File_write, GetMemberPtr(&File::write)},
        { (void*)va_File_write2, GetMemberPtr(&File::write2)},
        { (void*)va_File_writeByte, GetMemberPtr(&File::writeByte)},
        { (void*)va_File_writeShort, GetMemberPtr(&File::writeShort)},
        { (void*)va_File_writeLong, GetMemberPtr(&File::writeLong)},
        { (void*)va_File_writeWord, GetMemberPtr(&File::writeWord)},
        { (void*)va_File_writeFloat, GetMemberPtr(&File::writeFloat)},
        { (void*)va_File_writeString, GetMemberPtr(&File::writeString)},
    }
};