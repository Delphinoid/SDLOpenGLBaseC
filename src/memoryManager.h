#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "memoryStack.h"
//#include "memoryList.h"
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
** rigid bodies.
*/

#ifndef MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE

	/*
	** Ideally the minimum recommended
	** memory should be the heap size so
	** that only a single heap is necessary
	** for the entire application.
	**
	** For more demanding applications,
	** however, this may not be possible,
	** as a large, contiguous block of
	** memory may not be available without
	** relying on slow virtual memory.
	*/

	#define MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE 536870912

#endif

typedef struct {

	/*
	** Linked list of virtual heaps.
	**
	** Points to the memory region used by
	** the heap, which begins with a general
	** purpose allocator and is followed by
	** a pointer to the next heap.
	*/
	memoryTree *heap;

	// The size of each heap.
	size_t heapSize;

} memoryManager;

/*
** The "next" pointer is stored directly after the arena.
*/
#define memMngrNextArenaStack(arena) *((memoryStack **)(arena + sizeof(memoryStack)))
//#define memMngrNextArenaList(arena)  *((memoryList  **)(arena + sizeof(memoryList )))
#define memMngrNextArenaPool(arena)  *((memoryPool  **)(arena + sizeof(memoryPool )))
#define memMngrNextArenaArray(arena) *((memoryArray **)(arena + sizeof(memoryArray)))
#define memMngrNextArenaTree(arena)  *((memoryTree  **)(arena + sizeof(memoryTree )))

byte_t *memMngrAllocateVirtualHeap(memoryTree **heap, const size_t heapSize);
signed char memMngrInit(memoryManager *memMngr, const size_t heapSize);
memoryStack *memMngrNewArenaStack(memoryManager *memMngr, memoryStack *last, const size_t bytes, const size_t length);
//memoryList  *memMngrNewArenaList (memoryManager *memMngr, memoryList  *last, const size_t bytes, const size_t length);
memoryPool  *memMngrNewArenaPool (memoryManager *memMngr, memoryPool  *last, const size_t bytes, const size_t length);
memoryArray *memMngrNewArenaArray(memoryManager *memMngr, memoryArray *last, const size_t bytes, const size_t length);
memoryTree  *memMngrNewArenaTree (memoryManager *memMngr, memoryTree  *last, const size_t bytes, const size_t length);
void memMngrDelete(memoryManager *memMngr);

#endif
