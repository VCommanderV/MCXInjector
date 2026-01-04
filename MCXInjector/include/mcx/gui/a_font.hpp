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

VADDR(va_fonts, 0x007aadf8);
VADDR(va_systemFont, 0x007ab16c);
VADDR(va_blackFont, 0x007ab170);
VADDR(va_greyFont, 0x007ab174);
VADDR(va_whiteFont, 0x007ab178);
VADDR(va_redFont, 0x007ab17c);
VADDR(va_greenFont, 0x007ab180);
VADDR(va_dimFont, 0x007ab184);
VADDR(va_blueFont, 0x007ab188);
VADDR(va_yellowFont, 0x007ab18c);
VADDR(va_yellowDropFont, 0x007ab190);
VADDR(va_blueDropFont, 0x007ab194);
VADDR(va_medBlackFont, 0x007ab198);
VADDR(va_medGreyFont, 0x007ab19c);
VADDR(va_medWhiteFont, 0x007ab1a0);
VADDR(va_medRedFont, 0x007ab1a4);
VADDR(va_medGreenFont, 0x007ab1a8);
VADDR(va_medDimFont, 0x007ab1ac);
VADDR(va_medBlueFont, 0x007ab1b0);
VADDR(va_medYellowFont, 0x007ab1b4);
VADDR(va_lgBlackFont, 0x007ab1b8);
VADDR(va_lgGreyFont, 0x007ab1bc);
VADDR(va_lgWhiteFont, 0x007ab1c0);
VADDR(va_lgRedFont, 0x007ab1c4);
VADDR(va_lgGreenFont, 0x007ab1c8);
VADDR(va_lgDimFont, 0x007ab1cc);
VADDR(va_lgBlueFont, 0x007ab1d0);
VADDR(va_lgYellowFont, 0x007ab1d4);
#pragma endregion

#pragma region Globals
ARR_REF(aFont*, 29, g_fonts, va_fonts);
PTR_REF(aFont, g_systemFont, va_systemFont);

PTR_REF(aFont, g_blackFont, va_blackFont);
PTR_REF(aFont, g_redFont, va_redFont);
PTR_REF(aFont, g_yellowFont, va_yellowFont);
PTR_REF(aFont, g_greenFont, va_greenFont);
PTR_REF(aFont, g_blueFont, va_blueFont);
PTR_REF(aFont, g_greyFont, va_greyFont);
PTR_REF(aFont, g_whiteFont, va_whiteFont);
PTR_REF(aFont, g_dimFont, va_dimFont);
PTR_REF(aFont, g_yellowDropFont, va_yellowDropFont);
PTR_REF(aFont, g_blueDropFont, va_blueDropFont);

PTR_REF(aFont, g_medBlackFont, va_medBlackFont);
PTR_REF(aFont, g_medRedFont, va_medRedFont);
PTR_REF(aFont, g_medYellowFont, va_medYellowFont);
PTR_REF(aFont, g_medGreenFont, va_medGreenFont);
PTR_REF(aFont, g_medBlueFont, va_medBlueFont);
PTR_REF(aFont, g_medGreyFont, va_medGreyFont);
PTR_REF(aFont, g_medWhiteFont, va_medWhiteFont);
PTR_REF(aFont, g_medDimFont, va_medDimFont);

PTR_REF(aFont, g_lgBlackFont, va_lgBlackFont);
PTR_REF(aFont, g_lgRedFont, va_lgRedFont);
PTR_REF(aFont, g_lgYellowFont, va_lgYellowFont);
PTR_REF(aFont, g_lgGreenFont, va_lgGreenFont);
PTR_REF(aFont, g_lgBlueFont, va_lgBlueFont);
PTR_REF(aFont, g_lgGreyFont, va_lgGreyFont);
PTR_REF(aFont, g_lgWhiteFont, va_lgWhiteFont);
PTR_REF(aFont, g_lgDimFont, va_lgDimFont);
#pragma endregion

#pragma region Layout asserts (aObject)
ASSERT_SIZE(aFont, 260);
#pragma endregion
