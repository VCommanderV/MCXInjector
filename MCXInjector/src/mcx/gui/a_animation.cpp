#include "mcx/gui/a_animation.hpp"
#include "mcx/lib/a_globs.hpp"
#include "mcx/lib/error.hpp"
#include "mcx/lib/file.hpp"
#include "mcx/lib/heap.hpp"

MCXI_CTOR_HOOK Hook_aAnimation_ctor(aAnimation* this_ptr, void* unused_edx)
{
    this_ptr->curr_frame = 0;
    this_ptr->num_frames = 0;
    this_ptr->shape_table = nullptr;
    this_ptr->frame_rate = DEFAULT_FRAME_RATE;
    this_ptr->width = 0;
    this_ptr->height = 0;
    MOV_THIS_TO_EAX(this_ptr);
}

#pragma warning(disable:26495, justification:"Initialization happens in Hook_aAnimation_ctor" )
aAnimation::aAnimation()
{
    Hook_aAnimation_ctor(this, nullptr);
}
#pragma warning(default:26495)

void static __forceinline __fastcall Hook_aAnimation_dtor(aAnimation* this_ptr, void* edx_unused)
{
    this_ptr->frame_rate = DEFAULT_FRAME_RATE;
    this_ptr->curr_frame = 0;
    this_ptr->num_frames = 0;
    this_ptr->shape_table = nullptr;
    this_ptr->width = 0;
    this_ptr->height = 0;
}

aAnimation::~aAnimation()
{
    Hook_aAnimation_dtor(this, nullptr);
}

long aAnimation::init(char* filename)
{
    this->curr_frame = 0;
    this->num_frames = 0;
    this->width = 0;
    this->height = 0;
    this->frame_rate = DEFAULT_FRAME_RATE;

    UserHeap* gui_heap = g_guiHeap;

    if (this->shape_table != nullptr)
    {
        gui_heap->free(this->shape_table);
        this->shape_table = nullptr;
    }

    if (filename != nullptr)
    {
        return this->loadShape(filename);
    }

    return 0;
}

long aAnimation::loadShape(char* filename)
{
    UserHeap* gui_heap = g_guiHeap;
    char* art_path = &g_artPath;
    char filepath_buf[128];

    if (this->shape_table != nullptr)
    {
        gui_heap->free(this->shape_table);
        this->shape_table = nullptr;
    }

    File* file = new File();
    sprintf(filepath_buf, "%s%s", art_path, filename);

    if (File::fileExists(filepath_buf) == FALSE)
    {
        char err_msg[256];
        sprintf(err_msg, "Unable to find '%s'", filepath_buf);
        GeneralMsg(err_msg);
        delete file;
        return -1;
    }

    file->open(filepath_buf, READ, FILE_DEFAULT_MAX_CHILDREN);
    FileSize file_size = file->fileSize();

    if (file_size == 0)
    {
        delete file;
        return -2;
    }

    void* dst_buf = gui_heap->malloc(file_size);
    this->shape_table = dst_buf;
    
    if (dst_buf == nullptr)
    {
        delete file;
        return 3;
    }

    uint8_t* gif_buf = reinterpret_cast<uint8_t*>(dst_buf);
    file->read2(gif_buf, file_size);
    file->close();

    long shape_count = _VFX_shape_count(gif_buf);
    this->num_frames = shape_count;
    this->curr_frame = 0;

    if (shape_count != 0)
    {

    }

    return 0;
}

long aAnimation::currentFrame()
{
    return this->curr_frame;
}

float aAnimation::frameRate()
{
    return this->frame_rate;
}

long aAnimation::nextFrame()
{
    long next_frame = this->curr_frame + 1;
    return next_frame > this->num_frames ? 0 : next_frame;
}

long aAnimation::numberOfFrames()
{
    return this->num_frames;
}

void aAnimation::setFrame(long new_frame)
{
    this->curr_frame = new_frame;
}

void aAnimation::setFrameRate(float new_framerate)
{
    this->frame_rate = new_framerate;
}

void* aAnimation::shapeTable()
{
    return this->shape_table;
}

long aAnimation::getWidth()
{
    return this->width;
}

long aAnimation::getHeight()
{
    return this->height;
}

void aAnimation::draw(_pane* pane, long x, long y)
{
}

void aAnimation::drawFrame(_pane* pane, long x, long y)
{
}

void aAnimation::reset()
{
    this->curr_frame = 0;
    this->num_frames = 0;
    this->frame_rate = DEFAULT_FRAME_RATE;
    this->width = 0;
    this->height = 0;

    if (this->shape_table != nullptr)
    {
        UserHeap* gui_heap = g_guiHeap;
        gui_heap->free(this->shape_table);
        this->shape_table = nullptr;
    }
}

static AutoFuncPatchRegister reg_aAnimation{
    "aAnimation",
    {
        { (void*)va_aAnimation_ctor, (void*)Hook_aAnimation_ctor },
        { (void*)va_aAnimation_dtor, (void*)Hook_aAnimation_dtor },
        { (void*)va_aAnimation_init, GetMemberPtr(&aAnimation::init) },
        { (void*)va_aAnimation_shapeTable, GetMemberPtr(&aAnimation::shapeTable) },
        { (void*)va_aAnimation_loadShape, GetMemberPtr(&aAnimation::loadShape) },
        { (void*)va_aAnimation_drawFrame, GetMemberPtr(&aAnimation::drawFrame) },
        { (void*)va_aAnimation_draw, GetMemberPtr(&aAnimation::draw) },
        { (void*)va_aAnimation_width, GetMemberPtr(&aAnimation::getWidth) },
        { (void*)va_aAnimation_currentFrame, GetMemberPtr(&aAnimation::currentFrame) },
        { (void*)va_aAnimation_nextFrame, GetMemberPtr(&aAnimation::nextFrame) },
        { (void*)va_aAnimation_setFrame, GetMemberPtr(&aAnimation::setFrame) },
        { (void*)va_aAnimation_frameRate, GetMemberPtr(&aAnimation::frameRate) },
        { (void*)va_aAnimation_setFrameRate, GetMemberPtr(&aAnimation::setFrameRate) },
        { (void*)va_aAnimation_numberOfFrames, GetMemberPtr(&aAnimation::numberOfFrames) }
    }
};

