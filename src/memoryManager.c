#include "memoryManager.h"

#define memMngrVirtualHeapPointer(heap) *((byte_t **)heap)
#define memMngrVirtualHeapData(heap)     ((byte_t *)(heap + sizeof(memoryTree *)))

byte_t *memMngrAllocateVirtualHeap(memoryManager *memMngr, const size_t bytes){

	/*
	** Allocate a new virtual heap.
	*/

	byte_t *data;

	// Allocate memory for the virtual heap.
	data = malloc(bytes);
	if(data != NULL){

		byte_t *start = memMngrVirtualHeapData(data);

		// Set the previous heap pointer.
		memMngrVirtualHeapPointer(data) = memMngr->heap;

		if(memMngr->heap == NULL){

			// Initialize the general purpose allocator.
			memTreeInit(
				&memMngr->allocator,
				start,
				bytes - sizeof(memoryTree *),
				MEMORY_TREE_UNSPECIFIED_LENGTH
			);

		}else{

			// Initialize the virtual heap.
			memTreeReset(
				start,
				bytes - sizeof(memoryTree *),
				MEMORY_TREE_UNSPECIFIED_LENGTH
			);

			// Append the new virtual heap to the allocator.
			memTreeInsert(&memMngr->allocator, start, memTreeBlockGetCurrent(start));

		}

	}

	return data;

}

signed char memMngrInit(memoryManager *memMngr, const size_t bytes, size_t num){

	/*
	** Initialize the memory manager.
	*/

	memMngr->heap = NULL;

	// Allocate "num" virtual heaps.
	while(num > 0){
		if(memMngrAllocateVirtualHeap(memMngr, bytes) == NULL){
			memMngrDelete(memMngr);
			return -1;
		}
		--num;
	}

	return 1;

}

memoryStack *memMngrArenaNewStack(memoryManager *memMngr, memoryStack *start, const size_t bytes, const size_t length){

	/*
	** Create a new stack arena.
	*/

	const size_t totalBytes = sizeof(memoryStack) + sizeof(memoryStack *) + memStackAllocationSize(NULL, bytes, length);

	// Try and allocate space for the arena
	// on the current virtual heap.
	byte_t *arena = memTreeAllocate(&memMngr->allocator, totalBytes);
	if(arena == NULL){

		#ifndef MEMORY_MANAGER_ENFORCE_STATIC_VIRTUAL_HEAP
		// If we need a new virtual heap, allocate it.
		if(
			totalBytes > MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE ||
			memMngrAllocateVirtualHeap(memMngr, MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE) == NULL
		){
		#endif

			return NULL;

		#ifndef MEMORY_MANAGER_ENFORCE_STATIC_VIRTUAL_HEAP
		}else{
			arena = memTreeAllocate(&memMngr->allocator, totalBytes);
			if(arena == NULL){
				return NULL;
			}
		}
		#endif

	}

	// Initialize the arena.
	memStackInit((memoryStack *)arena, arena + sizeof(memoryStack) + sizeof(memoryStack *), bytes, length);
	// Set the next arena pointer.
	memMngrArenaNextStack(arena) = NULL;

	// Make the previous arena point to this one.
	if(start != NULL){
		while(memMngrArenaNextStack(start) != NULL){
			start = memMngrArenaNextStack(start);
		}
		memMngrArenaNextStack(start) = (memoryStack *)arena;
	}

	return (memoryStack *)arena;

}

memoryList *memMngrArenaNewList(memoryManager *memMngr, memoryList *start, const size_t bytes, const size_t length){

	/*
	** Create a new free-list arena.
	*/

	const size_t totalBytes = sizeof(memoryList) + sizeof(memoryList *) + memListAllocationSize(NULL, bytes, length);

	// Try and allocate space for the arena
	// on the current virtual heap.
	byte_t *arena = memTreeAllocate(&memMngr->allocator, totalBytes);
	if(arena == NULL){

		#ifndef MEMORY_MANAGER_ENFORCE_STATIC_VIRTUAL_HEAP
		// If we need a new virtual heap, allocate it.
		if(
			totalBytes > MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE ||
			memMngrAllocateVirtualHeap(memMngr, MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE) == NULL
		){
		#endif

			return NULL;

		#ifndef MEMORY_MANAGER_ENFORCE_STATIC_VIRTUAL_HEAP
		}else{
			arena = memTreeAllocate(&memMngr->allocator, totalBytes);
			if(arena == NULL){
				return NULL;
			}
		}
		#endif

	}

	// Initialize the arena.
	memListInit((memoryList *)arena, arena + sizeof(memoryList) + sizeof(memoryList *), bytes, length);
	// Set the next arena pointer.
	memMngrArenaNextList(arena) = NULL;

	// Make the previous arena point to this one.
	if(start != NULL){
		// Append the new arena to the chain.
		memListAppend(start, ((memoryList *)arena));
		// Make the previous arena point to this one.
		while(memMngrArenaNextList(start) != NULL){
			start = memMngrArenaNextList(start);
		}
		memMngrArenaNextList(start) = (memoryList *)arena;
	}

	return (memoryList *)arena;

}

memoryPool *memMngrArenaNewPool(memoryManager *memMngr, memoryPool *start, const size_t bytes, const size_t length){

	/*
	** Create a new object pool arena.
	*/

	const size_t totalBytes = sizeof(memoryPool) + sizeof(memoryPool *) + memPoolAllocationSize(NULL, bytes, length);

	// Try and allocate space for the arena
	// on the current virtual heap.
	byte_t *arena = memTreeAllocate(&memMngr->allocator, totalBytes);
	if(arena == NULL){

		#ifndef MEMORY_MANAGER_ENFORCE_STATIC_VIRTUAL_HEAP
		// If we need a new virtual heap, allocate it.
		if(
			totalBytes > MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE ||
			memMngrAllocateVirtualHeap(memMngr, MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE) == NULL
		){
		#endif

			return NULL;

		#ifndef MEMORY_MANAGER_ENFORCE_STATIC_VIRTUAL_HEAP
		}else{
			arena = memTreeAllocate(&memMngr->allocator, totalBytes);
			if(arena == NULL){
				return NULL;
			}
		}
		#endif

	}

	// Initialize the arena.
	memPoolInit((memoryPool *)arena, arena + sizeof(memoryPool) + sizeof(memoryPool *), bytes, length);
	// Set the next arena pointer.
	memMngrArenaNextPool(arena) = NULL;

	if(start != NULL){
		// Append the new arena to the chain.
		memPoolAppend(start, ((memoryPool *)arena));
		// Make the previous arena point to this one.
		while(memMngrArenaNextPool(start) != NULL){
			start = memMngrArenaNextPool(start);
		}
		memMngrArenaNextPool(start) = (memoryPool *)arena;
	}

	return (memoryPool *)arena;

}

memorySLink *memMngrArenaNewSLink(memoryManager *memMngr, memorySLink *start, const size_t bytes, const size_t length){

	/*
	** Create a new doubly-linked list arena.
	*/

	const size_t totalBytes = sizeof(memorySLink) + sizeof(memorySLink *) + memSLinkAllocationSize(NULL, bytes, length);

	// Try and allocate space for the arena
	// on the current virtual heap.
	byte_t *arena = memTreeAllocate(&memMngr->allocator, totalBytes);
	if(arena == NULL){

		#ifndef MEMORY_MANAGER_ENFORCE_STATIC_VIRTUAL_HEAP
		// If we need a new virtual heap, allocate it.
		if(
			totalBytes > MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE ||
			memMngrAllocateVirtualHeap(memMngr, MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE) == NULL
		){
		#endif

			return NULL;

		#ifndef MEMORY_MANAGER_ENFORCE_STATIC_VIRTUAL_HEAP
		}else{
			arena = memTreeAllocate(&memMngr->allocator, totalBytes);
			if(arena == NULL){
				return NULL;
			}
		}
		#endif

	}

	// Initialize the arena.
	memSLinkInit((memorySLink *)arena, arena + sizeof(memorySLink) + sizeof(memorySLink *), bytes, length);
	// Set the next arena pointer.
	memMngrArenaNextSLink(arena) = NULL;

	if(start != NULL){
		// Append the new arena to the chain.
		memSLinkAppend(start, ((memorySLink *)arena));
		// Make the previous arena point to this one.
		while(memMngrArenaNextSLink(start) != NULL){
			start = memMngrArenaNextSLink(start);
		}
		memMngrArenaNextSLink(start) = (memorySLink *)arena;
	}

	return (memorySLink *)arena;

}

memoryDLink *memMngrArenaNewDLink(memoryManager *memMngr, memoryDLink *start, const size_t bytes, const size_t length){

	/*
	** Create a new doubly-linked list arena.
	*/

	const size_t totalBytes = sizeof(memoryDLink) + sizeof(memoryDLink *) + memDLinkAllocationSize(NULL, bytes, length);

	// Try and allocate space for the arena
	// on the current virtual heap.
	byte_t *arena = memTreeAllocate(&memMngr->allocator, totalBytes);
	if(arena == NULL){

		#ifndef MEMORY_MANAGER_ENFORCE_STATIC_VIRTUAL_HEAP
		// If we need a new virtual heap, allocate it.
		if(
			totalBytes > MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE ||
			memMngrAllocateVirtualHeap(memMngr, MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE) == NULL
		){
		#endif

			return NULL;

		#ifndef MEMORY_MANAGER_ENFORCE_STATIC_VIRTUAL_HEAP
		}else{
			arena = memTreeAllocate(&memMngr->allocator, totalBytes);
			if(arena == NULL){
				return NULL;
			}
		}
		#endif

	}

	// Initialize the arena.
	memDLinkInit((memoryDLink *)arena, arena + sizeof(memoryDLink) + sizeof(memoryDLink *), bytes, length);
	// Set the next arena pointer.
	memMngrArenaNextDLink(arena) = NULL;

	if(start != NULL){
		// Append the new arena to the chain.
		memDLinkAppend(start, ((memoryDLink *)arena));
		// Make the previous arena point to this one.
		while(memMngrArenaNextDLink(start) != NULL){
			start = memMngrArenaNextDLink(start);
		}
		memMngrArenaNextDLink(start) = (memoryDLink *)arena;
	}

	return (memoryDLink *)arena;

}

memoryTree *memMngrArenaNewTree(memoryManager *memMngr, memoryTree *start, const size_t bytes, const size_t length){

	/*
	** Create a new tree arena.
	*/

	const size_t totalBytes = sizeof(memoryTree) + sizeof(memoryTree *) + memTreeAllocationSize(NULL, bytes, length);

	// Try and allocate space for the arena
	// on the current virtual heap.
	byte_t *arena = memTreeAllocate(&memMngr->allocator, totalBytes);
	if(arena == NULL){

		#ifndef MEMORY_MANAGER_ENFORCE_STATIC_VIRTUAL_HEAP
		// If we need a new virtual heap, allocate it.
		if(
			totalBytes > MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE ||
			memMngrAllocateVirtualHeap(memMngr, MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE) == NULL
		){
		#endif

			return NULL;

		#ifndef MEMORY_MANAGER_ENFORCE_STATIC_VIRTUAL_HEAP
		}else{
			arena = memTreeAllocate(&memMngr->allocator, totalBytes);
			if(arena == NULL){
				return NULL;
			}
		}
		#endif

	}

	// Initialize the arena.
	memTreeInit((memoryTree *)arena, arena + sizeof(memoryTree) + sizeof(memoryTree *), totalBytes, MEMORY_TREE_UNSPECIFIED_LENGTH);
	// Set the next arena pointer.
	memMngrArenaNextTree(arena) = NULL;

	if(start != NULL){
		// Append the new arena to the chain.
		memTreeAppend(start, ((memoryTree *)arena));
		// Make the previous arena point to this one.
		while(memMngrArenaNextTree(start) != NULL){
			start = memMngrArenaNextTree(start);
		}
		memMngrArenaNextTree(start) = (memoryTree *)arena;
	}

	return (memoryTree *)arena;

}

void memMngrDelete(memoryManager *memMngr){
	/*
	** Free each virtual heap.
	*/
	byte_t *heap = memMngr->heap;
	while(heap != NULL){
		byte_t *prev = memMngrVirtualHeapPointer(heap);
		free(heap);
		heap = prev;
	}
}
