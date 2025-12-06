#pragma once
#include <cstdint>
#include <cstddef>
#include "mcx/lib/vfx.hpp"

enum HideDirection : char
{
    DIRECTION_0 = 0,
    DIRECTION_1 = 1,
    DIRECTION_2 = 2,
    DIRECTION_3 = 3
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

    static void* operator new(size_t size);

    static void* operator new(size_t size, void* ptr);

    static void operator delete(void* ptr);
};

#pragma region VAs
static constexpr uintptr_t va_aPort_screenPort = 0x007aa7d0;
static constexpr uintptr_t va_aPort_vftable = 0x0077aa20;
static constexpr uintptr_t va_aPort_ctor = 0x0060c440;
static constexpr uintptr_t va_aPort_operator_new = 0x0060c400;
static constexpr uintptr_t va_aPort_operator_delete = 0x0060c420;
#pragma endregion

#pragma region Globals
static aPort** g_screenPort_ptr = reinterpret_cast<aPort**>(MCX_RUNTIME_ADDR(va_aPort_screenPort));
#pragma endregion

#pragma region Layout asserts
ASSERT_SIZE(aPort, 0x14);
ASSERT_ALIGN(aPort, 0x1);
#pragma endregion