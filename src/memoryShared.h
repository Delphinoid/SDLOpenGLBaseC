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

typedef uint8_t byte_t;
typedef int8_t  signed_byte_t;

// Defines the region in memory that an
// allocator consumes. The end of the
// region is implicitly defined as the
// address of the memoryRegion struct.
typedef struct {
	byte_t *start;
	byte_t *next;
} memoryRegion;

void memRegionAppend(memoryRegion **first, memoryRegion *region, byte_t *data);
void memRegionPrepend(memoryRegion **first, memoryRegion *region, byte_t *data);
void memRegionFree(memoryRegion *region);

#endif
