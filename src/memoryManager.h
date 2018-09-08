#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "memoryStack.h"
#include "memoryList.h"
#include "memoryPool.h"
#include "memorySLink.h"
#include "memoryDLink.h"
#include "memoryTree.h"

/*
** The memory manager maintains a linked
** list of "virtual heaps", which contain
** their own collections of memory arenas.
**
** Each arena is assigned to a specific
** module and data type, e.g. physics
** rigid bodies.
**
** When an allocation that will not fit
** into the virtual heap is requested,
** it will check for room in each of
** the previously allocated heaps. If
** none of them can accommodate the data,
** a new virtual heap is created and
** the memory manager's "heap" member is
** updated.
**
** MEMORY_MANAGER_ENFORCE_STATIC_VIRTUAL_HEAP
** may be defined to prevent the automatic
** allocation of new virtual heaps.
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
	** The allocator used for managing
	** all of the allocated virtual heaps.
	*/
	memoryTree allocator;

	/*
	** Linked list of virtual heaps.
	**
	** Points to the last memory region
	** that was allocated, which begins
	** with a pointer to the previous
	** virtual heap followed by the
	** virtual heap's data.
	*/
	byte_t *heap;

} memoryManager;

/*
** The "next" pointer is stored directly after the arena.
*/
#define memMngrArenaNextStack(arena) *((memoryStack **)(((byte_t *)arena) + sizeof(memoryStack)))
#define memMngrArenaNextList(arena)  *((memoryList  **)(((byte_t *)arena) + sizeof(memoryList )))
#define memMngrArenaNextPool(arena)  *((memoryPool  **)(((byte_t *)arena) + sizeof(memoryPool )))
#define memMngrArenaNextSLink(arena) *((memorySLink **)(((byte_t *)arena) + sizeof(memorySLink)))
#define memMngrArenaNextDLink(arena) *((memoryDLink **)(((byte_t *)arena) + sizeof(memoryDLink)))
#define memMngrArenaNextTree(arena)  *((memoryTree  **)(((byte_t *)arena) + sizeof(memoryTree )))

#define memMngrAllocate(bytes)         memTreeAllocate(&memMngr->allocator, bytes)
#define memMngrReallocate(data, bytes) memTreeReallocate(&memMngr->allocator, data, bytes)
#define memMngrFree(data)              memTreeFree(&memMngr->allocator, data)

byte_t *memMngrAllocateVirtualHeap(memoryManager *memMngr, const size_t bytes);
signed char memMngrInit(memoryManager *memMngr, const size_t bytes, const size_t num);
memoryStack *memMngrArenaNewStack(memoryManager *memMngr, memoryStack *last, const size_t bytes, const size_t length);
memoryList  *memMngrArenaNewList (memoryManager *memMngr, memoryList  *last, const size_t bytes, const size_t length);
memoryPool  *memMngrArenaNewPool (memoryManager *memMngr, memoryPool  *last, const size_t bytes, const size_t length);
memorySLink *memMngrArenaNewSLink(memoryManager *memMngr, memorySLink *last, const size_t bytes, const size_t length);
memoryDLink *memMngrArenaNewDLink(memoryManager *memMngr, memoryDLink *last, const size_t bytes, const size_t length);
memoryTree  *memMngrArenaNewTree (memoryManager *memMngr, memoryTree  *last, const size_t bytes, const size_t length);
void memMngrDelete(memoryManager *memMngr);

#endif
