#pragma once
#include "mcx_inject.hpp"

static constexpr uintptr_t va_ErrorHandler = 0x0078def4;

static constexpr uintptr_t va_GlobalLastError = 0x007d8f9c;

static DWORD* g_LastError = reinterpret_cast<DWORD*>(MCX_RUNTIME_ADDR(va_GlobalLastError));

static void SetGlobalLastError(DWORD last_error)
{
	*g_LastError = last_error;
}

void ErrorHandler(unsigned int err, char* err_msg, char* undefined);

#define STOP(err, fmt, ...) \
	char err_msg[164]; \
	sprintf(err_msg, fmt, __VA_ARGS__); \
	ErrorHandler(err, err_msg, nullptr);