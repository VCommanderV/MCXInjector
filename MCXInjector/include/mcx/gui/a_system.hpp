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

#ifndef ENABLE_LOG_ATIMERMANAGER
#define ENABLE_LOG_ATIMERMANAGER 0
#endif

#ifndef NO_ERR
#define NO_ERR	0
#endif

static constexpr auto MAX_CHILDREN = 255;

// In a windows 256 color palette:
//  0–9:     system colors (low indices)
//  246–255: system colors (high indices)
//  10–245:  free/application defined colors
//
// See: 
static constexpr uint32_t DDPAL_FIRST_NON_SYSTEM_COLOR = 10;
static constexpr uint32_t DDPAL_NUM_NON_SYSTEM_COLOR = 256 - 20;

enum Processor : int
{
    PROC_UNSUPPORTED     = 0x0, // 386 or 486?
    PROC_PENTIUM_BASIC   = 0x1, // Pentium (no MMX)
    PROC_PENTIUM_MMX     = 0x2, // Pentium MMX or later
    PROC_PENTIUM_NOCPUID = 0x3  // Pentium without working CPUID?
};

enum CursorType : int
{
    CURSOR_0 = 0,
    CURSOR_15 = 0xf,
    CURSOR_16 = 0x10,
    CURSOR_17 = 0x11,
    CURSOR_18 = 0x12
};

#pragma pack(push, 1) // ensure no padding
struct TGAHeader
{
    uint8_t  idLength;           // 0: Length of image ID field
    uint8_t  colorMapType;       // 1: 0 = no palette, 1 = palette
    uint8_t  imageType;          // 2: 1 = uncompressed color-mapped, 2 = true-color, 3 = grayscale
    uint16_t colorMapFirstEntry; // 3-4: Index of first palette entry
    uint16_t colorMapLength;     // 5-6: Number of palette entries
    uint8_t  colorMapEntrySize;  // 7: Bits per palette entry (usually 24 or 32)
    uint16_t xOrigin;            // 8-9: X-origin of image
    uint16_t yOrigin;            // 10-11: Y-origin of image
    uint16_t width;              // 12-13: Image width in pixels
    uint16_t height;             // 14-15: Image height in pixels
    uint8_t  pixelDepth;         // 16: Bits per pixel
    uint8_t  imageDescriptor;    // 17: Image descriptor (alpha bits, origin)
};
#pragma pack(pop)

class aObject;

class aEvent
{
public:
    uint8_t event_id;
    aObject* sender;
    uint8_t field_0x8;
    uint8_t field_0x9;
    uint8_t field_0xa;
    uint8_t field_0xb;
    uint8_t field_0xc;
    uint8_t field_0xd;
    uint8_t field_0xe;
    uint8_t padding_0xf;
    uint16_t field_0x10;
    uint16_t field_0x12;
    uint32_t param_1; // 0x14
    uint32_t param_2; // 0x18
    uint32_t field_0x1c;
    uint32_t field_0x20;
    uint32_t field_0x24;
    uint32_t field_0x28;
public:
    void clear();
};

static constexpr uint8_t EVT_MOUSE_DOWN = 0x1;
static constexpr uint8_t EVT_RIGHT_MOUSE_DOWN = 0x3;
static constexpr uint8_t EVT_MOUSE_UP = 0x4;
static constexpr uint8_t EVT_RIGHT_MOUSE_UP = 0x6;
static constexpr uint8_t EVT_MOUSE_MOVE = 0x7;

using event_routine_t = void(__cdecl*)(class aObject*, aEvent*);
using paint_routine_t = void(__cdecl*)(class aObject*);
using callback_func_t = void(__cdecl*)();

enum aObjectState
{
    NORMAL = 0,
    MAXIMIZED = 1,
    ICONIZED = 2,
};

class aObject;
class aAnimation;
class aCallback;

struct aCallback_vtable
{
    void(__thiscall* ExecCallback)(aCallback*);
};

class aCallback
{
public:
    aCallback_vtable* vtable;
    callback_func_t func;
    aObject* owner;
    int32_t event_id;
public:
    static void* operator new(size_t size);

    static void* operator new(size_t size, void* ptr);

    static void operator delete(void* ptr);

    // public: __thiscall aCallback::aCallback(void)
    aCallback();

    // public: __thiscall aCallback::~aCallback(void)
    ~aCallback();

    // public: void __thiscall aCallback::destroy(void)
    void destroy();

    // public: void __thiscall aCallback::setExec(void (__cdecl*)(void))
    void setExec(callback_func_t f);

    // public: virtual void __thiscall aCallback::setMessage(class aObject *,long)
    void setMessage(aObject* owner, int32_t event_id);

    void ExecCallback();
};

static constexpr size_t MAX_TIMERS = 99;
static constexpr size_t MAX_CALLBACKS = 99;

class aTimer
{
public:
    DWORD start_time; // 0x0
    DWORD ticks; // 0x4
    aObject* owner; // 0x8
    short event_id; // 0x1c
    short field_0xxe; // 0xxE
    int32_t event_type; // 0x10
    int32_t event_param; // 0x14
    int32_t use_scenario_time; // 0x18
};

struct aTimerRemoval
{
    aObject* owner;
    int32_t event_id;
};

class aTimerManager
{
public:
    int32_t num_timers;
    int32_t removal_idx;
    aTimer* timers[MAX_TIMERS];
    aTimerRemoval deferred_removals[MAX_TIMERS];
    aCallback* timers_callback;
    int32_t lock_state;
    aTimer* locked_timer;
public:
    // public: __thiscall aTimerManager::aTimerManager(void)
    aTimerManager();

    // public: __thiscall aTimerManager::~aTimerManager(void)
    ~aTimerManager();

    // public: long __thiscall aTimerManager::Init(void)
    long Init();

    // public: long __thiscall aTimerManager::AddTimer(class aObject *,short,unsigned long, long, long, int)
    long AddTimer(aObject* owner, short event_id, uint32_t start_time, int32_t event_type, int32_t event_param, int32_t use_scenario_time);

    // public: long __thiscall aTimerManager::AddUniqueTimer(class aObject *,short,unsigned long, long, long, int)
    long AddUniqueTimer(aObject* owner, short event_id, uint32_t start_time, int32_t event_type, int32_t event_param, int32_t use_scenario_time);
    
    // public: struct aTimer * __thiscall aTimerManager::GetTimer(short)
    aTimer* GetTimer(short timer_idx);

    // public: struct aTimer * __thiscall aTimerManager::GetTimer(class aObject *,short)
    aTimer* GetTimer2(aObject* owner, short event_id);

    // public: void __thiscall aTimerManager::LockTimersExcept(struct aTimer *)
    void LockTimersExcept(aTimer* timer);

    // public: void __thiscall aTimerManager::RemoveTimer(class aObject *,short)
    void RemoveTimer(aObject* owner, short event_id);

    // public: void __thiscall aTimerManager::RemoveTimer(long)
    void RemoveTimer2(int32_t timer_idx);

    // public: void __thiscall aTimerManager::RemoveTimers(class aObject *)
    void RemoveTimers(aObject* owner);

    // public: void __thiscall aTimerManager::UnlockTimers(void)
    void UnlockTimers();
};

struct aObject_vtable
{
    void(__thiscall* vector_deleting_destructor)(aObject*, unsigned int);
    void(__thiscall* init)(aObject*, int32_t, int32_t, int32_t, int32_t, int32_t);
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
    void(__thiscall* setDepth)(aObject*, int32_t);
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
class aSystem : public aObject
{
#pragma region Fields
public:
    IDirectDraw2* ddraw2;                      // 0x0
    IDirectDraw* ddraw1;                       // 0x4
    IDirectDrawSurface* ddraw_surf1;           // 0x4b4
    IDirectDrawSurface* ddraw_attached_surf;   // 0x4b8
    int32_t field_0x4bc; // 0x4bc
    IDirectDrawPalette* ddraw_palette;         // 0x4c0
    int32_t field_0x4c4; // 0x4c4
    int32_t palette_cycle; // 0x4c8
    int32_t flip_to_gdi; // 0x4cc
    VFX_RGB master_palette[256]; // 0x4d0
    int32_t gamma; // 0x7d0 // gamma
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
    int32_t display_width; // 0x8fc
    int32_t display_height; // 0x900
    int32_t screen_offset_x; // 0x904
    int32_t screen_offset_y; // 0x908
    HWND window_handle; // 0x90c
    int32_t num_callbacks; // 0x910
    aCallback* callbacks[MAX_CALLBACKS]; // 0x914
    aObject* grabbed_object; // 0xaa0
    aObject* text; // 0xaa4
    aObject* curr_object; // 0xaa8
    aObject* modal; // 0xaac
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
    int32_t cursor_type_1; // 0xadc
    int32_t cursor_visible; // 0xae0
    aTimerManager* timer_manager; // 0xae4
    int32_t some_code; // 0xae8
    BOOL allow_sys_palette_changes; // 0xaec
    int32_t field_0xaf0; // 0xaf0
    int32_t field_0xaf4; // 0xaf4
    int32_t can_scroll_horizontally; // 0xaf8
    int32_t can_scroll_vertically; // 0xafc
    int32_t scroll_width; // 0xb00
    int32_t scroll_height; // 0xb04
    int32_t cursor_type_2; // 0xb08
#pragma endregion
public:
    aSystem();

    // public: long __thiscall aSystem::startupDirectDraw(long,long,long)
    long startupDirectDraw(long width, long height, long bits_per_pixel);

    // public: long __thiscall aSystem::resetDirectDraw(long,long,long)
    long resetDirectDraw(long width, long height, long bits_per_pixel);

    // public: long __thiscall aSystem::shutdownDirectDraw(void)
    long shutdownDirectDraw();

    // public: int __thiscall aSystem::tweakDDPalette(int,int,struct VFX_RGB *,int)
    int tweakDDPalette(int pal_first_entry, int pal_entries_to_change, VFX_RGB* pal, BOOL param_4);

    // public: void __thiscall aSystem::activatePalette(unsigned char *,int,int)
    void activatePalette(uint8_t* rgb_data, int pal_first_entry, int pal_num_entries);

    // public: void __thiscall aSystem::activatePaletteFromTGA(char *)
    void activatePaletteFromTGA(char* pal_file_name);

    // public: void __thiscall aSystem::activateSmackerPalette(unsigned char *)
    void activateSmackerPalette(uint8_t* data);

    // public: void __thiscall aSystem::activatePaletteFromGIF(char *)
    void activatePaletteFromGIF(char* pal_file_name);

    // public: void __thiscall aSystem::fadeDownCurrentPalette(void)
    void fadeDownCurrentPalette();

    // public: long __thiscall aSystem::addCallback(class aCallback *)
    long addCallback(aCallback* cb);

    // public: int __thiscall aSystem::start(void *,void *,char *,int,short,short)
    int32_t start(HINSTANCE instance, HINSTANCE old_instance, char* cmd_line, int param_4, short param_5, short param_6);

    // public: void __thiscall aSystem::setScrollRect(void)
    void setScrollRect();

    // public: void __thiscall aSystem::SetCurrentCursor(enum CursorType)
    void SetCurrentCursor(CursorType cursor_type);

    // public: void __thiscall aSystem::showCursor(int)
    void showCursor(BOOL visible);
};
#pragma endregion

#pragma region VAs
VADDR(va_aObject_ctor, 0x0060e440);
VADDR(va_aObject_dtor, 0x0060e4d0);
VADDR(va_aObject_y, 0x0060f520);
VADDR(va_aObject_x, 0x0060f510);
VADDR(va_aObject_width, 0x0060f4c0);
VADDR(va_aObject_height, 0x0060f4d0);
VADDR(va_aObject_top, 0x0060a7f0);
VADDR(va_aObject_stopModal, 0x0060eae0);
VADDR(va_aObject_stopDrag, 0x0060f430);
VADDR(va_aObject_stopAnimation, 0x0060eac0);
VADDR(va_aObject_state, 0x0060fc50);
VADDR(va_aObject_startModal, 0x0060ead0);
VADDR(va_aObject_startDrag, 0x0060f400);
VADDR(va_aObject_startAnimation, 0x0060eab0);
VADDR(va_aObject_SetTransparent, 0x0060a7d0);
VADDR(va_aObject_setState, 0x0060fc60);
VADDR(va_aObject_setParent, 0x0060ea50);
VADDR(va_aObject_setPaintRoutine, 0x0060e990);
VADDR(va_aObject_setIcon, 0x0060fe10);
VADDR(va_aObject_SetHideDirection, 0x0060a7c0);
VADDR(va_aObject_setEventRoutine, 0x0060e9a0);
VADDR(va_aObject_setDisplayPort, 0x0060e880);
VADDR(va_aObject_setDepth, 0x0060ea60);
VADDR(va_aObject_SetBit, 0x0060fed0);
VADDR(va_aObject_setBackground, 0x0060fb20);
VADDR(va_aObject_setBackground2, 0x0060fba0);
VADDR(va_aObject_setBackColor, 0x0060eaf0);
VADDR(va_aObject_setAnimation, 0x0060fda0);
VADDR(va_aObject_resize, 0x0060f650);
VADDR(va_aObject_removeChild, 0x0060f2a0);
VADDR(va_aObject_rectIntersect1, 0x0060e950);
VADDR(va_aObject_rectIntersect2, 0x0060e910);
VADDR(va_aObject_ptr, 0x0060f4e0);
VADDR(va_aObject_port, 0x0060f500);
VADDR(va_aObject_pointInside, 0x0060e8e0);
VADDR(va_aObject_paint, 0x0060e9b0);
VADDR(va_aObject_operator_new, 0x0060e4e0);
VADDR(va_aObject_operator_delete, 0x0060e500);
VADDR(va_aObject_numberOfChildren, 0x0060f1e0);
VADDR(va_aObject_normalize, 0x0060fc30);
VADDR(va_aObject_moveTo, 0x0060f5a0);
VADDR(va_aObject_maximize, 0x0060fc20);
VADDR(va_aObject_left, 0x0060a7e0);
VADDR(va_aObject_right, 0x0060a800);
VADDR(va_aObject_leave, 0x0060e8d0);
VADDR(va_aObject_IsShowing, 0x0060a7a0);
VADDR(va_aObject_IsHidden, 0x0060a7b0);
VADDR(va_aObject_init, 0x0060e520);
VADDR(va_aObject_iconize, 0x0060fc40);
VADDR(va_aObject_icon, 0x0060feb0);
VADDR(va_aObject_HideMe, 0x0060ff30);
VADDR(va_aObject_handleEvent, 0x0060f950);
VADDR(va_aObject_globalY, 0x0060f560);
VADDR(va_aObject_globalX, 0x0060f530);
VADDR(va_aObject_getDropTargetCount, 0x0060a850);
VADDR(va_aObject_getDropTarget, 0x0060a860);
VADDR(va_aObject_GetCamera, 0x0060a790);
VADDR(va_aObject_frame, 0x0060f590);
VADDR(va_aObject_foremostChild, 0x0060f460);
VADDR(va_aObject_findObject, 0x0060e9c0);
VADDR(va_aObject_FillBox, 0x0060eec0);
VADDR(va_aObject_drawFramed, 0x0060ebf0);
VADDR(va_aObject_drawBox, 0x0060a750);
VADDR(va_aObject_drawBox2, 0x0060eb10);
VADDR(va_aObject_draw, 0x0060f6f0);
VADDR(va_aObject_dragStartY, 0x0060f450);
VADDR(va_aObject_dragStartX, 0x0060f440);
VADDR(va_aObject_dragging, 0x0060f3f0);
VADDR(va_aObject_display, 0x0060f790);
VADDR(va_aObject_destroy, 0x0060e6b0);
VADDR(va_aObject_depth, 0x0060eaa0);
VADDR(va_aObject_children, 0x0060ea40);
VADDR(va_aObject_child, 0x0060f4a0);
VADDR(va_aObject_checkSmackerPalette, 0x0060a840);
VADDR(va_aObject_bottom, 0x0060a820);
VADDR(va_aObject_background, 0x0060fec0);
VADDR(va_aObject_backColor, 0x0060eb00);
VADDR(va_aObject_animation, 0x0060fea0);
VADDR(va_aObject_addChild, 0x0060f1f0);
VADDR(va_aObject_vftable, 0x0077add4);
VADDR(va_aObject_vector_deleting_destructor, 0x0060e4a0);

VADDR(va_aCallback_vftable, 0x0077b1a8);
VADDR(va_aCallback_ctor, 0x006157c0);
VADDR(va_aCallback_dtor, 0x006157e0);
VADDR(va_aCallback_operator_new, 0x00615770);
VADDR(va_aCallback_operator_delete, 0x006157a0);
VADDR(va_aCallback_ExecFunc, 0x00615830);
VADDR(va_aCallback_destroy, 0x00615800);
VADDR(va_aCallback_setExec, 0x00615880);
VADDR(va_aCallback_setMessage, 0x006158a0);
VADDR(va_TimersCallback, 0x006158f0);

VADDR(va_aTimerManager_Init, 0x00615b80);
VADDR(va_aTimerManager_AddTimer, 0x00615cb0);
VADDR(va_aTimerManager_AddUniqueTimer, 0x00615c30);
VADDR(va_aTimerManager_GetTimer, 0x00615fc0);
VADDR(va_aTimerManager_GetTimer2, 0x00615fe0);
VADDR(va_aTimerManager_LockTimersExcept, 0x00616030);
VADDR(va_aTimerManager_RemoveTimer, 0x00615e40);
VADDR(va_aTimerManager_RemoveTimer2, 0x00615f10);
VADDR(va_aTimerManager_RemoveTimers, 0x00615d60);
VADDR(va_aTimerManager_UnlockTimers, 0x00616050);

VADDR(va_aSystem_vftable, 0x0077af08);
VADDR(va_aSystem_startupDirectDraw, 0x0060db20);
VADDR(va_aSystem_resetDirectDraw, 0x0060df40);
VADDR(va_aSystem_shutdownDirectDraw, 0x0060e330);
VADDR(va_aSystem_addCallback, 0x006147f0);
VADDR(va_aSystem_tweakDDPalette, 0x00614a70);
VADDR(va_aSystem_activatePalette, 0x006151c0);
VADDR(va_aSystem_activatePaletteFromTGA, 0x00615240);
VADDR(va_aSystem_activateSmackerPalette, 0x00615510);
VADDR(va_aSystem_activatePaletteFromGIF, 0x00615400);
VADDR(va_aSystem_fadeDownCurrentPalette, 0x00614f90);
VADDR(va_aSystem_start, 0x00612c10);
VADDR(va_aSystem_run, 0x00614490);
VADDR(va_aSystem_stop, 0x00613b70);
VADDR(va_aSystem_windowProc, 0x00612080);

VADDR(va_bmi, 0x007a9f90);
VADDR(va_aRedrawScreen, 0x00610410);
VADDR(va_screenWindow, 0x007ab154);
VADDR(va_logicalPalette, 0x007aa3c0);
VADDR(va_DesktopDC, 0x007aae7c);
VADDR(va_OffscreenBufferDC, 0x007aae70);
VADDR(va_hPalette, 0x007aae84);
VADDR(va_holdPalette, 0x007aa990);
VADDR(va_OffScreenhDIBSection, 0x007aae74);
VADDR(va_OffScreenhOldBitmap, 0x007aadf0);

VADDR(va_FullScreen, 0x007ab0e4);
VADDR(va_Width, 0x00789f70);
VADDR(va_Height, 0x00789f74);
VADDR(va_DAT_007ab0d4, 0x007ab0d4);

VADDR(va_DAT_007ab104, 0x007ab104);
VADDR(va_DAT_007ab108, 0x007ab108);
VADDR(va_SystemPalette, 0x007aa3c4); // va_DAT_007aa3c4
VADDR(va_mouseThreadStarted, 0x007bbbc0);
VADDR(va_MouseCritSec, 0x007bbb88);
VADDR(va_InMouseCritSec, 0x007bbbb0);
VADDR(va_BitDepth, 0x00789f78);
VADDR(va_aLockScreen, 0x00610450);
VADDR(va_aUnlockScreen, 0x00610530);
VADDR(va_InitWindowMode, 0x006125d0);
VADDR(va_WindowPlacement, 0x007a9ed8);
VADDR(va_SavedPosition, 0x007ab110);
VADDR(va_InitFullScreen, 0x00612710);
VADDR(va_GammaColorTranslation, 0x00789f8c);
VADDR(va_paletteRgb, 0x007ab1ec);
VADDR(va_globalEntries, 0x007ab1f0);
VADDR(va_globalFirst, 0x007ab1f4);
VADDR(va_InitAlphaLookup, 0x0006b51b0);
VADDR(va_countsPerSecond, 0x007aa7c8);
VADDR(va_scenarioTime, 0x007f09c4);
VADDR(va_aPostMessage, 0x00610590);
VADDR(va_handleEvent, 0x00610fb0);
VADDR(va_CheckMouse, 0x006127e0);
VADDR(va_leftMouseButtonDown, 0x007aa998);
VADDR(va_rightMouseButtonDown, 0x007aa99c);
VADDR(va_DAT_0078aaf0, 0x0078aaf0);
VADDR(va_DAT_0078aaf4, 0x0078aaf4);
VADDR(va_oldMouseX, 0x007a9ed0);
VADDR(va_oldMouseY, 0x007a9ed4);
VADDR(va_backpbmi, 0x007ab14c);
VADDR(va_thePalette, 0x007ab150);
VADDR(va_thisInstance, 0x007aaedc);
VADDR(va_appName, 0x007aa9a0);
VADDR(va_WindowTitle, 0x007f050c);
VADDR(va_Processor, 0x007ab10c);
VADDR(va_displayWidth, 0x007a9e70);
VADDR(va_displayHeight, 0x007aa920);
VADDR(va_systemInit, 0x007590d0);
VADDR(va_ParseCommandLine, 0x00610940);
VADDR(va_ghWindow, 0x007ab0ec);
VADDR(va_cursorShapes, 0x007ab114);
VADDR(va_MouseTimerInit, 0x0061ed30);
VADDR(va_UpdateDisplay, 0x0061e2e0);
VADDR(va_userInit, 0x0075a470);
VADDR(va_AG_mouseFrame, 0x007bbbbc);
VADDR(va_mouseTrackerCallback, 0x007ab1f8);
VADDR(va_RealWinMain, 0x00610be0);
VADDR(va_topOfStack, 0x007ab21c);
VADDR(va_stackSize, 0x0078a090);
VADDR(va_memoryStatus, 0x007ab1e4);
#pragma endregion

#pragma region Globals
OBJ_REF(uint32_t, g_stackSize, va_stackSize);
OBJ_REF(uint32_t, g_topOfStack, va_topOfStack);
PTR_REF(MEMORYSTATUS, g_memoryStatus, va_memoryStatus);
PTR_REF(aCallback, g_mouseTrackerCallback, va_mouseTrackerCallback);
OBJ_REF(int32_t, g_AG_mouseFrame, va_AG_mouseFrame);
OBJ_REF(int32_t, g_displayWidth, va_displayWidth);
OBJ_REF(int32_t, g_displayHeight, va_displayHeight);
OBJ_REF(int32_t, g_mouse_x, va_DAT_0078aaf0);
OBJ_REF(int32_t, g_mouse_y, va_DAT_0078aaf4);
OBJ_REF(int32_t, g_oldMouseX, va_oldMouseX);
OBJ_REF(int32_t, g_oldMouseY, va_oldMouseY);
OBJ_REF(int32_t, g_leftMouseButtonDown, va_leftMouseButtonDown);
OBJ_REF(int32_t, g_rightMouseButtonDown, va_rightMouseButtonDown);
OBJ_REF(float, g_scenarioTime, va_scenarioTime);
OBJ_REF(LARGE_INTEGER, g_countsPerSecond, va_countsPerSecond);
OBJ_REF(int32_t, g_globalEntries, va_globalEntries);
OBJ_REF(int32_t, g_globalFirst, va_globalFirst);
OBJ_REF(BOOL, g_SavedPosition, va_SavedPosition);
OBJ_REF(WINDOWPLACEMENT, g_WindowPlacement, va_WindowPlacement);
OBJ_REF(RTL_CRITICAL_SECTION, g_MouseCritSec, va_MouseCritSec);
OBJ_REF(BOOL, g_FullScreen, va_FullScreen);
OBJ_REF(BOOL, g_mouseThreadStarted, va_mouseThreadStarted);
OBJ_REF(BOOL, g_InMouseCritSec, va_InMouseCritSec);
OBJ_REF(int32_t, g_BitDepth, va_BitDepth);
OBJ_REF(int32_t, g_Width, va_Width);
OBJ_REF(int32_t, g_Height, va_Height);
PTR_REF(VFX_RGB, g_paletteRgb, va_paletteRgb);
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
ARR_REF(uint8_t, 256, g_GammaColorTranslation, va_GammaColorTranslation);
PTR_REF(void, g_DIBBits, va_DAT_007ab104);
PTR_REF(BITMAPINFO, g_backpbmi, va_backpbmi);
PTR_REF(HPALETTE, g_thePalette, va_thePalette);
OBJ_REF(HINSTANCE, g_thisInstance, va_thisInstance);
ARR_REF(char, 256, g_appName, va_appName);
OBJ_REF(char, g_WindowTitle, va_WindowTitle);
OBJ_REF(Processor, g_Processor, va_Processor);
OBJ_REF(HWND, g_hWindow, va_ghWindow);
PTR_REF(uint8_t*, g_cursorShapes, va_cursorShapes);

static void __cdecl aRedrawScreen();
static void __cdecl InitFullScreen(void);
static void __cdecl InitWindowMode(void);
static void __cdecl TimerCallback(void);
static void __cdecl CheckMouse(void);
static void __cdecl handleEvent(aEvent*);
static int __cdecl RealWinMain(struct HINSTANCE__* h_instance, struct HINSTANCE__* param_2, char* cmd_line, int param_4);
POINT __cdecl GetMessageCursorLoc(void);

inline auto aLockScreen_ptr = FUN_PTR<void(__cdecl*)(void)>(va_aLockScreen);
inline auto aUnlockScreen_ptr = FUN_PTR<void(__cdecl*)(void)>(va_aUnlockScreen);
inline auto InitAlphaLookup_ptr = FUN_PTR<void(__cdecl*)(VFX_RGB*)>(va_InitAlphaLookup);
inline auto aPostMessage_ptr = FUN_PTR<void(__cdecl*)(aObject*, int32_t)>(va_aPostMessage);
inline auto handleEvent_ptr = FUN_PTR<void(__cdecl*)(aEvent*)>(va_handleEvent);
inline auto systemInit_ptr = FUN_PTR<void(__cdecl*)(void)>(va_systemInit);
inline auto ParseCommandLine_ptr = FUN_PTR<void(__cdecl*)(char* cmd_line)>(va_ParseCommandLine);
inline auto MouseTimerInit_ptr = FUN_PTR<void(__cdecl*)(void)>(va_MouseTimerInit);
inline auto UpdateDisplay_ptr = FUN_PTR<void(__cdecl*)(int,int,long,int,long)>(va_UpdateDisplay);
inline auto userInit_ptr = FUN_PTR<long(__cdecl*)(void)>(va_userInit);

static __forceinline void EnterMouseCritSec() noexcept
{
    if (g_mouseThreadStarted != FALSE)
    {
        EnterCriticalSection(&g_MouseCritSec);
        g_InMouseCritSec = TRUE;
    }
}

static __forceinline void LeaveMouseCritSec() noexcept
{
    if (g_mouseThreadStarted != FALSE)
    {
        LeaveCriticalSection(&g_MouseCritSec);
        g_InMouseCritSec = FALSE;
    }
}
#pragma endregion

#pragma region Layout asserts (aObject)
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

#pragma region Layout asserts (aCallback)
ASSERT_SIZE(aCallback, 0x10);
#pragma endregion

#pragma region Layout asserts (aObject)
ASSERT_SIZE(aSystem, 0xb0c);
ASSERT_ALIGN(aSystem, 0x4);

ASSERT_FIELD_OFFSET(aSystem, ddraw2, 0x4ac);
ASSERT_FIELD_OFFSET(aSystem, ddraw1, 0x4b0);
ASSERT_FIELD_OFFSET(aSystem, ddraw_surf1, 0x4b4);
ASSERT_FIELD_OFFSET(aSystem, ddraw_attached_surf, 0x4b8);
ASSERT_FIELD_OFFSET(aSystem, field_0x4bc, 0x4bc);
ASSERT_FIELD_OFFSET(aSystem, ddraw_palette, 0x4c0);
ASSERT_FIELD_OFFSET(aSystem, master_palette, 0x4d0);
ASSERT_FIELD_OFFSET(aSystem, gamma, 0x7d0);
ASSERT_FIELD_OFFSET(aSystem, window_handle, 0x90c);
ASSERT_FIELD_OFFSET(aSystem, num_callbacks, 0x910);
ASSERT_FIELD_OFFSET(aSystem, callbacks, 0x914);
ASSERT_FIELD_OFFSET(aSystem, grabbed_object, 0xaa0);
ASSERT_FIELD_OFFSET(aSystem, cursor_type_2, 0xb08);
#pragma endregion