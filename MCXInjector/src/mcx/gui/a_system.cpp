#include "mcx_inject.hpp"
#include "mcx/lib/a_globs.hpp"
#include "mcx/lib/error.hpp"
#include "mcx/lib/vfx.hpp"
#include "mcx/gui/a_animation.hpp"
#include "mcx/gui/a_system.hpp"

#pragma region aObject
MCXI_CTOR_HOOK Hook_aObject_ctor(aObject* this_ptr, void* unused_edx)
{
    this_ptr->vtable = (aObject_vtable*)MCX_RUNTIME_ADDR(va_aObject_vftable);
    this_ptr->display_port = nullptr;
    this_ptr->background_port = nullptr;
    this_ptr->animation_playing = FALSE;
    this_ptr->render_port = nullptr;
    this_ptr->force_45_pixels_maybe = FALSE;
    this_ptr->initialization_state = 0;
    this_ptr->paint_routine = nullptr;
    this_ptr->event_routine = 0;
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
    this->icon_height = height;
    this->width = width;
    this->x = x;
    this->y = y;
    this->field_0x20 = width;
    this->field_0x28 = x;
    this->field_0x2C = y;
    this->field_0x30 = width;
    this->field_0x38 = x;
    this->field_0x3C = y;
    this->icon_width = width;
    this->field_0x48 = x;
    this->field_0x4C = y;
    this->animation_offset = 0;
    this->original_x = x;
    this->original_y = y;
    this->state = aObjectState::NORMAL;
    this->showing = -1;
    this->dragging = FALSE;
    this->transparent = FALSE;
    this->back_color = 0xff;
    this->drop_target_num = 0;

    if (this->render_port != nullptr)
    {
        this->render_port->vtable->destroy(this->render_port);
        if (this->render_port != nullptr)
        {
            delete this->render_port;
        }
        this->render_port = nullptr;
    }

    this->render_port = new aPort();
    
    if (this->render_port != nullptr)
    {
        new (render_port) aPort();
    }

    long init_res = this->render_port->vtable->init(this->render_port, width, height);

    if (init_res != NO_ERR)
    {
        return init_res;
    }

    if (this->display_port != nullptr)
    {
        delete this->display_port;
        this->display_port = nullptr;
    }

    this->display_port = new _pane();
        
    if (this->display_port == nullptr)
    {
        return 3;
    }

    aPort* screenPort = g_screenPort;
    this->display_port->window = screenPort->bitmap;
    this->display_port->x0 = x;
    this->display_port->y0 = y;   
    this->display_port->x1 = x + width;
    this->display_port->y1 = y + height;

    this->hidden = FALSE;
    this->hide_direction = HideDirection::DIRECTION_DOWN;

    if (this->paint_routine != nullptr)
    {
        this->paint_routine = nullptr;
    }

    if (this->event_routine != 0)
    {
        this->event_routine = 0;
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

void aObject::destroy()
{
    // TODO: requires aSystem
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

int32_t aObject::getTop()
{
    return this->vtable->getY(this);
}

int32_t aObject::getLeft()
{
    return this->vtable->getX(this);
}

int32_t aObject::getRight() const
{
    return this->x + this->width;
}

int32_t aObject::getBottom() const
{
    return this->y + this->height;
}

int32_t aObject::getGlobalX() const
{
    aObject* parent = this->parent;
    if (parent != nullptr) {
        return parent->vtable->globalX(parent) + this->x;
    }
    return this->x;
}

int32_t aObject::getGlobalY() const
{
    aObject* parent = this->parent;
    if (parent != nullptr) {
        return parent->vtable->globalY(parent) + this->y;
    }
    return this->y;
}

// State getters
int32_t aObject::getState() const
{
    return this->state;
}

int aObject::isShowing() const
{
    return this->showing;
}

int aObject::isHidden() const
{
    return this->hidden;
}

int aObject::isDragging() const
{
    return this->dragging;
}

int32_t aObject::getDragStartX() const
{
    return this->drag_start_x;
}

int32_t aObject::getDragStartY() const
{
    return this->drag_start_y;
}

int32_t aObject::getDepth() const
{
    return this->depth;
}

int32_t aObject::getBackColor() const
{
    return this->back_color;
}

int32_t aObject::getNumberOfChildren() const
{
    return this->children_num;
}

aObject* aObject::getChildren() const
{
    return nullptr;
}

aPort* aObject::getPort() const
{
    return this->render_port;
}

aPort* aObject::getBackground() const
{
    return this->background_port;
}

_pane* aObject::getFrame() const
{
    return this->display_port;
}

void* aObject::getPtr() const
{
    if (this->render_port != nullptr)
    {
        return this->render_port->getPixels();
    }

    return nullptr;
}

aAnimation* aObject::getAnimation()
{
    return this->animation;
}

aAnimation* aObject::getIcon() const
{
    return this->icon;
}

void* aObject::getCamera() const
{
    return nullptr;
}

int32_t aObject::getDropTargetCount() const
{
    return this->drop_target_num;
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

aObject* aObject::child(long child_idx)
{
    if (child_idx >= this->children_num)
    {
        return nullptr;
    }

    return this->children[child_idx];
}

void aObject::display()
{
    if (this->showing == FALSE)
    {
        return;
    }

    if (this->vtable->IsHidden(this) != FALSE && this->animation_offset == 0)
    {
        return;
    }

    if (this->state == ICONIZED)
    {
        if (this->icon != nullptr)
        {
            this->vtable->draw(this);
        }
    }
    else
    {
        if (this->animation != nullptr)
        {
            auto anim_draw = reinterpret_cast<void(__thiscall*)(aAnimation*, _pane*, long, long)>(
                MCX_RUNTIME_ADDR(va_aAnimation_draw)
            );
            anim_draw(this->animation, this->render_port->frame, 0, 0);
            this->vtable->draw(this);
        }
    }

    if (this->animation_offset != 0)
    {
        // handle animation effect?
        if (this->hide_direction == DIRECTION_LEFT || this->hide_direction == DIRECTION_RIGHT  )
        {
            this->vtable->moveTo(this, this->getX() + this->animation_offset, this->getY(), -1);
        }
        else
        {
            this->vtable->moveTo(this, this->getX(), this->getY() + this->animation_offset, -1);
        }

        bool should_reset_offset = false;

        if (this->hidden == 0)
        {
            // not explicitly hidden, check if sliding animation went off-screen
            int offset = this->animation_offset;
            bool is_negative_offset = offset < 0;
            bool is_out_of_bounds = false;

            int global_x = this->vtable->globalX(this);
            int global_y = this->vtable->globalY(this);

            if (is_negative_offset)
            {
                // Moving left/up - check if went past left/top edge
                is_out_of_bounds = (this->original_x < global_x) || (this->original_y < global_y);
            }
            else if (offset > 0) 
            {
                // moving right/down, check if went past right/bottom edge
                is_out_of_bounds = (global_x < this->original_x) && (this->original_y < global_y);
            }

            if (is_out_of_bounds && this->parent != nullptr) 
            {
                // Object slid out of bounds, reposition relatively to parent
                aObject* parent = this->parent;
                int parent_global_x = parent->vtable->globalX(parent);
                int parent_global_y = parent->vtable->globalY(parent);
                this->vtable->moveTo(this, 
                    this->original_x - parent_global_x,
                    this->original_y - parent_global_y, -1);
            }
            else if (!is_out_of_bounds) 
            {
                should_reset_offset = true;
            }
        }
        else {
            // explicitly hidden, check if still intersects application window
            aObject* application = reinterpret_cast<aObject*>(g_application);
            int32_t app_width = application->vtable->getWidth(application);
            int32_t app_height = application->vtable->getHeight(application);
            int intersects = this->vtable->rectIntersect(this, 0, 0, app_width, app_height);

            if (intersects == FALSE) {
                // no longer visible, animation complete
                should_reset_offset = true;
            }
        }

        if (should_reset_offset != FALSE) {
            this->animation_offset = 0;
        }
    }
    
    if (this->render_port != nullptr) 
    {
        this->render_port->copyTo(this->display_port, 0, 0, this->transparent);
    }

    if (this->state != ICONIZED && this->children_num > 0) 
    {
        for (int i = 0; i < this->children_num; i++) 
        {
            aObject* child = this->children[i];
            child->vtable->display(child);
        }
    }

    return;
}

void aObject::draw()
{
    if (this->state == ICONIZED)
    {
        this->icon->draw(this->render_port->frame, 0, 0);
        return;
    }
    
    if (this->background_port != nullptr)
    {
        this->background_port->copyTo(this->render_port->frame, 0, 0, -1);
    }

    if (this->animation != nullptr && this->animation_playing != FALSE)
    {
        this->animation->draw(this->render_port->frame, 0, 0);
    }
    
    this->vtable->paint(this);

    if (this->children_num > 0)
    {
        for (int i = 0; i < this->children_num; i++)
        {
            aObject* child = this->children[i];
            child->vtable->draw(child);
        }
    }
}

aObject* aObject::findObject(long pos_x, long pos_y)
{
    // HOOK_DLOGF(AOBJECT, INFO, "aObject::findObject(%p,%d,%d)", this, pos_x, pos_y);

    if (this->state != ICONIZED)
    {
        if (this->showing == 0)
        {
            return nullptr;
        }

        if (this->children_num > 0)
        {
            // iterate backwards to find first children in z-order
            for (int i = this->children_num - 1; i >= 0; i--)
            {
                aObject* child = this->children[i];
                aObject* found = child->vtable->findObject(child, pos_x, pos_y);
                if (found != nullptr)
                {
                    //HOOK_DLOGF(AOBJECT, INFO, "aObject::findObject(%p,%d,%d) -> %p", this, pos_x, pos_y, found);
                    return found;
                }
            }
        }
    }

    if (this->showing != 0 
        && this->display_port != nullptr
        && this->vtable->pointInside(this, pos_x, pos_y) != FALSE)
    {
        //HOOK_DLOGF(AOBJECT, INFO, "aObject::findObject(%p,%d,%d) -> (self)", this, pos_x, pos_y);
        return this;
    }

    //HOOK_DLOGF(AOBJECT, INFO, "aObject::findObject(%p,%d,%d) -> NOT_FOUND", this, pos_x, pos_y);
    return nullptr;
}

aObject* aObject::foremostChild(long find_depth)
{
    if (this->children_num <= 0)
    {
        return nullptr;
    }

    for (int i = 0; i < this->children_num; i++)
    {
        aObject* child = this->children[i];
        if (child->depth == find_depth)
        {
            return child;
        }
    }

    return nullptr;
}

void aObject::HideMe(BOOL hide)
{
    if (this->hidden == hide || this->original_x != 0)
    {
        return;
    }

    if (hide != 0)
    {
        this->original_x = this->vtable->globalX(this);
        this->original_y = this->vtable->globalY(this);
        switch (this->hide_direction)
        {
        case DIRECTION_LEFT:
            {
                int32_t global_x = this->vtable->globalX(this);
                int32_t this_width = this->vtable->getWidth(this);
                this->hidden = hide;
                this->original_x = -(global_x + this_width);
            }
            return;
        case DIRECTION_UP:
            {
                int32_t global_y = this->vtable->globalY(this);
                int32_t this_height = this->vtable->getHeight(this);
                this->hidden = hide;
                this->original_x = -(global_y + this_height);
            }
            return;
        case DIRECTION_RIGHT:
            {
                int32_t global_x = this->vtable->globalX(this);
                aObject* application = reinterpret_cast<aObject*>(g_application);
                int32_t app_width = application->vtable->getWidth(application);
                this->animation_offset = app_width - global_x;
                this->hidden = hide;
            }
            return;
        case DIRECTION_DOWN:
            {
                int32_t global_y = this->vtable->globalY(this);
                aObject* application = reinterpret_cast<aObject*>(g_application);
                int32_t app_height = application->vtable->getHeight(application);
                this->animation_offset = app_height - global_y;
                this->hidden = hide;
            }
            return;
        default:
            this->hidden = FALSE;
            this->original_x = 0;
            return;
        }
    }

    int32_t global_x = this->vtable->globalX(this);

    if (this->original_x != global_x)
    {
        this->hidden = FALSE;
        this->original_x -= global_x;
        return;
    }

    this->hidden = FALSE;
    this->original_x = original_y - this->vtable->globalY(this);
}

void aObject::moveTo(int32_t new_x, int32_t  new_y, int flag)
{
    this->x = new_x;
    this->y = new_y;

    aObject* parent = this->parent;
    int32_t glob_x = 0;
    int32_t glob_y = 0;

    if (parent != nullptr)
    {
        glob_x = parent->vtable->globalX(parent);
        glob_y = parent->vtable->globalY(parent);
    }

    _pane* d = this->display_port;
    d->x0 = glob_x + new_x;
    d->y0 = glob_y + new_y;
    d->x1 = (this->width - 1) + d->x0;
    d->y1 = (this->height - 1) + d->y0;

    if (flag == 0)
    {
        this->original_x = new_x;
        this->original_y = new_y;
    }

    if (this->children_num > 0)
    {
        for (int i = 0; i < this->children_num; i++)
        {
            aObject* child = this->children[i];
            int32_t child_x = child->vtable->getX(child);
            int32_t child_y = child->vtable->getY(child);
            child->vtable->moveTo(child, child_x, child_y, 0);
        }
    }
}

void aObject::drawBox(uint8_t color, RECT rect)
{
    this->vtable->drawBox2(this, color, rect.left, rect.top, rect.right, rect.bottom);
}

void aObject::drawBox2(uint8_t color, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    if (left == -1)
    {
        left = 0;
    }

    if (top == -1)
    {
        top = 0;
    }

    if (right == -1)
    {
        right = this->vtable->getWidth(this) - 1;
    }

    if (bottom == -1)
    {
        bottom = this->vtable->getHeight(this) - 1;
    }

    _pane* dst = this->render_port->frame;

    // top,left -> top,right (upper edge)
    _VFX_draw_line_PTR(dst, left, top, right, top, 0, color);
    // top,left -> bottom,lef (left edge)
    _VFX_draw_line_PTR(dst, left, top, left, bottom, 0, color);
    // bottom,left -> bottom,right (bottom edge)
    _VFX_draw_line_PTR(dst, left, bottom, right, bottom, 0, color);
    // top,right -> bottom,right (right edge)
    _VFX_draw_line_PTR(dst, right, top, right, bottom, 0, color);
}

void aObject::drawFramed(BOOL inverted_frame, BOOL wipe)
{
    int inset1 = inverted_frame ? 12 : 4;
    int inset2 = inverted_frame ? 10 : 6;

    inset1 = 32;

    if (wipe != FALSE && this->back_color != 0xff)
    {
        long bg_color = this->vtable->backColor(this);
        _VFX_pane_wipe_PTR(
            this->render_port->frame,
            bg_color
        );
    }

    _pane* dst = this->render_port->frame;
    
    int right = this->vtable->getWidth(this) - 1;
    int bottom = this->vtable->getHeight(this) - 1;

    // outer frame
    _VFX_draw_line_PTR(dst, 0, 0, right, 0, 0, 0); 
    _VFX_draw_line_PTR(dst, 0, 0, 0, bottom, 0, 0);
    _VFX_draw_line_PTR(dst, 0, bottom, right, bottom, 0, 0);
    _VFX_draw_line_PTR(dst, right, 0, right, bottom, 0, 0);

    // inner 1-pixel frame
    _VFX_draw_line_PTR(dst, 1, 1, right - 2, 1, 0, 0);
    _VFX_draw_line_PTR(dst, 1, 1, 1, bottom - 2, 0, 0);
    _VFX_draw_line_PTR(dst, 1, bottom - 1, right - 2, bottom - 1, 0, 0);
    _VFX_draw_line_PTR(dst, right - 1, 1, right - 1, bottom - 2, 0, 0);

    // inner 2-pixel frame
    _VFX_draw_line_PTR(dst, 2, 2, right - 3, 2, 0, 0);
    _VFX_draw_line_PTR(dst, 2, 2, 2, bottom - 3, 0, 0);
    _VFX_draw_line_PTR(dst, 2, bottom - 2, right - 3, bottom - 2, 0, 0);
    _VFX_draw_line_PTR(dst, right - 2, 2, right - 2, bottom - 3, 0, 0);

    // bevel lines based on frame style
    _VFX_draw_line_PTR(dst, inset1, inset1, right - inset1, inset1, 0, 0);
    _VFX_draw_line_PTR(dst, inset2, inset2, right - inset2, inset2, 0, 0);
    _VFX_draw_line_PTR(dst, inset1, inset1, inset1, bottom - inset1, 0, 0);
    _VFX_draw_line_PTR(dst, inset2, inset2, inset2, bottom - inset2, 0, 0);
}

void aObject::FillBox(short top, short right, short bottom, short left, uint8_t color)
{
    _pane* pane = this->render_port->frame;

    // local copy of the pane rect
    _pane rect = *pane  ;

    // override edges
    rect.x0 = left;
    rect.y0 = top;
    rect.x1 = right;
    rect.y1 = bottom;

    // fill the region
    _VFX_pane_wipe_PTR(&rect, color);
}

long aObject::setAnimation(char* filename)
{
    if (this->animation != nullptr)
    {
        this->animation->reset();
        if (this->animation != nullptr)
        {
            delete this->animation;
        }
        this->animation = nullptr;
    }

    this->animation = new aAnimation();
    return this->animation->init(filename);
}

long aObject::setBackground(char* filename)
{
    if (this->background_port != nullptr)
    {
        this->background_port->destroy();
        if (this->background_port != nullptr)
        {
            delete this->background_port;
        }
        this->background_port = nullptr;
    }

    this->background_port = new aPort();

    if (this->background_port == nullptr)
    {
        Fatal(0, (char*)"Not enough memory to create background port", nullptr);
    }

    return this->background_port->init3(filename);
}

long aObject::setBackground2(long art_packet)
{
    if (this->background_port != nullptr)
    {
        this->background_port->destroy();
        if (this->background_port != nullptr)
        {
            delete this->background_port;
        }
        this->background_port = nullptr;
    }

    this->background_port = new aPort();

    if (this->background_port == nullptr)
    {
        Fatal(0, (char*)"Not enough memory to create background port", nullptr);
    }

    return this->background_port->init2(art_packet);
}

void aObject::SetBit(long x_, long y_, uint8_t bit)
{
    if (x_ <= -1)
    {
        return;
    }

    int32_t width_ = this->vtable->getWidth(this);

    if (x_ < width_ && y_ > -1)
    {
        int32_t height_ = this->vtable->getHeight(this);
        if (y_ < height_)
        {
            uint8_t* pixels = this->render_port->getPixels();
            if (pixels != nullptr)
            {
                pixels[width_ * y_ + x_] = bit;
            }
        }
    }
}

void aObject::setDepth(long new_depth)
{
    aObject* parent = this->parent;

    if (parent != nullptr)
    {
        parent->vtable->removeChild(parent, this);
    }
    
    this->depth = new_depth;

    if (parent != nullptr)
    {
        parent->vtable->addChild(parent, this);
    }
}

void aObject::setDisplayPort(aPort* new_display_port)
{
    _window* bitmap = new_display_port->bitmap;
    this->display_port->window = bitmap;
    if (this->children_num > 0)
    {
        for (int i = 0; i < this->children_num; i++)
        {
            aObject* child = this->children[i];
            child->vtable->setDisplayPort(child, new_display_port);
        }
    }
}

void aObject::setEventRoutine(event_routine_t new_event_routine)
{
    this->event_routine = new_event_routine;
}

long aObject::setIcon(char* filename)
{
    if (this->icon != nullptr)
    {
        this->icon->reset();
        if (this->icon != nullptr)
        {
            delete this->icon;
        }
        this->icon = nullptr;
    }

    this->icon = new aAnimation();
    long init_res = this->icon->init(filename);

    if (init_res == NO_ERR)
    {
        this->icon_width = this->icon->getWidth();
        this->icon_height = this->icon->getHeight();
    }

    return init_res;
}

void aObject::paint()
{
    if (this->paint_routine != nullptr)
    {
        this->paint_routine(this);
    }
}

void aObject::handleEvent(aEvent* evt)
{
    if (evt->event_id == 0x1)
    {
        if (this->parent != nullptr)
        {
            this->vtable->FUN_0060ef20(this, 0);
            aRedrawScreen();
        }
    }
    else if (evt->event_id == 0x12 && this->children_num > 0)
    {
        // broadcast to children?
        for (size_t i = 0; i < this->children_num; i++)
        {
            aObject* child = this->children[i];
            child->vtable->handleEvent(child, evt);
        }
    }

    // TODO: requires aSystem
    if (this->state == ICONIZED)
    {
        switch (evt->event_id)
        {
        case 0x1:
            break;
        case 0x4:
            break;
        case 0x7:
            break;
        case 0x10:
            break;
        default:
            break;
        }
    }
}
#pragma endregion

#pragma region aSystem
long aSystem::startupDirectDraw(long width, long height, long bits_per_pixel)
{
    IDirectDraw** pp_ddraw = &this->ddraw1;
    IDirectDraw2** pp_ddraw2 = &this->ddraw2;

    *pp_ddraw = nullptr;
    *pp_ddraw2 = nullptr;

    long ddraw_create_res = DirectDrawCreate(nullptr, pp_ddraw, nullptr);

    if (ddraw_create_res != S_OK)
    {
        Fatal(ddraw_create_res, (char*)"Cannot initialize DirectDraw.", nullptr);
    }

    if (this->ddraw1->QueryInterface(IID_IDirectDraw2, (void**)pp_ddraw2) != S_OK)
    {
        Fatal(ddraw_create_res, (char*)"Cannot initialize DirectDraw2.", nullptr);
    }

    IDirectDraw2* ddraw = *pp_ddraw2;

    // Retrieve hardware and Hardware Emulation Layer (HEL) capabilities
    DDCAPS hal_caps;
    DDCAPS hel_caps;
    memset(&hal_caps, 0, sizeof(DDCAPS));
    memset(&hel_caps, 0, sizeof(DDCAPS));
    hal_caps.dwSize = sizeof(DDCAPS);
    hel_caps.dwSize = sizeof(DDCAPS);
    ddraw->GetCaps(&hal_caps, &hel_caps);

    // Setup bitmap infos
    BMI* bmi = &g_bmi;
    memset(bmi, 0, sizeof(BMI));
    bmi->biSize = sizeof(BMI);
    bmi->biWidth = width;
    bmi->biHeight = -height; // Top down DIB
    bmi->biPlanes = 1;
    bmi->biBitCount = 8; // 8-bit palettized
    bmi->biCompression = BI_RGB;

    // Initialize Logical palette
    LogicalPalette* logical_palette = &g_logicalPalette;
    assert(sizeof(LogicalPalette) == (sizeof(LOGPALETTE) + 255 * sizeof(PALETTEENTRY)));
    memset(logical_palette, 0, sizeof(LOGPALETTE) + 255 * sizeof(PALETTEENTRY)); // sizeof(LogicalPalette)
    logical_palette->palNumEntries = 256;
    logical_palette->palVersion = 0x300; // Windows palette version?

    g_DesktopDC = GetDC(this->window_handle);
    HDC desktop_dc = g_DesktopDC;
    auto system_palette = g_SystemPalette;
    GetSystemPaletteEntries(desktop_dc, 0, 256, system_palette);

    for (size_t i = 0; i < 256; i++)
    {
        logical_palette->palPalEntry[i].peRed = system_palette[i].peRed;
        logical_palette->palPalEntry[i].peGreen = system_palette[i].peGreen;
        logical_palette->palPalEntry[i].peBlue = system_palette[i].peBlue;
        logical_palette->palPalEntry[i].peFlags = PC_NONE;

        if (i >= 10 && i <= 245)
        {
            system_palette[i].peFlags = PC_NOCOLLAPSE;
        }
        else 
        {
            system_palette[i].peFlags = PC_NONE;
        }
    }

    g_hPalette = CreatePalette((LOGPALETTE*)logical_palette);
    g_holdPalette = SelectPalette(desktop_dc, g_hPalette, FALSE);
    RealizePalette(desktop_dc);

    g_OffscreenBufferDC = CreateCompatibleDC(desktop_dc);
    g_OffScreenhDIBSection = CreateDIBSection(
        g_OffscreenBufferDC, (BITMAPINFO*)bmi, DIB_RGB_COLORS, &g_DIBBits, 0, 0
    );

    g_OffScreenhOldBitmap = (HBITMAP)SelectObject(g_OffscreenBufferDC, g_OffScreenhDIBSection);
    SetStretchBltMode(g_OffscreenBufferDC, COLORONCOLOR);

    IDirectDrawSurface** pp_ddraw_surf1 = &this->ddraw_surf1;
    IDirectDrawSurface** pp_ddraw_attached_surf = &this->ddraw_attached_surf;
    IDirectDrawPalette** pp_ddraw_palette = &this->ddraw_palette;
    *pp_ddraw_surf1 = nullptr;
    *pp_ddraw_attached_surf = nullptr;
    *pp_ddraw_palette = nullptr;

    g_Width = width;
    g_Height = height;

    HRESULT res = DD_OK;

    // TODO: test in fullscreen
    g_DAT_007ab0d4 = 0;
    if (g_FullScreen != FALSE)
    {
        res = ddraw->SetCooperativeLevel(this->window_handle, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT);
        
        if (res != DD_OK)
        {
            Fatal(res, (char*) " Unable to Set Exclusive Mode ", nullptr);
        }
        
        ShowWindow(this->window_handle, SW_SHOW);

        res = ddraw->SetDisplayMode(width, height, bits_per_pixel, 0, 0);

        while (res == DDERR_WASSTILLDRAWING)
        {
            res = ddraw->SetDisplayMode(width, height, bits_per_pixel, 0, 0);
        }

        if (res != DD_OK)
        {
            Fatal(res, (char*)" Unable to Set Display Mode ", nullptr);
        }

        DDSURFACEDESC surf_desc;
        memset(&surf_desc, 0, sizeof(DDSURFACEDESC));
        surf_desc.dwSize = sizeof(DDSURFACEDESC);
        surf_desc.dwFlags = DDSD_BACKBUFFERCOUNT | DDSD_CAPS;
        surf_desc.dwBackBufferCount = 2;

        assert(surf_desc.dwFlags == 33);

        // Fullscreen primary surface with flipping (front + back buffers) stored in video memory
        surf_desc.ddsCaps.dwCaps = 
            DDSCAPS_PRIMARYSURFACE |
            DDSCAPS_FLIP |
            DDSCAPS_COMPLEX |
            DDSCAPS_VIDEOMEMORY;

        assert(surf_desc.ddsCaps.dwCaps == 16920);

        int fallback = 2;

        res = this->ddraw2->CreateSurface(&surf_desc, pp_ddraw_surf1, 0);

        if (res != DD_OK)
        {
            fallback = 1;
            surf_desc.dwBackBufferCount = 1;
            res = this->ddraw2->CreateSurface(&surf_desc, pp_ddraw_surf1, 0);

            if (res != DD_OK)
            {
                fallback = 0;
                surf_desc.dwBackBufferCount = 0;
                surf_desc.dwFlags = DDSD_CAPS;
                surf_desc.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_PRIMARYSURFACE;
                assert(surf_desc.ddsCaps.dwCaps == 16896);

                res = this->ddraw2->CreateSurface(&surf_desc, pp_ddraw_surf1, 0);

                if (res != DD_OK)
                {

                    surf_desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
                    res = this->ddraw2->CreateSurface(&surf_desc, pp_ddraw_surf1, 0);

                    if (res != DD_OK)
                    {
                        Fatal(res, (char*)"Unable to Create Primary Surface", nullptr);
                    }
                }
            }
        }
        
        g_DAT_007ab0d4 = fallback;

        if (fallback != 0)
        {
            DDSCAPS ddcaps;
            memset(&ddcaps, 0, sizeof(DDSCAPS));
            ddcaps.dwCaps = DDSCAPS_BACKBUFFER;
            this->ddraw_surf1->GetAttachedSurface(&ddcaps, pp_ddraw_attached_surf);
        }

        if (bits_per_pixel != 8)
        {
            g_DAT_007ab108 = 1;
            return 0;
        }

        res = this->ddraw2->CreatePalette(DDPCAPS_8BIT, system_palette, pp_ddraw_palette, 0);

        if (res != DD_OK)
        {
            Fatal(res, (char*)" Unable to Create Primary Palette ", nullptr);
        }

        res = this->ddraw_surf1->SetPalette(*pp_ddraw_palette);

        if (res == DDERR_SURFACELOST)
        {
            this->ddraw_surf1->Restore();
            res = this->ddraw_surf1->SetPalette(*pp_ddraw_palette);
        }

        if (res != DD_OK)
        {
            Fatal(res, (char*)" Unable to Attach Palette to Primary Surface ", nullptr);
        }
    }

    g_DAT_007ab108 = 1;
    return 0;
}

long aSystem::resetDirectDraw(long width, long height, long bit_depth)
{
    if (g_mouseThreadStarted != FALSE)
    {
        EnterCriticalSection(&g_MouseCritSec);
        g_InMouseCritSec = TRUE;
    }

    if (this->ddraw_surf1 != nullptr)
    {
        if (g_BitDepth == 8)
        {
            aLockScreen_ptr();
            _VFX_pane_wipe_PTR(g_screenPort->frame, 0);
        }
        else 
        {
            aLockScreen_ptr();
            _window* window = g_screenPort->frame->window;
            size_t len = window->height_zero_based * window->width_zero_based;
            uint8_t* pixels = window->pixels;
            memset(pixels, 0, len);
        }
        aUnlockScreen_ptr();
    }

    if (this->ddraw_surf1 != nullptr)
    {
        if (bit_depth != 16)
        {
            if (g_FullScreen == FALSE)
            {
                SetWindowLongA(
                    this->window_handle,
                    GWL_STYLE,
                    WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
                );
            }
            this->ddraw2->SetCooperativeLevel(this->window_handle, DDSCL_NORMAL);
        }

        this->ddraw_surf1->Release();
        this->ddraw_surf1 = nullptr;
    }

    if (this->ddraw_palette != nullptr)
    {
        this->ddraw_palette->Release();
        this->ddraw_palette = nullptr;
    }

    g_Width = width;
    g_Height = height;
    g_BitDepth = bit_depth;
    g_DAT_007ab0d4 = 0;
    this->ddraw_surf1 = nullptr; 
    this->ddraw_attached_surf = nullptr;

    if (g_FullScreen == FALSE)
    {
        // Update GDI palette entries 10-245
        // iStart/iStart index determines from what entry to update,
        // while logical_palette->palPalEntry[10] / bmi_->bmiColors[10]
        // what are the new colors to copy from (aka 10-245 in the logical palette).
        LogicalPalette* logical_palette = &g_logicalPalette;
        AnimatePalette(
            g_hPalette,
            DDPAL_FIRST_NON_SYSTEM_COLOR,
            DDPAL_NUM_NON_SYSTEM_COLOR,
            &logical_palette->palPalEntry[10]
        );
        BITMAPINFO* bmi_ = (BITMAPINFO*)&g_bmi;
        SetDIBColorTable(
            g_OffscreenBufferDC,
            DDPAL_FIRST_NON_SYSTEM_COLOR,
            DDPAL_NUM_NON_SYSTEM_COLOR,
            &bmi_->bmiColors[10]
        );
    }
    else
    {
        HRESULT res = this->ddraw2->SetCooperativeLevel(
            this->window_handle,
            DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT
        );

        if (res != DD_OK)
        {
            Fatal(res, (char*)" Unable to Set Exclusive Mode ", nullptr);
        }

        ShowWindow(this->window_handle, SW_SHOW);

        res = this->ddraw2->SetDisplayMode(width, height, bit_depth, 0, 0);

        if (res != DD_OK)
        {
            Fatal(res, (char*)" Unable to Set Display Mode ", nullptr);
        }

        DDSURFACEDESC surf_desc;
        memset(&surf_desc, 0, sizeof(DDSURFACEDESC));
        surf_desc.dwSize = sizeof(DDSURFACEDESC);
        surf_desc.dwFlags = DDSD_BACKBUFFERCOUNT | DDSD_CAPS;
        surf_desc.dwBackBufferCount = 2;

        assert(surf_desc.dwFlags == 33);

        // Fullscreen primary surface with flipping (front + back buffers) stored in video memory
        surf_desc.ddsCaps.dwCaps =
            DDSCAPS_PRIMARYSURFACE |
            DDSCAPS_FLIP |
            DDSCAPS_COMPLEX |
            DDSCAPS_VIDEOMEMORY;

        assert(surf_desc.ddsCaps.dwCaps == 16920);

        int fallback = 2;

        res = this->ddraw2->CreateSurface(&surf_desc, &this->ddraw_surf1, 0);

        if (res != DD_OK)
        {
            fallback = 1;
            surf_desc.dwBackBufferCount = 1;
            res = this->ddraw2->CreateSurface(&surf_desc, &this->ddraw_surf1, 0);

            if (res != DD_OK)
            {
                fallback = 0;
                surf_desc.dwBackBufferCount = 0;
                surf_desc.dwFlags = DDSD_CAPS;
                surf_desc.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_PRIMARYSURFACE;
                assert(surf_desc.ddsCaps.dwCaps == 16896);

                res = this->ddraw2->CreateSurface(&surf_desc, &this->ddraw_surf1, 0);

                if (res != DD_OK)
                {

                    surf_desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
                    res = this->ddraw2->CreateSurface(&surf_desc, &this->ddraw_surf1, 0);

                    if (res != DD_OK)
                    {
                        Fatal(res, (char*)"Unable to Create Primary Surface", nullptr);
                    }
                }
            }
        }
        g_Width = width;
        g_Height = height;
        g_DAT_007ab0d4 = fallback;

        //this->ddraw_attached_surf = nullptr;

        if (fallback != 0)
        {
            DDSCAPS ddcaps;
            memset(&ddcaps, 0, sizeof(DDSCAPS));
            ddcaps.dwCaps = DDSCAPS_BACKBUFFER;
            this->ddraw_surf1->GetAttachedSurface(&ddcaps, &this->ddraw_attached_surf);
        }

        if (this->ddraw_surf1 != nullptr)
        {
            if (g_BitDepth == 8)
            {
                aLockScreen_ptr();
                _VFX_pane_wipe_PTR(g_screenPort->frame, 0);
                aUnlockScreen_ptr();
            }
            else {
                aLockScreen_ptr();
                _window* window = g_screenPort->frame->window;
                size_t len = window->height_zero_based * window->width_zero_based;
                uint8_t* pixels = window->pixels;
                memset(pixels, 0, len);
                aUnlockScreen_ptr();
            }
        }

        if (bit_depth == 8)
        {
            res = this->ddraw2->CreatePalette(DDPCAPS_8BIT, g_SystemPalette, &this->ddraw_palette, 0);

            if (res != DD_OK)
            {
                Fatal(res, (char*)" Unable to Create Primary Palette ", nullptr);
            }

            res = this->ddraw_surf1->SetPalette(this->ddraw_palette);

            if (res == DDERR_SURFACELOST)
            {
                this->ddraw_surf1->Restore();
                res = this->ddraw_surf1->SetPalette(this->ddraw_palette);
            }

            if (res != DD_OK)
            {
                Fatal(res, (char*)" Unable to Attach Palette to Primary Surface ", nullptr);
            }
        }
    }

    g_DAT_007ab108 = 1;

    if (g_mouseThreadStarted != FALSE)
    {
        LeaveCriticalSection(&g_MouseCritSec);
        g_InMouseCritSec = FALSE;
    }

    return 0;
}

long aSystem::shutdownDirectDraw()
{
    if (this->ddraw2 != nullptr)
    {
        this->ddraw2->SetCooperativeLevel(this->window_handle, DDSCL_NORMAL);
        this->ddraw2->Release();
        this->ddraw2 = nullptr;
    }

    if (this->ddraw1 != nullptr)
    {
        this->ddraw1->Release();
        this->ddraw1 = nullptr;
    }

    SelectObject(g_OffscreenBufferDC, g_OffScreenhOldBitmap);
    DeleteDC(g_OffscreenBufferDC);
    DeleteObject(g_OffScreenhOldBitmap);
    DeleteObject(g_OffScreenhDIBSection);

    g_OffscreenBufferDC = nullptr;
    g_OffScreenhDIBSection = nullptr;
    g_OffScreenhOldBitmap = nullptr;
    g_DAT_007ab108 = 0;

    return 0;
}

int aSystem::tweakDDPalette(int pal_first_entry, int pal_entries_to_change, VFX_RGB* pal, int param_4)
{
    if (g_FullScreen != FALSE)
    { 
        assert(this->ddraw_palette != nullptr);
        assert(this->ddraw_surf1 != nullptr);

        DDSURFACEDESC desc{};
        desc.dwSize = sizeof(desc);
        HRESULT hr = this->ddraw_attached_surf->GetSurfaceDesc(&desc);
        assert(hr == DD_OK);

        assert(pal_first_entry >= 0);
        assert(pal_entries_to_change > 0);
        assert(pal_first_entry + pal_entries_to_change <= 256);

        HOOK_DLOGF(AOBJECT, INFO, "SetEntries palette=%p first=%d count=%d",
            this->ddraw_palette,
            pal_first_entry,
            pal_entries_to_change);

        DWORD pcaps = 0;
        this->ddraw_palette->GetCaps(&pcaps);
        auto log_palette = g_logicalPalette.palPalEntry;
        DWORD num_e = g_logicalPalette.palNumEntries;
        for (size_t i = 0; i < num_e; i++)
        {
            PALETTEENTRY pe = log_palette[i];
            HOOK_DLOGF(AOBJECT, INFO, "LogcalPalette[%d] = (%d,%d,%d,%d)", i, pe.peRed, pe.peGreen, pe.peBlue, pe.peFlags);
        }
    }
    auto orig = reinterpret_cast<int(__thiscall*)(aSystem*, int, int, VFX_RGB*, int)>(GetPatchedFuncOrigFromVA((void*)va_aSystem_tweakDDPalette));
    return orig(this, pal_first_entry, pal_entries_to_change, pal, param_4);
}
#pragma endregion

static void __cdecl aRedrawScreen()
{
    aObject* screen_window = g_screenWindow;
    int32_t num_children = screen_window->vtable->numberOfChildren(screen_window);

    if (num_children <= 0)
    {
        return;
    }

    for (size_t i = 0; i < num_children; i++)
    {
        aObject* child = screen_window->child(i);
        child->vtable->draw(child);
    }
}


void __cdecl InitWindowMode(void)
{
    if (g_FullScreen == FALSE) // alrady windowed
    {
        return;
    }

    if (g_mouseThreadStarted != FALSE)
    {
        EnterCriticalSection(&g_MouseCritSec);
        g_InMouseCritSec = TRUE;
    }

    g_FullScreen = FALSE;
    g_application->resetDirectDraw(g_Width, g_Height, 8);
    g_DAT_007ab108 = 0;
        
    WINDOWPLACEMENT* wp = &g_WindowPlacement;

    if (g_SavedPosition == FALSE)
    {
        memset(wp, 0, sizeof(WINDOWPLACEMENT));
        int32_t screen_w = GetSystemMetrics(SM_CXSCREEN);
        int32_t screen_h = GetSystemMetrics(SM_CYSCREEN);
        SetRect(
            &wp->rcNormalPosition,
            (screen_w - g_Width) / 2, // left
            (screen_h - g_Height) / 2, // top
            (screen_w + g_Width) / 2, // right
            (screen_h + g_Height) / 2 // bottom
        );
        AdjustWindowRect(
            &wp->rcNormalPosition,
            WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
            0
        );
    }

    assert(sizeof(WINDOWPLACEMENT) == 44);
    wp->length = sizeof(WINDOWPLACEMENT);
    wp->showCmd = TRUE;
    SetWindowPlacement(g_application->window_handle, wp);
    ShowWindow(g_application->window_handle, SW_SHOW);
    g_DAT_007ab108 = 1;

    if (g_mouseThreadStarted != FALSE)
    {
        LeaveCriticalSection(&g_MouseCritSec);
        g_InMouseCritSec = FALSE;
    }
}


static AutoFuncPatchRegister reg_aObject{
    "aObject",
    {
        { (void*)va_aRedrawScreen, (void*)aRedrawScreen },
        { (void*)va_aObject_ctor, (void*)Hook_aObject_ctor },
        { (void*)va_aObject_init, GetMemberPtr(&aObject::init)},
        { (void*)va_aObject_x, GetMemberPtr(&aObject::getX) },
        { (void*)va_aObject_y, GetMemberPtr(&aObject::getY) },
        { (void*)va_aObject_width, GetMemberPtr(&aObject::getWidth) },
        { (void*)va_aObject_height, GetMemberPtr(&aObject::getHeight) },
        { (void*)va_aObject_top, GetMemberPtr(&aObject::getTop) },
        { (void*)va_aObject_left, GetMemberPtr(&aObject::getLeft) },
        { (void*)va_aObject_right, GetMemberPtr(&aObject::getRight) },
        { (void*)va_aObject_bottom, GetMemberPtr(&aObject::getBottom) },
        { (void*)va_aObject_globalX, GetMemberPtr(&aObject::getGlobalX) },
        { (void*)va_aObject_globalY, GetMemberPtr(&aObject::getGlobalY) },
        { (void*)va_aObject_state, GetMemberPtr(&aObject::getState) },
        { (void*)va_aObject_IsShowing, GetMemberPtr(&aObject::isShowing) },
        { (void*)va_aObject_IsHidden, GetMemberPtr(&aObject::isHidden) },
        { (void*)va_aObject_dragging, GetMemberPtr(&aObject::isDragging) },
        { (void*)va_aObject_dragStartX, GetMemberPtr(&aObject::getDragStartX) },
        { (void*)va_aObject_dragStartY, GetMemberPtr(&aObject::getDragStartY) },
        { (void*)va_aObject_depth, GetMemberPtr(&aObject::getDepth) },
        { (void*)va_aObject_backColor, GetMemberPtr(&aObject::getBackColor) },
        { (void*)va_aObject_numberOfChildren, GetMemberPtr(&aObject::getNumberOfChildren) },
        { (void*)va_aObject_children, GetMemberPtr(&aObject::getChildren) },
        { (void*)va_aObject_port, GetMemberPtr(&aObject::getPort) },
        { (void*)va_aObject_background, GetMemberPtr(&aObject::getBackground) },
        { (void*)va_aObject_frame, GetMemberPtr(&aObject::getFrame) },
        { (void*)va_aObject_ptr, GetMemberPtr(&aObject::getPtr) },
        { (void*)va_aObject_icon, GetMemberPtr(&aObject::getIcon) },
        { (void*)va_aObject_GetCamera, GetMemberPtr(&aObject::getCamera) },
        { (void*)va_aObject_getDropTargetCount, GetMemberPtr(&aObject::getDropTargetCount) },
        { (void*)va_aObject_addChild, GetMemberPtr(&aObject::addChild)},
        { (void*)va_aObject_display, GetMemberPtr(&aObject::display)},
        { (void*)va_aObject_draw, GetMemberPtr(&aObject::draw)},
        { (void*)va_aObject_findObject, GetMemberPtr(&aObject::findObject)},
        { (void*)va_aObject_foremostChild, GetMemberPtr(&aObject::foremostChild)},
        { (void*)va_aObject_HideMe, GetMemberPtr(&aObject::HideMe)},
        { (void*)va_aObject_moveTo, GetMemberPtr(&aObject::moveTo)},
        { (void*)va_aObject_animation, GetMemberPtr(&aObject::getAnimation)},
        { (void*)va_aObject_child, GetMemberPtr(&aObject::child)},
        { (void*)va_aObject_drawBox, GetMemberPtr(&aObject::drawBox)},
        { (void*)va_aObject_drawBox2, GetMemberPtr(&aObject::drawBox2)},
        { (void*)va_aObject_drawFramed, GetMemberPtr(&aObject::drawFramed)},
        { (void*)va_aObject_FillBox, GetMemberPtr(&aObject::FillBox)},
        { (void*)va_aObject_setAnimation, GetMemberPtr(&aObject::setAnimation)},
        { (void*)va_aObject_setBackground, GetMemberPtr(&aObject::setBackground)},
        { (void*)va_aObject_setBackground2, GetMemberPtr(&aObject::setBackground2)},
        { (void*)va_aObject_SetBit, GetMemberPtr(&aObject::SetBit)},
        { (void*)va_aObject_setDisplayPort, GetMemberPtr(&aObject::setDisplayPort)},
        { (void*)va_aObject_setDepth, GetMemberPtr(&aObject::setDepth)},
        { (void*)va_aObject_setEventRoutine, GetMemberPtr(&aObject::setEventRoutine)},
        { (void*)va_aObject_setIcon, GetMemberPtr(&aObject::setIcon)},
        { (void*)va_aObject_paint, GetMemberPtr(&aObject::paint)},
    }
};

static AutoFuncPatchRegister reg_aSystem{
    "aSystem",
    {
        { (void*)va_aSystem_startupDirectDraw, GetMemberPtr(&aSystem::startupDirectDraw)},
        { (void*)va_aSystem_resetDirectDraw, GetMemberPtr(&aSystem::resetDirectDraw)},
        { (void*)va_aSystem_shutdownDirectDraw, GetMemberPtr(&aSystem::shutdownDirectDraw)},
        // { (void*)va_InitWindowMode, (void*)InitWindowMode},
        { (void*)va_aSystem_tweakDDPalette, GetMemberPtr(&aSystem::tweakDDPalette)},
    }
};
