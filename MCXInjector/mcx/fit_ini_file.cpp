#include "file.hpp"
#include "fit_ini_file.hpp"
#include "mcx_inject.hpp"
#include <set>

std::set<void*> constructed_objects;

// __thiscall FitIniFile::open(File*, int flags, int extra)
static int __fastcall Hook_FitIniFile_open(FitIniFile* this_ptr, void* edx_unused, char* file, int flags, int extra)
{
    LogDebug("FitIniFile", "FitIniFile::open(%p,%d) => this=%p.", (void*)file, flags, this_ptr);
    auto orig = GetTrampoline<int(__thiscall*)(FitIniFile*, char*, int, int)>(va_FitIniFile_open);
    auto ret = orig(this_ptr, file, flags, extra);
    LogDebug("FitIniFile", "FitIniFile::open(%p, %d) => 0x%08x", (void*)file, flags, ret);
    LogDebug("FitIniFile", "%s",this_ptr->filename_0x04);
    return ret;
}

// __thiscall FitIniFile::FitIniFile(void)
static void __fastcall Hook_FitIni_ctor(FitIniFile* this_ptr, void* edx_unused)
{
    LogDebug("FitIniFile", "FitIniFile::ctor this=%p", this_ptr);

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

    LogDebug("FitIniFile", "File::ctor => vtable=%p.", file->vtable);

    file->vtable = (void*)MCX_RUNTIME_ADDR(va_FitIniFile_vftable);
    FitIniFile* fit_ini_file = reinterpret_cast<FitIniFile*>(file);
    fit_ini_file->field_0x4c = 0;
    fit_ini_file->field_0x50 = 0;
    fit_ini_file->field_0x54 = 0;
    fit_ini_file->field_0x58 = 0;
    fit_ini_file->field_0x5c = 0;

    LogDebug("FitIniFile", "FitIniFile::ctor => vtable=%p.", fit_ini_file->vtable);

    MOV_THIS_TO_EAX(fit_ini_file)
}

// __thiscall FitIniFile::FitIniFile(char*, long*)
static long __fastcall Hook_FitIniFile_readIdLong(void* thisPtr, void* edx_unused, char* key, long* outValue)
{
    auto orig = GetTrampoline<long(__thiscall*)(void*, char*, long*)>(va_FitIniFile_readIdLong);
    long ret = orig(thisPtr, key, outValue);
    LogDebug("FitIniFile", "Hook_FitIniFile_readIdLong: '%s' = %d (%d)", key, ret == 0 ? *outValue : 0, ret);
    return ret;
}


static AutoFuncPatchRegister reg_FitIniFile{
    "FitIniFile",
    {
        { (void*)va_FitIniFile_ctor, (void*)Hook_FitIni_ctor },
        { (void*)va_FitIniFile_open, (void*)Hook_FitIniFile_open },
        { (void*)va_FitIniFile_readIdLong, (void*)Hook_FitIniFile_readIdLong }
    }
};