#include "mcx_inject.hpp"
#include "mcx/lib/heap.hpp"
#include "mcx/lib/file.hpp"
#include "mcx/lib/a_globs.hpp"
#include "mcx/lib/c_ident.hpp"
#include "mcx/engine/font.hpp"


uint8_t Font::getHeight()
{
	uint8_t max_height = 0;

	if (!this->glyphs)
	{
		return max_height;
	}

	GlyphRecord* glyph = reinterpret_cast<GlyphRecord*>(this->glyphs);

	// iterate over the glyph table
	while (glyph->glyph_id != GLYPH_END_OF_TABLE) 
	{
		if (glyph->glyph_id != INVALID_GLYPH) 
		{
			if (glyph->height > max_height)
			{
				max_height = glyph->height;
			}
		}

		glyph = NextGlyph(reinterpret_cast<uint8_t*>(glyph));
	}

	return max_height;
}

GlyphRecord* Font::find(unsigned char letter)
{
	GlyphRecord* glyph = reinterpret_cast<GlyphRecord*>(this->glyphs);
	
	uint8_t* cached = this->glyphs_cache[letter];

	if (cached != (uint8_t*)-1 || this->glyphs == nullptr)
	{
		return reinterpret_cast<GlyphRecord*>(cached);
	}

	while (true)
	{
		if (glyph->glyph_id == letter)
		{
			break;
		}

		glyph = NextGlyph(reinterpret_cast<uint8_t*>(glyph));
	}

	this->glyphs_cache[letter] = reinterpret_cast<uint8_t*>(glyph);

	return glyph;
}

GlyphRecord* Font::findLetter(unsigned char letter)
{
	GlyphRecord* glyph = this->find(letter);

	if (glyph == nullptr && letter > 96 && letter < 123) // ASCII range 'a'-'z'
	{
		glyph = this->find(letter ^ 32); // toggle ASCII uppercase
	}

	return glyph;
}

int32_t Font::init(char* font_name)
{
	this->field_0x0 = 0;
	this->field_0x4 = 0;
	this->field_0x8 = 0xf;
	this->field_0x10 = 0x40000000;
	this->field_0x14 = 0;
	this->field_0x18 = 1;
	
	this->glyphs = nullptr;
	// Original was: UserHeap::free(systemHeap,(void *)0x0).
	// is this intentional, or was the field accidentally null-ed before
	// free, causing the compiler to optimize it out.
	g_systemHeap->free(this->glyphs);
	this->glyphs = nullptr;

	for (size_t i = 0; i < 256; i++)
	{
		this->glyphs_cache[i] = (uint8_t*)-1;
	}

	FullPathFileName* path = new FullPathFileName();
	path->buf = nullptr;
	path->init(&g_fontPath, font_name, (char*)".bin");

	File* file = new File();
	long res = file->open(path->buf, READ, FILE_DEFAULT_MAX_CHILDREN);
	if (res != NO_ERR)
	{
		delete file;
		delete path;
		return res;
	}

	FileSize size = file->fileSize();
	void* dst_buf = g_systemHeap->malloc(size);
	this->glyphs = reinterpret_cast<uint8_t*>(dst_buf);
	if (dst_buf == nullptr)
	{
		delete file;
		delete path;
		return 0xcccc0000;
	}

	size = file->fileSize();
	file->read2(reinterpret_cast<unsigned char*>(dst_buf), size);
	uint8_t height = this->getHeight();
	this->glyph_max_height = height;

	delete file;
	delete path;
	return 0;
}

static AutoFuncPatchRegister reg_Font{
	"Font",
	{
		{ (void*)va_Font_init, GetMemberPtr(&Font::init)},
		{ (void*)va_Font_getHeight, GetMemberPtr(&Font::getHeight)},
		{ (void*)va_Font_find, GetMemberPtr(&Font::find)},
		{ (void*)va_Font_findLetter, GetMemberPtr(&Font::findLetter)},
	}
};