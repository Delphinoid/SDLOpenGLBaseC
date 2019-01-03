#ifndef MEMORYARRAY_H
#define MEMORYARRAY_H

#include "memoryShared.h"

/*
** Regular array allocator.
**
** Not much more sophisticated than the stack.
** Very finnicky, as it has very specific use-cases.
*/

#ifdef MEMORY_ARRAY_LEAN
	#define MEMORY_ARRAY_ALIGN(x) x
#else
	#define MEMORY_ARRAY_ALIGN(x) MEMORY_ALIGN(x)
#endif

typedef struct {
	size_t block;  // Block size.
	size_t size;   // Number of allocated elements.
	memoryRegion *region;  // Pointer to the allocator's memory region.
} memoryArray;

#define memArrayBlockSize(bytes)          MEMORY_ARRAY_ALIGN(bytes)
#define memArrayBlockSizeUnaligned(bytes) bytes
#ifndef MEMORY_ALLOCATOR_ALIGNED
	#define memArrayAlignStartBlock(start) (byte_t *)MEMORY_ARRAY_ALIGN((uintptr_t)start)
	#define memArrayAlignStartData(start)  (byte_t *)MEMORY_ARRAY_ALIGN((uintptr_t)start)
#else
	#define memArrayAlignStartBlock(start) start
	#define memArrayAlignStartData(start)  start
#endif
#define memArrayAllocationSize(start, bytes, length) \
	(memArrayBlockSize(bytes) * length + (uintptr_t)memArrayAlignStartBlock(start) - (uintptr_t)start + sizeof(memoryRegion))
	// The following can save small amounts of memory but can't be predicted as easily:
	//(memArrayBlockSize(bytes) * (length - 1) + memArrayBlockSizeUnaligned(bytes) + (uintptr_t)memArrayAlignStartBlock(start) - (uintptr_t)start + sizeof(memoryRegion))

#define memArrayFirst(region)           ((void *)memArrayAlignStartData((region)->start))
#define memArrayBlockNext(array, i)     (void *)((byte_t *)i + (array).block)
#define memArrayBlockPrevious(array, i) (void *)((byte_t *)i - (array).block)

void memArrayInit(memoryArray *const restrict array);
void *memArrayCreate(memoryArray *const restrict array, void *const start, const size_t bytes, const size_t length);
void *memArrayPush(memoryArray *const restrict array);
void *memArrayPushFast(memoryArray *const restrict array, void **const block, memoryRegion **const container);
void memArrayPop(memoryArray *const restrict array);
void *memArrayIndex(memoryArray *const restrict array, const size_t i);
void *memArrayIndexRegion(memoryArray *const restrict array, const size_t i, memoryRegion **const container);
void memArrayClear(memoryArray *const restrict array);
void *memArrayExtend(memoryArray *const restrict array, void *const start, const size_t bytes, const size_t length);
void memArrayDelete(memoryArray *const restrict array);

#define MEMORY_ARRAY_LOOP_BEGIN(allocator, n, type)                           \
	{                                                                         \
		const memoryRegion *__region_##n = allocator.region;                  \
		size_t __left_##n = allocator.size;                                   \
		do {                                                                  \
			type n = memArrayFirst(__region_##n);                             \
			while(__left_##n < 0 && n < (type)memAllocatorEnd(__region_##n)){ \

#define MEMORY_ARRAY_LOOP_END(allocator, n, earlyexit)                        \
				n = memArrayBlockNext(allocator, n);                          \
				--__left_##n;                                                 \
			}                                                                 \
			__region_##n = memAllocatorNext(__region_##n);                    \
		} while(__region_##n != NULL);                                        \
	}

#endif