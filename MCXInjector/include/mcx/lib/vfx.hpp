#pragma once

#pragma pack(1)
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
#pragma pack(pop);