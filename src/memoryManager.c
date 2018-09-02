#include "memoryManager.h"

#define memMngrVirtualHeapNextPointer(heap) ((memoryTree  **)(heap + sizeof(memoryTree)))
#define memMngrVirtualHeapData(heap)        ((byte_t  *)     (heap + sizeof(memoryTree) + sizeof(memoryTree *)))

byte_t *memMngrAllocateVirtualHeap(memoryTree **heap, const size_t heapSize){

	/*
	** Allocate a new virtual heap.
	*/

	byte_t *data;

	// Find the next free virtual heap slot.
	while(*heap != NULL){
		heap = memMngrVirtualHeapNextPointer(*heap);
	}

	// Allocate memory for the virtual heap.
	data = malloc(heapSize);
	if(data != NULL){

		*heap = (memoryTree *)data;
		*memMngrVirtualHeapNextPointer(data) = NULL;

		// Initialize the general purpose allocator.
		return memTreeInit(
			(memoryTree *)data,
			memMngrVirtualHeapData(data),
			heapSize - sizeof(memoryTree) - sizeof(memoryTree *),
			MEMORY_TREE_UNSPECIFIED_LENGTH
		);

	}

	return NULL;

}

signed char memMngrInit(memoryManager *memMngr, const size_t heapSize){
	/*
	** Initialize the memory manager.
	** Only allocate the first virtual heap.
	*/
	memMngr->heap = NULL;
	if(memMngrAllocateVirtualHeap(&memMngr->heap, heapSize) == NULL){
		return -1;
	}
	memMngr->heapSize = heapSize;
	return 1;
}

memoryStack *memMngrNewArenaStack(memoryManager *memMngr, memoryStack *last, const size_t bytes, const size_t length){

	/*
	** Create a new stack arena.
	*/

	byte_t *arena;
	const size_t totalBytes = sizeof(memoryStack) + sizeof(memoryStack *) + memStackAllocationSize(NULL, bytes, length);
	memoryTree **heap = &memMngr->heap;

	for(;;){

		// If we need a new virtual heap, allocate it.
		if(*heap == NULL){
			if(memMngrAllocateVirtualHeap(heap, memMngr->heapSize) == NULL){
				return NULL;
			}
		}

		// Try and allocate space for the arena
		// on the current virtual heap.
		arena = memTreeAllocate(*heap, totalBytes);
		if(arena != NULL){

			// Initialize the arena.
			memStackInit((memoryStack *)arena, arena + sizeof(memoryStack) + sizeof(memoryStack *), bytes, length);
			// Set the next arena pointer.
			memMngrNextArenaStack(arena) = NULL;

			// Make the previous arena point to this one.
			if(last != NULL){
				while(memMngrNextArenaStack(last) != NULL){
					last = memMngrNextArenaStack(last);
				}
				memMngrNextArenaStack(last) = (memoryStack *)arena;
			}

			return (memoryStack *)arena;

		}else{
			// Get the next virtual heap.
			heap = memMngrVirtualHeapNextPointer(*heap);
		}

	}

}

/*memoryList *memMngrNewArenaList(memoryManager *memMngr, memoryList *last, const size_t bytes, const size_t length){

	*
	** Create a new free-list arena.
	*

	byte_t *arena;
	const size_t totalBytes = sizeof(memoryList) + sizeof(memoryList *) + memListAllocationSize(NULL, bytes, length);
	memoryTree **heap = &memMngr->heap;

	for(;;){

		// If we need a new virtual heap, allocate it.
		if(*heap == NULL){
			if(memMngrAllocateVirtualHeap(heap, memMngr->heapSize) == NULL){
				return NULL;
			}
		}

		// Try and allocate space for the arena
		// on the current virtual heap.
		arena = memTreeAllocate(*heap, totalBytes);
		if(arena != NULL){

			// Initialize the arena.
			memListInit((memoryList *)arena, arena + sizeof(memoryList) + sizeof(memoryList *), bytes, length);
			// Set the next arena pointer.
			memMngrNextArenaList(arena) = NULL;

			// Make the previous arena point to this one.
			if(last != NULL){
				while(memMngrNextArenaList(last) != NULL){
					last = memMngrNextArenaList(last);
				}
				memMngrNextArenaList(last) = (memoryList *)arena;
			}

			return (memoryList *)arena;

		}else{
			// Get the next virtual heap.
			heap = memMngrVirtualHeapNextPointer(*heap);
		}

	}

}*/

memoryPool *memMngrNewArenaPool(memoryManager *memMngr, memoryPool *last, const size_t bytes, const size_t length){

	/*
	** Create a new object pool arena.
	*/

	byte_t *arena;
	const size_t totalBytes = sizeof(memoryPool) + sizeof(memoryPool *) + memPoolAllocationSize(NULL, bytes, length);
	memoryTree **heap = &memMngr->heap;

	for(;;){

		// If we need a new virtual heap, allocate it.
		if(*heap == NULL){
			if(memMngrAllocateVirtualHeap(heap, memMngr->heapSize) == NULL){
				return NULL;
			}
		}

		// Try and allocate space for the arena
		// on the current virtual heap.
		arena = memTreeAllocate(*heap, totalBytes);
		if(arena != NULL){

			// Initialize the arena.
			memPoolInit((memoryPool *)arena, arena + sizeof(memoryPool) + sizeof(memoryPool *), bytes, length);
			// Set the next arena pointer.
			memMngrNextArenaPool(arena) = NULL;

			// Make the previous arena point to this one.
			if(last != NULL){
				while(memMngrNextArenaPool(last) != NULL){
					last = memMngrNextArenaPool(last);
				}
				memMngrNextArenaPool(last) = (memoryPool *)arena;
			}

			return (memoryPool *)arena;

		}else{
			// Get the next virtual heap.
			heap = memMngrVirtualHeapNextPointer(*heap);
		}

	}

}

memoryArray *memMngrNewArenaArray(memoryManager *memMngr, memoryArray *last, const size_t bytes, const size_t length){

	/*
	** Create a new array arena.
	*/

	byte_t *arena;
	const size_t totalBytes = sizeof(memoryArray) + sizeof(memoryArray *) + memArrayAllocationSize(NULL, bytes, length);
	memoryTree **heap = &memMngr->heap;

	for(;;){

		// If we need a new virtual heap, allocate it.
		if(*heap == NULL){
			if(memMngrAllocateVirtualHeap(heap, memMngr->heapSize) == NULL){
				return NULL;
			}
		}

		// Try and allocate space for the arena
		// on the current virtual heap.
		arena = memTreeAllocate(*heap, totalBytes);
		if(arena != NULL){

			// Initialize the arena.
			memArrayInit((memoryArray *)arena, arena + sizeof(memoryArray) + sizeof(memoryArray *), bytes, length);
			// Set the next arena pointer.
			memMngrNextArenaArray(arena) = NULL;

			// Make the previous arena point to this one.
			if(last != NULL){
				while(memMngrNextArenaArray(last) != NULL){
					last = memMngrNextArenaArray(last);
				}
				memMngrNextArenaArray(last) = (memoryArray *)arena;
			}

			return (memoryArray *)arena;

		}else{
			// Get the next virtual heap.
			heap = memMngrVirtualHeapNextPointer(*heap);
		}

	}

}

memoryTree *memMngrNewArenaTree(memoryManager *memMngr, memoryTree *last, const size_t bytes, const size_t length){

	/*
	** Create a new array arena.
	*/

	byte_t *arena;
	const size_t totalBytes = sizeof(memoryTree) + sizeof(memoryTree *) + memTreeAllocationSize(NULL, bytes, length);
	memoryTree **heap = &memMngr->heap;

	for(;;){

		// If we need a new virtual heap, allocate it.
		if(*heap == NULL){
			if(memMngrAllocateVirtualHeap(heap, memMngr->heapSize) == NULL){
				return NULL;
			}
		}

		// Try and allocate space for the arena
		// on the current virtual heap.
		arena = memTreeAllocate(*heap, totalBytes);
		if(arena != NULL){

			// Initialize the arena.
			memTreeInit((memoryTree *)arena, arena + sizeof(memoryTree) + sizeof(memoryTree *), totalBytes, MEMORY_TREE_UNSPECIFIED_LENGTH);
			// Set the next arena pointer.
			memMngrNextArenaTree(arena) = NULL;

			// Make the previous arena point to this one.
			if(last != NULL){
				while(memMngrNextArenaTree(last) != NULL){
					last = memMngrNextArenaTree(last);
				}
				memMngrNextArenaTree(last) = (memoryTree *)arena;
			}

			return (memoryTree *)arena;

		}else{
			// Get the next virtual heap.
			heap = memMngrVirtualHeapNextPointer(*heap);
		}

	}

}

void memMngrDelete(memoryManager *memMngr){
	/*
	** Free each virtual heap.
	*/
	memoryTree *heap = memMngr->heap;
	while(heap != NULL){
		memoryTree *next = *((memoryTree **)(heap + sizeof(memoryTree)));
		free(heap);
		heap = next;
	}
}
