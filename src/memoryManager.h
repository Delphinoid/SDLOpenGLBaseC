#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "memoryTree.h"
#include "return.h"

// The memory manager maintains a linked
// list of "virtual heaps", which contain
// their own collections of memory arenas.
//
// Each arena is assigned to a specific
// module and data type, e.g. physics
// rigid bodies.
//
// When an allocation that will not fit
// into the virtual heap is requested,
// it will check for room in each of
// the previously allocated heaps. If
// none of them can accommodate the data,
// a new virtual heap is created and
// the memory manager's "heap" member is
// updated.
//
// MEMORY_MANAGER_ENFORCE_STATIC_VIRTUAL_HEAP
// may be defined in memorySettings.h to
// prevent the automatic allocation of new
// virtual heaps.

#ifndef MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE

	// Ideally the minimum recommended
	// memory should be the heap size so
	// that only a single heap is necessary
	// for the entire application.
	//
	// For more demanding applications,
	// however, this may not be possible,
	// as a large, contiguous block of
	// memory may not be available without
	// relying on slow virtual memory.

	#define MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE 536870912

#endif

#ifndef MEMORY_MANAGER_VIRTUAL_HEAP_REALLOC_SIZE
	#define MEMORY_MANAGER_VIRTUAL_HEAP_REALLOC_SIZE MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE
#endif

typedef struct {

	// The allocator used for managing
	// all of the allocated virtual heaps.
	memoryTree allocator;

} memoryManager;

#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION

	#define memAllocateStatic(bytes)        memTreeAllocate(&__memmngr->allocator, bytes)
	#define memReallocateFixed(data, bytes) memTreeReallocate(&__memmngr->allocator, data, bytes)
	#define memFree(data)                   memTreeFree(&__memmngr->allocator, data)

	#ifdef MEMORY_DEBUG
		#define memPrintFreeBlocks(recursions)  memTreePrintFreeBlocks(&__memmngr->allocator, recursions)
		#define memPrintAllBlocks()             memTreePrintAllBlocks(&__memmngr->allocator)
	#else
		#define memPrintFreeBlocks(recursions)
		#define memPrintAllBlocks()
	#endif

	byte_t *memMngrReallocateVirtualHeap(memoryManager *const restrict memMngr, const size_t bytes);
	return_t memMngrInit(memoryManager *const restrict memMngr, const size_t bytes, const size_t num);
	void memMngrDelete(memoryManager *const restrict memMngr);

	#ifdef MEMORY_MANAGER_ENFORCE_STATIC_VIRTUAL_HEAP
		#define memAllocate(bytes)         memTreeAllocate(&__memmngr->allocator, bytes)
		#define memReallocate(data, bytes) memTreeReallocate(&__memmngr->allocator, data, bytes)
	#else
		#define memAllocate(bytes)         NULL  // Undefined for now.
		#define memReallocate(data, bytes) NULL  // Undefined for now.
	#endif

#else

	//#ifdef MEMORY_DEBUG
		//extern memoryManager __memmngr;
	//#endif

	//#define memAllocateStatic(bytes)         memTreeAllocate(&__memmngr.allocator, bytes)
	//#define memReallocateFixed(data, bytes) memTreeReallocate(&__memmngr.allocator, data, bytes)
	//#define memFree(data)                   memTreeFree(&__memmngr.allocator, data)

	//#ifdef MEMORY_DEBUG
	//	#define memPrintFreeBlocks(recursions)  memTreePrintFreeBlocks(&__memmngr.allocator, recursions)
	//	#define memPrintAllBlocks()             memTreePrintAllBlocks(&__memmngr.allocator)
	//#else
	//	#define memPrintFreeBlocks(recursions)
	//	#define memPrintAllBlocks()
	//#endif

	extern memoryManager __memmngr;

	void *memAllocateStatic(const size_t bytes);
	void *memReallocateStatic(void *const restrict data, const size_t bytes);
	void memFree(void *const restrict data);

	void memPrintFreeBlocks(const unsigned int recursions);
	void memPrintAllBlocks();

	byte_t *memMngrAllocateVirtualHeap(const size_t bytes);
	return_t memMngrInit(const size_t bytes, const size_t num);
	void memMngrDelete();

	void *memAllocate(const size_t bytes);
	void *memReallocate(void *const restrict data, const size_t bytes);

#endif

#endif
