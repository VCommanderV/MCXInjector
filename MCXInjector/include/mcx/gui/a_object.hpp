#pragma once
#include <cstdint>
#include <cstddef>
#include "mcx_inject.hpp"
#include "mcx/gui/a_port.hpp"
#include "mcx/lib/vfx.hpp"

#ifndef ENABLE_LOG_AOBJECT
#define ENABLE_LOG_AOBJECT 0
#endif

#ifndef NO_ERR
#define NO_ERR	0
#endif

static constexpr auto MAX_CHILDREN = 255;

enum aObjectState
{
    NORMAL = 0,
    MAXIMIZED = 1,
    ICONIZED = 2,
};

class aObject;
class aAnimation;

struct aObject_vtable
{
    void(__thiscall* vector_deleting_destructor)(unsigned int);
    void(__thiscall* init)();
    void(__thiscall* destroy)();
    void(__thiscall* FUN_0060f4c0)();
    void(__thiscall* height)(aObject*, int32_t);
    void(__thiscall* x)(aObject*, int32_t);
    void(__thiscall* y)(aObject*, int32_t);
    void(__thiscall* moveTo)(aObject*, int32_t, int32_t, int);
    void(__thiscall* resize)();
    void(__thiscall* frame)();
    void(__thiscall* addChild)();
    void(__thiscall* removeChild)(aObject*, aObject*);
    void(__thiscall* setParent)(aObject*, aObject*);
};

#pragma pack(push, 4)
class aObject {
# pragma region Fields
public:
    aObject_vtable* vtable;
    int32_t force_45_pixels_maybe; // 0x04
    _pane* panel; // 0x08
    int32_t width; // 0x0C
    int32_t height; // 0x10
    int32_t x; // 0x14
    int32_t y; // 0x18
    int32_t depth; // 0x1C
    int32_t field_0x20; // 0x20
    int32_t field_0x24; // 0x24
    int32_t field_0x28; // 0x28
    int32_t field_0x2C; // 0x2C
    int32_t field_0x30; // 0x30
    int32_t field_0x34; // 0x34
    int32_t field_0x38; // 0x38
    int32_t field_0x3C; // 0x3C
    int32_t field_0x40; // 0x40
    int32_t field_0x44; // 0x44
    int32_t field_0x48; // 0x48
    int32_t field_0x4C; // 0x4C
    aObjectState state; // 0x50
    BOOL showing; // 0x54
    short initialization_state; // 0x58
    char field_0x5A; // 0x5A
    char field_0x5B; // 0x5B
    BOOL hidden; // 0x5C
    int32_t some_x; // 0x60
    int32_t hide_direction; // 0x64
    int32_t field_0x68; // 0x68
    int32_t field_0x6C; // 0x6C
    BOOL transparent; // 0x70
    aObject* children[255]; // 0x74
    int32_t children_num; // 0x470
    aObject* parent; // 0x474
    void* background; // 0x478
    aAnimation* icon; // 0x47C
    aAnimation* animation; // 0x480
    int32_t drag_start_x; // 0x484
    int32_t drag_start_y; // 0x488
    int32_t dragging; // 0x48C
    int32_t animation_playing; // 0x490
    int32_t back_color; // 0x494
    aPort* port; // 0x498
    void* paint_routine; // 0x49C
    int32_t field_0x4A0; // 0x4A0
    int32_t drop_target_num; // 0x4A4
    void* field_0x4a8_maybe_rect_arr; // 0x4A8
#pragma endregion

public:
    static aObject* fromPtr(void* p) noexcept {
        return reinterpret_cast<aObject*>(p);
    }

    // public: __thiscall aObject::aObject(void)
    aObject();

    // public: virtual long __thiscall aObject::init(long,long,long,long,char *)

    long init(long x, long y, long width, long height, char* param_5);

    // public: virtual int32_t __thiscall aObject::x(void)
    int32_t getX() const;

    // public: virtual int32_t __thiscall aObject::y(void)
    int32_t getY() const;

    // public: virtual int32_t __thiscall aObject::width(void)
    int32_t getWidth() const;

    // public: virtual int32_t __thiscall aObject::height(void)
    int32_t getHeight() const;

    // public: virtual void __thiscall aObject::addChild(class aObject *)
    void addChild(aObject* child);

    // public: virtual class aAnimation * __thiscall aObject::animation(void)
    aAnimation* getAnimation();
};
#pragma pack(pop)

#pragma region VAs
static constexpr uintptr_t va_aObject_ctor = 0x0060e440;
static constexpr uintptr_t va_aObject_dtor = 0x0060e4d0;
static constexpr uintptr_t va_aObject_y = 0x0060f520;
static constexpr uintptr_t va_aObject_x = 0x0060f510;
static constexpr uintptr_t va_aObject_top = 0x0060a7f0;
static constexpr uintptr_t va_aObject_stopModal = 0x0060eae0;
static constexpr uintptr_t va_aObject_stopDrag = 0x0060f430;
static constexpr uintptr_t va_aObject_stopAnimation = 0x0060eac0;
static constexpr uintptr_t va_aObject_state = 0x0060fc50;
static constexpr uintptr_t va_aObject_startModal = 0x0060ead0;
static constexpr uintptr_t va_aObject_startDrag = 0x0060f400;
static constexpr uintptr_t va_aObject_startAnimation = 0x0060eab0;
static constexpr uintptr_t va_aObject_SetTransparent = 0x0060a7d0;
static constexpr uintptr_t va_aObject_setState = 0x0060fc60;
static constexpr uintptr_t va_aObject_setParent = 0x0060ea50;
static constexpr uintptr_t va_aObject_setPaintRoutine = 0x0060e990;
static constexpr uintptr_t va_aObject_setIcon = 0x0060fe10;
static constexpr uintptr_t va_aObject_SetHideDirection = 0x0060a7c0;
static constexpr uintptr_t va_aObject_setEventRoutine = 0x0060e9a0;
static constexpr uintptr_t va_aObject_setDisplayPort = 0x0060e880;
static constexpr uintptr_t va_aObject_setDepth = 0x0060ea60;
static constexpr uintptr_t va_aObject_SetBit = 0x0060fed0;
static constexpr uintptr_t va_aObject_setBackground_1 = 0x0060fb20;
static constexpr uintptr_t va_aObject_setBackground_2 = 0x0060fba0;
static constexpr uintptr_t va_aObject_setBackColor = 0x0060eaf0;
static constexpr uintptr_t va_aObject_setAnimation = 0x0060fda0;
static constexpr uintptr_t va_aObject_resize = 0x0060f650;
static constexpr uintptr_t va_aObject_removeChild = 0x0060f2a0;
static constexpr uintptr_t va_aObject_rectIntersect_1 = 0x0060e950;
static constexpr uintptr_t va_aObject_rectIntersect_2 = 0x0060e910;
static constexpr uintptr_t va_aObject_ptr = 0x0060f4e0;
static constexpr uintptr_t va_aObject_port = 0x0060f500;
static constexpr uintptr_t va_aObject_pointInside = 0x0060e8e0;
static constexpr uintptr_t va_aObject_paint = 0x0060e9b0;
static constexpr uintptr_t va_aObject_operator_new = 0x0060e4e0;
static constexpr uintptr_t va_aObject_operator_delete = 0x0060e500;
static constexpr uintptr_t va_aObject_numberOfChildren = 0x0060f1e0;
static constexpr uintptr_t va_aObject_normalize = 0x0060fc30;
static constexpr uintptr_t va_aObject_moveTo = 0x0060f5a0;
static constexpr uintptr_t va_aObject_maximize = 0x0060fc20;
static constexpr uintptr_t va_aObject_left = 0x0060a7e0;
static constexpr uintptr_t va_aObject_leave = 0x0060e8d0;
static constexpr uintptr_t va_aObject_IsShowing = 0x0060a7a0;
static constexpr uintptr_t va_aObject_IsHidden = 0x0060a7b0;
static constexpr uintptr_t va_aObject_init = 0x0060e520;
static constexpr uintptr_t va_aObject_iconize = 0x0060fc40;
static constexpr uintptr_t va_aObject_icon = 0x0060feb0;
static constexpr uintptr_t va_aObject_HideMe = 0x0060ff30;
static constexpr uintptr_t va_aObject_height = 0x0060f4d0;
static constexpr uintptr_t va_aObject_handleEvent = 0x0060f950;
static constexpr uintptr_t va_aObject_globalY = 0x0060f560;
static constexpr uintptr_t va_aObject_globalX = 0x0060f530;
static constexpr uintptr_t va_aObject_getDropTargetCount = 0x0060a850;
static constexpr uintptr_t va_aObject_getDropTarget = 0x0060a860;
static constexpr uintptr_t va_aObject_GetCamera = 0x0060a790;
static constexpr uintptr_t va_aObject_frame = 0x0060f590;
static constexpr uintptr_t va_aObject_foremostChild = 0x0060f460;
static constexpr uintptr_t va_aObject_findObject = 0x0060e9c0;
static constexpr uintptr_t va_aObject_FillBox = 0x0060eec0;
static constexpr uintptr_t va_aObject_drawFramed = 0x0060ebf0;
static constexpr uintptr_t va_aObject_drawBox_1 = 0x0060eb10;
static constexpr uintptr_t va_aObject_drawBox_2 = 0x0060a750;
static constexpr uintptr_t va_aObject_draw = 0x0060f6f0;
static constexpr uintptr_t va_aObject_dragStartY = 0x0060f450;
static constexpr uintptr_t va_aObject_dragStartX = 0x0060f440;
static constexpr uintptr_t va_aObject_dragging = 0x0060f3f0;
static constexpr uintptr_t va_aObject_display = 0x0060f790;
static constexpr uintptr_t va_aObject_destroy = 0x0060e6b0;
static constexpr uintptr_t va_aObject_depth = 0x0060eaa0;
static constexpr uintptr_t va_aObject_children = 0x0060ea40;
static constexpr uintptr_t va_aObject_child = 0x0060f4a0;
static constexpr uintptr_t va_aObject_checkSmackerPalette = 0x0060a840;
static constexpr uintptr_t va_aObject_bottom = 0x0060a820;
static constexpr uintptr_t va_aObject_background = 0x0060fec0;
static constexpr uintptr_t va_aObject_backColor = 0x0060eb00;
static constexpr uintptr_t va_aObject_animation = 0x0060fea0;
static constexpr uintptr_t va_aObject_addChild = 0x0060f1f0;
static constexpr uintptr_t va_aObject_vftable = 0x0077add4;
static constexpr uintptr_t va_aObject_vector_deleting_destructor = 0x0060e4a0;
#pragma endregion

#pragma region Layout asserts
ASSERT_SIZE(aObject, 0x4AC);
ASSERT_ALIGN(aObject, 0x4);
ASSERT_FIELD_OFFSET(aObject, vtable, 0x00);
ASSERT_FIELD_OFFSET(aObject, force_45_pixels_maybe, 0x04);
ASSERT_FIELD_OFFSET(aObject, panel, 0x08);
ASSERT_FIELD_OFFSET(aObject, width, 0x0C);
ASSERT_FIELD_OFFSET(aObject, height, 0x10);
ASSERT_FIELD_OFFSET(aObject, x, 0x14);
ASSERT_FIELD_OFFSET(aObject, y, 0x18);
ASSERT_FIELD_OFFSET(aObject, depth, 0x1C);
ASSERT_FIELD_OFFSET(aObject, field_0x20, 0x20);
ASSERT_FIELD_OFFSET(aObject, field_0x24, 0x24);
ASSERT_FIELD_OFFSET(aObject, field_0x28, 0x28);
ASSERT_FIELD_OFFSET(aObject, field_0x2C, 0x2C);
ASSERT_FIELD_OFFSET(aObject, field_0x30, 0x30);
ASSERT_FIELD_OFFSET(aObject, field_0x34, 0x34);
ASSERT_FIELD_OFFSET(aObject, field_0x38, 0x38);
ASSERT_FIELD_OFFSET(aObject, field_0x3C, 0x3C);
ASSERT_FIELD_OFFSET(aObject, field_0x40, 0x40);
ASSERT_FIELD_OFFSET(aObject, field_0x44, 0x44);
ASSERT_FIELD_OFFSET(aObject, field_0x48, 0x48);
ASSERT_FIELD_OFFSET(aObject, field_0x4C, 0x4C);
ASSERT_FIELD_OFFSET(aObject, state, 0x50);
ASSERT_FIELD_OFFSET(aObject, showing, 0x54);
ASSERT_FIELD_OFFSET(aObject, initialization_state, 0x58);
ASSERT_FIELD_OFFSET(aObject, hidden, 0x5C);
ASSERT_FIELD_OFFSET(aObject, some_x, 0x60);
ASSERT_FIELD_OFFSET(aObject, hide_direction, 0x64);
ASSERT_FIELD_OFFSET(aObject, field_0x68, 0x68);
ASSERT_FIELD_OFFSET(aObject, field_0x6C, 0x6C);
ASSERT_FIELD_OFFSET(aObject, transparent, 0x70);
ASSERT_FIELD_OFFSET(aObject, children, 0x74);
ASSERT_FIELD_OFFSET(aObject, children_num, 0x470);
ASSERT_FIELD_OFFSET(aObject, parent, 0x474);
ASSERT_FIELD_OFFSET(aObject, background, 0x478);
ASSERT_FIELD_OFFSET(aObject, icon, 0x47C);
ASSERT_FIELD_OFFSET(aObject, animation, 0x480);
ASSERT_FIELD_OFFSET(aObject, drag_start_x, 0x484);
ASSERT_FIELD_OFFSET(aObject, drag_start_y, 0x488);
ASSERT_FIELD_OFFSET(aObject, dragging, 0x48C);
ASSERT_FIELD_OFFSET(aObject, animation_playing, 0x490);
ASSERT_FIELD_OFFSET(aObject, back_color, 0x494);
ASSERT_FIELD_OFFSET(aObject, port, 0x498);
ASSERT_FIELD_OFFSET(aObject, paint_routine, 0x49C);
ASSERT_FIELD_OFFSET(aObject, field_0x4A0, 0x4A0);
ASSERT_FIELD_OFFSET(aObject, drop_target_num, 0x4A4);
ASSERT_FIELD_OFFSET(aObject, field_0x4a8_maybe_rect_arr, 0x4A8);
#pragma endregion