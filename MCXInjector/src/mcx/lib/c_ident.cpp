#include "mcx_inject.hpp"
#include "mcx/lib/heap.hpp"
#include "mcx/lib/c_ident.hpp"

static void __fastcall Hook_FullPathFileName_dtor(FullPathFileName* this_ptr, void* unused_edx)
{
    this_ptr->destroy();
}

FullPathFileName::~FullPathFileName()
{
    this->destroy();
}

void __forceinline FullPathFileName::destroy()
{
    if (g_systemHeap != nullptr && g_systemHeap->heap_size != 0)
    {
        g_systemHeap->free(this->buf);
        this->buf = nullptr;
        return;
    }
    
    delete this->buf;
    this->buf = nullptr;
}

void FullPathFileName::init(char* path_1, char* path_2, char* path_3)
{
    this->destroy();
    size_t len_1 = strlen(path_1);
    size_t len_2 = strlen(path_2);
    size_t len_3 = path_3 ? strlen(path_3) : 0;
    size_t size = len_1 + len_2 + len_3 + 1;
    char* buf = mcx::heap::adaptive_malloc<char>(g_systemHeap, size);
    this->buf = buf;
    strcpy(this->buf, path_1);
    strcat(this->buf, path_2);
    if (path_3)
    {
        strcat(this->buf, path_3);
    }
}

static AutoFuncPatchRegister reg_FullPathFileName{
    "FullPathFileName",
    {
        { (void*)va_FullPathFileName_dtor, (void*)Hook_FullPathFileName_dtor},
        { (void*)va_FullPathFileName_destroy, GetMemberPtr(&FullPathFileName::destroy)},
        { (void*)va_FullPathFileName_init, GetMemberPtr(&FullPathFileName::init)},
    }
};