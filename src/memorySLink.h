#ifndef MEMORYSLINK_H
#define MEMORYSLINK_H

#include "memoryShared.h"

/*
** Singly-linked list allocator.
**
** Effectively a free-list allocator
** but each block has a pointer to the
** next element in the array it is a
** part of, contained in a header.
**
** If the block is inactive, the pointer
** will contain a 1 in its LSB.
**
** Free-list pointers point to the
** data, not the beginning of the block.
**
** It can also be treated as an object
** pool if necessary.
*/

#define MEMORY_SLINK_BLOCK_ACTIVE        (uintptr_t)0x00
#define MEMORY_SLINK_BLOCK_INACTIVE      (uintptr_t)0x01
#define MEMORY_SLINK_BLOCK_INVALID       (uintptr_t)0x02
#define MEMORY_SLINK_BLOCK_INACTIVE_MASK (uintptr_t)0x03
#define MEMORY_SLINK_BLOCK_ACTIVE_MASK   (uintptr_t)~MEMORY_SLINK_BLOCK_INACTIVE_MASK

#define MEMORY_SLINK_BLOCK_POINTER_SIZE sizeof(byte_t *)
#define MEMORY_SLINK_BLOCK_FLAGS_SIZE   sizeof(uintptr_t)
#define MEMORY_SLINK_BLOCK_NEXT_SIZE    MEMORY_SLINK_BLOCK_POINTER_SIZE
#ifdef MEMORY_SLINK_ALIGN_HEADER
	#define MEMORY_SLINK_BLOCK_HEADER_SIZE MEMORY_ALIGN(MEMORY_SLINK_BLOCK_NEXT_SIZE)
#else
	#define MEMORY_SLINK_BLOCK_HEADER_SIZE MEMORY_SLINK_BLOCK_NEXT_SIZE
#endif

#define MEMORY_SLINK_NEXT_OFFSET_FROM_BLOCK  0
#define MEMORY_SLINK_FLAGS_OFFSET_FROM_BLOCK MEMORY_SLINK_NEXT_OFFSET_FROM_BLOCK
#define MEMORY_SLINK_DATA_OFFSET_FROM_BLOCK  MEMORY_SLINK_BLOCK_HEADER_SIZE
#define MEMORY_SLINK_NEXT_OFFSET_FROM_DATA  -MEMORY_SLINK_BLOCK_NEXT_SIZE
#define MEMORY_SLINK_FLAGS_OFFSET_FROM_DATA  MEMORY_SLINK_NEXT_OFFSET_FROM_DATA
#define MEMORY_SLINK_BLOCK_OFFSET_FROM_DATA -MEMORY_SLINK_BLOCK_HEADER_SIZE

#define memSLinkBlockGetFlags(block)         *((uintptr_t *)((byte_t *)block + MEMORY_SLINK_FLAGS_OFFSET_FROM_BLOCK))
#define memSLinkBlockGetNext(block)          *((byte_t **)((byte_t *)block + MEMORY_SLINK_NEXT_OFFSET_FROM_BLOCK))
#define memSLinkBlockGetNextPointer(block)    ((byte_t **)((byte_t *)block + MEMORY_SLINK_NEXT_OFFSET_FROM_BLOCK))
#define memSLinkBlockGetNextFree(block)      *((byte_t **)((byte_t *)block + MEMORY_SLINK_NEXT_OFFSET_FROM_BLOCK))
#define memSLinkBlockGetNextFreeMasked(block) ((byte_t *)(*((uintptr_t *)((byte_t *)block + MEMORY_SLINK_NEXT_OFFSET_FROM_BLOCK)) & MEMORY_SLINK_BLOCK_ACTIVE_MASK))
#define memSLinkBlockGetData(block)           ((byte_t *)((byte_t *)block + MEMORY_SLINK_DATA_OFFSET_FROM_BLOCK))

#define memSLinkBlockGetActive(block)        *((uintptr_t *)((byte_t *)block + MEMORY_SLINK_FLAGS_OFFSET_FROM_BLOCK))
#define memSLinkBlockGetActiveMasked(block) (*((uintptr_t *)((byte_t *)block + MEMORY_SLINK_FLAGS_OFFSET_FROM_BLOCK)) & MEMORY_SLINK_BLOCK_INACTIVE_MASK)
#define memSLinkBlockGetActivePointer(block)  ((uintptr_t *)((byte_t *)block + MEMORY_SLINK_FLAGS_OFFSET_FROM_BLOCK))
#define memSLinkBlockSetActive(block)        *((uintptr_t *)((byte_t *)block + MEMORY_SLINK_FLAGS_OFFSET_FROM_BLOCK)) &= MEMORY_SLINK_BLOCK_ACTIVE_MASK
#define memSLinkBlockSetInactive(block)      *((uintptr_t *)((byte_t *)block + MEMORY_SLINK_FLAGS_OFFSET_FROM_BLOCK)) |= MEMORY_SLINK_BLOCK_INACTIVE_MASK

#define memSLinkDataGetFlags(data)         *((uintptr_t *)((byte_t *)data + MEMORY_SLINK_FLAGS_OFFSET_FROM_DATA))
#define memSLinkDataGetNext(data)          *((byte_t **)((byte_t *)data + MEMORY_SLINK_NEXT_OFFSET_FROM_DATA))
#define memSLinkDataGetNextPointer(data)    ((byte_t **)((byte_t *)data + MEMORY_SLINK_NEXT_OFFSET_FROM_DATA))
#define memSLinkDataGetNextFree(data)      *((byte_t **)((byte_t *)data + MEMORY_SLINK_NEXT_OFFSET_FROM_DATA))
#define memSLinkDataGetNextFreeMasked(data) ((byte_t *)(*((uintptr_t *)((byte_t *)data + MEMORY_SLINK_NEXT_OFFSET_FROM_DATA)) & MEMORY_SLINK_BLOCK_ACTIVE_MASK))
#define memSLinkDataGetBlock(data)          ((byte_t *)((byte_t *)data + MEMORY_SLINK_BLOCK_OFFSET_FROM_DATA))

#define memSLinkDataGetActive(data)        *((uintptr_t *)((byte_t *)data + MEMORY_SLINK_FLAGS_OFFSET_FROM_DATA))
#define memSLinkDataGetActiveMasked(data) (*((uintptr_t *)((byte_t *)data + MEMORY_SLINK_FLAGS_OFFSET_FROM_DATA)) & MEMORY_SLINK_BLOCK_INACTIVE_MASK)
#define memSLinkDataGetActivePointer(data)  ((uintptr_t *)((byte_t *)data + MEMORY_SLINK_FLAGS_OFFSET_FROM_DATA))
#define memSLinkDataSetActive(data)        *((uintptr_t *)((byte_t *)data + MEMORY_SLINK_FLAGS_OFFSET_FROM_DATA)) &= MEMORY_SLINK_BLOCK_ACTIVE_MASK
#define memSLinkDataSetInactive(data)      *((uintptr_t *)((byte_t *)data + MEMORY_SLINK_FLAGS_OFFSET_FROM_DATA)) |= MEMORY_SLINK_BLOCK_INACTIVE_MASK

#ifdef MEMORY_SLINK_LEAN
	#define MEMORY_SLINK_ALIGN(x) x
#else
	#define MEMORY_SLINK_ALIGN(x) MEMORY_ALIGN(x)
#endif

typedef struct {
	size_t block;  // Block size.
	byte_t *free;  // Next free block pointer.
	memoryRegion *region;  // Pointer to the allocator's memory region.
} memorySLink;

#define memSLinkBlockSize(bytes) MEMORY_SLINK_ALIGN(bytes + MEMORY_SLINK_BLOCK_HEADER_SIZE)
#define memSLinkBlockSizeUnaligned(bytes)          (bytes + MEMORY_SLINK_BLOCK_HEADER_SIZE)
#ifndef MEMORY_ALLOCATOR_ALIGNED
	#define memSLinkAlignStartBlock(start) ((byte_t *)MEMORY_SLINK_ALIGN((uintptr_t)start + MEMORY_SLINK_BLOCK_HEADER_SIZE) - MEMORY_SLINK_BLOCK_HEADER_SIZE)
	#define memSLinkAlignStartData(start)  ((byte_t *)MEMORY_SLINK_ALIGN((uintptr_t)start + MEMORY_SLINK_BLOCK_HEADER_SIZE))
#else
	#define memSLinkAlignStartBlock(start) start
	#define memSLinkAlignStartData(start)  ((byte_t *)start + MEMORY_SLINK_BLOCK_HEADER_SIZE)
#endif
#define memSLinkAllocationSize(start, bytes, length) \
	(memSLinkBlockSize(bytes) * length + (uintptr_t)memSLinkAlignStartBlock(start) - (uintptr_t)start + sizeof(memoryRegion))
	// The following can save small amounts of memory but can't be predicted as easily:
	//(memSLinkBlockSize(bytes) * (length - 1) + memSLinkBlockSizeUnaligned(bytes) + (uintptr_t)memSLinkAlignStartBlock(start) - (uintptr_t)start + sizeof(memoryRegion))

#ifdef MEMORY_SLINK_ALIGN_HEADER
	#define memSLinkFirst(region) ((void *)(region)->start)
#else
	#define memSLinkFirst(region) ((void *)memSLinkAlignStartData((region)->start))
#endif
#define memSLinkNext(i)             memSLinkDataGetNext(i)
#define memSLinkBlockStatus(block)  memSLinkDataGetActiveMasked(block)
#define memSLinkBlockNext(array, i) (void *)((byte_t *)i + (array).block)

void memSLinkInit(memorySLink *const restrict array);
void *memSLinkCreate(memorySLink *const restrict array, void *const start, const size_t bytes, const size_t length);
void *memSLinkCreateInit(memorySLink *const restrict array, void *const start, const size_t bytes, const size_t length, void (*func)(void *const restrict block));
void *memSLinkAllocate(memorySLink *const restrict array);
void *memSLinkPrepend(memorySLink *const restrict array, void **const start);
void *memSLinkAppend(memorySLink *const restrict array, void **const start);
void *memSLinkInsertBefore(memorySLink *const restrict array, void **const start, void *const element, const void *const previous);
void *memSLinkInsertAfter(memorySLink *const restrict array, void **const start, void *const element);
void memSLinkFree(memorySLink *const restrict array, void **const start, void *const element, const void *const previous);
void *memSLinkSetupMemory(void *start, const size_t bytes, const size_t length);
void *memSLinkSetupMemoryInit(void *start, const size_t bytes, const size_t length, void (*func)(void *const restrict block));
void memSLinkClear(memorySLink *const restrict array);
void memSLinkClearInit(memorySLink *const restrict array, void (*func)(void *const restrict block));
void *memSLinkExtend(memorySLink *const restrict array, void *const start, const size_t bytes, const size_t length);
void *memSLinkExtendInit(memorySLink *const restrict array, void *const start, const size_t bytes, const size_t length, void (*func)(void *const restrict block));
void memSLinkDelete(memorySLink *const restrict array);

#define MEMORY_SLINK_LOOP_BEGIN(allocator, n, type)                 \
	{                                                               \
		const memoryRegion *__region_##n = allocator.region;        \
		do {                                                        \
			type n = memSLinkFirst(__region_##n);                   \
			while(n < (type)memAllocatorEnd(__region_##n)){         \
				const byte_t __flag_##n = memSLinkBlockStatus(n);   \
				if(__flag_##n == MEMORY_SLINK_BLOCK_ACTIVE){

#define MEMORY_SLINK_LOOP_END(allocator, n, earlyexit)              \
				}else if(__flag_##n == MEMORY_SLINK_BLOCK_INVALID){ \
					earlyexit                                       \
				}                                                   \
				n = memSLinkBlockNext(allocator, n);                \
			}                                                       \
			__region_##n = memAllocatorNext(__region_##n);          \
		} while(__region_##n != NULL);                              \
	}

#endif
