#pragma once
#include <cstdint>
#include <cstddef>
#include "mcx_inject.hpp"
#include "mcx/lib/vfx.hpp"

#ifndef ENABLE_LOG_AFONT
#define ENABLE_LOG_AFONT 0
#endif

class aFont
{
public:
	uint8_t* ptr;
	uint8_t color_map[256]; // color_translate
public:
	// public: __thiscall aFont::aFont(void)
	aFont();

	// public: __thiscall aFont::~aFont(void)
	~aFont();

	// public: void __thiscall aFont::destroy(void)
	void destroy();

	// long __thiscall aFont::height(aFont *this)
	int32_t height();

	// public: long __thiscall aFont::init(char *)
	int32_t init(char* filename);

	// public: long __thiscall aFont::load(char *)
	int32_t load(char* filename);

	// long __thiscall aFont::width(aFont *this,uchar param_1)
	int32_t width2(unsigned char ch);

	// long __thiscall aFont::width2(aFont *this,uchar param_1)
	int32_t width(unsigned char* ch);

	// public: long __thiscall aFont::charactersToWidth(unsigned char *,long,int)
	int32_t charactersToWidth(unsigned char* text, long max_width, int wrap_mode);

	// public: long __thiscall aFont::writeString(struct _pane *,long,long,unsigned char *,long)
	int32_t writeString(_pane* pane, int32_t x, int32_t y, unsigned char* text, int32_t max_width);

	// public: long __thiscall aFont::writeStringToNewline(struct _pane *,long,long,unsigned char *)
	int32_t writeStringToNewline(_pane* pane, int32_t x, int32_t y, unsigned char* text);
};

#pragma region VAs
VADDR(va_aFont_ctor, 0x0060a970);
VADDR(va_aFont_dtor, 0x0060a980);
VADDR(va_aFont_destroy, 0x0060aaa0);
VADDR(va_aFont_height, 0x0060aae0);
VADDR(va_aFont_init, 0x0060a990);
VADDR(va_aFont_load, 0x0060aac0);
VADDR(va_aFont_width, 0x0060ab00);
VADDR(va_aFont_width2, 0x0060ab70);
VADDR(va_aFont_charactersToWidth, 0x0060ad20);
VADDR(va_aFont_writeString, 0x0060abf0);
VADDR(va_aFont_writeStringToNewline, 0x0060aca0);
#pragma endregion

#pragma region Layout asserts (aObject)
ASSERT_SIZE(aFont, 260);
#pragma endregion
