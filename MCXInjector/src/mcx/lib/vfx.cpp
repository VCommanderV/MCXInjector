#include "mcx_inject.hpp"
#include "mcx/lib/vfx.hpp"

long __cdecl _VFX_GIF_resolution(uint8_t* gif_buf)
{
    const uint8_t* ptr = gif_buf + 0xd;

    if ((gif_buf[10] & 0x80) != 0)
    {
        ptr = ptr + (1 << ((gif_buf[10] & 7) + 1)) * 3;
    }

    // Read two 16-bit values
    uint16_t width = *(uint16_t*)(ptr + 5);
    uint16_t height = *(uint16_t*)(ptr + 7);

    // Pack the values width => high 16 bits, heght => lower 16 bits
    return ((uint32_t)width << 16) | height;
}

long _VFX_GIF_draw(_pane* pane, uint8_t* gif_buf, void* tmp_buf)
{
    auto orig = reinterpret_cast<long(__cdecl*)(_pane*, uint8_t*, void*)>(MCX_RUNTIME_ADDR((uintptr_t)0x00771c39));
    return orig(pane, gif_buf, tmp_buf);
}

long __cdecl _VFX_shape_count(void* gif_buf)
{
    const uint8_t* ptr = (uint8_t*)gif_buf;
    const long* shape_count = reinterpret_cast<const long*>(ptr + 4);
    return *shape_count;
}

long __cdecl _VFX_shape_bounds(void* shape_table, long shape_idx)
{
    return 0;
}

long __cdecl _VFX_panel_copy(_pane* src_pane, int src_x, int src_y, _pane* dst_pane, int dst_x, int dst_y, unsigned int flag)
{
    return 0;
}

//long __cdecl _VFX_font_height(uint8_t* font)
//{
//    return *(font + 8);
//}
//
//long __cdecl _VFX_character_width(uint8_t* font, unsigned char ch)
//{
//    return *(long*)(*(int*)(ch * 4 + font + 16) + font);
//}
