#include "memoryManager.h"

static byte_t *memMngrAllocateHeap(memoryTree **heap){

	/*
	** Allocate a new virtual heap.
	*/

	byte_t *data;
	size_t bytes;

	// Find the next free virtual heap slot.
	while(*heap != NULL){
		heap = (memoryTree **)((byte_t *)(*heap) + sizeof(memoryTree));
	}

	// Allocate memory for the virtual heap.
	data = malloc(MEMORY_VIRTUAL_HEAP_SIZE);
	if(data != NULL){

		*heap = (memoryTree *)data;
		*((memoryTree **)(data + sizeof(memoryTree))) = NULL;

		// Get the total allocation size minus
		// any overhead from the allocator.
		bytes = MEMORY_VIRTUAL_HEAP_SIZE - sizeof(memoryTree) - sizeof(memoryTree *);
		bytes -= memTreeAllocationOverhead((byte_t *)(data + sizeof(memoryTree) + sizeof(memoryTree *)), bytes);

		// Initialize the general purpose allocator.
		return memTreeInit((memoryTree *)data, (byte_t *)(data + sizeof(memoryTree) + sizeof(memoryTree *)), bytes);

	}

	return NULL;

}

signed char memMngrInit(memoryManager *memMngr){
	/*
	** Initialize the memory manager.
	** Only allocate the first virtual heap.
	*/
	memMngr->heap = NULL;
	if(memMngrAllocateHeap(&memMngr->heap) == NULL){
		return -1;
	}
	return 1;
}

inline byte_t **memMngrNextArena(byte_t *arena, const size_t size){
	return (byte_t **)(arena + size);
}

memoryStack *memMngrNewArenaStack(memoryManager *memMngr, const size_t bytes){

	/*
	** Create a new stack arena.
	*/

	byte_t *arena;
	const size_t totalBytes = bytes + sizeof(memoryStack) + sizeof(memoryStack *) + memStackAllocationOverhead(NULL, bytes);
	memoryTree **heap = &memMngr->heap;

	for(;;){
		// If we need a new virtual heap, allocate it.
		if(*heap == NULL){
			if(memMngrAllocateHeap(heap) == NULL){
				return NULL;
			}
		}
		// Try and allocate space for the arena
		// on the current virtual heap.
		arena = memTreeAllocate(*heap, totalBytes);
		if(arena != NULL){
			// Set the next arena pointer.
			*((memoryStack **)(arena + sizeof(memoryStack))) = NULL;
			memStackInit((memoryStack *)arena, arena + sizeof(memoryStack) + sizeof(memoryStack *), bytes);
			return (memoryStack *)arena;
		}else{
			// Get the next virtual heap.
			heap = (memoryTree **)(*heap + sizeof(memoryTree));
		}
	}

}

memoryList *memMngrNewArenaList(memoryManager *memMngr, const size_t bytes, const size_t length){

	/*
	** Create a new free-list arena.
	*/

	byte_t *arena;
	const size_t totalBytes = bytes + sizeof(memoryList) + sizeof(memoryList *) + memListAllocationOverhead(NULL, bytes, length);
	memoryTree **heap = &memMngr->heap;

	for(;;){
		// If we need a new virtual heap, allocate it.
		if(*heap == NULL){
			if(memMngrAllocateHeap(heap) == NULL){
				return NULL;
			}
		}
		// Try and allocate space for the arena
		// on the current virtual heap.
		arena = memTreeAllocate(*heap, totalBytes);
		if(arena != NULL){
			// Set the next arena pointer.
			*((memoryList **)(arena + sizeof(memoryList))) = NULL;
			memListInit((memoryList *)arena, arena + sizeof(memoryList) + sizeof(memoryList *), bytes, length);
			return (memoryList *)arena;
		}else{
			// Get the next virtual heap.
			heap = (memoryTree **)(*heap + sizeof(memoryTree));
		}
	}

}

memoryPool *memMngrNewArenaPool(memoryManager *memMngr, const size_t bytes, const size_t length){

	/*
	** Create a new object pool arena.
	*/

	byte_t *arena;
	const size_t totalBytes = bytes + sizeof(memoryPool) + sizeof(memoryPool *) + memPoolAllocationOverhead(NULL, bytes, length);
	memoryTree **heap = &memMngr->heap;

	for(;;){
		// If we need a new virtual heap, allocate it.
		if(*heap == NULL){
			if(memMngrAllocateHeap(heap) == NULL){
				return NULL;
			}
		}
		// Try and allocate space for the arena
		// on the current virtual heap.
		arena = memTreeAllocate(*heap, totalBytes);
		if(arena != NULL){
			// Set the next arena pointer.
			*((memoryPool **)(arena + sizeof(memoryPool))) = NULL;
			memPoolInit((memoryPool *)arena, arena + sizeof(memoryPool) + sizeof(memoryPool *), bytes, length);
			return (memoryPool *)arena;
		}else{
			// Get the next virtual heap.
			heap = (memoryTree **)(*heap + sizeof(memoryTree));
		}
	}

}

memoryArray *memMngrNewArenaArray(memoryManager *memMngr, const size_t bytes, const size_t length){

	/*
	** Create a new array arena.
	*/

	byte_t *arena;
	const size_t totalBytes = bytes + sizeof(memoryArray) + sizeof(memoryArray *) + memArrayAllocationOverhead(NULL, bytes, length);
	memoryTree **heap = &memMngr->heap;

	for(;;){
		// If we need a new virtual heap, allocate it.
		if(*heap == NULL){
			if(memMngrAllocateHeap(heap) == NULL){
				return NULL;
			}
		}
		// Try and allocate space for the arena
		// on the current virtual heap.
		arena = memTreeAllocate(*heap, totalBytes);
		if(arena != NULL){
			// Set the next arena pointer.
			*((memoryArray **)(arena + sizeof(memoryArray))) = NULL;
			memArrayInit((memoryArray *)arena, arena + sizeof(memoryArray) + sizeof(memoryArray *), bytes, length);
			return (memoryArray *)arena;
		}else{
			// Get the next virtual heap.
			heap = (memoryTree **)(*heap + sizeof(memoryTree));
		}
	}

}

memoryTree *memMngrNewArenaTree(memoryManager *memMngr, const size_t bytes){

	/*
	** Create a new array arena.
	*/

	byte_t *arena;
	const size_t totalBytes = bytes + sizeof(memoryTree) + sizeof(memoryTree *) + memTreeAllocationOverhead(NULL, bytes);
	memoryTree **heap = &memMngr->heap;

	for(;;){
		// If we need a new virtual heap, allocate it.
		if(*heap == NULL){
			if(memMngrAllocateHeap(heap) == NULL){
				return NULL;
			}
		}
		// Try and allocate space for the arena
		// on the current virtual heap.
		arena = memTreeAllocate(*heap, totalBytes);
		if(arena != NULL){
			// Set the next arena pointer.
			*((memoryTree **)(arena + sizeof(memoryTree))) = NULL;
			memTreeInit((memoryTree *)arena, arena + sizeof(memoryTree) + sizeof(memoryTree *), bytes);
			return (memoryTree *)arena;
		}else{
			// Get the next virtual heap.
			heap = (memoryTree **)(*heap + sizeof(memoryTree));
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
