#pragma once
#include "mcx_inject.hpp"

VADDR(va_ErrorHandler, 0x0078def4);
VADDR(va_Assert, 0x00644250);
VADDR(va_Fatal, 0x00644180);
VADDR(va_GeneralMsg, 0x006440b0);

VADDR(va_GlobalLastError, 0x007d8f9c);

OBJ_REF(DWORD, g_LastError, va_GlobalLastError);

static void SetGlobalLastError(DWORD last_error)
{
	g_LastError = last_error;
}

void ErrorHandler(unsigned int err, char* err_msg, char* undefined);

// void __cdecl Assert(int,unsigned int,char *,char*)
void __cdecl Assert(int test, unsigned int err_code, char* err_msg, char* param_4);

// void __cdecl Fatal(unsigned int,char *,char *)
void __cdecl Fatal(unsigned int err_code, char* err_msg, char* param_3);

// void __cdecl GeneralMsg(char *)
void __cdecl GeneralMsg(char*);

#define STOP(err, fmt, ...) \
	char err_msg[164]; \
	sprintf(err_msg, fmt, __VA_ARGS__); \
	ErrorHandler(err, err_msg, nullptr);