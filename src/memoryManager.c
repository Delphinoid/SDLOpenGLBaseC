#include "memoryManager.h"
#include "memoryStack.h"
#include "memoryList.h"
#include "memoryPool.h"
#include "memorySLink.h"
#include "memoryDLink.h"
#include "memoryTree.h"
#include "inline.h"

#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION

	#define memInit()                      memTreeInit(&__memmngr->allocator);
	#define memCreate(data, bytes, length) memTreeCreate(&__memmngr->allocator, data, bytes, MEMORY_TREE_UNSPECIFIED_LENGTH)
	#define memExtend(data, bytes, length) memTreeExtend(&__memmngr->allocator, data, bytes, MEMORY_TREE_UNSPECIFIED_LENGTH)
	#define memAllocateVirtualHeap(bytes)  memMngrAllocateVirtualHeap(__memmngr, bytes)

#else

	static memoryManager __memmngr;

	#define memInit()                      memTreeInit(&__memmngr.allocator);
	#define memCreate(data, bytes, length) memTreeCreate(&__memmngr.allocator, data, bytes, MEMORY_TREE_UNSPECIFIED_LENGTH)
	#define memExtend(data, bytes, length) memTreeExtend(&__memmngr.allocator, data, bytes, MEMORY_TREE_UNSPECIFIED_LENGTH)
	#define memAllocateVirtualHeap(bytes)  memMngrAllocateVirtualHeap(bytes)

	__FORCE_INLINE__ void *memAllocate(const size_t bytes){
		return memTreeAllocate(&__memmngr.allocator, bytes);
	}

	__FORCE_INLINE__ void *memReallocate(void *data, const size_t bytes){
		return memTreeReallocate(&__memmngr.allocator, data, bytes);
	}

	__FORCE_INLINE__ void memFree(void *data){
		memTreeFree(&__memmngr.allocator, data);
	}

	__FORCE_INLINE__ void memPrintFreeBlocks(const unsigned int recursions){
		#ifdef MEMORY_DEBUG
		memTreePrintFreeBlocks(&__memmngr.allocator, recursions);
		#endif
	}

	__FORCE_INLINE__ void memPrintAllBlocks(){
		#ifdef MEMORY_DEBUG
		memTreePrintAllBlocks(&__memmngr.allocator);
		#endif
	}

#endif

#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
byte_t *memMngrAllocateVirtualHeap(memoryManager *memMngr, const size_t bytes){
#else
byte_t *memMngrAllocateVirtualHeap(const size_t bytes){
#endif

	/*
	** Allocate a new virtual heap.
	*/

	void *data;

	// Allocate memory for the virtual heap.
	data = memHeapLowLevelAllocate(bytes);
	if(data != NULL){

		#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
		if(memMngr->allocator.root == NULL){
		#else
		if(__memmngr.allocator.root == NULL){
		#endif

			// Initialize the general purpose allocator.
			memCreate(data, bytes, MEMORY_TREE_UNSPECIFIED_LENGTH);

		}else{

			#if defined(MEMORY_MANAGER_ENFORCE_STATIC_VIRTUAL_HEAP) && defined(MEMORY_DEBUG)
			fputs("MEMORY_DEBUG: Manually extending the heap with static heap size enforced?\n"
			      "Perhaps consider using a larger default heap size if possible.\n", stdout)
			#endif

			// Extend the virtual heap.
			memExtend(data, bytes, MEMORY_TREE_UNSPECIFIED_LENGTH);

		}

	}

	return data;

}

#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION
return_t memMngrInit(memoryManager *memMngr, const size_t bytes, size_t num){
#else
return_t memMngrInit(const size_t bytes, size_t num){
#endif

	/*
	** Initialize the memory manager.
	*/

	memInit();

	// Allocate "num" virtual heaps.
	while(num > 0){
		if(memAllocateVirtualHeap(bytes) == NULL){
			/** Memory allocation failure. **/
			memMngrDelete();
			return -1;
		}
		--num;
	}

	return 1;

}

#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION

void memMngrDelete(memoryManager *memMngr){
	/*
	** Free each virtual heap.
	*/
	memRegionFree(&memMngr->allocator);
}

#else

void memMngrDelete(){
	/*
	** Free each virtual heap.
	*/
	memTreeDelete(&__memmngr.allocator);
}

__FORCE_INLINE__ void *memForceAllocate(const size_t bytes){
	#ifdef MEMORY_MANAGER_ENFORCE_STATIC_VIRTUAL_HEAP
	return memTreeAllocate(&__memmngr.allocator, bytes);
	#else
	void *r = memTreeAllocate(&__memmngr.allocator, bytes);
	if(
		r == NULL &&
		memTreeAllocationSize(NULL, bytes, MEMORY_UNSPECIFIED_LENGTH)
		<
		MEMORY_MANAGER_VIRTUAL_HEAP_REALLOC_SIZE - sizeof(memoryRegion)
	){
		memMngrAllocateVirtualHeap(MEMORY_MANAGER_VIRTUAL_HEAP_REALLOC_SIZE);
		r = memTreeAllocate(&__memmngr.allocator, bytes);
	}
	return r;
	#endif
}

#endif
