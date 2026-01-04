#pragma once
#include "mcx_inject.hpp"

class FullPathFileName
{
public:
	char* buf;
public:
	// public: __thiscall FullPathFileName::~FullPathFileName(void)
	~FullPathFileName();

	// public: void __thiscall FullPathFileName::destroy(void)
	void destroy();

	// public: void __thiscall FullPathFileName::init(char *,char *,char *)
	void init(char* path_1, char* path_2, char* path_3);
};

#pragma region VAs
VADDR(va_FullPathFileName_dtor, 0x006448f0);
VADDR(va_FullPathFileName_destroy, 0x00644900);
VADDR(va_FullPathFileName_init, 0x00644940);
#pragma endregion