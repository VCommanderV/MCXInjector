#pragma once
#include <cstdint>
#include <cstddef>
#include "mcx_inject.hpp"
#include "mcx/gui/a_port.hpp"
#include "mcx/lib/vfx.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <objbase.h>   // or <objectbase.h>?

#define DIRECTDRAW_VERSION 0x0700
#include <ddraw.h>

#ifndef ENABLE_LOG_ASYSTEM
#define ENABLE_LOG_ASYSTEM 0
#endif

#ifndef ENABLE_LOG_AOBJECT
#define ENABLE_LOG_AOBJECT 0
#endif

#ifndef NO_ERR
#define NO_ERR	0
#endif

static constexpr auto MAX_CHILDREN = 255;

class aObject;

class aEvent
{
public:
    uint8_t event_id;
    aObject* sender;
    int32_t mouse_x;
    int32_t mouse_y;
    uint8_t mouse_left;
    uint8_t mouse_right;
    uint8_t special_flag;
    uint8_t alt_pressed;
    uint8_t ctrl_pressed;
    uint8_t shift_pressed;
    uint8_t orig_mask;
};

static constexpr uint8_t EVT_MOUSE_DOWN = 0x1;

using event_routine_t = void(__cdecl*)(class aObject*, aEvent*);
using paint_routine_t = void(__cdecl*)(class aObject*);

enum aObjectState
{
    NORMAL = 0,
    MAXIMIZED = 1,
    ICONIZED = 2,
};

class aObject;
class aAnimation;
class aTimeManager;

struct aObject_vtable
{
    void(__thiscall* vector_deleting_destructor)(unsigned int);
    void(__thiscall* init)();
    void(__thiscall* destroy)();
    int32_t(__thiscall* getWidth)(aObject*); // width
    int32_t(__thiscall* getHeight)(aObject*);
    int32_t(__thiscall* getX)(aObject*);
    int32_t(__thiscall* getY)(aObject*);
    void(__thiscall* moveTo)(aObject*, int32_t, int32_t, int);
    void(__thiscall* resize)();
    void(__thiscall* frame)();
    void(__thiscall* addChild)(aObject*, aObject*);
    void(__thiscall* removeChild)(aObject*, aObject*);
    void(__thiscall* setParent)(aObject*, aObject*);
    int32_t(__thiscall* numberOfChildren)(aObject*);
    long(__thiscall* globalX)(aObject*);
    long(__thiscall* globalY)(aObject*);
    void(__thiscall* draw)(aObject*);
    aObject* (__thiscall* findObject)(aObject*, int32_t, int32_t);
    BOOL(__thiscall* pointInside)(aObject*, int32_t, int32_t);
    BOOL(__thiscall* rectIntersect)(aObject*, int32_t, int32_t, int32_t, int32_t);
    void(__thiscall* rectIntersect2)();
    void(__thiscall* handleEvent)(aObject*, aEvent* evt);
    void(__thiscall* setBackground)();
    void(__thiscall* setBackground2)();
    void(__thiscall* setAnimation)();
    void(__thiscall* startAnimation)();
    void(__thiscall* stopAnimation)();
    void(__thiscall* background)();
    void(__thiscall* animation)();
    void(__thiscall* icon)();
    void(__thiscall* setDepth)();
    void(__thiscall* children)();
    void(__thiscall* FUN_0060ef20)(aObject*, int32_t);
    void(__thiscall* foremostChild)();
    void(__thiscall* depth)();
    void(__thiscall* child)();
    void(__thiscall* dragStartX)();
    void(__thiscall* dragStartY)();
    void(__thiscall* dragging)();
    void(__thiscall* startDrag)();
    void(__thiscall* stopDrag)();
    void(__thiscall* FUN_0060e8c0)();
    void(__thiscall* leave)();
    long(__thiscall* backColor)(aObject*);
    void(__thiscall* setBackColor)();
    void(__thiscall* setIcon)();
    void(__thiscall* setState)();
    void(__thiscall* iconize)();
    void(__thiscall* normalize)();
    void(__thiscall* maximize)();
    void(__thiscall* display)(aObject*);
    void(__thiscall* setPaintRoutine)();
    void(__thiscall* setEventRoutine)();
    void(__thiscall* paint)(aObject*);
    void(__thiscall* ptr)();
    void(__thiscall* startModal)();
    void(__thiscall* stopModal)();
    void(__thiscall* state)();
    void(__thiscall* port)();
    void(__thiscall* setDisplayPort)(aObject*, aPort*);
    void(__thiscall* drawFramed)();
    void(__thiscall* drawBox)(aObject*, RECT);
    void(__thiscall* drawBox2)(aObject*, uint8_t, int32_t, int32_t, int32_t, int32_t);
    void(__thiscall* SetShowing)();
    void(__thiscall* GetCamera)();
    int32_t(__thiscall* IsShowing)(aObject*);
    void(__thiscall* HideMe)();
    int32_t(__thiscall* IsHidden)(aObject*);
    void(__thiscall* SetHideDirection)();
    void(__thiscall* SetTransparent)();
    void(__thiscall* left)();
    void(__thiscall* top)();
    void(__thiscall* right)();
    void(__thiscall* bottom)();
    void(__thiscall* checkSmackerPalette)();
    void(__thiscall* getDropTargetCount)();
    void(__thiscall* getDropTarget)();
};

typedef aObject_vtable aSystem_vtable;
typedef BITMAPINFOHEADER BMI;

struct LogicalPalette {
    WORD palVersion;
    WORD palNumEntries;
    PALETTEENTRY palPalEntry[256];  // Full 256 entries
};

#pragma region aObject
#pragma pack(push, 4)
class aObject {
# pragma region Fields
public:
    aObject_vtable* vtable;
    int32_t force_45_pixels_maybe; // 0x04
    _pane* display_port; // 0x08
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
    int32_t icon_width; // 0x40
    int32_t icon_height; // 0x44
    int32_t field_0x48; // 0x48
    int32_t field_0x4C; // 0x4C
    aObjectState state; // 0x50
    BOOL showing; // 0x54
    short initialization_state; // 0x58
    char field_0x5A; // 0x5A
    char field_0x5B; // 0x5B
    BOOL hidden; // 0x5C
    int32_t animation_offset; // 0x60 - hide_animation_offset?
    int32_t hide_direction; // 0x64
    int32_t original_x; // 0x68
    int32_t original_y; // 0x6C
    BOOL transparent; // 0x70
    aObject* children[255]; // 0x74
    int32_t children_num; // 0x470
    aObject* parent; // 0x474
    aPort* background_port; // 0x478
    aAnimation* icon; // 0x47C
    aAnimation* animation; // 0x480
    int32_t drag_start_x; // 0x484
    int32_t drag_start_y; // 0x488
    int32_t dragging; // 0x48C
    int32_t animation_playing; // 0x490
    int32_t back_color; // 0x494
    aPort* render_port; // 0x498
    paint_routine_t paint_routine; // 0x49C
    event_routine_t event_routine; // 0x4A0
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

    // public: virtual void __thiscall aObject::destroy(void)
    void destroy();

    // public: virtual int32_t __thiscall aObject::x(void)
    int32_t getX() const;

    // public: virtual int32_t __thiscall aObject::y(void)
    int32_t getY() const;

    // public: virtual int32_t __thiscall aObject::width(void)
    int32_t getWidth() const;

    // public: virtual int32_t __thiscall aObject::height(void)
    int32_t getHeight() const;

    // public: virtual long __thiscall aObject::top(void)
    int32_t getTop();

    // public: virtual long __thiscall aObject::left(void)
    int32_t getLeft();

    // public: virtual int __thiscall aObject::right(void)
    int32_t getRight() const;

    // public: virtual long __thiscall aObject::bottom(void)
    int32_t getBottom() const;

    // public: virtual long __thiscall aObject::globalX(void)
    int32_t getGlobalX() const;

    // public: virtual long __thiscall aObject::globalY(void)
    int32_t getGlobalY() const;

    // public: virtual long __thiscall aObject::state(void)
    int32_t getState() const;

    // public: virtual int __thiscall aObject::IsShowing(void)
    BOOL isShowing() const;

    // public: virtual int __thiscall aObject::IsHidden(void)
    BOOL isHidden() const;

    // public: virtual int __thiscall aObject::dragging(void)
    BOOL isDragging() const;

    // public: virtual long __thiscall aObject::dragStartX(void)
    int32_t getDragStartX() const;

    // public: virtual long __thiscall aObject::dragStartY(void)
    int32_t getDragStartY() const;

    // public: virtual long __thiscall aObject::depth(void)
    int32_t getDepth() const;

    // public: virtual long __thiscall aObject::backColor(void)
    int32_t getBackColor() const;

    // public: virtual long __thiscall aObject::getNumberOfChildren(void)
    int32_t getNumberOfChildren() const;

    // public: virtual aObject * __thiscall aObject::children(void)
    aObject* getChildren() const;

    // public: virtual aPort * __thiscall aObject::port(void)
    aPort* getPort() const;

    // public: virtual aPort * __thiscall aObject::background(void)
    aPort* getBackground() const;

    // public: virtual _pane * __thiscall aObject::frame(void)
    _pane* getFrame() const;

    // public: virtual void * __thiscall aObject::ptr(void)
    void* getPtr() const;

    // public: virtual aAnimation * __thiscall aObject::icon(void)
    aAnimation* getIcon() const;

    // public: virtual Camera * __thiscall aObject::GetCamera(void)
    void* getCamera() const;

    // public: virtual long __thiscall aObject::getDropTargetCount(void)
    int32_t getDropTargetCount() const;

    // public: virtual void __thiscall aObject::addChild(class aObject *)
    void addChild(aObject* child);

    // public: virtual class aObject * __thiscall aObject::child(long)
    aObject* child(long child_idx);

    // public: virtual class aAnimation * __thiscall aObject::animation(void)
    aAnimation* getAnimation();

    // public: virtual void __thiscall aObject::display(void)
    void display();

    // public: virtual void __thiscall aObject::draw(void)
    void draw();

    // public: virtual class aObject * __thiscall aObject::findObject(long,long)
    aObject* findObject(long pos_x, long pos_y);

    // public: virtual class aObject * __thiscall aObject::foremostChild(long) */
    aObject* foremostChild(long find_depth);

    // public: virtual void __thiscall aObject::HideMe(int)
    void HideMe(BOOL hide);

    // public: virtual void __thiscall aObject::moveTo(long,long,int)
    void moveTo(int32_t new_x, int32_t new_y, int flag);

    // public: virtual void __thiscall aObject::drawBox(unsigned char,struct tagRECT)
    void drawBox(uint8_t color, RECT rect);

    // public: virtual void __thiscall aObject::drawBox(unsigned char,long,long,long,long)
    void drawBox2(uint8_t color, int32_t box_x, int32_t box_y, int32_t box_w, int32_t box_h);

    // public: virtual void __thiscall aObject::drawFramed(int,int)
    void drawFramed(BOOL inverted_frame, BOOL param_2);

    // public: void __thiscall aObject::FillBox(short,short,short,short,unsigned char)
    void FillBox(short left, short top, short right, short bottom, uint8_t color);

    // public: virtual long __thiscall aObject::setAnimation(char *)
    long setAnimation(char* filename);

    // public: virtual long __thiscall aObject::setBackground(char *)
    long setBackground(char* filename);

    // public: virtual long __thiscall aObject::setBackground(long)
    long setBackground2(long art_packet_idx);

    // public: void __thiscall aObject::SetBit(long,long,unsigned char)
    void SetBit(long x_, long y_, uint8_t bit);

    // public: virtual void __thiscall aObject::setDepth(long)
    void setDepth(long new_depth);

    // public: virtual void __thiscall aObject::setDisplayPort(class aPort *)
    void setDisplayPort(aPort* new_display_port);

    // public: virtual void __thiscall aObject::setEventRoutine(void (__cdecl*)(class aObject *,class aEvent*))
    void setEventRoutine(event_routine_t new_event_routine);

    // void __thiscall aObject::handleEvent(aObject *this,aEvent *param_1)
    void handleEvent(aEvent* evt);

    // public: virtual long __thiscall aObject::setIcon(char *)
    long setIcon(char* filename);

    // public: virtual void __thiscall aObject::paint(void)
    void paint();
};
#pragma pack(pop)
#pragma endregion

#pragma region aSystem
class aSystem : aObject
{
#pragma region Fields
public:
    IDirectDraw2* ddraw2;                      // 0x0
    IDirectDraw* ddraw1;                       // 0x4
    IDirectDrawSurface* ddraw_surf1;           // 0x4b4
    IDirectDrawSurface* ddraw_attached_surf;   // 0x4b8
    int32_t field_0x4c0; // 0x4bc
    IDirectDrawPalette* ddraw_palette;         // 0x4c0
    int32_t field_0x4c4; // 0x4c4
    int32_t field_0x4c8; // 0x4c8
    int32_t field_0x4cc; // 0x4cc
    int32_t field_0x4d0; // 0x4d0
    int32_t field_0x4d4; // 0x4d4
    int32_t field_0x4d8; // 0x4d8
    int32_t field_0x4dc; // 0x4dc
    int32_t field_0x4e0; // 0x4e0
    int32_t field_0x4e4; // 0x4e4
    int32_t field_0x4e8; // 0x4e8
    int32_t field_0x4ec; // 0x4ec
    int32_t field_0x4f0; // 0x4f0
    int32_t field_0x4f4; // 0x4f4
    int32_t field_0x4f8; // 0x4f8
    int32_t field_0x4fc; // 0x4fc
    int32_t field_0x500; // 0x500
    int32_t field_0x504; // 0x504
    int32_t field_0x508; // 0x508
    int32_t field_0x50c; // 0x50c
    int32_t field_0x510; // 0x510
    int32_t field_0x514; // 0x514
    int32_t field_0x518; // 0x518
    int32_t field_0x51c; // 0x51c
    int32_t field_0x520; // 0x520
    int32_t field_0x524; // 0x524
    int32_t field_0x528; // 0x528
    int32_t field_0x52c; // 0x52c
    int32_t field_0x530; // 0x530
    int32_t field_0x534; // 0x534
    int32_t field_0x538; // 0x538
    int32_t field_0x53c; // 0x53c
    int32_t field_0x540; // 0x540
    int32_t field_0x544; // 0x544
    int32_t field_0x548; // 0x548
    int32_t field_0x54c; // 0x54c
    int32_t field_0x550; // 0x550
    int32_t field_0x554; // 0x554
    int32_t field_0x558; // 0x558
    int32_t field_0x55c; // 0x55c
    int32_t field_0x560; // 0x560
    int32_t field_0x564; // 0x564
    int32_t field_0x568; // 0x568
    int32_t field_0x56c; // 0x56c
    int32_t field_0x570; // 0x570
    int32_t field_0x574; // 0x574
    int32_t field_0x578; // 0x578
    int32_t field_0x57c; // 0x57c
    int32_t field_0x580; // 0x580
    int32_t field_0x584; // 0x584
    int32_t field_0x588; // 0x588
    int32_t field_0x58c; // 0x58c
    int32_t field_0x590; // 0x590
    int32_t field_0x594; // 0x594
    int32_t field_0x598; // 0x598
    int32_t field_0x59c; // 0x59c
    int32_t field_0x5a0; // 0x5a0
    int32_t field_0x5a4; // 0x5a4
    int32_t field_0x5a8; // 0x5a8
    int32_t field_0x5ac; // 0x5ac
    int32_t field_0x5b0; // 0x5b0
    int32_t field_0x5b4; // 0x5b4
    int32_t field_0x5b8; // 0x5b8
    int32_t field_0x5bc; // 0x5bc
    int32_t field_0x5c0; // 0x5c0
    int32_t field_0x5c4; // 0x5c4
    int32_t field_0x5c8; // 0x5c8
    int32_t field_0x5cc; // 0x5cc
    int32_t field_0x5d0; // 0x5d0
    int32_t field_0x5d4; // 0x5d4
    int32_t field_0x5d8; // 0x5d8
    int32_t field_0x5dc; // 0x5dc
    int32_t field_0x5e0; // 0x5e0
    int32_t field_0x5e4; // 0x5e4
    int32_t field_0x5e8; // 0x5e8
    int32_t field_0x5ec; // 0x5ec
    int32_t field_0x5f0; // 0x5f0
    int32_t field_0x5f4; // 0x5f4
    int32_t field_0x5f8; // 0x5f8
    int32_t field_0x5fc; // 0x5fc
    int32_t field_0x600; // 0x600
    int32_t field_0x604; // 0x604
    int32_t field_0x608; // 0x608
    int32_t field_0x60c; // 0x60c
    int32_t field_0x610; // 0x610
    int32_t field_0x614; // 0x614
    int32_t field_0x618; // 0x618
    int32_t field_0x61c; // 0x61c
    int32_t field_0x620; // 0x620
    int32_t field_0x624; // 0x624
    int32_t field_0x628; // 0x628
    int32_t field_0x62c; // 0x62c
    int32_t field_0x630; // 0x630
    int32_t field_0x634; // 0x634
    int32_t field_0x638; // 0x638
    int32_t field_0x63c; // 0x63c
    int32_t field_0x640; // 0x640
    int32_t field_0x644; // 0x644
    int32_t field_0x648; // 0x648
    int32_t field_0x64c; // 0x64c
    int32_t field_0x650; // 0x650
    int32_t field_0x654; // 0x654
    int32_t field_0x658; // 0x658
    int32_t field_0x65c; // 0x65c
    int32_t field_0x660; // 0x660
    int32_t field_0x664; // 0x664
    int32_t field_0x668; // 0x668
    int32_t field_0x66c; // 0x66c
    int32_t field_0x670; // 0x670
    int32_t field_0x674; // 0x674
    int32_t field_0x678; // 0x678
    int32_t field_0x67c; // 0x67c
    int32_t field_0x680; // 0x680
    int32_t field_0x684; // 0x684
    int32_t field_0x688; // 0x688
    int32_t field_0x68c; // 0x68c
    int32_t field_0x690; // 0x690
    int32_t field_0x694; // 0x694
    int32_t field_0x698; // 0x698
    int32_t field_0x69c; // 0x69c
    int32_t field_0x6a0; // 0x6a0
    int32_t field_0x6a4; // 0x6a4
    int32_t field_0x6a8; // 0x6a8
    int32_t field_0x6ac; // 0x6ac
    int32_t field_0x6b0; // 0x6b0
    int32_t field_0x6b4; // 0x6b4
    int32_t field_0x6b8; // 0x6b8
    int32_t field_0x6bc; // 0x6bc
    int32_t field_0x6c0; // 0x6c0
    int32_t field_0x6c4; // 0x6c4
    int32_t field_0x6c8; // 0x6c8
    int32_t field_0x6cc; // 0x6cc
    int32_t field_0x6d0; // 0x6d0
    int32_t field_0x6d4; // 0x6d4
    int32_t field_0x6d8; // 0x6d8
    int32_t field_0x6dc; // 0x6dc
    int32_t field_0x6e0; // 0x6e0
    int32_t field_0x6e4; // 0x6e4
    int32_t field_0x6e8; // 0x6e8
    int32_t field_0x6ec; // 0x6ec
    int32_t field_0x6f0; // 0x6f0
    int32_t field_0x6f4; // 0x6f4
    int32_t field_0x6f8; // 0x6f8
    int32_t field_0x6fc; // 0x6fc
    int32_t field_0x700; // 0x700
    int32_t field_0x704; // 0x704
    int32_t field_0x708; // 0x708
    int32_t field_0x70c; // 0x70c
    int32_t field_0x710; // 0x710
    int32_t field_0x714; // 0x714
    int32_t field_0x718; // 0x718
    int32_t field_0x71c; // 0x71c
    int32_t field_0x720; // 0x720
    int32_t field_0x724; // 0x724
    int32_t field_0x728; // 0x728
    int32_t field_0x72c; // 0x72c
    int32_t field_0x730; // 0x730
    int32_t field_0x734; // 0x734
    int32_t field_0x738; // 0x738
    int32_t field_0x73c; // 0x73c
    int32_t field_0x740; // 0x740
    int32_t field_0x744; // 0x744
    int32_t field_0x748; // 0x748
    int32_t field_0x74c; // 0x74c
    int32_t field_0x750; // 0x750
    int32_t field_0x754; // 0x754
    int32_t field_0x758; // 0x758
    int32_t field_0x75c; // 0x75c
    int32_t field_0x760; // 0x760
    int32_t field_0x764; // 0x764
    int32_t field_0x768; // 0x768
    int32_t field_0x76c; // 0x76c
    int32_t field_0x770; // 0x770
    int32_t field_0x774; // 0x774
    int32_t field_0x778; // 0x778
    int32_t field_0x77c; // 0x77c
    int32_t field_0x780; // 0x780
    int32_t field_0x784; // 0x784
    int32_t field_0x788; // 0x788
    int32_t field_0x78c; // 0x78c
    int32_t field_0x790; // 0x790
    int32_t field_0x794; // 0x794
    int32_t field_0x798; // 0x798
    int32_t field_0x79c; // 0x79c
    int32_t field_0x7a0; // 0x7a0
    int32_t field_0x7a4; // 0x7a4
    int32_t field_0x7a8; // 0x7a8
    int32_t field_0x7ac; // 0x7ac
    int32_t field_0x7b0; // 0x7b0
    int32_t field_0x7b4; // 0x7b4
    int32_t field_0x7b8; // 0x7b8
    int32_t field_0x7bc; // 0x7bc
    int32_t field_0x7c0; // 0x7c0
    int32_t field_0x7c4; // 0x7c4
    int32_t field_0x7c8; // 0x7c8
    int32_t field_0x7cc; // 0x7cc
    int32_t field_0x7d0; // 0x7d0
    int32_t field_0x7d4; // 0x7d4
    int32_t field_0x7d8; // 0x7d8
    int32_t field_0x7dc; // 0x7dc
    int32_t field_0x7e0; // 0x7e0
    int32_t field_0x7e4; // 0x7e4
    int32_t field_0x7e8; // 0x7e8
    int32_t field_0x7ec; // 0x7ec
    int32_t field_0x7f0; // 0x7f0
    int32_t field_0x7f4; // 0x7f4
    int32_t field_0x7f8; // 0x7f8
    int32_t field_0x7fc; // 0x7fc
    int32_t field_0x800; // 0x800
    int32_t field_0x804; // 0x804
    int32_t field_0x808; // 0x808
    int32_t field_0x80c; // 0x80c
    int32_t field_0x810; // 0x810
    int32_t field_0x814; // 0x814
    int32_t field_0x818; // 0x818
    int32_t field_0x81c; // 0x81c
    int32_t field_0x820; // 0x820
    int32_t field_0x824; // 0x824
    int32_t field_0x828; // 0x828
    int32_t field_0x82c; // 0x82c
    int32_t field_0x830; // 0x830
    int32_t field_0x834; // 0x834
    int32_t field_0x838; // 0x838
    int32_t field_0x83c; // 0x83c
    int32_t field_0x840; // 0x840
    int32_t field_0x844; // 0x844
    int32_t field_0x848; // 0x848
    int32_t field_0x84c; // 0x84c
    int32_t field_0x850; // 0x850
    int32_t field_0x854; // 0x854
    int32_t field_0x858; // 0x858
    int32_t field_0x85c; // 0x85c
    int32_t field_0x860; // 0x860
    int32_t field_0x864; // 0x864
    int32_t field_0x868; // 0x868
    int32_t field_0x86c; // 0x86c
    int32_t field_0x870; // 0x870
    int32_t field_0x874; // 0x874
    int32_t field_0x878; // 0x878
    int32_t field_0x87c; // 0x87c
    int32_t field_0x880; // 0x880
    int32_t field_0x884; // 0x884
    int32_t field_0x888; // 0x888
    int32_t field_0x88c; // 0x88c
    int32_t field_0x890; // 0x890
    int32_t field_0x894; // 0x894
    int32_t field_0x898; // 0x898
    int32_t field_0x89c; // 0x89c
    int32_t field_0x8a0; // 0x8a0
    int32_t field_0x8a4; // 0x8a4
    int32_t field_0x8a8; // 0x8a8
    int32_t field_0x8ac; // 0x8ac
    int32_t field_0x8b0; // 0x8b0
    int32_t field_0x8b4; // 0x8b4
    int32_t field_0x8b8; // 0x8b8
    int32_t field_0x8bc; // 0x8bc
    int32_t field_0x8c0; // 0x8c0
    int32_t field_0x8c4; // 0x8c4
    int32_t field_0x8c8; // 0x8c8
    int32_t field_0x8cc; // 0x8cc
    int32_t field_0x8d0; // 0x8d0
    int32_t field_0x8d4; // 0x8d4
    int32_t field_0x8d8; // 0x8d8
    int32_t field_0x8dc; // 0x8dc
    int32_t field_0x8e0; // 0x8e0
    int32_t field_0x8e4; // 0x8e4
    int32_t field_0x8e8; // 0x8e8
    int32_t field_0x8ec; // 0x8ec
    int32_t field_0x8f0; // 0x8f0
    int32_t field_0x8f4; // 0x8f4
    int32_t field_0x8f8; // 0x8f8
    int32_t field_0x8fc; // 0x8fc
    int32_t field_0x900; // 0x900
    int32_t field_0x904; // 0x904
    int32_t field_0x908; // 0x908
    HWND window_handle; // 0x90c
    int32_t field_0x910; // 0x910
    int32_t field_0x914; // 0x914
    int32_t field_0x918; // 0x918
    int32_t field_0x91c; // 0x91c
    int32_t field_0x920; // 0x920
    int32_t field_0x924; // 0x924
    int32_t field_0x928; // 0x928
    int32_t field_0x92c; // 0x92c
    int32_t field_0x930; // 0x930
    int32_t field_0x934; // 0x934
    int32_t field_0x938; // 0x938
    int32_t field_0x93c; // 0x93c
    int32_t field_0x940; // 0x940
    int32_t field_0x944; // 0x944
    int32_t field_0x948; // 0x948
    int32_t field_0x94c; // 0x94c
    int32_t field_0x950; // 0x950
    int32_t field_0x954; // 0x954
    int32_t field_0x958; // 0x958
    int32_t field_0x95c; // 0x95c
    int32_t field_0x960; // 0x960
    int32_t field_0x964; // 0x964
    int32_t field_0x968; // 0x968
    int32_t field_0x96c; // 0x96c
    int32_t field_0x970; // 0x970
    int32_t field_0x974; // 0x974
    int32_t field_0x978; // 0x978
    int32_t field_0x97c; // 0x97c
    int32_t field_0x980; // 0x980
    int32_t field_0x984; // 0x984
    int32_t field_0x988; // 0x988
    int32_t field_0x98c; // 0x98c
    int32_t field_0x990; // 0x990
    int32_t field_0x994; // 0x994
    int32_t field_0x998; // 0x998
    int32_t field_0x99c; // 0x99c
    int32_t field_0x9a0; // 0x9a0
    int32_t field_0x9a4; // 0x9a4
    int32_t field_0x9a8; // 0x9a8
    int32_t field_0x9ac; // 0x9ac
    int32_t field_0x9b0; // 0x9b0
    int32_t field_0x9b4; // 0x9b4
    int32_t field_0x9b8; // 0x9b8
    int32_t field_0x9bc; // 0x9bc
    int32_t field_0x9c0; // 0x9c0
    int32_t field_0x9c4; // 0x9c4
    int32_t field_0x9c8; // 0x9c8
    int32_t field_0x9cc; // 0x9cc
    int32_t field_0x9d0; // 0x9d0
    int32_t field_0x9d4; // 0x9d4
    int32_t field_0x9d8; // 0x9d8
    int32_t field_0x9dc; // 0x9dc
    int32_t field_0x9e0; // 0x9e0
    int32_t field_0x9e4; // 0x9e4
    int32_t field_0x9e8; // 0x9e8
    int32_t field_0x9ec; // 0x9ec
    int32_t field_0x9f0; // 0x9f0
    int32_t field_0x9f4; // 0x9f4
    int32_t field_0x9f8; // 0x9f8
    int32_t field_0x9fc; // 0x9fc
    int32_t field_0xa00; // 0xa00
    int32_t field_0xa04; // 0xa04
    int32_t field_0xa08; // 0xa08
    int32_t field_0xa0c; // 0xa0c
    int32_t field_0xa10; // 0xa10
    int32_t field_0xa14; // 0xa14
    int32_t field_0xa18; // 0xa18
    int32_t field_0xa1c; // 0xa1c
    int32_t field_0xa20; // 0xa20
    int32_t field_0xa24; // 0xa24
    int32_t field_0xa28; // 0xa28
    int32_t field_0xa2c; // 0xa2c
    int32_t field_0xa30; // 0xa30
    int32_t field_0xa34; // 0xa34
    int32_t field_0xa38; // 0xa38
    int32_t field_0xa3c; // 0xa3c
    int32_t field_0xa40; // 0xa40
    int32_t field_0xa44; // 0xa44
    int32_t field_0xa48; // 0xa48
    int32_t field_0xa4c; // 0xa4c
    int32_t field_0xa50; // 0xa50
    int32_t field_0xa54; // 0xa54
    int32_t field_0xa58; // 0xa58
    int32_t field_0xa5c; // 0xa5c
    int32_t field_0xa60; // 0xa60
    int32_t field_0xa64; // 0xa64
    int32_t field_0xa68; // 0xa68
    int32_t field_0xa6c; // 0xa6c
    int32_t field_0xa70; // 0xa70
    int32_t field_0xa74; // 0xa74
    int32_t field_0xa78; // 0xa78
    int32_t field_0xa7c; // 0xa7c
    int32_t field_0xa80; // 0xa80
    int32_t field_0xa84; // 0xa84
    int32_t field_0xa88; // 0xa88
    int32_t field_0xa8c; // 0xa8c
    int32_t field_0xa90; // 0xa90
    int32_t field_0xa94; // 0xa94
    int32_t field_0xa98; // 0xa98
    int32_t field_0xa9c; // 0xa9c
    int32_t field_0xaa0; // 0xaa0
    int32_t field_0xaa4; // 0xaa4
    int32_t field_0xaa8; // 0xaa8
    int32_t field_0xaac; // 0xaac
    int32_t field_0xab0; // 0xab0
    int32_t field_0xab4; // 0xab4
    int32_t field_0xab8; // 0xab8
    int32_t field_0xabc; // 0xabc
    int32_t field_0xac0; // 0xac0
    int32_t field_0xac4; // 0xac4
    int32_t field_0xac8; // 0xac8
    int32_t field_0xacc; // 0xacc
    int32_t field_0xad0; // 0xad0
    int32_t field_0xad4; // 0xad4
    int32_t field_0xad8; // 0xad8
    int32_t field_0xadc; // 0xadc
    int32_t field_0xae0; // 0xae0
    int32_t field_0xae4; // 0xae4
    int32_t field_0xae8; // 0xae8
    int32_t field_0xaec; // 0xaec
    int32_t field_0xaf0; // 0xaf0
    int32_t field_0xaf4; // 0xaf4
    int32_t field_0xaf8; // 0xaf8
    int32_t field_0xafc; // 0xafc
    int32_t field_0xb00; // 0xb00
    int32_t field_0xb04; // 0xb04
    int32_t field_0xb08; // 0xb08
#pragma endregion
public:
    // public: long __thiscall aSystem::startupDirectDraw(long,long,long)
    long startupDirectDraw(long width, long height, long bits_per_pixel);

    // public: int __thiscall aSystem::tweakDDPalette(int,int,struct VFX_RGB *,int)
    int tweakDDPalette(int pal_first_entry, int pal_entries_to_change, VFX_RGB* pal, int param_4);
};
#pragma endregion

#pragma region VAs
static constexpr uintptr_t va_aObject_ctor = 0x0060e440;
static constexpr uintptr_t va_aObject_dtor = 0x0060e4d0;
static constexpr uintptr_t va_aObject_y = 0x0060f520;
static constexpr uintptr_t va_aObject_x = 0x0060f510;
static constexpr uintptr_t va_aObject_width = 0x0060f4c0;
static constexpr uintptr_t va_aObject_height = 0x0060f4d0;
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
static constexpr uintptr_t va_aObject_setBackground = 0x0060fb20;
static constexpr uintptr_t va_aObject_setBackground2 = 0x0060fba0;
static constexpr uintptr_t va_aObject_setBackColor = 0x0060eaf0;
static constexpr uintptr_t va_aObject_setAnimation = 0x0060fda0;
static constexpr uintptr_t va_aObject_resize = 0x0060f650;
static constexpr uintptr_t va_aObject_removeChild = 0x0060f2a0;
static constexpr uintptr_t va_aObject_rectIntersect1 = 0x0060e950;
static constexpr uintptr_t va_aObject_rectIntersect2 = 0x0060e910;
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
static constexpr uintptr_t va_aObject_right = 0x0060a800;
static constexpr uintptr_t va_aObject_leave = 0x0060e8d0;
static constexpr uintptr_t va_aObject_IsShowing = 0x0060a7a0;
static constexpr uintptr_t va_aObject_IsHidden = 0x0060a7b0;
static constexpr uintptr_t va_aObject_init = 0x0060e520;
static constexpr uintptr_t va_aObject_iconize = 0x0060fc40;
static constexpr uintptr_t va_aObject_icon = 0x0060feb0;
static constexpr uintptr_t va_aObject_HideMe = 0x0060ff30;
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
static constexpr uintptr_t va_aObject_drawBox = 0x0060a750;
static constexpr uintptr_t va_aObject_drawBox2 = 0x0060eb10;
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

static constexpr uintptr_t va_aSystem_vftable = 0x0077af08;
static constexpr uintptr_t va_aSystem_startupDirectDraw = 0x0060db20;
static constexpr uintptr_t va_aSystem_tweakDDPalette = 0x00614a70;

static constexpr uintptr_t va_bmi = 0x007a9f90;
static constexpr uintptr_t va_aRedrawScreen = 0x00610410;
static constexpr uintptr_t va_screenWindow = 0x007ab154;
static constexpr uintptr_t va_logicalPalette = 0x007aa3c0;
static constexpr uintptr_t va_DesktopDC = 0x007aae7c;
static constexpr uintptr_t va_OffscreenBufferDC = 0x007aae70;
static constexpr uintptr_t va_hPalette = 0x007aae84;
static constexpr uintptr_t va_holdPalette = 0x007aa990;
static constexpr uintptr_t va_OffScreenhDIBSection = 0x007aae74;
static constexpr uintptr_t va_OffScreenhOldBitmap = 0x007aadf0;

static constexpr uintptr_t va_FullScreen = 0x007ab0e4;
static constexpr uintptr_t va_Width = 0x00789f70;
static constexpr uintptr_t va_Height = 0x00789f74;
static constexpr uintptr_t va_DAT_007ab0d4 = 0x007ab0d4;

static constexpr uintptr_t va_DAT_007ab104 = 0x007ab104;
static constexpr uintptr_t va_DAT_007ab108 = 0x007ab108;
static constexpr uintptr_t va_SystemPalette = 0x007aa3c4; // va_DAT_007aa3c4
#pragma endregion

#pragma region Globals
OBJ_REF(BOOL, g_FullScreen, va_FullScreen);
OBJ_REF(int32_t, g_Width, va_Width);
OBJ_REF(int32_t, g_Height, va_Height);
PTR_REF(aObject, g_screenWindow, va_screenWindow);
OBJ_REF(BMI, g_bmi, va_bmi);
OBJ_REF(LogicalPalette, g_logicalPalette, va_logicalPalette);
OBJ_REF(HDC, g_DesktopDC, va_DesktopDC);
OBJ_REF(HDC, g_OffscreenBufferDC, va_OffscreenBufferDC);
OBJ_REF(HPALETTE, g_hPalette, va_hPalette);
OBJ_REF(HPALETTE, g_holdPalette, va_holdPalette);
OBJ_REF(HBITMAP, g_OffScreenhDIBSection, va_OffScreenhDIBSection);
OBJ_REF(HBITMAP, g_OffScreenhOldBitmap, va_OffScreenhOldBitmap);
OBJ_REF(uint32_t, g_DAT_007ab108, va_DAT_007ab108);
OBJ_REF(int32_t, g_DAT_007ab0d4, va_DAT_007ab0d4);
ARR_REF(PALETTEENTRY, 256, g_SystemPalette, va_SystemPalette);
PTR_REF(void, g_DIBBits, va_DAT_007ab104);

static void __cdecl aRedrawScreen();
#pragma endregion

#pragma region Layout asserts
ASSERT_SIZE(aObject, 0x4AC);
ASSERT_ALIGN(aObject, 0x4);
ASSERT_FIELD_OFFSET(aObject, vtable, 0x00);
ASSERT_FIELD_OFFSET(aObject, force_45_pixels_maybe, 0x04);
ASSERT_FIELD_OFFSET(aObject, display_port, 0x08);
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
ASSERT_FIELD_OFFSET(aObject, icon_width, 0x40);
ASSERT_FIELD_OFFSET(aObject, icon_height, 0x44);
ASSERT_FIELD_OFFSET(aObject, field_0x48, 0x48);
ASSERT_FIELD_OFFSET(aObject, field_0x4C, 0x4C);
ASSERT_FIELD_OFFSET(aObject, state, 0x50);
ASSERT_FIELD_OFFSET(aObject, showing, 0x54);
ASSERT_FIELD_OFFSET(aObject, initialization_state, 0x58);
ASSERT_FIELD_OFFSET(aObject, hidden, 0x5C);
ASSERT_FIELD_OFFSET(aObject, animation_offset, 0x60);
ASSERT_FIELD_OFFSET(aObject, hide_direction, 0x64);
ASSERT_FIELD_OFFSET(aObject, original_x, 0x68);
ASSERT_FIELD_OFFSET(aObject, original_y, 0x6C);
ASSERT_FIELD_OFFSET(aObject, transparent, 0x70);
ASSERT_FIELD_OFFSET(aObject, children, 0x74);
ASSERT_FIELD_OFFSET(aObject, children_num, 0x470);
ASSERT_FIELD_OFFSET(aObject, parent, 0x474);
ASSERT_FIELD_OFFSET(aObject, background_port, 0x478);
ASSERT_FIELD_OFFSET(aObject, icon, 0x47C);
ASSERT_FIELD_OFFSET(aObject, animation, 0x480);
ASSERT_FIELD_OFFSET(aObject, drag_start_x, 0x484);
ASSERT_FIELD_OFFSET(aObject, drag_start_y, 0x488);
ASSERT_FIELD_OFFSET(aObject, dragging, 0x48C);
ASSERT_FIELD_OFFSET(aObject, animation_playing, 0x490);
ASSERT_FIELD_OFFSET(aObject, back_color, 0x494);
ASSERT_FIELD_OFFSET(aObject, render_port, 0x498);
ASSERT_FIELD_OFFSET(aObject, paint_routine, 0x49C);
ASSERT_FIELD_OFFSET(aObject, event_routine, 0x4A0);
ASSERT_FIELD_OFFSET(aObject, drop_target_num, 0x4A4);
ASSERT_FIELD_OFFSET(aObject, field_0x4a8_maybe_rect_arr, 0x4A8);
#pragma endregion