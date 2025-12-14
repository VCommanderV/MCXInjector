#pragma once
#include <cstdint>
#include <cstddef>
#include "mcx_inject.hpp"
#include "mcx/lib/vfx.hpp"

#ifndef ENABLE_LOG_APORT
#define ENABLE_LOG_APORT 0
#endif

#ifndef NO_ERR
#define NO_ERR	0
#endif

static constexpr long ERR_FAILED_TO_CREATE_BITMAP = 3;

enum HideDirection : char
{
    DIRECTION_LEFT = 0,
    DIRECTION_UP = 1,
    DIRECTION_RIGHT = 2,
    DIRECTION_DOWN = 3
};

class aPort;

struct aPort_vtable
{
    //  public: virtual long __thiscall aPort::init(long,long)
    long(__thiscall* init)(aPort*, long, long);
    // public: virtual void __thiscall aPort::destroy(void)
    void(__thiscall* destroy)(aPort*);
    // public: virtual long __thiscall aPort::resize(long,long) 
    long(__thiscall* resize)(aPort*, long, long);
};

class aPort
{
public:
    aPort_vtable* vtable;
    int32_t width;
    int32_t height;
    _window* bitmap;
    _pane* frame;
public:
    aPort();

    ~aPort();

    static void* operator new(size_t size);

    static void* operator new(size_t size, void* ptr);

    static void operator delete(void* ptr);

    // public: void __thiscall aPort::destroy(void)
    void destroy();

    // public: long __thiscall aPort::width(void)
    int32_t getWidth() const;
 
    // public: long __thiscall aPort::height(void)
    int32_t getHeight() const;

    // public: struct _window * __thiscall aPort::bitmap(void)
    _window* getBitmap() const;

    // public: struct _pane * __thiscall aPort::frame(void)
    _pane* getFrame() const;

    // public: virtual long __thiscall aPort::init(long,long)
    long init(int32_t width, int32_t height);

    // public: long __thiscall aPort::init(long)
    long init2(long art_packet);

    // public: long __thiscall aPort::init(char *)
    long init3(char* file_name);

    // public: virtual long __thiscall aPort::resize(long,long)
    long resize(int32_t width, int32_t height);

    // public: void __thiscall aPort::copyTo(struct _pane *,long,long,int)
    void copyTo(_pane* frame, long x, long y, int transparency);

    uint8_t* getPixels();
};

#pragma region VAs
static constexpr uintptr_t va_aPort_screenPort = 0x007aa7d0;
static constexpr uintptr_t va_aPort_vftable = 0x0077aa20;
static constexpr uintptr_t va_aPort_ctor = 0x0060c440;
static constexpr uintptr_t va_aPort_dtor = 0x0060c460;
static constexpr uintptr_t va_aPort_destroy = 0x0060ca70;
static constexpr uintptr_t va_aPort_resize = 0x0060cad0;
static constexpr uintptr_t va_aPort_operator_new = 0x0060c400;
static constexpr uintptr_t va_aPort_operator_delete = 0x0060c420;
static constexpr uintptr_t va_aPort_init = 0x0060c470;
static constexpr uintptr_t va_aPort_init2 = 0x0060c580;
static constexpr uintptr_t va_aPort_init3 = 0x0060c6e0;
#pragma endregion

#pragma region Globals
static aPort** g_screenPort_ptr = reinterpret_cast<aPort**>(MCX_RUNTIME_ADDR(va_aPort_screenPort));
#pragma endregion

#pragma region Layout asserts
ASSERT_SIZE(aPort, 0x14);
ASSERT_ALIGN(aPort, 0x4);
#pragma endregion