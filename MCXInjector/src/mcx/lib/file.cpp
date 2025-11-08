#include "mcx_inject.hpp"
#include "mcx/lib/file.hpp"

MCXI_CTOR_HOOK Hook_File_ctor(File* this_ptr, void* unused_edx)
{
    this_ptr->vtable = (void*)MCX_RUNTIME_ADDR(va_File_vftable);
    this_ptr->filename_0x04 = (char*)0x0;
    this_ptr->open_flags_0x08 = 0;
    this_ptr->filehandle_0xc = (HANDLE)0xffffffff;
    this_ptr->file_size_0x18 = 0;
    this_ptr->field_0x1c = 0;
    this_ptr->field_0x20 = 0;
    this_ptr->field_0x30 = 0;
    this_ptr->field_0x34 = 0;
    this_ptr->field_0x38 = 0;
    this_ptr->children_0x24 = 0;
    this_ptr->children_count = 0;
    this_ptr->flags_0x44 = 0;
    this_ptr->fastfile_buffer_0x48 = 0;
    this_ptr->p_fast_file_ptr_0x10 = (void*)0x0;
    MOV_THIS_TO_EAX(this_ptr);
}

#pragma warning(disable:26495, justification : "Initialization happens in Hook_File_ctor" )
File::File()
{
    Hook_File_ctor(this, nullptr);
}
#pragma warning(default:26495)

static AutoFuncPatchRegister reg_File{
    "File",
    {
        { (void*)va_File_ctor, (void*)Hook_File_ctor }
    }
};