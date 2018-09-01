#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "memoryStack.h"
#include "memoryList.h"
#include "memoryPool.h"
#include "memoryArray.h"
#include "memoryTree.h"

/*
** The memory manager maintains a linked
** list of "virtual heaps", which contain
** their own collections of memory arenas.
**
** Each arena is assigned to a specific
** module and data type, e.g. physics
** bodies.
*/

#ifndef MEMORY_VIRTUAL_HEAP_SIZE
	// Ideally the minimum recommended
	// memory should be the heap size so
	// that only a single heap is necessary.
	#define MEMORY_VIRTUAL_HEAP_SIZE 536870912
#endif

/**
*** Where should memory arenas be stored?
*** Do we need an array of them, or can
*** they just precede the memory they
*** own in the virtual heap?
**/

typedef struct {

	// Linked list of virtual heaps.
	// Points to the memory region used by
	// the heap, which begins with a general
	// purpose allocator and is followed by
	// a pointer to the next heap.
	memoryTree *heap;

} memoryManager;

signed char memMngrInit(memoryManager *memMngr);
void memMngrDelete(memoryManager *memMngr);

byte_t **memMngrNextArena(byte_t *arena, const size_t size);
memoryStack *memMngrNewArenaStack(memoryManager *memMngr, const size_t bytes);
memoryList *memMngrNewArenaList(memoryManager *memMngr, const size_t bytes, const size_t length);
memoryPool *memMngrNewArenaPool(memoryManager *memMngr, const size_t bytes, const size_t length);
memoryArray *memMngrNewArenaArray(memoryManager *memMngr, const size_t bytes, const size_t length);
memoryTree *memMngrNewArenaTree(memoryManager *memMngr, const size_t bytes);

#endif
