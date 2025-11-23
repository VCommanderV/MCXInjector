#pragma once

#include "mcx_inject.hpp"
#include "mcx/lib/file.hpp"

class PacketFile : File
{
public:
	uint32_t field_0x4c;
	uint32_t current_packet;
	uint32_t field_0x54;
	uint32_t field_0x58;
	uint32_t field_0x5c;
	uint32_t field_0x60;
	uint32_t field_0x64;
	uint32_t field_0x68;
};

#pragma region VAs
static constexpr uintptr_t va_PacketFile_0064d250 = 0x0064d250;
#pragma endregion

#pragma region Layout asserts
ASSERT_SIZE(PacketFile, 0x6c);
ASSERT_ALIGN(PacketFile, 0x4);
ASSERT_FIELD_OFFSET(PacketFile, field_0x4c, 0x4c);
ASSERT_FIELD_OFFSET(PacketFile, current_packet, 0x50);
ASSERT_FIELD_OFFSET(PacketFile, field_0x54, 0x54);
ASSERT_FIELD_OFFSET(PacketFile, field_0x58, 0x58);
ASSERT_FIELD_OFFSET(PacketFile, field_0x5c, 0x5c);
ASSERT_FIELD_OFFSET(PacketFile, field_0x60, 0x60);
ASSERT_FIELD_OFFSET(PacketFile, field_0x64, 0x64);
ASSERT_FIELD_OFFSET(PacketFile, field_0x68, 0x68);
#pragma endregion