#pragma once
#include "mcx_inject.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma region VAs
VADDR(va_languageOffset, 0x0080a86c);
OBJ_REF(int32_t, g_languageOffset, va_languageOffset);
#pragma endregion

// long __cdecl cLoadString(struct HINSTANCE__*, unsigned int, char*, int)
static inline int32_t __cdecl cLoadString(HINSTANCE* instance, uint32_t base, char* buf, int32_t buf_size)
{
	uint32_t strid = base + g_languageOffset;
	memset(buf, '\0', buf_size);
	return LoadStringA(*instance, strid, buf, buf_size);
}