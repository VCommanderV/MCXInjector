#pragma once
#include <cstdint>
#include "mcx_inject.hpp"

/*
 - GIFs are GIF89a format.
*/

static constexpr size_t GIF_DECOMP_BUF_SIZE = 20526;

#pragma pack(push, 1)
struct _window {
    uint8_t* pixels;
    int32_t width_zero_based;
    int32_t height_zero_based;
    uint32_t* stencil;
    uint8_t* shadow;
};

struct _pane {
    _window* window;
    int32_t x0;
    int32_t y0;
    int32_t x1;
    int32_t y1;
};

struct shape
{
    uint8_t x;
    uint8_t y;
    uint8_t width;
    uint8_t height;
};

struct shape_table
{
};

struct shape_header
{
    long bounds;
    long field_0x4;
    long field_0x8;
    long field_0xc;
    long field_0x10;
    long field_0x14;
};

struct VFX_RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};
#pragma pack(pop)

long __cdecl _VFX_GIF_resolution(uint8_t* gif_buf);
long __cdecl _VFX_GIF_draw(_pane* pane, uint8_t* gif_buf, void* tmp_buf);
long __cdecl _VFX_shape_count(void* shape_table);
long __cdecl _VFX_shape_bounds(void* shape_table, long shape_idx);
long __cdecl _VFX_panel_copy(_pane* src_pane, int src_x, int src_y, _pane* dst_pane, int dst_x, int dst_y, unsigned int flag);
long __cdecl _VFX_font_height(uint8_t* font);
// undefined4 _VFX_character_width(int param_1,int param_2)
long __cdecl _VFX_character_width(uint8_t* font, unsigned char ch);

#pragma region VAs
VADDR(va_VFX_panel_copy, 0x0076f8ab);
VADDR(va_VFX_draw_line, 0x0076cdfd);
VADDR(va_VFX_pane_wipe, 0x0076f7cc);
VADDR(va_VFX_font_height, 0x00771488);
VADDR(va_VFX_string_draw, 0x0077164e);
#pragma endregion

#pragma region FuncPtrs
static auto _VFX_panel_copy_PTR = reinterpret_cast<long(__cdecl*)(_pane*, int, int, _pane*, int, int, unsigned int)>(MCX_RUNTIME_ADDR(va_VFX_panel_copy));
// __cdecl void _VFX_draw_line_PTR(x0,y0,x1,y1,???,color)
static auto _VFX_draw_line_PTR = reinterpret_cast<long(__cdecl*)(_pane*, int, int, int, int, int, int)>(MCX_RUNTIME_ADDR(va_VFX_draw_line));
// undefined4 _VFX_pane_wipe(*pane pane, long color)
static auto _VFX_pane_wipe_PTR = reinterpret_cast<long(__cdecl*)(_pane*, long)>(MCX_RUNTIME_ADDR(va_VFX_pane_wipe));
// undefined4 _VFX_font_height(int param_1)
inline auto _VFX_font_height_PTR = FUN_PTR<long(__cdecl*)(void*)>(va_VFX_font_height);
inline auto _VFX_string_draw_PTR = FUN_PTR<void(__cdecl*)(_pane* pane, int32_t x, int32_t y, uint8_t* font, char* string, unsigned char* color_translate)>(va_VFX_string_draw);
#pragma endregion