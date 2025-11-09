#pragma once
#include "mcx_inject.hpp"

#ifndef ENABLE_LOG_HEAP
#define ENABLE_LOG_HEAP 0
#endif

constexpr auto ERR_HEAP_OUT_OF_MEMORY = 0xbadd0007; // -0x4522fff9
constexpr auto ERR_HEAP_FAILED_COMMIT = 0xbadd0008; // -0x4522fff8
constexpr auto ERR_HEAP_FAILED_ALLOC = 0xbadd0009; // -0x4522fff7
constexpr auto ERR_HEAP_FULLY_COMMITTED = 0xbadd0001; // -0x4522ffff

class HeapManager {
public:
	void* vtable;			// 0x00
	void* base_addr;		// 0x04
	int32_t heap_allocated; // 0x08
	size_t heap_size;		// 0x0c
	void* commit_ptr;		// 0x10
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
};

class UserHeap {

};

#pragma region VAs
static constexpr uintptr_t va_systemHeap = 0x007aaf8c;

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

static constexpr uintptr_t va_UserHeap_vftable = 0x0077c324;
#pragma endregion

#pragma region Layout asserts
ASSERT_SIZE(HeapManager, 20);
ASSERT_ALIGN(HeapManager, 0x4);

ASSERT_FIELD_OFFSET(HeapManager, vtable, 0x0);
ASSERT_FIELD_OFFSET(HeapManager, base_addr, 0x04);
ASSERT_FIELD_OFFSET(HeapManager, heap_allocated, 0x08);
ASSERT_FIELD_OFFSET(HeapManager, heap_size, 0x0c);
ASSERT_FIELD_OFFSET(HeapManager, commit_ptr, 0x10);
#pragma endregion