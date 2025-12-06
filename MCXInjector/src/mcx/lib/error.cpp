#include "mcx_inject.hpp"
#include "mcx/lib/error.hpp"
#include "mcx/lib/error.hpp"

// void (__cdecl ErrorHandler*)(unsigned int, char*, char*)
static void __forceinline __cdecl Hook_ErrorHandler(unsigned int err, char* err_msg, char* undefined)
{
    LOGF(FATAL, "ErrorHandler: err=%d msg=%s undef=%s", err, err_msg, undefined);
    auto orig = (void(__cdecl*)(unsigned int, char*, char*))GetPatchedFuncOrigFromVA((void*)va_ErrorHandler);
    // TODO: calls into Fatal(...)
    orig(err, err_msg, undefined);
}

void ErrorHandler(unsigned int err, char* err_msg, char* undefined)
{
    Hook_ErrorHandler(err, err_msg, undefined);
}

void __cdecl Assert(int flag, unsigned int param_2, char *message, char* param_4)
{
    auto orig = (void(__cdecl*)(int, unsigned int, char*, char*))MCX_RUNTIME_ADDR(va_Assert);
    return orig(flag, param_2, (char*)message, param_4);
}

static AutoFuncPatchRegister reg_ErrorHandler{
    "ErrorHandler",
    {
        { (void*)va_ErrorHandler, (void*)Hook_ErrorHandler }
    }
};
