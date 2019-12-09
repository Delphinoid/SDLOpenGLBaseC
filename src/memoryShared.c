#include "memoryShared.h"
#include "inline.h"

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define VC_EXTRALEAN
	#include <Windows.h>
#else
	#include <unistd.h>
#endif

void *memHeapLowLevelAllocate(const size_t bytes){
#ifdef _WIN32
	return HeapAlloc(GetProcessHeap(), 0x01, bytes);
#else
	return sbrk(bytes);
#endif
}

void *memHeapLowLevelReallocate(void *const restrict block, const size_t bytes){
#ifdef _WIN32
	return HeapReAlloc(GetProcessHeap(), 0x01, block, bytes);
#else
	if(brk((byte_t *)block + bytes) == 0){
		return block;
	}
	return NULL;
#endif
}

int memHeapLowLevelFree(void *const restrict block){
#ifdef _WIN32
	return HeapFree(GetProcessHeap(), 0x01, block);
#else
	return brk(block);
#endif
}

#define memRegionAppendMacro(first, region) \
	while(*first != NULL){ \
		first = (memoryRegion **)(&(*first)->next); \
	} \
	region->next = NULL; \
	*first = region;

#define memRegionPrependMacro(first, region) \
	region->next = *first; \
	*first = region;

__FORCE_INLINE__ void memRegionExtend(memoryRegion **first, memoryRegion *const region, byte_t *const data){
	// Extends a memory region.
	// Same as memRegionAppend() unless
	// MEMORY_FORCE_ORDERED_EXTENSIONS is defined.
	#if !defined(MEMORY_FORCE_ORDERLESS_EXTENSIONS) || defined(MEMORY_FORCE_ORDERED_EXTENSIONS)
		memRegionAppendMacro(first, region);
	#else
		memRegionPrependMacro(first, region);
	#endif
	region->start = data;
}

__FORCE_INLINE__ void memRegionAppend(memoryRegion **first, memoryRegion *const region, byte_t *const data){
	// Appends a new memory region
	// to the list. Used when order
	// does matter.
	memRegionAppendMacro(first, region);
	region->start = data;
}

__FORCE_INLINE__ void memRegionPrepend(memoryRegion **first, memoryRegion *const region, byte_t *const data){
	// Prepends a new memory region
	// to the list. Used when order
	// doesn't matter.
	memRegionPrependMacro(first, region);
	region->start = data;
}

__FORCE_INLINE__ void memRegionFree(const memoryRegion *region){
	// Frees an allocator by looping through
	// each of its memory regions.
	while(region != NULL){
		const memoryRegion *const next = (memoryRegion *)region->next;
		memHeapLowLevelFree(region->start);
		region = next;
	}
}
