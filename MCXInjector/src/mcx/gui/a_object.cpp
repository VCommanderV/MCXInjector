#include "mcx_inject.hpp"
#include "mcx/lib/error.hpp"
#include "mcx/lib/vfx.hpp"
#include "mcx/gui/a_object.hpp"

MCXI_CTOR_HOOK Hook_aObject_ctor(aObject* this_ptr, void* unused_edx)
{
    this_ptr->vtable = (aObject_vtable*)MCX_RUNTIME_ADDR(va_aObject_vftable);
    this_ptr->panel = nullptr;
    this_ptr->background = nullptr;
    this_ptr->animation_playing = FALSE;
    this_ptr->port = nullptr;
    this_ptr->force_45_pixels_maybe = FALSE;
    this_ptr->initialization_state = 0;
    this_ptr->paint_routine = nullptr;
    this_ptr->field_0x4A0 = 0;
    this_ptr->children_num = 0;
    this_ptr->parent = nullptr;
    this_ptr->animation = nullptr;
    this_ptr->icon = nullptr;
    this_ptr->field_0x4a8_maybe_rect_arr = nullptr;
    this_ptr->drop_target_num = 0;
    MOV_THIS_TO_EAX(this_ptr);
}

#pragma warning(disable:26495, justification:"Initialization happens in Hook_aObject_ctor" )
aObject::aObject()
{
    Hook_aObject_ctor(this, nullptr);
}
#pragma warning(default:26495)

long aObject::init(long x, long y, long width, long height, char* param_5)
{
    this->height = height;
    this->field_0x24 = height;
    this->field_0x34 = height;
    this->field_0x44 = height;
    this->width = width;
    this->x = x;
    this->y = y;
    this->field_0x20 = width;
    this->field_0x28 = x;
    this->field_0x2C = y;
    this->field_0x30 = width;
    this->field_0x38 = x;
    this->field_0x3C = y;
    this->field_0x40 = width;
    this->field_0x48 = x;
    this->field_0x4C = y;
    this->some_x = 0;
    this->field_0x68 = x;
    this->field_0x6C = y;
    this->state = aObjectState::NORMAL;
    this->showing = -1;
    this->dragging = FALSE;
    this->transparent = FALSE;
    this->back_color = 0xff;
    this->drop_target_num = 0;

    if (this->port != nullptr)
    {
        this->port->vtable->destroy(this->port);
        if (this->port != nullptr)
        {
            delete this->port;
        }
        this->port = nullptr;
    }

    this->port = new aPort();
    
    if (this->port != nullptr)
    {
        new (port) aPort();
    }

    long init_res = this->port->vtable->init(this->port, width, height);

    if (init_res != NO_ERR)
    {
        return init_res;
    }

    if (this->panel != nullptr)
    {
        delete this->panel;
        this->panel = nullptr;
    }

    this->panel = new _pane();
        
    if (this->panel == nullptr)
    {
        return 3;
    }

    aPort* screenPort = *g_screenPort_ptr;
    this->panel->window = screenPort->bitmap;
    this->panel->x0 = x;
    this->panel->y0 = y;   
    this->panel->x1 = x + width;
    this->panel->y1 = y + height;

    this->hidden = FALSE;
    this->hide_direction = HideDirection::DIRECTION_3;

    if (this->paint_routine != nullptr)
    {
        this->paint_routine = nullptr;
    }

    if (this->field_0x4A0 != 0)
    {
        this->field_0x4A0 = 0;
    }

    this->children_num = 0;
    this->parent = nullptr;
    this->depth = 0;
    this->animation = nullptr;
    this->animation_playing = FALSE;
    this->icon = nullptr;
    this->initialization_state = -1;

    return NO_ERR;
}

int32_t aObject::getX() const
{
    return this->x;
}

int32_t aObject::getY() const
{
    return this->y;
}

int32_t aObject::getWidth() const
{
    return this->width;
}

int32_t aObject::getHeight() const
{
    return this->height;
}

void aObject::addChild(aObject* child)
{
    Assert(
        this->children_num < MAX_CHILDREN,
        this->children_num + 1,
        (char*)"Too many children!", 
        nullptr
    );

    Assert(
        child->parent == nullptr || child->parent == this,
        0,
        (char*)" Adding child that's someone else's ",
        nullptr
    );

    if (child == nullptr)
    {
        return;
    }

    this->vtable->removeChild(this, child);
    child->vtable->setParent(child, this);
    this->children[this->children_num] = child;
    this->children_num += 1;

    auto FUN_0060ef20 = reinterpret_cast<void(__thiscall*)(aObject*, int32_t)>(
        *((void**)((char*)child->vtable + 0x80))
    );
    FUN_0060ef20(child, -1);
    
    child->vtable->moveTo(child, child->x, child->y, 0);
}

aAnimation* aObject::getAnimation()
{
    return this->animation;
}

static AutoFuncPatchRegister reg_aObject{
    "aObject",
    {
        { (void*)va_aObject_ctor, (void*)Hook_aObject_ctor },
        { (void*)va_aObject_init, GetMemberPtr(&aObject::init)},
     /*   { (void*)va_aObject_x, GetMemberPtr(&aObject::getX)},
        { (void*)va_aObject_y, GetMemberPtr(&aObject::getY)},*/
        { (void*)va_aObject_addChild, GetMemberPtr(&aObject::addChild)},
    }
};
