#pragma once
#include <cstdint>
#include <cstddef>
#include "mcx_inject.hpp"

#ifndef ENABLE_LOG_COLOR
#define ENABLE_LOG_COLOR 0
#endif

#ifndef NO_ERR
#define NO_ERR	0
#endif

class PaletteBlock
{
public:
	short length; // 0x0
	short field_0x02; // 0x02
	uint8_t* rgb_data; // 0x04
public:
	//  public: void __thiscall PaletteBlock::initRgbData(unsigned char *)
	void initRgbData(unsigned char* data);
};

class Palette
{
public:
};

#pragma region VAs
VADDR(va_PaletteBlock_initRgbData, 0x006b2180);
#pragma endregion

#pragma region Layout asserts
#pragma endregion