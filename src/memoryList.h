#ifndef MEMORYLIST_H
#define MEMORYLIST_H

#include "memoryShared.h"

/*
** Free-list allocator.
*/

typedef struct {
	byte_t *next;
} memoryList, memoryListBlock;

size_t memListAllocationOverhead(const byte_t *start, const size_t bytes, const size_t length);
byte_t *memListInit(memoryList *list, byte_t *start, const size_t bytes, const size_t length);
byte_t *memListAllocate(memoryList *list);
void memListFree(memoryList *list, byte_t *block);

#endif
