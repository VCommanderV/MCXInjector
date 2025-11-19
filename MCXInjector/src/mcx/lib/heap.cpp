#include <intrin.h>
#include "mcx_inject.hpp"
#include "mcx/lib/heap.hpp"
#include "mcx/lib/error.hpp"

static _SYSTEM_INFO* g_system_info = nullptr;
static DWORD DwAllocationGranularity = 0;

#pragma region HeapManager
MCXI_CTOR_HOOK Hook_HeapManager_ctor(HeapManager* this_ptr, void* edx_unused) {
	this_ptr->vtable = (void*)MCX_RUNTIME_ADDR(va_HeapManager_vftable);
	this_ptr->init();
	MOV_THIS_TO_EAX(this_ptr);
}

#pragma warning(disable:26495, justification : "Initialization happens in Hook_HeapManager_ctor" )
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
	this->base_addr = 0;
	this->mem_reserved = 0;
	this->total_size = 0;
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

	this->base_addr = (MemoryPtr)base_addr;

	if (base_addr == 0) {
		return ERR_HEAP_FAILED_ALLOC;
	}

	this->mem_reserved = 1;
	this->total_size = size;

	HOOK_DLOGF(HEAP, INFO, "Created heap this=%p size=%d base=%p", this, size, base_addr);

	return NO_ERR;
}

long HeapManager::commitHeap(size_t commit_size)
{
	HOOK_DLOGF(HEAP, INFO, "Committing heap this=%p size=%d", this, commit_size);

	long result = 0;

	if (commit_size == 0)
	{
		commit_size = this->total_size;
	}

	if (this->total_size < commit_size) {
		return ERR_HEAP_ALLOC_TOO_BIG;
	}

	size_t free_size = this->total_size - (uintptr_t)this->commit_ptr;

	if (free_size == 0)
	{
		return ERR_HEAP_OUT_OF_MEMORY;
	}

	if (free_size < commit_size) {
		commit_size = free_size;
	}

	// actually commit the memory
	void* base_addr = VirtualAlloc(this->base_addr, commit_size, MEM_COMMIT, PAGE_READWRITE);

	if (base_addr == this->base_addr)
	{
		// round to page size.
		// TODO: is this correct?
		this->commit_ptr = (void*)(
			(uintptr_t)this->commit_ptr + (commit_size / g_system_info->dwPageSize + 1) * g_system_info->dwPageSize
		);

		HOOK_DLOGF(HEAP, INFO, "Committed heap this=%p size=%d head=%p", this, commit_size, this->commit_ptr);

		//void* return_addr = _ReturnAddress();
		//this->owner = (uintptr_t)return_addr;

		// Track the return address of the "owner"
		uintptr_t curr_ebp;
		uintptr_t prev_ebp;
		uintptr_t return_addr;

		__asm
		{
			mov curr_ebp, esp
		}

		prev_ebp = *((uintptr_t*)curr_ebp);
		return_addr = *((uintptr_t*)(curr_ebp + 4));
		this->owner = return_addr;

		return NO_ERR;
	}

	DWORD last_err = GetLastError();
	char err_msg[216];
	sprintf(err_msg, "Could not commit %d bytes in Heap of size %d", commit_size, this->total_size);
	
	ErrorHandler(last_err, err_msg, nullptr);

	return ERR_HEAP_FAILED_COMMIT;
}

long HeapManager::decommitHeap(size_t decommit_size)
{
	HOOK_DLOGF(HEAP, INFO, "Decommitting heap this=%p size=%d", this, decommit_size);

	if (decommit_size == 0)
	{
		decommit_size = this->total_size;
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
		BOOL decommit_result = VirtualFree(this->base_addr, this->total_size, MEM_DECOMMIT);

		if (decommit_result == FALSE)
		{
			last_err = GetLastError();
			// TODO: Dassert?
		}
	}

	if (this->total_size != 0 && this->mem_reserved != 0 && this->base_addr != 0)
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

unsigned char* HeapManager::getHeapPtr()
{
	if (this->total_size == 0 || this->mem_reserved == 0 || this->base_addr == 0)
	{
		return nullptr;
	}
	return (unsigned char*)this->base_addr;
}

unsigned char HeapManager::heapType()
{
	return BASE_HEAP;
}

unsigned long HeapManager::getOwner()
{
	return this->owner;
}

uintptr_t HeapManager::tSize()
{
	return (uintptr_t)this->commit_ptr;
}

//int HeapManager::VMQuery(void* buf, VMQUERY* vm_query)
//{
//	auto mbi = new _MEMORY_BASIC_INFORMATION();
//
//	if (DwAllocationGranularity == 0)
//	{
//		auto sys_info = new _SYSTEM_INFO();
//		GetSystemInfo(sys_info);
//		DwAllocationGranularity = sys_info->dwAllocationGranularity;
//	}
//
//	memset(vm_query, 0, sizeof(VMQUERY));
//	auto mbi_size = VirtualQuery(buf, mbi, sizeof(_MEMORY_BASIC_INFORMATION)); // 28 byte on x86
//
//	if (mbi_size != sizeof(_MEMORY_BASIC_INFORMATION))
//	{
//		if (mbi->State != MEM_COMMIT)
//		{
//
//		}
//	}
//}
#pragma endregion

#pragma region HeapList
__forceinline HeapList* HeapList::GetGlobalHeapList() {
	auto global_heap_list_ptr = (void*)MCX_RUNTIME_ADDR(va_globalHeapList);
	return reinterpret_cast<HeapList*>(global_heap_list_ptr);
}

void HeapList::addHeap(HeapManager* heap)
{
	int i = 0;

	for (; i < MaxHeaps; i++)
	{
		if (this->heaps[i] == nullptr)
		{
			this->heaps[i] = heap;
			HOOK_DLOG(HEAP, INFO) << "Added heap 0x" << heap << " to heap list 0x" << this << " index " << i;
			break;
		}
	}
}

void HeapList::removeHeap(HeapManager* heap)
{
	int i = 0;

	for (; i < MaxHeaps; i++)
	{
		if (this->heaps[i] == heap)
		{
			this->heaps[i] = nullptr;
			HOOK_DLOG(HEAP, INFO) << "Removed heap 0x" << heap << " from heap list 0x" << this << " index " << i;
			break;
		}
	}
}
#pragma endregion

#pragma region UserHeap
MCXI_CTOR_HOOK Hook_UserHeap_ctor(UserHeap* this_ptr, void* edx_unused) {
	HeapManager* heap_manager = reinterpret_cast<HeapManager*>(this_ptr);
	new (heap_manager) HeapManager();
	this_ptr->vtable = (void*)MCX_RUNTIME_ADDR(va_UserHeap_vftable);
	this_ptr->heap_start = (HeapBlock*)0x0;
	this_ptr->heap_end = (HeapBlock*)0x0;
	this_ptr->first_block = (HeapBlock*)0x0;
	this_ptr->heap_size = 0;

#ifdef CHECK_HEAP
	this_ptr->malloc_fatals = TRUE;
#else
	this_ptr->malloc_fatals = FALSE;
#endif	

	this_ptr->malloc_fatals = 0;
	this_ptr->last_error = 0;
	MOV_THIS_TO_EAX(this_ptr);
}

#pragma warning(disable:26495, justification : "Initialization happens in Hook_UserHeap_ctor" )
UserHeap::UserHeap() : HeapManager()
{
	Hook_UserHeap_ctor(this, nullptr);
}
#pragma warning(default:26495)

void static __forceinline __fastcall Hook_UserHeap_dtor(UserHeap* this_ptr, void* edx_unused)
{
	this_ptr->destroy();
	// TODO: call HeapManager::destroy again?
}

UserHeap::~UserHeap()
{
	Hook_UserHeap_dtor(this, nullptr);
}

__forceinline void UserHeap::relink(HeapBlockPtr new_block)
{
	auto first_block = this->first_block;

	if (first_block == nullptr) // empty free list
	{
		// Empty free list, create circular list with single element
		first_block = new_block;
		new_block->prev = new_block;
		new_block->next = new_block;
		this->first_block = new_block;
		return;
	}

	auto curr = first_block;

	// If new_block is smaller than the current first_block,
	// it becomes the new first (smallest) block.
	if (new_block->block_size < curr->block_size)
	{
		first_block = new_block;
	}
	else
	{
		do
		{
			curr = curr->next;
			if (curr == first_block) break;
		} while (curr->block_size < new_block->block_size);
	}

	// Insert the new block
	curr = curr->prev;
	new_block->next = curr->next;
	new_block->prev = curr;
	curr->next = new_block;
	new_block->next->prev = new_block;

	this->first_block = first_block;
}

__forceinline void UserHeap::unlink(HeapBlockPtr block_to_unlink)
{
	HeapBlockPtr first_block = this->first_block;

	if (block_to_unlink->next == block_to_unlink) {
		this->first_block = nullptr;
		return;
	}

	if (block_to_unlink == first_block)
	{
		first_block = block_to_unlink->next;
	}

	HeapBlockPtr next = block_to_unlink->next;
	HeapBlockPtr prev = block_to_unlink->prev;
	next->prev = prev;
	prev->next = next;

	this->first_block = first_block;
}

__forceinline void UserHeap::sort(HeapBlock* block)
{
	size_t block_size = block->block_size;
	HeapBlock* next_ptr = block->next;
	HeapBlock* prev_ptr = block->prev;

	bool relink_needed = false;

	if (next_ptr == prev_ptr) {
		// We've only 2 elements in the list (curr_block and one other)

		if (next_ptr != block && next_ptr->block_size < block_size) {
			// The other block is now smaller than us, so a relink is needed
			relink_needed = true;
		}
	}
	else
	{
		// Multiple elements, check if the ordering is still correct
		bool next_ok = (block_size <= next_ptr->block_size) ||
			(next_ptr == this->first_block);

		bool prev_ok = (prev_ptr->block_size <= block_size) ||
			(block == this->first_block);

		if (!next_ok || !prev_ok) {
			relink_needed = true;
		}
	}

	if (relink_needed)
	{
		this->unlink(block);
		this->relink(block);
	}
}

long UserHeap::init(size_t init_size, char* heap_name)
{
	HOOK_DLOGF(HEAP, INFO, "Init global heap ptr=%p size=%d name='%s'", this, init_size, heap_name);

	if (heap_name != nullptr)
	{
		this->heap_name = (char*)std::malloc(strnlen_s(heap_name, 249) + 1);
		strcpy(this->heap_name, heap_name);
	}
	else
	{
		this->heap_name = nullptr;
	}

	// Allocate and commit the heap which will result in a single free block.
	long res = this->createHeap(init_size);

	if (res != NO_ERR)
	{
		return res;
	}

	res = this->commitHeap(init_size);

	if (res != NO_ERR)
	{
		return res;
	}

#pragma region Track owner
	// Track the return address of who is creating this heap
	//uintptr_t curr_ebp;
	//uintptr_t prev_ebp;
	//uintptr_t return_addr;

	//__asm
	//{
	//	mov curr_ebp, esp
	//}

	//prev_ebp = *((uintptr_t*)curr_ebp);
	//return_addr = *((uintptr_t*)(curr_ebp + 4));
	//this->owner = return_addr;

	// x86 only
	unsigned long caller_ret_addr = 0;
	
	__asm {
		mov eax, [ebp + 4]         // Our return address
		mov caller_ret_addr, eax
	}

	// void** frame_ptr = (void**)_AddressOfReturnAddress();
	// caller_ret_addr = (unsigned long)frame_ptr[0];

	// GCC/Clang: __builtin_return_address(0) = our return address
	//            __builtin_return_address(1) = caller's return address
	// caller_ret_addr = (unsigned long)__builtin_return_address(1);

	this->owner = caller_ret_addr;
#pragma endregion

	uintptr_t end_addr = (uintptr_t)this->base_addr + init_size - sizeof(HeapBlock);
	end_addr &= 0xfffffffc;  // Align to 4-byte (DWORD) boundary
	
	HeapBlock* free_blk_start = (HeapBlock*)this->base_addr;
	HeapBlock* free_blk_end = (HeapBlock*)end_addr;

	this->heap_start = free_blk_start;
	this->heap_end = free_blk_end;

	/*HOOK_LOGF(HEAP, INFO, "heap_end 0x%p", heap_start);
	HOOK_LOGF(HEAP, INFO, "free_block_start 0x%p", free_blk_start);
	HOOK_LOGF(HEAP, INFO, "free_block_end 0x%p", free_blk_end);*/

	// actual block size, minus header and footer
	free_blk_start->block_size = (uintptr_t)free_blk_end - (uintptr_t)free_blk_start;
	free_blk_start->upper_phys_block = nullptr;

	free_blk_end->block_size = 1;			            // Sentinel marker
	free_blk_end->prev = (HeapBlock*)0x1572;		    // Boundary tag
	free_blk_end->upper_phys_block = free_blk_start;  // Points to previous physical block

	// +1 skips 16 bytes so we do NOT overwrite the block header
	void* fill_start = (void*)(free_blk_start + 1);
	size_t fill_size = (uintptr_t)free_blk_end - (uintptr_t)fill_start;
	memset(fill_start, 0xff, fill_size);

	this->first_block = nullptr;

	this->relink(free_blk_start);

	//auto relinkOrig = (void(__thiscall*)(UserHeap*, HeapBlockPtr))(MCX_RUNTIME_ADDR(va_UserHeap_relink));
	//relinkOrig(this, this->heap_start);

	this->heap_size = init_size;

	return NO_ERR;
}

void UserHeap::destroy()
{
	HeapManager::destroy();

	this->heap_start = nullptr;
	this->heap_end = nullptr;
	this->first_block = nullptr;
	this->heap_size = 0;
	
	if (this->heap_name != nullptr)
	{
		std::free(this->heap_name);
		this->heap_name = nullptr;
	}

	this->last_error = NO_ERR;
}

size_t UserHeap::coreLeft()
{
	if (this->last_error == NO_ERR && this->first_block != nullptr)
	{
		return this->first_block->prev->block_size - 8;
	}
	return 0;
}

size_t UserHeap::totalCoreLeft()
{
	size_t max_iters = 10000;

	if (this->last_error != NO_ERR)
	{
		return 0;
	}

	HeapBlock* curr_block = this->first_block;
	size_t size = 0;

	do
	{
		curr_block = curr_block->next;
		size += curr_block->block_size - USED_BLOCK_HEAD_SIZE;
		max_iters += 1;
		
		if (max_iters == 0) 
		{
			return 0;
		}
	} while (curr_block != this->first_block);

	return size;
}

long UserHeap::getLastError()
{
	return this->last_error;
}

void* UserHeap::malloc(size_t mem_size)
{
	HeapBlock* first_block = this->first_block;

	HeapBlock* allocated_block = (HeapBlock*)((char*) - USED_BLOCK_HEAD_SIZE);

	if (mem_size == 0) {
		return nullptr;
	}

	// Apply alignment (mem_size + (11)) & ~3
	// +11 comes from:
	//   - +8 for HeapBlock header fields that get reused in ALLOCATED blocks (block_size + upper_phys_block)
	//   - +3 for rounding up to next 4-byte boundary
	// The & NOT 3 clears bottom 2 bits (rounds to multiple of 4 aka DWORD)
	size_t mem_size_align = (mem_size + 0xb) & 0xfffffffc;

	// Enforce minimum block size of 16 bytes to hold a full HeapBlock when block is FREE.
	if (mem_size_align < sizeof(HeapBlock)) {
		mem_size_align = sizeof(HeapBlock);
	}

	// Overflow check to ensure alignment didn't wrap around
	if (mem_size > mem_size_align) {
		return nullptr;
	}

	// Empty free list?
	if (first_block == nullptr) {
		return nullptr;
	}

	// Search the free-list for a suitable block

	HeapBlock* curr_block = first_block;

	do
	{
		if (curr_block->block_size >= mem_size_align) 
		{
			// Found a suitable block!
			size_t curr_block_avail_size = curr_block->block_size - mem_size_align;

			// now, there are 2 possible scenarios:
			//   1) remainder < sizeof(HeapBlock) => too small to split
			//	 2) remainder >= sizeof(HeapBlock) => SPIT the block

			// Case 1 => allocate full block
			if (curr_block_avail_size < sizeof(HeapBlock))
			{
				// HOOK_LOGF(HEAP, INFO, "Malloc, alloc full block=%p allocd=%d avail=%d", curr_block, mem_size_align, curr_block_avail_size);
				
				// Mark entire block as allocated
				curr_block->block_size |= 1;

				// unlink from the free-list
				this->unlink(curr_block);

				// Update the local copy after unlink modified this->first_block
				first_block = this->first_block;

				allocated_block = curr_block;

				break;
			}
			// Case 2 => split the block:
			//   - Lower part remains free (with new smaller size)
			//   - Upper part becomes allocated
			// 

			// Update the lower neighbor back pointer
			// Find the block AFTER the current block and update its upper physical block
			// because it is not us no more.

			// HOOK_LOGF(HEAP, INFO, "Malloc, split block=%p allocd=%d avail=%d", curr_block, mem_size_align, curr_block_avail_size);

			HeapBlock* lower_neighbor = (HeapBlock*)((char*)curr_block + curr_block->block_size);
			lower_neighbor->upper_phys_block = (HeapBlock*)((char*)curr_block + curr_block_avail_size);

			// shrink the free block
			curr_block->block_size = curr_block_avail_size;

			// Create the allocated block in the upper portion
			allocated_block = (HeapBlock*)((char*)curr_block + curr_block_avail_size);
			allocated_block->block_size = mem_size_align | 1;  // Mark as allocated
			allocated_block->upper_phys_block = curr_block;

			// Check if we need to relink the block
			this->sort(curr_block);

			break;
		}

		curr_block = curr_block->next;
	} 
	while (curr_block != first_block);  // Stop when we wrap around

	this->first_block = first_block;

	// adjust to pointer to point AFTER block header.
	return (void*)((char*)allocated_block + USED_BLOCK_HEAD_SIZE);
}

long UserHeap::free(void* mem_block)
{
	HOOK_LOG(HEAP, WARNING) << "Start free block = 0x" << mem_block;
	if (mem_block == nullptr)
	{
		return NO_ERR; // no-op
	}

	void* heap_base = this->getHeapPtr();
	void* heap_end = (void*)((char*)heap_base + this->total_size);

	if (mem_block < heap_base || mem_block >= heap_end) {
		// Pointer is outside heap bounds so something is off, bail.
		HOOK_LOG(HEAP, WARNING) << "Tried to free a pointer outside the heap.";
		return NO_ERR; // we just let this go?
	}

	// Go from user ptr to block ptr
	HeapBlock* freed_block = (HeapBlock*)((char*)mem_block - USED_BLOCK_HEAD_SIZE);

	BOOL merge_result = this->mergeWithLower(freed_block);

#ifdef SAFE_HEAP // bail here in case
	if (merge_result == FALSE) {
		return NO_ERR;
	}
#endif

	// Merge with the previuos physical block if free
	// NOTE: this is separed because mergeWithLower() coalesce downwards.
	HeapBlock* block_above = freed_block->upper_phys_block; // mem_block -4

	if (block_above != nullptr && block_above->block_size == 0)
	{
		// There's a physically close block which is free, do the merge
		// 
		// Get the freed block actual size (clear allocation flag)
		size_t freed_size = freed_block->block_size & 0xfffffffe;

		// Update the lower neighbor back pointer.
		// The block after freed_block now has block_above as its upper neighbor
		
		HeapBlock* lower_neighbor = (HeapBlock*)((char*)block_above + block_above->block_size);
		lower_neighbor->upper_phys_block = block_above;

		this->sort(block_above);
	}
	else 
	{
		// No upper block, simply mark the block as free and add to free list

		// Clear allocation flag (mark as free)
		freed_block->block_size &= 0xfffffffe;

		// Add to the free-list
		this->relink(freed_block);
	}

	HOOK_LOG(HEAP, WARNING) << "End free block = 0x" << mem_block << " result=" << merge_result;

#ifdef CHECK_HEAP
	this->walkHeap(FALSE, FALSE, " Free Heap Ck ");
#endif

	return merge_result;
}

void UserHeap::walkHeap(BOOL print, BOOL skip_allocated, const char* message)
{
	//auto walkHeapOrig = (void(__thiscall*)(UserHeap*, int, int, char*))MCX_RUNTIME_ADDR(va_UserHeap_walkHeap);
	//walkHeapOrig(this, param_1, param_2, message);

	if (!this->heap_start || this->last_error != NO_ERR)
	{
		return;
	}

	HeapBlock* curr_block = this->heap_start;

	unsigned int heap_sane = TRUE;

	while (curr_block->block_size != 1) {  // While not last block sentinel
		HeapBlock* block_ptr = nullptr;

		if (curr_block->upper_phys_block != nullptr)
		{
			block_ptr = (HeapBlock*)
				((int)&curr_block->upper_phys_block->block_size +
					(curr_block->upper_phys_block->block_size & 0xfffffffe));

			heap_sane = block_ptr == curr_block;
		}

		if (heap_sane == FALSE)
		{
			this->last_error = ERR_HEAP_CORRUPTED;
			STOP(ERR_HEAP_CORRUPTED, "%s %s\n", "Heap check failed.\n", this->heap_name);
			return;
		}

		block_ptr = (HeapBlock*)((int)&curr_block->block_size + (curr_block->block_size & 0xfffffffe));

		heap_sane = block_ptr->upper_phys_block == curr_block;

		if (heap_sane == FALSE || (curr_block->block_size & 1) != 0)
		{
			if (heap_sane == FALSE)
			{
				this->last_error = ERR_HEAP_CORRUPTED;
				STOP(ERR_HEAP_CORRUPTED, "%s %s\n", "Heap check failed.\n", this->heap_name);
				return;
			}
		}
		else 
		{
			heap_sane = (curr_block->prev->next == curr_block) && (curr_block->next->prev == curr_block);
		}

		if (heap_sane == FALSE)
		{
			this->last_error = ERR_HEAP_CORRUPTED;
			STOP(ERR_HEAP_CORRUPTED, "%s %s\n", "Heap check failed.\n", this->heap_name);
			return;
		}

		BOOL allocated = curr_block->block_size & 1;

		if (((print != FALSE) && (allocated == FALSE)) || ((print != FALSE && (skip_allocated == FALSE))))
		{
			char err_msg[256];
			sprintf(err_msg, "%s block at DS:%08X, size = %u",
				(allocated) ? "Allocated" : "Free", curr_block, curr_block->block_size & 0xfffffffe);
			OutputDebugStringA(err_msg);
		}

		// Advance to next physical block
		curr_block = (HeapBlock*)(
			(char*)curr_block + 
			(curr_block->block_size & ~1) // adjust allocation flag to get the real size
		);
	}
}

unsigned char UserHeap::heapType()
{
	return USER_HEAP;
}

BOOL UserHeap::mergeWithLower(HeapBlockPtr freed_block)
{
	size_t block_size = freed_block->block_size;

#ifdef SAFE_HEAP
	// 1. Block must be marked as allocated
	if ((block_size & 1) == 0) {
		this->walkHeap(FALSE, FALSE, (char*)MCX_RUNTIME_ADDR(0x0078e1bc)); // merge failed;
		return FALSE;
	}

	size_t actual_size = block_size & 0xfffffffe;  // Clear allocation flag
	HeapBlock* next_block = (HeapBlock*)((char*)freed_block + actual_size);

	// 2. The next physical block should point back to back to us
	if (next_block->upper_phys_block != freed_block) {
		this->walkHeap(FALSE, FALSE, (char*)MCX_RUNTIME_ADDR(0x0078e1bc)); // merge failed;
		return FALSE;
	}

	// 3.If there's a previous physical block verify it correctly points to us
	HeapBlock* upper_block = freed_block->upper_phys_block;
	if (upper_block != nullptr) {
		// Calculate where upper_block thinks the next block should be
		size_t upper_size = upper_block->block_size & 0xfffffffe;
		HeapBlock* upper_next = (HeapBlock*)((char*)upper_block + upper_size);

		if (upper_next != freed_block) {
			this->walkHeap(FALSE, FALSE, (char*)MCX_RUNTIME_ADDR(0x0078e1bc)); // merge failed;
			return FALSE;
		}
	}
#endif

	// Clear allocation flag to get actual corrected size
	block_size &= 0xfffffffe;

	// Get the lower (next physical) neighbor block
	HeapBlock* lower_neighbor = (HeapBlock*)((char*)freed_block + block_size);
	size_t lower_size = lower_neighbor->block_size;

	// Check if lower neighbor is free (bit 0 = 0)
	if (lower_size & 1) {
		// Lower block is allocated, cannot merge
		return TRUE;
	}

	// Merge the blocks
	size_t new_size = block_size + lower_size;
	freed_block->block_size = new_size | 1;  // Mark as allocated

	// Update the new lower neighbor back pointer
	HeapBlock* new_lower_neighbor = (HeapBlock*)((char*)freed_block + new_size);
	new_lower_neighbor->upper_phys_block = freed_block;

	// remove the lower neighbor from the free list
	this->unlink(lower_neighbor);

	return TRUE;
}
#pragma endregion

static AutoFuncPatchRegister reg_HeapManager{
    "HeapManager",
    {
        { (void*)va_HeapManager_ctor, (void*)Hook_HeapManager_ctor},
        { (void*)va_HeapManager_init, GetMemberPtr(&HeapManager::init)},
		{ (void*)va_HeapManager_dtor, GetMemberPtr(&HeapManager::destroy)},
		{ (void*)va_HeapManager_createHeap, GetMemberPtr(&HeapManager::createHeap)},
		{ (void*)va_HeapManager_commitHeap, GetMemberPtr(&HeapManager::commitHeap)},
		{ (void*)va_HeapManager_destroy, GetMemberPtr(&HeapManager::destroy)},
		{ (void*)va_HeapManager_getHeapPtr, GetMemberPtr(&HeapManager::getHeapPtr)},
		{ (void*)va_HeapManager_heapType, GetMemberPtr(&HeapManager::heapType)},
		{ (void*)va_HeapManager_owner, GetMemberPtr(&HeapManager::getOwner)},
		{ (void*)va_HeapManager_tSize, GetMemberPtr(&HeapManager::tSize)},
    }
};		

static AutoFuncPatchRegister reg_UserHeap{
	"UserHeap",
	{
		{ (void*)va_UserHeap_ctor, (void*)Hook_UserHeap_ctor},
		{ (void*)va_UserHeap_dtor, (void*)Hook_UserHeap_dtor},
		{ (void*)va_UserHeap_init, GetMemberPtr(&UserHeap::init)},
		{ (void*)va_UserHeap_destroy, GetMemberPtr(&UserHeap::destroy)},
		{ (void*)va_UserHeap_coreLeft, GetMemberPtr(&UserHeap::coreLeft)},
		{ (void*)va_UserHeap_totalCoreLeft, GetMemberPtr(&UserHeap::totalCoreLeft)},
		{ (void*)va_UserHeap_getLastError, GetMemberPtr(&UserHeap::getLastError)},
		{ (void*)va_UserHeap_heapType, GetMemberPtr(&UserHeap::heapType)},
		{ (void*)va_UserHeap_relink, GetMemberPtr(&UserHeap::relink)},
		{ (void*)va_UserHeap_unlink, GetMemberPtr(&UserHeap::unlink)},
		{ (void*)va_UserHeap_mergeWithLower, GetMemberPtr(&UserHeap::mergeWithLower)},
		{ (void*)va_UserHeap_malloc, GetMemberPtr(&UserHeap::malloc)},
		{ (void*)va_UserHeap_free, GetMemberPtr(&UserHeap::free)},
		{ (void*)va_UserHeap_walkHeap, GetMemberPtr(&UserHeap::walkHeap)},
	}
};

static AutoFuncPatchRegister reg_HeapList{
	"HeapList",
	{
		{ (void*)va_HeapList_addHeap, GetMemberPtr(&HeapList::addHeap)},
		{ (void*)va_HeapList_removeHeap, GetMemberPtr(&HeapList::removeHeap)},
	}
};