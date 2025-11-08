#pragma once
#include <cstdint>
#include <cstddef>
#include "mcx_inject.hpp"

struct _window {
    uint8_t* pixels;
    int32_t width_zero_based;
    int32_t height_zero_based;
    uint8_t undefined_0xc;
    uint8_t undefined_0xd;
    uint8_t undefined_0xe;
    uint8_t undefined_0xf;
    uint8_t undefined_0x10;
    uint8_t undefined_0x11;
    uint8_t undefined_0x12;
    uint8_t undefined_0x13;
};

struct _pane {
    _window* window;
    int32_t width;
    int32_t height;
};

// #pragma pack(push, 4)
class aObject {
# pragma region Fields
public:
    void* vtable;
    uint8_t field1_0x4;
    uint8_t field2_0x5;
    uint8_t field3_0x6;
    uint8_t field4_0x7;
    _pane* panel;
    uint8_t field9_0xc;
    uint8_t field10_0xd;
    uint8_t field11_0xe;
    uint8_t field12_0xf;
    uint8_t field13_0x10;
    uint8_t field14_0x11;
    uint8_t field15_0x12;
    uint8_t field16_0x13;
    int32_t x; // 0x14
    int32_t y; // 0x18
    uint8_t depth;
    uint8_t field26_0x1d;
    uint8_t field27_0x1e;
    uint8_t field28_0x1f;
    uint8_t field29_0x20;
    uint8_t field30_0x21;
    uint8_t field31_0x22;
    uint8_t field32_0x23;
    uint8_t field33_0x24;
    uint8_t field34_0x25;
    uint8_t field35_0x26;
    uint8_t field36_0x27;
    uint8_t field37_0x28;
    uint8_t field38_0x29;
    uint8_t field39_0x2a;
    uint8_t field40_0x2b;
    uint8_t field41_0x2c;
    uint8_t field42_0x2d;
    uint8_t field43_0x2e;
    uint8_t field44_0x2f;
    uint8_t field45_0x30;
    uint8_t field46_0x31;
    uint8_t field47_0x32;
    uint8_t field48_0x33;
    uint8_t field49_0x34;
    uint8_t field50_0x35;
    uint8_t field51_0x36;
    uint8_t field52_0x37;
    uint8_t field53_0x38;
    uint8_t field54_0x39;
    uint8_t field55_0x3a;
    uint8_t field56_0x3b;
    uint8_t field57_0x3c;
    uint8_t field58_0x3d;
    uint8_t field59_0x3e;
    uint8_t field60_0x3f;
    uint8_t field61_0x40;
    uint8_t field62_0x41;
    uint8_t field63_0x42;
    uint8_t field64_0x43;
    uint8_t field65_0x44;
    uint8_t field66_0x45;
    uint8_t field67_0x46;
    uint8_t field68_0x47;
    uint8_t field69_0x48;
    uint8_t field70_0x49;
    uint8_t field71_0x4a;
    uint8_t field72_0x4b;
    uint8_t field73_0x4c;
    uint8_t field74_0x4d;
    uint8_t field75_0x4e;
    uint8_t field76_0x4f;
    uint8_t field77_0x50;
    uint8_t field78_0x51;
    uint8_t field79_0x52;
    uint8_t field80_0x53;
    uint8_t field81_0x54;
    uint8_t field82_0x55;
    uint8_t field83_0x56;
    uint8_t field84_0x57;
    uint8_t field85_0x58;
    uint8_t field86_0x59;
    uint8_t field87_0x5a;
    uint8_t field88_0x5b;
    uint8_t field89_0x5c;
    uint8_t field90_0x5d;
    uint8_t field91_0x5e;
    uint8_t field92_0x5f;
    uint8_t field93_0x60;
    uint8_t field94_0x61;
    uint8_t field95_0x62;
    uint8_t field96_0x63;
    uint8_t field97_0x64;
    uint8_t field98_0x65;
    uint8_t field99_0x66;
    uint8_t field100_0x67;
    uint8_t field101_0x68;
    uint8_t field102_0x69;
    uint8_t field103_0x6a;
    uint8_t field104_0x6b;
    uint8_t field105_0x6c;
    uint8_t field106_0x6d;
    uint8_t field107_0x6e;
    uint8_t field108_0x6f;
    uint8_t field109_0x70;
    uint8_t field110_0x71;
    uint8_t field111_0x72;
    uint8_t field112_0x73;
    struct aObject* children[255];
    int32_t children_num;
    struct aObject* parent;
    uint8_t pBackground;
    uint8_t field117_0x479;
    uint8_t field118_0x47a;
    uint8_t field119_0x47b;
    uint8_t field120_0x47c;
    uint8_t field121_0x47d;
    uint8_t field122_0x47e;
    uint8_t field123_0x47f;
    void* animation;
    int32_t drag_start_x;
    int32_t drag_start_Y;
    int32_t dragging;
    int32_t animation_playing;
    int32_t back_color;
    uint8_t some_port;
    uint8_t field149_0x499;
    uint8_t field150_0x49a;
    uint8_t field151_0x49b;
    uint8_t field152_0x49c;
    uint8_t field153_0x49d;
    uint8_t field154_0x49e;
    uint8_t field155_0x49f;
    uint8_t field156_0x4a0;
    uint8_t field157_0x4a1;
    uint8_t field158_0x4a2;
    uint8_t field159_0x4a3;
    uint8_t field160_0x4a4;
    uint8_t field161_0x4a5;
    uint8_t field162_0x4a6;
    uint8_t field163_0x4a7;
    uint8_t field164_0x4a8;
    uint8_t field165_0x4a9;
    uint8_t field166_0x4aa;
    uint8_t field167_0x4ab;
    uint8_t field168_0x4ac;
#pragma endregion

public:
    static aObject* fromPtr(void* p) noexcept {
        return reinterpret_cast<aObject*>(p);
    }
};
// #pragma pack(pop)

static_assert(offsetof(_window, pixels) == 0x0, "_window.pixels offset mismatch");
static_assert(offsetof(_window, width_zero_based) == 0x4, "_window.width_zero_based offset mismatch");
static_assert(offsetof(_window, height_zero_based) == 0x8, "_window.height_zero_based offset mismatch");
static_assert(sizeof(_window) == 0x14, "Unspected size for _window struct.");

static_assert(offsetof(_pane, window) == 0x0, "_pane.window offset mismatch");
static_assert(offsetof(_pane, width) == 0x4, "_pane.width offset mismatch");
static_assert(offsetof(_pane, height) == 0x8, "_pane.height offset mismatch");
static_assert(sizeof(_pane) == 0xc, "Unspected size for _window struct.");


static_assert(offsetof(aObject, vtable) == 0x0, "aObject.vtable offset mismatch");
static_assert(offsetof(aObject, panel) == 0x8, "aObject.panel offset mismatch");
static_assert(offsetof(aObject, x) == 0x14, "aObject.x offset mismatch");
