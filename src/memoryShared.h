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

void *memHeapLowLevelAllocate(const size_t bytes);
void *memHeapLowLevelReallocate(void *block, const size_t bytes);
int memHeapLowLevelFree(void *block);

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

void memRegionExtend(memoryRegion **first, memoryRegion *region, byte_t *data);
void memRegionAppend(memoryRegion **first, memoryRegion *region, byte_t *data);
void memRegionPrepend(memoryRegion **first, memoryRegion *region, byte_t *data);
void memRegionFree(memoryRegion *region);

#endif
