#include "mcx_inject.hpp"
#include "mcx/lib/heap.hpp"
#include "mcx/lib/file.hpp"
#include "mcx/lib/packet_file.hpp"
#include "mcx/lib/lz_decomp.hpp"

// __thiscall PacketFile::PacketFile(void)
MCXI_CTOR_HOOK Hook_PacketFile_ctor(PacketFile* this_ptr, void* edx_unused)
{
    File* file = reinterpret_cast<File*>(this_ptr);
    new (file) File();

    file->vtable = (File_vtable*)MCX_RUNTIME_ADDR(va_PacketFile_vftable);
    PacketFile* pkt_file = reinterpret_cast<PacketFile*>(file);
    pkt_file->seek_table = nullptr;
    pkt_file->check_integrity = FALSE;
    pkt_file->clear();

    MOV_THIS_TO_EAX(pkt_file)
}

#pragma warning(disable:26495, justification:"Initialization happens in Hook_PacketFile_ctor" )
PacketFile::PacketFile()
{
    Hook_PacketFile_ctor(this, nullptr);
}
#pragma warning(default:26495)

void PacketFile::clear()
{
    UserHeap* system_heap = g_systemHeap;
    this->current_packet = -1;
    this->packets_num = 0;
    this->packet_offset = 0;
    this->packet_size = 0;
    system_heap->free(this->seek_table);
    this->seek_table = nullptr;
}

long PacketFile::readPacketOffset(long packet_idx, PacketKind* packet_kind)
{
    if (packet_idx >= this->packets_num)
    {
        return 0xffffffff;
    }

    unsigned int packed_data = 0xffffffff;

    if (this->seek_table != nullptr)
    {
        packed_data = this->seek_table[packet_idx];
    }
    
    if (packet_kind != nullptr)
    {
        // extract top 3 bits (31-29);
        *packet_kind = (PacketKind)(packed_data >> 29);
    }

    // extract lower 29 bits (offset);
    packed_data &= 0x1fffffff;

    return packed_data;
}

long PacketFile::getPacketSize()
{
    return this->packet_real_size;
}

long PacketFile::seekPacket(long packet_idx)
{
    if (packet_idx < 0)
    {
        return ERR_INVALID_PACKET_IDX;
    }

    unsigned long last_offset = 0;

    long offset = this->readPacketOffset(packet_idx, &this->packet_kind);
    this->current_packet = packet_idx;

    if (packet_idx + 1 == this->packets_num)
    {
        last_offset = this->getLength();
    }
    else 
    {
        last_offset = this->readPacketOffset(packet_idx + 1, nullptr);
    }

    this->packet_size = last_offset - offset;
    this->packet_offset = offset;
    
    this->seek(offset, SEEK_SET);   

    if (this->packet_kind == PacketKind::RAW)
    {
        this->packet_real_size = this->packet_size;
    }
    else if (this->packet_kind == PacketKind::FIL)
    {
        this->packet_real_size = this->readLong();
    }
    else 
    {
        if (this->packet_kind != PacketKind::NUL)
        {
            return ERR_BAD_PACKET_KIND;
        }
        this->packet_real_size = 0;
    }

    HOOK_DLOGF(PACKETFILE, INFO, "PacketFile::readPacketOffset(%d) => kind=%d offset=%d rsize=%d", 
        packet_idx, this->packet_kind, offset, this->packet_real_size);

    return (offset < 1) ? ERR_INVALID_PACKET_IDX : NO_ERR;
}

static AutoFuncPatchRegister reg_aPort{
    "PacketFile",
    {
        { (void*)va_PacketFile_ctor, (void*)Hook_PacketFile_ctor },
        { (void*)va_PacketFile_clear, GetMemberPtr(&PacketFile::clear)},
        { (void*)va_PacketFile_seekPacket, GetMemberPtr(&PacketFile::seekPacket)}
    }
};