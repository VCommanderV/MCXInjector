#include "mcx_inject.hpp"
#include "mcx/lib/heap.hpp"
#include "mcx/gui/a_port.hpp"

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

static AutoFuncPatchRegister reg_aPort{
    "aPort",
    {
        { (void*)va_aPort_ctor, (void*)Hook_aPort_ctor },
        { (void*)va_aPort_operator_new, (void*)Hook_aPort_new},
        { (void*)va_aPort_operator_delete, (void*)Hook_aPort_delete}
    }
};
