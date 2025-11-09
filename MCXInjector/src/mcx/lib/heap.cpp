#include <intrin.h>
#include "mcx_inject.hpp"
#include "mcx/lib/heap.hpp"
#include "mcx/lib/error.hpp"

static _SYSTEM_INFO* g_system_info = nullptr;

MCXI_CTOR_HOOK Hook_HeapManager_ctor(HeapManager* this_ptr, void* edx_unused) {
	this_ptr->vtable = (void*)MCX_RUNTIME_ADDR(va_HeapManager_vftable);
	this_ptr->init();
	MOV_THIS_TO_EAX(this_ptr);
}

#pragma warning(disable:26495, justification : "Initialization happens in Hook_Heap_ctor" )
HeapManager::HeapManager()
{
	Hook_HeapManager_ctor(this, nullptr);
}
#pragma warning(default:26495)

HeapManager::~HeapManager()
{
	this->destroy();
}

void HeapManager::init()
{
	HOOK_DLOG(HEAP, INFO) << "Initializing heap this=" << this;
	this->base_addr = nullptr;
	this->heap_allocated = 0;
	this->heap_size = 0;
	this->commit_ptr = 0;
	HOOK_DLOG(HEAP, INFO) << "Initialized heap this=" << this;
}

long HeapManager::createHeap(size_t size)
{
	HOOK_DLOGF(HEAP, INFO, "Creating heap this=%p size=%d", this, size);

	if (g_system_info == nullptr) {
		g_system_info = new _SYSTEM_INFO();
		GetSystemInfo(g_system_info);
		HOOK_DLOG(HEAP, INFO) << "Retrieved system info pageSize=" << g_system_info->dwPageSize;
	}

	auto base_addr = VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_READWRITE);
	
	this->base_addr = base_addr;

	if (base_addr == 0) {
		return ERR_HEAP_FAILED_ALLOC;
	}

	this->heap_allocated = 1;
	this->heap_size = size;

	HOOK_DLOGF(HEAP, INFO, "Created heap this=%p size=%d base=%p", this, size, base_addr);

	return 0;
}

long HeapManager::commitHeap(size_t commit_size)
{
	HOOK_DLOGF(HEAP, INFO, "Committing heap this=%p size=%d", this, commit_size);

	long result = 0;

	if (commit_size == 0)
	{
		commit_size = this->heap_size;
	}

	if (this->heap_size < commit_size) {
		return ERR_HEAP_OUT_OF_MEMORY;
	}

	size_t free_size = this->heap_size - (uintptr_t)this->commit_ptr;

	if (free_size == 0)
	{
		return ERR_HEAP_FULLY_COMMITTED;
	}

	if (free_size < commit_size) {
		commit_size = free_size;
	}

	void* base_addr = VirtualAlloc(this->base_addr, commit_size, MEM_COMMIT, PAGE_READWRITE);

	if (base_addr == this->base_addr)
	{
		// TODO: is this correct?
		this->commit_ptr = (void*)(
			(uintptr_t)this->commit_ptr + (commit_size / g_system_info->dwPageSize + 1) * g_system_info->dwPageSize
		);

		HOOK_DLOGF(HEAP, INFO, "Committed heap this=%p size=%d head=%p", this, commit_size, this->commit_ptr);

		// Store return address at offset 0x14 for debugging/tracking ???
		void* return_addr = _ReturnAddress();
		*(void**)((char*)this + 0x14) = return_addr;

		return 0;
	}

	DWORD last_err = GetLastError();
	char err_msg[216];
	sprintf(err_msg, "Could not commit %d bytes in Heap of size %d", commit_size, this->heap_size);
	
	ErrorHandler(last_err, err_msg, nullptr);

	return ERR_HEAP_FAILED_COMMIT;
}

long HeapManager::decommitHeap(size_t decommit_size)
{
	HOOK_DLOGF(HEAP, INFO, "Decommitting heap this=%p size=%d", this, decommit_size);

	if (decommit_size == 0)
	{
		decommit_size = this->heap_size;
	}

	if ((uintptr_t)this->commit_ptr < decommit_size) {
		decommit_size = (size_t)commit_ptr;
	}

	this->commit_ptr = (void*)(
		(uintptr_t)this->commit_ptr - (decommit_size / g_system_info->dwPageSize) * g_system_info->dwPageSize
	);

	BOOL decommit_result = VirtualFree(this->commit_ptr, decommit_size, MEM_DECOMMIT);

	if (decommit_result == FALSE)
	{
		DWORD last_err = GetLastError();
		// TODO: Dassert?
	}

	HOOK_DLOGF(HEAP, INFO, "Decommitted heap this=%p size=%d", this, decommit_size);

	return 0;
}

void HeapManager::destroy()
{
	HOOK_DLOGF(HEAP, INFO, "Destroyin heap this=%p", this);

	int last_err = 0;

	if (this->commit_ptr != 0)
	{
		BOOL decommit_result = VirtualFree(this->base_addr, this->heap_size, MEM_DECOMMIT);

		if (decommit_result == FALSE)
		{
			last_err = GetLastError();
			// TODO: Dassert?
		}
	}

	if (this->heap_size != 0 && this->heap_allocated != 0 && this->base_addr != 0)
	{
		BOOL release_result = VirtualFree(this->base_addr, 0, MEM_RELEASE);

		if (release_result != TRUE)
		{
			last_err = GetLastError();
			// TODO: Dassert?
		}
	}

	HOOK_DLOGF(HEAP, INFO, "Destroyed heap this=%p", this);

	this->init();
}

static AutoFuncPatchRegister reg_HeapManager{
    "HeapManager",
    {
        { (void*)va_HeapManager_ctor, (void*)Hook_HeapManager_ctor},
        { (void*)va_HeapManager_init, GetMemberPtr(&HeapManager::init)},
		{ (void*)va_HeapManager_dtor, GetMemberPtr(&HeapManager::destroy)},
		{ (void*)va_HeapManager_createHeap, GetMemberPtr(&HeapManager::createHeap)},
		{ (void*)va_HeapManager_commitHeap, GetMemberPtr(&HeapManager::commitHeap)},
		{ (void*)va_HeapManager_destroy, GetMemberPtr(&HeapManager::destroy)},
    }
};