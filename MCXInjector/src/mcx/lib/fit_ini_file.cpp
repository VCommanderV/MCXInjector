#include <string_view>
#include "mcx_inject.hpp"
#include "mcx/lib/file.hpp"
#include "mcx/lib/fit_ini_file.hpp"


// __thiscall FitIniFile::FitIniFile(void)
MCXI_CTOR_HOOK Hook_FitIniFile_ctor(FitIniFile* this_ptr, void* edx_unused)
{
    File* file = reinterpret_cast<File*>(this_ptr);
    new (file) File();

    file->vtable = (File_vtable*)MCX_RUNTIME_ADDR(va_FitIniFile_vftable);
    FitIniFile* fit_ini_file = reinterpret_cast<FitIniFile*>(file);
    fit_ini_file->field_0x4c = 0;
    fit_ini_file->field_0x50 = 0;
    fit_ini_file->field_0x54 = 0;
    fit_ini_file->field_0x58 = 0;
    fit_ini_file->field_0x5c = 0;

    MOV_THIS_TO_EAX(fit_ini_file)
}

#pragma warning(disable:26495, justification:"Initialization happens in Hook_FastFile_ctor" )
FitIniFile::FitIniFile()
{
    Hook_FitIniFile_ctor(this, nullptr);
}
#pragma warning(default:26495)

// __thiscall FitIniFile::open(File*, int mode, int extra)
int FitIniFile::open(File* file, unsigned long file_size, long num_children)
{
    HOOK_DLOGF(FITINIFILE, INFO, "START FitIniFile::open(%p,%d,%d) => this=%p. (%s)",
        file,
        file_size,
        num_children,
        this,
        file->file_name
    );

    /*auto orig = GetTrampoline<int(__thiscall*)(FitIniFile*, char*, int, int)>(va_FitIniFile_open);
    auto ret = orig(this, file, flags, extra);*/
    File* f = reinterpret_cast<File*>(this);
    auto vtable = (File_vtable*)MCX_RUNTIME_ADDR(va_File_vftable);
    long ret = vtable->open(f, file, file_size, -1);

    if (ret == NO_ERR)
    {
        auto afterOpen = reinterpret_cast<long(__thiscall*)(FitIniFile*)>((void*)MCX_RUNTIME_ADDR(va_FitIniFile_afterOpen));
        ret = afterOpen(this);
    }

    HOOK_DLOGF(FITINIFILE, INFO, "END FitIniFile::open(%p,%d,%d) => %d",
        file,
        file_size,
        num_children,
        ret
    );

    return ret;
}

long FitIniFile::findNextBlockStart(char* line, unsigned long line_length)
{
    return 0;
}

long FitIniFile::readIdLong(char* key, long* out_value)
{
    auto orig = GetTrampoline<long(__thiscall*)(void*, char*, long*)>(va_FitIniFile_readIdLong);
    long ret = orig(this, key, out_value);
    HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::readIdLong('%s') => %d (%d)", key, ret == 0 ? *out_value : 0, ret);
    return ret;
}


static AutoFuncPatchRegister reg_FitIniFile{
    "FitIniFile",
    {
        { (void*)va_FitIniFile_ctor, (void*)Hook_FitIniFile_ctor },
        //{ (void*)va_FitIniFile_open, GetMemberPtr(&FitIniFile::open)},
        //{ (void*)va_FitIniFile_readIdLong, GetMemberPtr(&FitIniFile::readIdLong)}
    }
};