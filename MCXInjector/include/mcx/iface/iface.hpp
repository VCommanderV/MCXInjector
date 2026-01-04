#pragma once
#include "mcx_inject.hpp"
#include "mcx/gui/a_system.hpp"

class Parser; // command parser

class InterfaceObject
{
public:
    int32_t field_0x00;             // 0x000
    int32_t field_0x04;             // 0x004
    int32_t field_0x08;             // 0x008
    int16_t field_0x0c;             // 0x00C - constructor: MOV word ptr
    int16_t field_0x0e;             // 0x00E
    int32_t field_0x10;             // 0x010
    int32_t field_0x14;             // 0x014
    int32_t field_0x18;             // 0x018 - constructor sets to 0
    int32_t field_0x1c;             // 0x01C
    int32_t field_0x20;             // 0x020
    int32_t field_0x24;             // 0x024
    int32_t field_0x28;             // 0x028
    int32_t field_0x2c;             // 0x02C - constructor sets to 0 (dword)
    int32_t field_0x30;             // 0x030 - constructor sets to 0 (dword)
    int32_t field_0x34;             // 0x034
    int32_t field_0x38;             // 0x038
    int32_t field_0x3c;             // 0x03C
    int32_t field_0x40;             // 0x040
    int32_t field_0x44;             // 0x044
    int32_t field_0x48;             // 0x048
    int32_t field_0x4c;             // 0x04C
    int32_t field_0x50;             // 0x050
    int32_t field_0x54;             // 0x054
    int32_t field_0x58;             // 0x058
    int32_t field_0x5c;             // 0x05C
    int32_t field_0x60;             // 0x060
    int32_t field_0x64;             // 0x064
    int32_t field_0x68;             // 0x068
    int32_t field_0x6c;             // 0x06C
    int32_t field_0x70;             // 0x070
    int32_t field_0x74;             // 0x074
    int32_t field_0x78;             // 0x078
    int32_t field_0x7c;             // 0x07C
    int32_t field_0x80;             // 0x080
    int32_t field_0x84;             // 0x084
    int32_t field_0x88;             // 0x088
    int32_t field_0x8c;             // 0x08C
    int32_t field_0x90;             // 0x090
    int32_t field_0x94;             // 0x094
    int32_t field_0x98;             // 0x098 - constructor sets to 0
    int32_t field_0x9c;             // 0x09C - constructor sets to 0
    int32_t field_0xa0;             // 0x0A0 - constructor sets to -1
    int32_t field_0xa4;             // 0x0A4 - constructor sets to 0
    int16_t field_0xa8;             // 0x0A8 - constructor sets to 10
    int16_t field_0xaa;             // 0x0AA - constructor sets to 4
    int16_t field_0xac;             // 0x0AC - constructor sets to 1
    int16_t field_0xae;             // 0x0AE - constructor sets to 500
    int32_t field_0xb0;             // 0x0B0 - constructor sets to 0
    int32_t field_0xb4;             // 0x0B4
    int32_t field_0xb8[12];         // 0x0B8 - 48 bytes, zeroed (0xC iterations)
    int32_t field_0xe8;             // 0x0E8 - constructor sets to 0
    int32_t field_0xec;             // 0x0EC - constructor sets to 0
    int32_t field_0xf0;             // 0x0F0 - constructor sets to 0
    int32_t field_0xf4;             // 0x0F4 - constructor sets to 0
    int16_t field_0xf8;             // 0x0F8 - constructor: MOV word ptr
    int16_t field_0xfa;             // 0x0FA
    int32_t field_0xfc;             // 0x0FC
    int32_t field_0x100;            // 0x100
    int32_t field_0x104;            // 0x104 - constructor: MOV dword ptr
    int32_t field_0x108;            // 0x108 - constructor sets to 1
    int32_t field_0x10c;            // 0x10C - constructor sets to -1
    int32_t field_0x110;            // 0x110 - constructor sets to 0
    int32_t field_0x114;            // 0x114
    int32_t field_0x118[0x4D];      // 0x118 - 77 elements = 308 bytes
    // End: 0x118 + 0x134 = 0x24C (588 bytes)

    // MISSING 64 bytes to reach 652 (0x28C)
    uint8_t unknown_tail[64];       // 0x24C - 0x28C
public:
    InterfaceObject();

	static void* operator new(size_t size);

	static void* operator new(size_t size, void* ptr);

	static void operator delete(void* ptr);
};

#pragma region VAs
VADDR(va_InterfaceObject_operator_new, 0x006cb8e0);
VADDR(va_theInterface, 0x007ab1dc);
VADDR(va_InterfaceObject_init, 0x006cb920);

PTR_REF(InterfaceObject, g_theInterface, va_theInterface);
inline auto InterfaceObject_init_ptr = FUN_PTR<void(__thiscall*)(InterfaceObject*)>(va_InterfaceObject_init);
#pragma endregion