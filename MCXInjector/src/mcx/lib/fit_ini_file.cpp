#include <string_view>
#include "mcx_inject.hpp"
#include "mcx/lib/file.hpp"
#include "mcx/lib/fit_ini_file.hpp"


// __thiscall FitIniFile::FitIniFile(void)
static void __fastcall Hook_FitIni_ctor(FitIniFile* this_ptr, void* edx_unused)
{
    File* file = reinterpret_cast<File*>(this_ptr);
    file->vtable = (void*)MCX_RUNTIME_ADDR(va_File_vftable);
    file->filename_0x04 = (char*)0x0;
    file->open_flags_0x08 = 0;
    file->filehandle_0xc = (HANDLE)0xffffffff;
    file->file_size_0x18 = 0;
    file->field_0x1c = 0;
    file->field_0x20 = 0;
    file->field_0x30 = 0;
    file->field_0x34 = 0;
    file->field_0x38 = 0;
    file->children_0x24 = 0;
    file->children_count = 0;
    file->flags_0x44 = 0;
    file->fastfile_buffer_0x48 = 0;
    file->p_fast_file_ptr_0x10 = (void*)0x0;

    file->vtable = (void*)MCX_RUNTIME_ADDR(va_FitIniFile_vftable);
    FitIniFile* fit_ini_file = reinterpret_cast<FitIniFile*>(file);
    fit_ini_file->field_0x4c = 0;
    fit_ini_file->field_0x50 = 0;
    fit_ini_file->field_0x54 = 0;
    fit_ini_file->field_0x58 = 0;
    fit_ini_file->field_0x5c = 0;

    //LOG_DEBUG << "FitIniFile::ctor => vtable=%p." << fit_ini_file->vtable;

    MOV_THIS_TO_EAX(fit_ini_file)
}


// __thiscall FitIniFile::open(File*, int flags, int extra)
int FitIniFile::open(char* file, int flags, int extra)
{
    auto orig = GetTrampoline<int(__thiscall*)(FitIniFile*, char*, int, int)>(va_FitIniFile_open);
    auto ret = orig(this, file, flags, extra);

    //LOGF_DEBUG("FitIniFile::open(%p,%d) => this=%p. (%s)",
    //    (void*)file,
    //    flags,
    //    this,
    //    this->filename_0x04
    //);

    return ret;
}


long FitIniFile::readIdLong(char* key, long* out_value)
{
    auto orig = GetTrampoline<long(__thiscall*)(void*, char*, long*)>(va_FitIniFile_readIdLong);
    long ret = orig(this, key, out_value);
    // LOGF_DEBUG("FitIniFile::readIdLong('%s') => %d (%d)", key, ret == 0 ? *out_value : 0, ret);
    return ret;
}


static AutoFuncPatchRegister reg_FitIniFile{
    "FitIniFile",
    {
        { (void*)va_FitIniFile_ctor, (void*)Hook_FitIni_ctor },
        { (void*)va_FitIniFile_open, GetMemberPtr(&FitIniFile::open)},
        { (void*)va_FitIniFile_readIdLong, GetMemberPtr(&FitIniFile::readIdLong)}
    }
};