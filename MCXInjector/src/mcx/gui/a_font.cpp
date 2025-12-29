#include "mcx_inject.hpp"
#include "mcx/lib/heap.hpp"
#include "mcx/lib/vfx.hpp"
#include "mcx/lib/file.hpp"
#include "mcx/lib/error.hpp"
#include "mcx/lib/a_globs.hpp"
#include "mcx/gui/a_font.hpp"

MCXI_CTOR_HOOK Hook_aFont_ctor(aFont* this_ptr, void* unused_edx)
{
    this_ptr->ptr = nullptr;
    MOV_THIS_TO_EAX(this_ptr);
}

static void __fastcall Hook_aFont_dtor(aFont* this_ptr, void* unused_edx)
{
}

#pragma warning(disable:26495, justification:"Initialization happens in Hook_aFont_ctor" )
aFont::aFont()
{
    Hook_aFont_ctor(this, nullptr);
}
#pragma warning(default:26495)

aFont::~aFont()
{
}

void aFont::destroy()
{
    if (this->ptr != nullptr)
    {
        g_guiHeap->free(this->ptr);
        this->ptr = nullptr;
    }
}

int32_t aFont::height()
{
    if (this->ptr == nullptr)
    {
        return 0;
    }

    int32_t font_height = _VFX_font_height(this->ptr);

    return font_height;
}

int32_t aFont::init(char* filename)
{
    if (this->ptr != nullptr)
    {
        this->destroy();
    }

    File* file = new File();
    char* font_path = &g_fontPath;
    char filepath_buf[128];
    sprintf(filepath_buf, "%s%s", font_path, filename);

    long open_res = file->open(filepath_buf, READ, FILE_DEFAULT_MAX_CHILDREN);

    HOOK_DLOGF(AFONT, INFO, "aFont::init(%s) => %d", filepath_buf, open_res);

    if (open_res != NO_ERR)
    {
        char err_msg[256];
        sprintf(err_msg, (char*)"Unable to find '%s'", filepath_buf);
        GeneralMsg(err_msg);
        delete file;
        return -1;
    }

    FileSize size = file->fileSize();

    if (size == 0)
    {
        delete file;
        return -2;
    }

    void* dst_buf = g_guiHeap->malloc(size);
    this->ptr = reinterpret_cast<uint8_t*>(dst_buf);

    if (dst_buf == nullptr)
    {
        delete file;
        return 3;
    }

    file->read2(reinterpret_cast<unsigned char*>(dst_buf), size);
    file->close();

    for (int i = 0; i < 256; i++) {
        this->color_map[i] = (uint8_t)i;
    }

    delete file;
    return 0;
}

int32_t aFont::width(unsigned char* ch)
{
    if (this->ptr == nullptr || *ch == '\0')
    {
        return 0;
    }

    int32_t width = 0;
    size_t len = strlen((char*)ch);

    if (len > 0)
    {
        for (size_t i = 0; i < len; i++)
        {
            width += _VFX_character_width(this->ptr, ch[i]);
        }
    }

    return width;
}

int32_t aFont::width2(unsigned char ch)
{
    if (ch == '\0')
    {
        return 0;
    }

    if (this->ptr == nullptr)
    {
        return 0;
    }

    return _VFX_character_width(this->ptr, ch);
}

int32_t aFont::charactersToWidth(unsigned char* text, long max_width, int wrap_mode)
{
    if (wrap_mode == 0) // truncate from the end
    {
        size_t len = strlen((char*)text);
        unsigned char* end = text + len;
        
        while (text < end)
        {
            long current_width = this->width(text);

            if (current_width <= max_width)
            {
                break;  // fits
            }

            // too wide, remove the last character
            end -= 1;
            unsigned char saved_char = *end;
            *end = '\0';  // null terminate
        }

        // actual number of characters that fit
        return end - text;
    }
    else 
    {
        // forward word-wrap like, 
        // aka find last space that fits and break there
        long current_width = this->width(text);

        if (current_width <= max_width)
        {
            // entire text fits
            return strlen((char*)text);
        }

        // too wide, find last space to break at
        char* last_space = strrchr((char*)text, ' ');

        if (last_space == nullptr)
        {  
            // no word break possible
            return -1;
        }

        // try progressively shorter strings by removing words

        char* break_point = last_space;
        *break_point = '\0';

        char* search_end = break_point - 1;

        while (true)
        {
            *break_point = '\0';
            current_width = this->width(text);

            if (current_width <= max_width)
            {
                size_t result = strlen((char*)text);
                *break_point = ' ';
                return result;
            }

            *break_point = ' ';
            char saved = *search_end;
            *search_end = '\0';

            char* prev_space = strrchr((char*)text, ' ');
            *search_end = saved;

            if (!prev_space)
            {
                return -1;
            }

            break_point = prev_space;
            search_end = break_point - 1;
        }
    }
    return 0;
}

int32_t aFont::writeString(_pane* pane, int32_t x, int32_t y, unsigned char* text, int32_t max_width)
{
    if (this == nullptr || this->ptr == nullptr)
    {
        return -3;
    }

    if (text == nullptr || *text == '\0')
    {
        return 0;
    }

    // If max width is "unlimited", draw directly
    if (max_width == -1)
    {
        _VFX_string_draw_PTR(pane, x, y, this->ptr, (char*)text, this->color_map);
        return 0;
    }

    int len = strlen((char*)text);

    int width = this->width(text);

    unsigned char saved_char = 0;
    int original_len = len;

    // Truncate from the end until it fits max_width
    while (width > max_width && len > 0)
    {
        saved_char = text[len - 1]; // save last char
        text[len - 1] = '\0'; // remove it
        len--;

        width = this->width(text);
    }

    _VFX_string_draw_PTR(pane, x, y, this->ptr, (char*)text, this->color_map);

    // Restore removed character
    // NOTE: original seems to only restore 1?
    if (len < original_len)
    {
        text[len] = saved_char;
    }

    return 0;
}

int32_t aFont::writeStringToNewline(_pane* pane, int32_t x, int32_t y, unsigned char* text)
{
    if (this == nullptr || this->ptr == nullptr)
    {
        return -3;
    }

    if (text == nullptr || *text == '\0')
    {
        return 0;
    }

    unsigned char* newline = reinterpret_cast<unsigned char*>(strchr((char*)text, '\n'));

    if (newline != text)
    {
        if (newline != nullptr)
        {
            *newline = '\0';
        }

        _VFX_string_draw_PTR(pane, x, y, this->ptr, (char*)text, this->color_map);

        if (newline != nullptr)
        {
            *newline = '\n';
        }
    }

    return 0;
}

int32_t aFont::load(char* filename)
{
    this->destroy();
    return this->init(filename);
}

static AutoFuncPatchRegister reg_aFont{
    "aFont",
    {
        { (void*)va_aFont_ctor, (void*)Hook_aFont_ctor },
        { (void*)va_aFont_dtor, (void*)Hook_aFont_dtor },
        { (void*)va_aFont_destroy, GetMemberPtr(&aFont::destroy)},
        { (void*)va_aFont_height, GetMemberPtr(&aFont::height)},
        { (void*)va_aFont_init, GetMemberPtr(&aFont::init)},
        { (void*)va_aFont_load, GetMemberPtr(&aFont::load)},
        { (void*)va_aFont_width, GetMemberPtr(&aFont::width)},
        { (void*)va_aFont_width2, GetMemberPtr(&aFont::width2)},
        { (void*)va_aFont_charactersToWidth, GetMemberPtr(&aFont::charactersToWidth)},
        { (void*)va_aFont_writeString, GetMemberPtr(&aFont::writeString)},
        { (void*)va_aFont_writeStringToNewline, GetMemberPtr(&aFont::writeStringToNewline)},
    }
};