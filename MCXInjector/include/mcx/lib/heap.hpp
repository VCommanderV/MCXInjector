#pragma once
#include "mcx/lib/memory.hpp"
#include "mcx_inject.hpp"

#ifndef ENABLE_LOG_HEAP
#define ENABLE_LOG_HEAP 0
#endif

#ifndef NO_ERR
#define NO_ERR	0
#endif

#ifndef DEBUG
#define CHECK_HEAP
#define SAFE_HEAP
#endif

constexpr auto ERR_HEAP_OUT_OF_MEMORY = 0xbadd0001;	// -0x4522ffff
constexpr auto ERR_HEAP_CORRUPTED = 0xbadd0005;   // -0x4522fffb
constexpr auto ERR_HEAP_ALLOC_TOO_BIG = 0xbadd0007; // -0x4522fff9
constexpr auto ERR_HEAP_FAILED_COMMIT = 0xbadd0008;	// -0x4522fff8
constexpr auto ERR_HEAP_FAILED_ALLOC = 0xbadd0009;	// -0x4522fff7

static constexpr size_t MaxHeaps = 256;

static constexpr size_t USED_BLOCK_HEAD_SIZE = 8;

struct VMQUERY
{
	byte undefined[0x2c];
};

constexpr byte BASE_HEAP = 0;
constexpr byte USER_HEAP = 1;

struct HeapBlock {
	// This block size
	// 0x0
	size_t block_size;

	// the closest physical block at a lower memory address
	// 0x4
	HeapBlock* upper_phys_block;

	// The previous free block
	// 0x8
	HeapBlock* prev;

	// The next free block
	// 0x0c
	HeapBlock* next;
};

typedef HeapBlock*  HeapBlockPtr;
static constexpr size_t FREE_BLOCK_HEAD_SIZE = sizeof(HeapBlock);

class HeapManager {
public:
	void* vtable;			// 0x00
	MemoryPtr base_addr;	// 0x04
	BOOL mem_reserved;		// 0x08
	size_t total_size;		// 0x0c
	void* commit_ptr;		// 0x10
	unsigned long owner;    // 0x14
	HeapManager* next;		// 0x18
public:
	// void __thiscall HeapManager::HeapManager(void)
	HeapManager();

	// void __thiscall HeapManager::~HeapManager(void)
	~HeapManager();

	// void __thiscall HeapManager::init(void)
	void init();

	// void __thiscall HeapManager::destroy(void)
	void destroy();

	// long __thiscall HeapManager::createHeap(ulong)
	long createHeap(size_t size);

	// long __thiscall HeapManager::commitHeap(ulong)
	long commitHeap(size_t commit_size);

	// long __thiscall HeapManager::decommitHeap(ulong)
	long decommitHeap(size_t decommit_size);

	// unsigned char * __thiscall HeapManager::getHeapPtr(void)
	MemoryPtr getHeapPtr();

	// unsigned char __thiscall HeapManager::heapType(void)
	unsigned char heapType();

	// unsigned long __thiscall HeapManager::owner(void)
	unsigned long getOwner();

	// unsigned long __thiscall HeapManager::tSize(void)
	uintptr_t tSize();

	// __thiscall HeapManager::operator unsigned char * unsigned char * (void)
	operator MemoryPtr () {
		return this->getHeapPtr();
	}

	// void __thiscall HeapManager::MemoryDump(void)
	void MemoryDump();
protected:
	// int __thiscall HeapManager::VMQuery(void *, struct VMQUERY*)
	int VMQuery(void* buf, VMQUERY* vm_query);

	// char * __thiscall HeapManager::GetProtectText(unsigned long,char *,int)
	char* GetProtectText(unsigned long param_1, char* param_2, int param_3);

	// char const * __thiscall HeapManager::GetMemStorageText(unsigned long) */
	char const* GetMemStorageText(unsigned long param_1);

	// void __thiscall HeapManager::ConstructBlkInfoLine(struct VMQUERY *,char *,int)
	void ConstructBlkInfoLine(struct VMQUERY* vm_query, char* param_2, int param_3);

	// void __thiscall HeapManager::ConstructRgnInfoLine(struct VMQUERY *,char *,int)
	void ConstructRgnInfoLine(struct VMQUERY* vm_query, char* param_2, int param_3);
};


class UserHeap : public HeapManager {
public:
	HeapBlockPtr heap_start;
	HeapBlockPtr heap_end;
	HeapBlockPtr first_block;
	size_t heap_size;
	BOOL malloc_fatals;
	long last_error;
	char* heap_name;
public:
	UserHeap();

	~UserHeap();

	// long __thiscall InitNamedUserHeap_FUN_00647d40(size_t heap_size, char *heap_name)
	long init(size_t heap_size, char* heap_name);

	// long __thiscall UserHeap::free(void *) 
	long free(void* memory);

	// void* __thiscall UserHeap::malloc(size_t)
	void* malloc(size_t mem_size);

	// void __thiscall UserHeap::destroy(void)
	void destroy();

	// unsigned long __thiscall UserHeap::coreLeft(void)
	size_t coreLeft();

	// unsigned long __thiscall UserHeap::totalCoreLeft(void)
	size_t totalCoreLeft();

	// long __thiscall UserHeap::getLastError(void)
	// NOTE: inlined
	long getLastError();

	// void __thiscall UserHeap::walkHeap(int,int,char *)
	void walkHeap(BOOL param_1, BOOL param_2, const char* message);

	// virtual unsigned char __thiscall UserHeap::heapType(void)
	unsigned char heapType();

	// protected void __thiscall UserHeap::relink(struct HeapBlock *)
	__forceinline void relink(HeapBlockPtr new_block);

	// protected void __thiscall UserHeap::unlink(struct HeapBlock *)
	__forceinline void unlink(HeapBlockPtr block_to_unlink);

	// protected BOOL __thiscall UserHeap::mergeWithLower(struct HeapBlock *)
	BOOL mergeWithLower(HeapBlockPtr freed_block);
protected:
	__forceinline void sort(HeapBlock* block);
};


class HeapList {
public:
	HeapManager* heaps[MaxHeaps];
public:
	// void __thiscall HeapList::addHeap(class HeapManager *)
	void addHeap(HeapManager* heap);

	// void __thiscall HeapList::removeHeap(class HeapManager *)
	void removeHeap(HeapManager* heap);

	static HeapList* GetGlobalHeapList();
};

namespace mcx::heap
{
	// Mallocs in the specified heap if initialized or fallbacks to system heap.
	template<typename T>
	inline T* adaptive_malloc(UserHeap* heap, size_t bytes)
	{
		if (heap == nullptr || heap->heap_size == 0)
		{
			return static_cast<T*>(::operator new(bytes));
		}
		else
		{
			return static_cast<T*>(heap->malloc(bytes));
		}
	}

	// Frees the specified memory from the specified heap if initialized or fallbacks to system heap.
	template<typename T>
	inline void adaptive_free(UserHeap* heap, T* ptr)
	{
		if (ptr == nullptr)
		{
			return;
		}

		if (heap == nullptr || heap->heap_size == 0)
		{
			::operator delete(ptr);
		}
		else
		{
			heap->free(ptr);
		}
	}
}

#pragma region VAs
static constexpr uintptr_t va_systemHeap = 0x007aaf8c;
static constexpr uintptr_t va_guiHeap = 0x007aae78;

static constexpr uintptr_t va_globalHeapList = 0x007d8fb0;

static constexpr uintptr_t va_HeapManager_vftable = 0x0077c320;
static constexpr uintptr_t va_HeapManager_ctor = 0x00646ff0;
static constexpr uintptr_t va_HeapManager_dtor = 0x00646f20;
static constexpr uintptr_t va_HeapManager_heapType = 0x00646f40;
static constexpr uintptr_t va_HeapManager_destroy = 0x00646f50;
static constexpr uintptr_t va_HeapManager_init = 0x00647010;
static constexpr uintptr_t va_HeapManager_operator_unsigned_char_ptr = 0x00647050;
static constexpr uintptr_t va_HeapManager_getHeapPtr = 0x00647070;
static constexpr uintptr_t va_HeapManager_createHeap = 0x006470b0;
static constexpr uintptr_t va_HeapManager_commitHeap = 0x00647130;
static constexpr uintptr_t va_HeapManager_decommitHeap = 0x00647290;
static constexpr uintptr_t va_HeapManager_VMQuery = 0x006473d0;
static constexpr uintptr_t va_HeapManager_GetMemStorageText = 0x00647770;
static constexpr uintptr_t va_HeapManager_GetProtectText = 0x00647800;
static constexpr uintptr_t va_HeapManager_ConstructRgnInfoLine = 0x006479b0;
static constexpr uintptr_t va_HeapManager_ConstructBlkInfoLine = 0x00647ad0;
static constexpr uintptr_t va_HeapManager_MemoryDump = 0x00647b50;
static constexpr uintptr_t va_HeapManager_owner = 0x00649050;
static constexpr uintptr_t va_HeapManager_tSize = 0x00649070;
static constexpr uintptr_t va_HeapManager_systemInfo = 0x007d8fac;

static constexpr uintptr_t va_UserHeap_init = 0x00647d40;

static constexpr uintptr_t va_UserHeap_vftable = 0x0077c324;
static constexpr uintptr_t va_UserHeap_ctor = 0x00647cd0;
static constexpr uintptr_t va_UserHeap_dtor = 0x00647ed0;
static constexpr uintptr_t va_UserHeap_heapType = 0x00647d30;
static constexpr uintptr_t va_UserHeap_destroy = 0x00647f00;
static constexpr uintptr_t va_UserHeap_totalCoreLeft = 0x00647f70;
static constexpr uintptr_t va_UserHeap_coreLeft = 0x00647fd0;
static constexpr uintptr_t va_UserHeap_malloc = 0x00648020;
static constexpr uintptr_t va_UserHeap_free = 0x00648230;
static constexpr uintptr_t va_UserHeap_calloc = 0x006483b0;
static constexpr uintptr_t va_UserHeap_walkHeap = 0x006483c0;
static constexpr uintptr_t va_UserHeap_getLastError = 0x00648670;
static constexpr uintptr_t va_UserHeap_relink = 0x00648690;
static constexpr uintptr_t va_UserHeap_unlink = 0x00648700;
static constexpr uintptr_t va_UserHeap_sort = 0x00648760;
static constexpr uintptr_t va_UserHeap_mergeWithLower = 0x00648830;

static constexpr uintptr_t va_HeapList_addHeap = 0x006488f0;
static constexpr uintptr_t va_HeapList_removeHeap = 0x00648940;
#pragma endregion

#pragma region Globals
PTR_REF(UserHeap, g_systemHeap, va_systemHeap);
PTR_REF(UserHeap, g_guiHeap, va_guiHeap);
#pragma endregion

#pragma region Layout asserts
ASSERT_SIZE(HeapManager, 0x1c);
ASSERT_ALIGN(HeapManager, 0x4);

ASSERT_FIELD_OFFSET(HeapManager, vtable, 0x0);
ASSERT_FIELD_OFFSET(HeapManager, base_addr, 0x04);
ASSERT_FIELD_OFFSET(HeapManager, mem_reserved, 0x08);
ASSERT_FIELD_OFFSET(HeapManager, total_size, 0x0c);
ASSERT_FIELD_OFFSET(HeapManager, commit_ptr, 0x10);
ASSERT_FIELD_OFFSET(HeapManager, owner, 0x14);

ASSERT_SIZE(VMQUERY, 0x2c);

ASSERT_SIZE(HeapList, 0x400);
ASSERT_FIELD_OFFSET(HeapList, heaps, 0x0);
ASSERT_FIELD_OFFSET(UserHeap, heap_name, 0x34);
#pragma endregion