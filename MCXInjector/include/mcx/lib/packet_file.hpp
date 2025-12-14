#pragma once
#include "mcx_inject.hpp"
#include "mcx/lib/file.hpp"

#ifndef ENABLE_LOG_PACKETFILE
#define ENABLE_LOG_PACKETFILE 0
#endif

static constexpr long ERR_INVALID_PACKET_IDX = 0xBADF0004;
static constexpr long ERR_BAD_PACKET_KIND = 0xBADF000B;

enum PacketKind : long
{
	RAW = 0x00L,
	FIL = 0x02L,
	NUL = 0x07L,
};

class PacketFile : File
{
public:
	long packets_num;
	long current_packet;
	long packet_size;
	long packet_offset;
	PacketKind packet_kind;
	long packet_real_size;
	// [ top 3 bits ] [ lower 29 bits ]
	// packet  kind         offset
	long* seek_table;
	BOOL check_integrity;
public:
	PacketFile(); // File
	// protected: void __thiscall PacketFile::clear(void)
	void clear();

	// public: long __thiscall PacketFile::readPacketOffset(long,long*);
	long readPacketOffset(long packet_idx, PacketKind* packet_kind);

	// public: long __thiscall PacketFile::seekPacket(long)
	long seekPacket(long packet_idx);

	// public: long __thiscall PacketFile::getPacketSize(void)
	long getPacketSize();
};

#pragma region VAs
static constexpr uintptr_t va_PacketFile_ctor = 0x0064d110;
static constexpr uintptr_t va_PacketFile_clear = 0x0064ce50;
static constexpr uintptr_t va_PacketFile_seekPacket = 0x0064ce50;
static constexpr uintptr_t va_PacketFile_vftable = 0x0077c33c;
static constexpr uintptr_t va_PacketFile_0064d250 = 0x0064d250;
static constexpr uintptr_t va_PacketFile_artFile = 0x007ab20c;
#pragma endregion

#pragma region Globals
static PacketFile** g_artFile = reinterpret_cast<PacketFile**>(MCX_RUNTIME_ADDR(va_PacketFile_artFile));
#pragma endregion

#pragma region Layout asserts
ASSERT_SIZE(PacketFile, 0x6c);
ASSERT_ALIGN(PacketFile, 0x4);
ASSERT_FIELD_OFFSET(PacketFile, packets_num, 0x4c);
ASSERT_FIELD_OFFSET(PacketFile, current_packet, 0x50);
ASSERT_FIELD_OFFSET(PacketFile, packet_size, 0x54);
ASSERT_FIELD_OFFSET(PacketFile, packet_offset, 0x58);
ASSERT_FIELD_OFFSET(PacketFile, packet_kind, 0x5c);
ASSERT_FIELD_OFFSET(PacketFile, packet_real_size, 0x60);
ASSERT_FIELD_OFFSET(PacketFile, seek_table, 0x64);
ASSERT_FIELD_OFFSET(PacketFile, check_integrity, 0x68);
#pragma endregion