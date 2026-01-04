#pragma once
#include "mcx_inject.hpp"

struct GlyphSubrecord {
	uint8_t code;      // 0x0: 0 = TERMINATOR
	int8_t  field_0x1; // 0x1
	int8_t  field_0x2; // 0x2
	uint8_t field_0x3; // 0x3
	uint8_t field_0x4; // 0x4
};

struct GlyphRecord {
	int8_t  glyph_id;		      // 0x0: -1 = INVALID, 0 = END-OF-TABLE
	uint8_t field_0x1;			  // 0x1
	uint8_t height;			      // 0x2
	GlyphSubrecord subrecords[];  // 0x3: variable-length, terminated by subs[i].code == 0
};

static constexpr int8_t INVALID_GLYPH = -1;
static constexpr int8_t GLYPH_END_OF_TABLE = 0;
static constexpr int8_t TERMINATOR = 0;

static const char* FONT_NAME = "font";

/*
GlyphRecord:
+0  int8   glyph_id
+1  uint8  ?
+2  uint8  height
+3  GlyphSubrecord subrecords[]

GlyphSubrecord:
+0  uint8  code
+1  int8   ?
+2  int8   ?
+3  uint8  width
+4  uint8  ?
*/
static __forceinline GlyphRecord* NextGlyph(uint8_t* p)
{
	uint8_t* q = p + offsetof(GlyphRecord, subrecords); // +3

	// walk variable lenght subrecords list until terminator
	while (*q != TERMINATOR) // subrecord.code at offset 0x0
	{
		q += 5;
	}

	GlyphRecord* g = reinterpret_cast<GlyphRecord*>(q + 1);

	return g;
}

class Font
{
public:
	int32_t field_0x0;
	int32_t field_0x4;
	int32_t field_0x8;
	uint8_t glyph_max_height; // 0xc
	uint8_t padding_0xd;
	uint8_t padding_0xe;
	uint8_t padding_0xf;
	int32_t field_0x10;
	int32_t field_0x14;
	int32_t field_0x18;
	uint8_t* glyphs; // 0x1c
	uint8_t* glyphs_cache[256]; // 0x20
public:
	// public: long __thiscall Font::init(char *)
	int32_t init(char* font_name);

	// protected: unsigned char __thiscall Font::getHeight(void)
	uint8_t getHeight();

	// public: unsigned char * __thiscall Font::find(unsigned char)
	GlyphRecord* find(unsigned char letter);

	// public: unsigned char * __thiscall Font::findLetter(unsigned char)
	GlyphRecord* findLetter(unsigned char letter);
};

#pragma region VAs
VADDR(va_Font_init, 0x00646960);
VADDR(va_Font_getHeight, 0x00646bf0);
VADDR(va_Font_findLetter, 0x00646ad0);
VADDR(va_Font_find, 0x00646a70);
VADDR(va_lineFont, 0x007aa824);
#pragma endregion

#pragma region Layout asserts
ASSERT_SIZE(Font, 0x420);
PTR_REF(Font, g_lineFont, va_lineFont);
#pragma endregion