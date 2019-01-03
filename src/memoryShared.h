#ifndef MEMORYSHARED_H
#define MEMORYSHARED_H

#include "memorySettings.h"
#include <stdlib.h>
#include <stdint.h>

// Use 8 byte alignment for improved
// memory access performance.
#define MEMORY_ALIGNMENT 8

// Rounds x to the nearest address that
// is aligned with MEMORY_ALIGNMENT.
#define MEMORY_ALIGN(x) ((x + MEMORY_ALIGNMENT - 1) & ~(MEMORY_ALIGNMENT - 1))

#define MEMORY_UNSPECIFIED_LENGTH 0

typedef unsigned char byte_t;
typedef signed char   signed_byte_t;

typedef byte_t padding_t;

void *memHeapLowLevelAllocate(const size_t bytes);
void *memHeapLowLevelReallocate(void *const restrict block, const size_t bytes);
int memHeapLowLevelFree(void *const restrict block);

// Defines the region in memory that an
// allocator consumes. The end of the
// region is implicitly defined as the
// address of the memoryRegion struct.
typedef struct memoryRegion memoryRegion;
typedef struct memoryRegion {
	byte_t *start;
	memoryRegion *next;
} memoryRegion;

#define memAllocatorEnd(region)  ((byte_t *)(region))
#define memAllocatorNext(region) (region)->next

void memRegionExtend(memoryRegion **first, memoryRegion *const region, byte_t *const data);
void memRegionAppend(memoryRegion **first, memoryRegion *const region, byte_t *const data);
void memRegionPrepend(memoryRegion **first, memoryRegion *const region, byte_t *const data);
void memRegionFree(const memoryRegion *region);

#define MEMORY_REGION_LOOP_BEGIN(region, n, type)  \
	{                                              \
		const memoryRegion *__region_##n = region; \
		do {                                       \
			type n = (type)__region_##n->start;    \
			for(; n < (type)__region_##n; ++n){    \

#define MEMORY_REGION_LOOP_END(n)                          \
			}                                              \
			__region_##n = memAllocatorNext(__region_##n); \
		} while(__region_##n != NULL);                     \
	}

#define MEMORY_REGION_OFFSET_LOOP_BEGIN(region, n, type, start) \
	{                                                           \
		const memoryRegion *__region_##n = region;              \
		do {                                                    \
			type n = (type)(start);                             \
			for(; n < (type)__region_##n; ++n){                 \

#define MEMORY_REGION_OFFSET_LOOP_END(n)                   \
			}                                              \
			__region_##n = memAllocatorNext(__region_##n); \
		} while(__region_##n != NULL);                     \
	}

#endif
