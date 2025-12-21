#include "mcx_inject.hpp"
#include "mcx/lib/error.hpp"
#include "mcx/lib/heap.hpp"
#include "mcx/lib/packet_file.hpp"
#include "mcx/lib/a_globs.hpp"
#include "mcx/gui/a_port.hpp"
#include "mcx/lib/vfx_map_polygon.hpp"

MCXI_CTOR_HOOK Hook_aPort_ctor(aPort* this_ptr, void* unused_edx)
{
    this_ptr->vtable = (aPort_vtable*)MCX_RUNTIME_ADDR(va_aPort_vftable);
    this_ptr->bitmap = nullptr;
    this_ptr->frame = nullptr;
    this_ptr->height = -1;
    this_ptr->width = -1;
    MOV_THIS_TO_EAX(this_ptr);
}

#pragma warning(disable:26495, justification:"Initialization happens in Hook_aPort_ctor" )
aPort::aPort()
{
    Hook_aPort_ctor(this, nullptr);
}
#pragma warning(default:26495)

void static __forceinline __fastcall Hook_aPort_dtor(aPort* this_ptr, void* edx_unused)
{
    this_ptr->vtable = (aPort_vtable*)MCX_RUNTIME_ADDR(va_aPort_vftable);
    this_ptr->destroy();
}

aPort::~aPort()
{
    Hook_aPort_dtor(this, nullptr);
}

static __forceinline void* __cdecl Hook_aPort_new(size_t size)
{
    UserHeap* gui_heap = *g_guiHeap_ptr;
    return gui_heap->malloc(size);
}

static void __forceinline __cdecl Hook_aPort_delete(void* ptr)
{
    UserHeap* gui_heap = *g_guiHeap_ptr;
    gui_heap->free(ptr);
}

#pragma warning(disable:26495, justification : "Initialization happens in Hook_aPort_ctor" )

void* aPort::operator new(size_t size)
{
    return Hook_aPort_new(size);
}

void* aPort::operator new(size_t size, void* ptr) noexcept
{
    return ptr;
}

void aPort::operator delete(void* ptr)
{
    Hook_aPort_delete(ptr);
}

void aPort::destroy()
{
    UserHeap* gui_heap = mcx::heap::GetGuiHeap();
    
    _window* bitmap_mem = this->bitmap;
    
    this->width = -1;
    this->height = -1;

    if (bitmap_mem != nullptr)
    {
        if (bitmap_mem->pixels != nullptr)
        {
            gui_heap->free(bitmap_mem->pixels);
        }
        gui_heap->free(bitmap_mem);
        this->bitmap = nullptr;
    }

    if (this->frame != nullptr)
    {
        gui_heap->free(this->frame);
        this->frame = nullptr;
    }
}

int32_t aPort::getWidth() const
{
    return this->width;
}

int32_t aPort::getHeight() const
{
    return this->height;
}

_window* aPort::getBitmap() const
{
    return this->bitmap;
}

_pane* aPort::getFrame() const
{
    return this->frame;
}

long aPort::init(int32_t width, int32_t height)
{
    HOOK_DLOGF(APORT, INFO, "aPort::init(%p, %d, %d)", this, width, height);

    if (this->width == width && this->height == height)
    {
        return NO_ERR;
    }

    _window* bitmap_ = this->bitmap;
    UserHeap* gui_heap = *g_guiHeap_ptr;

    if (bitmap_ != nullptr)
    {
        if (bitmap_->pixels != nullptr)
        {
            gui_heap->free(bitmap_->pixels);
        }
        gui_heap->free(bitmap_);
    }

    bitmap_ = reinterpret_cast<_window*>(gui_heap->malloc(sizeof(_window)));
    this->bitmap = bitmap_;

    if (bitmap_ == nullptr)
    {
        return ERR_FAILED_TO_CREATE_BITMAP;
    }

    bitmap_->width_zero_based = width - 1;
    bitmap_->height_zero_based = height - 1;

    if (this != g_screenPort)
    {
        auto buf = reinterpret_cast<uint8_t*>(gui_heap->malloc(width * height));
        bitmap_->pixels = buf;
        if (buf == nullptr)
        {
            return ERR_FAILED_TO_CREATE_BITMAP;
        }
    }

    if (this->frame != nullptr)
    {
        gui_heap->free(this->frame);
    }

    _pane* frame_ = reinterpret_cast<_pane*>(gui_heap->malloc(sizeof(_pane)));
    this->frame = frame_;

    if (frame_ == nullptr)
    {
        return ERR_FAILED_TO_CREATE_BITMAP;
    }

    frame_->window = bitmap_;
    frame_->x0 = 0;
    frame_->y0 = 0;
    frame_->x1 = width - 1;
    frame_->y1 = height - 1;

    this->width = width;
    this->height = height;

    return NO_ERR;
}

long aPort::init2(long art_packet)
{
    HOOK_DLOGF(APORT, INFO, "aPort::init2(%p, %s)", this, art_packet);

    PacketFile* art_file = *g_artFile;

    long result = art_file->seekPacket(art_packet);

    if (result != NO_ERR)
    {
        Fatal(0, (char*)"Art packet not found", nullptr);
        return -1;
    }

    File* file = new File();
    long packet_size = art_file->getPacketSize();
    
    long open_res = file->open2(reinterpret_cast<File*>(art_file), packet_size, 32);
    
    if (open_res != NO_ERR)
    {
        Fatal(0, (char*)"Cant open child file.", nullptr);
    }

    FileSize file_size = file->fileSize();
    if (file_size == 0)
    {
        Fatal(0xfffffffe, (char*)"Bad child file in art.pak", nullptr);
    }

    UserHeap* gui_heap = mcx::heap::GetGuiHeap();
    unsigned char* file_buf = reinterpret_cast<unsigned char*>(gui_heap->malloc(file_size));

    if (file_buf == nullptr)
    {
        Fatal(3, (char*)"Not enough memory to read art file", nullptr);
    }

    file->read2(file_buf, file_size);
    file->close();

    // 2 16-bit packed
    uint32_t gif_resolution_packed = _VFX_GIF_resolution(file_buf);

    void* gif_decode_tmp_buf = std::malloc(GIF_DECOMP_BUF_SIZE);

    if (gif_decode_tmp_buf == nullptr)
    {
        Fatal(3, (char*)"Not enough memory to decode gif file", nullptr);
    }

    int32_t width = gif_resolution_packed >> 0x10;
    int32_t height = gif_resolution_packed & 0xffff;
    long init_res = this->init(width, height);

    if (init_res != NO_ERR)
    {
        delete file;
        return init_res;
    }

    _VFX_GIF_draw(this->frame, file_buf, gif_decode_tmp_buf);
    gui_heap->free(file_buf);
    delete file;

    return NO_ERR;
}

long aPort::init3(char* art_filename)
{
    HOOK_DLOGF(APORT, INFO, "aPort::init3(%p, %s)", this, art_filename);

    char filepath_buf[MAX_PATH - 8];
    File* file = new File();

    long curr_planet = mcx::GetCurrPlanet();
    char* art_path = mcx::GetArtPath();
    long open_res = -1;

    if (curr_planet == 1)
    {
        sprintf(filepath_buf, "%sx_%s", art_path, art_filename);
        long open_res = file->open(filepath_buf, FileMode::READ, FILE_DEFAULT_MAX_CHILDREN);
        HOOK_DLOGF(APORT, INFO, "  aPort::init3(%p, %s) -> %s (%d)", this, art_filename, filepath_buf, open_res);
    }

    if (open_res != 0)
    {
        sprintf(filepath_buf, "%s%s", art_path, art_filename);
        open_res = file->open(filepath_buf, FileMode::READ, FILE_DEFAULT_MAX_CHILDREN);
        HOOK_DLOGF(APORT, INFO, "  aPort::init3(%p, %s) -> %s (%d)", this, art_filename, filepath_buf, open_res);
        if (open_res != NO_ERR)
        {
            strcpy(filepath_buf, art_filename);
            long open_res = file->open(filepath_buf, FileMode::READ, FILE_DEFAULT_MAX_CHILDREN);
            if (open_res != NO_ERR)
            {
                char msg_buf[52];
                sprintf(msg_buf, "Error reading '%s'", filepath_buf);
                GeneralMsg(msg_buf);
                delete file;
                return -2;
            }
        }
    }
 
    long size = file->fileSize();
    if (size == 0)
    {
        char msg_buf[52];
        sprintf(msg_buf, "Error reading '%s'", filepath_buf);
        GeneralMsg(msg_buf);
        delete file;
        return -2;
    }

    unsigned char* buf = reinterpret_cast<unsigned char*>(std::malloc(size));
    if (buf == nullptr)
    {
        // TODO: fallbacks
    }
    else 
    {
        file->read2(buf, size);
        file->close();

        DWORD* dw_buf = reinterpret_cast<DWORD*>(buf);
        DWORD d = dw_buf[3];
        short w = (int)(short)d; // low
        short h = (short)((unsigned int)d >> 0x10); // high
        long init_res = this->init(w, h);

        if (init_res != NO_ERR)
        {
            char msg_buf[1024];
            sprintf(msg_buf, "Failed trying to create a %d by %d aPort with dataBuffer", w, h);
            Fatal(init_res, msg_buf, (char*)0x0);
        }

        memcpy(
            this->frame->window->pixels,
            buf + 0x312,
            w * h);

        free(buf);
    }

    delete file;
    return NO_ERR;
}

long aPort::resize(int32_t new_width, int32_t new_height)
{
    aPort* screen_port = g_screenPort;
    UserHeap* gui_heap = mcx::heap::GetGuiHeap();

    if (this != screen_port)
    {
        if (this->bitmap->pixels != nullptr)
        {
            gui_heap->free(this->bitmap->pixels);
        }
        this->bitmap->pixels = reinterpret_cast<uint8_t*>(gui_heap->malloc(new_width * new_height));
    }
    
    this->width = new_width;
    this->height = new_height;
    this->bitmap->width_zero_based = new_width - 1;
    this->bitmap->height_zero_based = new_height - 1;
    this->frame->window = this->bitmap;
    this->frame->x1 = new_width - 1;
    this->frame->y1 = new_height - 1;

    return NO_ERR;
}

void aPort::copyTo(_pane* dst_pane, long x, long y, int transparency)
{
    if (transparency != FALSE)
    {
        _window* window = this->frame->window;
        _DrawTransparent_PTR(
            dst_pane, window,
            x, y,
            window->width_zero_based + 1, window->height_zero_based + 1
        );
        return;
    }

    _VFX_panel_copy_PTR(this->frame, 0, 0, dst_pane, x, y, 0xffffffff);
}

uint8_t* aPort::getPixels()
{
    if (this->bitmap == nullptr)
    {
        return nullptr;
    }

    return this->bitmap->pixels;
}

static AutoFuncPatchRegister reg_aPort{
    "aPort",
    {
        { (void*)va_aPort_ctor, (void*)Hook_aPort_ctor },
        { (void*)va_aPort_dtor, (void*)Hook_aPort_dtor },
        { (void*)va_aPort_operator_new, (void*)Hook_aPort_new},
        { (void*)va_aPort_operator_delete, (void*)Hook_aPort_delete},
        { (void*)va_aPort_destroy, GetMemberPtr(&aPort::destroy)},
        { (void*)va_aPort_resize, GetMemberPtr(&aPort::resize)},
        { (void*)va_aPort_init, GetMemberPtr(&aPort::init)},
        { (void*)va_aPort_init2, GetMemberPtr(&aPort::init2)},
        { (void*)va_aPort_init3, GetMemberPtr(&aPort::init3)}
    }
};
