#include "memoryManager.h"

byte_t *memMngrAllocateVirtualHeap(memoryManager *memMngr, const size_t bytes){

	/*
	** Allocate a new virtual heap.
	*/

	byte_t *data;

	// Allocate memory for the virtual heap.
	data = malloc(bytes);
	if(data != NULL){

		if(memMngr->allocator.root == NULL){

			// Initialize the general purpose allocator.
			memTreeCreate(
				&memMngr->allocator,
				data,
				bytes,
				MEMORY_TREE_UNSPECIFIED_LENGTH
			);

		}else{

			// Extend the virtual heap.
			memTreeExtend(
				&memMngr->allocator,
				data,
				bytes,
				MEMORY_TREE_UNSPECIFIED_LENGTH
			);

		}

	}

	return data;

}

signed char memMngrInit(memoryManager *memMngr, const size_t bytes, size_t num){

	/*
	** Initialize the memory manager.
	*/

	memTreeInit(&memMngr->allocator);

	// Allocate "num" virtual heaps.
	while(num > 0){
		if(memMngrAllocateVirtualHeap(memMngr, bytes) == NULL){
			/** Memory allocation failure. **/
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

	const size_t totalBytes = sizeof(memoryStack) + memStackAllocationSize(NULL, bytes, length);

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
	memStackCreate((memoryStack *)arena, arena + sizeof(memoryStack), bytes, length);
	return (memoryStack *)arena;

}

memoryList *memMngrArenaNewList(memoryManager *memMngr, memoryList *start, const size_t bytes, const size_t length){

	/*
	** Create a new free-list arena.
	*/

	const size_t totalBytes = (start == NULL ? sizeof(memoryList) : 0) + memListAllocationSize(NULL, bytes, length);

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

	if(start == NULL){
		// Initialize the arena.
		memListCreate((memoryList *)arena, arena + sizeof(memoryList), bytes, length);
		return (memoryList *)arena;
	}else{
		// Extend the arena.
		memListExtend(start, arena, bytes, length);
		return start;
	}

}

memoryPool *memMngrArenaNewPool(memoryManager *memMngr, memoryPool *start, const size_t bytes, const size_t length){

	/*
	** Create a new object pool arena.
	*/

	const size_t totalBytes = (start == NULL ? sizeof(memoryPool) : 0) + memPoolAllocationSize(NULL, bytes, length);

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

	if(start == NULL){
		// Initialize the arena.
		memPoolCreate((memoryPool *)arena, arena + sizeof(memoryPool), bytes, length);
		return (memoryPool *)arena;
	}else{
		// Extend the arena.
		memPoolExtend(start, arena, bytes, length);
		return start;
	}

}

memorySLink *memMngrArenaNewSLink(memoryManager *memMngr, memorySLink *start, const size_t bytes, const size_t length){

	/*
	** Create a new doubly-linked list arena.
	*/

	const size_t totalBytes = (start == NULL ? sizeof(memorySLink) : 0) + memSLinkAllocationSize(NULL, bytes, length);

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

	if(start == NULL){
		// Initialize the arena.
		memSLinkCreate((memorySLink *)arena, arena + sizeof(memorySLink), bytes, length);
		return (memorySLink *)arena;
	}else{
		// Extend the arena.
		memSLinkExtend(start, arena, bytes, length);
		return start;
	}

}

memoryDLink *memMngrArenaNewDLink(memoryManager *memMngr, memoryDLink *start, const size_t bytes, const size_t length){

	/*
	** Create a new doubly-linked list arena.
	*/

	const size_t totalBytes = (start == NULL ? sizeof(memoryDLink) : 0) + memDLinkAllocationSize(NULL, bytes, length);

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

	if(start == NULL){
		// Initialize the arena.
		memDLinkCreate((memoryDLink *)arena, arena + sizeof(memoryDLink), bytes, length);
		return (memoryDLink *)arena;
	}else{
		// Extend the arena.
		memDLinkExtend(start, arena, bytes, length);
		return start;
	}

}

memoryTree *memMngrArenaNewTree(memoryManager *memMngr, memoryTree *start, const size_t bytes, const size_t length){

	/*
	** Create a new tree arena.
	*/

	const size_t totalBytes = (start == NULL ? sizeof(memoryTree) : 0) + memTreeAllocationSize(NULL, bytes, length);

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

	if(start == NULL){
		// Initialize the arena.
		memTreeCreate((memoryTree *)arena, arena + sizeof(memoryTree), bytes, length);
		return (memoryTree *)arena;
	}else{
		// Extend the arena.
		memTreeExtend(start, arena, bytes, length);
		return start;
	}

}

void memMngrDelete(memoryManager *memMngr){
	/*
	** Free each virtual heap.
	*/
	memRegionFree(memMngr->allocator.region);
}
