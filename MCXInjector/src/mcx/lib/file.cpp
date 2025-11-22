#include "mcx_inject.hpp"
#include "mcx/lib/error.hpp"
#include "mcx/lib/heap.hpp"
#include "mcx/lib/fast_file.hpp"
#include "mcx/lib/file.hpp"

DWORD File::lastError = NO_ERR;
BOOL File::logFileTraffic = FALSE;
File* fileTrafficLog = NULL;

MCXI_CTOR_HOOK Hook_File_ctor(File* this_ptr, void* unused_edx)
{
    this_ptr->vtable = (File_vtable*)MCX_RUNTIME_ADDR(va_File_vftable);
    this_ptr->filename = (char*)0x0;
    this_ptr->mode = (FileMode)0;
    this_ptr->filehandle = INVALID_HANDLE_VALUE;
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
    this_ptr->fast_file = (FastFile*)0x0;
    MOV_THIS_TO_EAX(this_ptr);
}

#pragma warning(disable:26495, justification : "Initialization happens in Hook_File_ctor" )
File::File()
{
    Hook_File_ctor(this, nullptr);
}
#pragma warning(default:26495)

long File::open(char* filename, FileMode mode, long max_children)
{
    HOOK_LOGF(BASEFILE, INFO, "File::open(%s, %d, %d)", filename, mode, max_children);

    UserHeap* system_heap = *g_systemHeap_ptr;

    auto filename_len = strnlen_s(filename, MAX_FILENAME_SIZE) + 1;

    if (system_heap == nullptr || system_heap->heap_size == 0)
    {
        this->filename = static_cast<char*>(::operator new(filename_len));
    }
    else
    {
        this->filename = (char*)system_heap->malloc(filename_len);
    }

    if (this->filename == nullptr)
    {
        return FILE_ERR_NO_SPACE_FOR_FILENAME;
    }

    strcpy(this->filename, filename);

    this->mode = mode;
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

        this->filehandle = handle;

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

        this->filehandle = handle;

        // Nope, try to see if the file is actually FastFile.
        if (handle == INVALID_HANDLE_VALUE)
        {
            FastFile* ff = FastFile::FastFileFind(filename);
            
            this->fast_file = ff;
            
            if (ff == nullptr)
            {
                if (system_heap == nullptr || system_heap->heap_size == 0)
                {
                    delete this->filename;
                }
                else
                {
                    system_heap->free(this->filename);
                }

                this->filename = nullptr;
                LOG_FILE_TRAFFIC("FNF    Length: 0000000000  File: %s", filename);
                return FILE_ERR_FILE_NOT_FOUND;
            }
            
            auto ff_handle = ff->openFast(filename);
            this->fastfile_handle = ff_handle;

            auto getSize = (FileSize(__thiscall*)(File*))(MCX_RUNTIME_ADDR(va_File_fileSize));
            auto file_size = getSize(this);
            LOG_FILE_TRAFFIC("FASTF    Length: %010d   File: %s", file_size, filename);

            this->in_ram = TRUE;
            this->fastfile_buf = system_heap->malloc(file_size);
            
            if (this->fastfile_buf != nullptr)
            {
                ff->readFast(ff_handle, this->fastfile_buf, file_size);
                ff->closeFast(ff_handle);
                this->fastfile_handle = -1;
                this->fast_file = nullptr;
                this->pos = 0;
                return NO_ERR;
            }
        }
        else 
        {
            auto getSize = (FileSize(__thiscall*)(File*))(MCX_RUNTIME_ADDR(va_File_fileSize));
            auto file_size = getSize(this);
            LOG_FILE_TRAFFIC("CFHandle    Length: %010d    File: %s", file_size, filename);
            this->pos = 0;
            auto isOpen = (BOOL(__thiscall*)(File*))(MCX_RUNTIME_ADDR(va_File_isOpen));
            auto is_open = isOpen(this);
            if (is_open == FALSE)
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

            if (system_heap == nullptr || system_heap->heap_size == 0)
            {
                this->children = (File**)std::malloc(max_children * sizeof(File*));
            }
            else
            {
                this->children = (File**)system_heap->malloc(max_children * sizeof(File*));
            }

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

long File::create(char* filename)
{
    return this->vtable->open2(this, filename, FileMode::CREATE, 0x32);
}

FileSize File::fileSize()
{
    return FileSize();
}

static AutoFuncPatchRegister reg_File{
    "File",
    {
        { (void*)va_File_ctor, (void*)Hook_File_ctor },
        { (void*)va_File_open, GetMemberPtr(&File::open)},
    }
};