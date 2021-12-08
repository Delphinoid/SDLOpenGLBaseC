#include "memoryShared.h"
#include "qualifiers.h"

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define VC_EXTRALEAN
	#include <Windows.h>
#else
	#include <unistd.h>
#endif

#ifndef MEMORY_ALLOCATOR_USE_MALLOC
#ifdef _WIN32
void *memHeapLowLevelAllocate(const size_t bytes){
	return HeapAlloc(GetProcessHeap(), 0x01, bytes);
}
#else
void *memHeapLowLevelAllocate(const size_t bytes){
	//return sbrk(bytes);
	// We no longer use brk, since it conflicts with the malloc family.
	int fd = open("/dev/zero", O_RDWR);
	void *p = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
	close(fd);
	return p;
}
#endif

#ifdef _WIN32
void *memHeapLowLevelReallocate(void *const __RESTRICT__ block, const size_t bytes){
	return HeapReAlloc(GetProcessHeap(), 0x01, block, bytes);
}
#else
void *memHeapLowLevelReallocate(void *const __RESTRICT__ block, const size_t bytes_old, const size_t bytes_new){
	//if(brk((byte_t *)block + bytes) == 0){
	//	return block;
	//}
	//return NULL;
	// We no longer use brk, since it conflicts with the malloc family.
	return mremap(block, bytes_old, bytes_new, MREMAP_MAYMOVE|MREMAP_FIXED);
}
#endif

#ifdef _WIN32
int memHeapLowLevelFree(void *const __RESTRICT__ block){
	return HeapFree(GetProcessHeap(), 0x01, block);
}
#else
int memHeapLowLevelFree(void *const __RESTRICT__ block, const size_t bytes){
	//return brk(block);
	// We no longer use brk, since it conflicts with the malloc family.
	return munmap(block, bytes);
}
#endif
#endif

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
		#if !defined(MEMORY_ALLOCATOR_USE_MALLOC) && !defined(_WIN32)
		memHeapLowLevelFree(region->start, region->bytes);
		#else
		memHeapLowLevelFree(region->start);
		#endif
		region = next;
	}
}
