#ifndef MEMORYQLINK_H
#define MEMORYQLINK_H

#include "memoryShared.h"

/*
** Quadruply-linked list allocator.
**
** Implements a doubly-linked list that
** may have two distinct, separate owners.
**
** Block format:
** [ Next block pointer A + active flag ][ Previous block pointer A ][ Next block pointer B ][ Previous block pointer B ][ Data (or free-list pointer) ]
*/

#define MEMORY_QLINK_BLOCK_ACTIVE        (uintptr_t)0x00
#define MEMORY_QLINK_BLOCK_INACTIVE      (uintptr_t)0x01
#define MEMORY_QLINK_BLOCK_INVALID       (uintptr_t)0x02
#define MEMORY_QLINK_BLOCK_INACTIVE_MASK (uintptr_t)0x03
#define MEMORY_QLINK_BLOCK_ACTIVE_MASK   (uintptr_t)~MEMORY_QLINK_BLOCK_INACTIVE_MASK

#define MEMORY_QLINK_BLOCK_POINTER_SIZE sizeof(byte_t *)
#define MEMORY_QLINK_BLOCK_FLAGS_SIZE   sizeof(uintptr_t)
#define MEMORY_QLINK_BLOCK_PREV_SIZE    MEMORY_QLINK_BLOCK_POINTER_SIZE
#define MEMORY_QLINK_BLOCK_NEXT_SIZE    MEMORY_QLINK_BLOCK_POINTER_SIZE
#define MEMORY_QLINK_BLOCK_HEADER_SIZE  ((MEMORY_QLINK_BLOCK_NEXT_SIZE + MEMORY_QLINK_BLOCK_PREV_SIZE) << 1)

#define MEMORY_QLINK_NEXT_A_OFFSET_FROM_BLOCK 0
#define MEMORY_QLINK_PREV_A_OFFSET_FROM_BLOCK (MEMORY_QLINK_NEXT_A_OFFSET_FROM_BLOCK + MEMORY_QLINK_BLOCK_NEXT_SIZE)
#define MEMORY_QLINK_NEXT_B_OFFSET_FROM_BLOCK (MEMORY_QLINK_PREV_A_OFFSET_FROM_BLOCK + MEMORY_QLINK_BLOCK_PREV_SIZE)
#define MEMORY_QLINK_PREV_B_OFFSET_FROM_BLOCK (MEMORY_QLINK_NEXT_B_OFFSET_FROM_BLOCK + MEMORY_QLINK_BLOCK_NEXT_SIZE)
#define MEMORY_QLINK_FLAGS_OFFSET_FROM_BLOCK  MEMORY_QLINK_NEXT_A_OFFSET_FROM_BLOCK
#define MEMORY_QLINK_DATA_OFFSET_FROM_BLOCK   MEMORY_QLINK_BLOCK_HEADER_SIZE
#define MEMORY_QLINK_NEXT_A_OFFSET_FROM_DATA  -MEMORY_QLINK_BLOCK_HEADER_SIZE
#define MEMORY_QLINK_PREV_A_OFFSET_FROM_DATA  (MEMORY_QLINK_NEXT_A_OFFSET_FROM_DATA + MEMORY_QLINK_BLOCK_NEXT_SIZE)
#define MEMORY_QLINK_NEXT_B_OFFSET_FROM_DATA  (MEMORY_QLINK_PREV_A_OFFSET_FROM_DATA + MEMORY_QLINK_BLOCK_PREV_SIZE)
#define MEMORY_QLINK_PREV_B_OFFSET_FROM_DATA  (MEMORY_QLINK_NEXT_B_OFFSET_FROM_DATA + MEMORY_QLINK_BLOCK_NEXT_SIZE)
#define MEMORY_QLINK_FLAGS_OFFSET_FROM_DATA   MEMORY_QLINK_NEXT_A_OFFSET_FROM_DATA
#define MEMORY_QLINK_BLOCK_OFFSET_FROM_DATA   -MEMORY_QLINK_BLOCK_HEADER_SIZE

#define memQLinkBlockGetFlags(block)         *((uintptr_t *)((byte_t *)block + MEMORY_QLINK_FLAGS_OFFSET_FROM_BLOCK))
#define memQLinkBlockGetNextA(block)         *((byte_t **)((byte_t *)block + MEMORY_QLINK_NEXT_A_OFFSET_FROM_BLOCK))
#define memQLinkBlockGetNextAPointer(block)   ((byte_t **)((byte_t *)block + MEMORY_QLINK_NEXT_A_OFFSET_FROM_BLOCK))
#define memQLinkBlockGetPrevA(block)         *((byte_t **)((byte_t *)block + MEMORY_QLINK_PREV_A_OFFSET_FROM_BLOCK))
#define memQLinkBlockGetPrevAPointer(block)   ((byte_t **)((byte_t *)block + MEMORY_QLINK_PREV_A_OFFSET_FROM_BLOCK))
#define memQLinkBlockGetNextB(block)         *((byte_t **)((byte_t *)block + MEMORY_QLINK_NEXT_B_OFFSET_FROM_BLOCK))
#define memQLinkBlockGetNextBPointer(block)   ((byte_t **)((byte_t *)block + MEMORY_QLINK_NEXT_B_OFFSET_FROM_BLOCK))
#define memQLinkBlockGetPrevB(block)         *((byte_t **)((byte_t *)block + MEMORY_QLINK_PREV_B_OFFSET_FROM_BLOCK))
#define memQLinkBlockGetPrevBPointer(block)   ((byte_t **)((byte_t *)block + MEMORY_QLINK_PREV_B_OFFSET_FROM_BLOCK))
#define memQLinkBlockGetNextFree(block)      *((byte_t **)((byte_t *)block + MEMORY_QLINK_NEXT_A_OFFSET_FROM_BLOCK))
#define memQLinkBlockGetNextFreeMasked(block) ((byte_t *)(*((uintptr_t *)((byte_t *)block + MEMORY_QLINK_NEXT_A_OFFSET_FROM_BLOCK)) & MEMORY_QLINK_BLOCK_ACTIVE_MASK))
#define memQLinkBlockGetData(block)           ((byte_t *)block + MEMORY_QLINK_DATA_OFFSET_FROM_BLOCK)

#define memQLinkBlockGetActive(block)        *((uintptr_t *)((byte_t *)block + MEMORY_QLINK_FLAGS_OFFSET_FROM_BLOCK))
#define memQLinkBlockGetActiveMasked(block) (*((uintptr_t *)((byte_t *)block + MEMORY_QLINK_FLAGS_OFFSET_FROM_BLOCK)) & MEMORY_QLINK_BLOCK_INACTIVE_MASK)
#define memQLinkBlockGetActivePointer(block)  ((uintptr_t *)((byte_t *)block + MEMORY_QLINK_FLAGS_OFFSET_FROM_BLOCK))
#define memQLinkBlockSetActive(block)        *((uintptr_t *)((byte_t *)block + MEMORY_QLINK_FLAGS_OFFSET_FROM_BLOCK)) &= MEMORY_QLINK_BLOCK_ACTIVE_MASK
#define memQLinkBlockSetInactive(block)      *((uintptr_t *)((byte_t *)block + MEMORY_QLINK_FLAGS_OFFSET_FROM_BLOCK)) |= MEMORY_QLINK_BLOCK_INACTIVE_MASK

#define memQLinkDataGetFlags(data)         *((uintptr_t *)((byte_t *)data + MEMORY_QLINK_FLAGS_OFFSET_FROM_DATA))
#define memQLinkDataGetNextA(data)         *((byte_t **)((byte_t *)data + MEMORY_QLINK_NEXT_A_OFFSET_FROM_DATA))
#define memQLinkDataGetNextAPointer(data)   ((byte_t **)((byte_t *)data + MEMORY_QLINK_NEXT_A_OFFSET_FROM_DATA))
#define memQLinkDataGetPrevA(data)         *((byte_t **)((byte_t *)data + MEMORY_QLINK_PREV_A_OFFSET_FROM_DATA))
#define memQLinkDataGetPrevAPointer(data)   ((byte_t **)((byte_t *)data + MEMORY_QLINK_PREV_A_OFFSET_FROM_DATA))
#define memQLinkDataGetNextB(data)         *((byte_t **)((byte_t *)data + MEMORY_QLINK_NEXT_B_OFFSET_FROM_DATA))
#define memQLinkDataGetNextBPointer(data)   ((byte_t **)((byte_t *)data + MEMORY_QLINK_NEXT_B_OFFSET_FROM_DATA))
#define memQLinkDataGetPrevB(data)         *((byte_t **)((byte_t *)data + MEMORY_QLINK_PREV_B_OFFSET_FROM_DATA))
#define memQLinkDataGetPrevBPointer(data)   ((byte_t **)((byte_t *)data + MEMORY_QLINK_PREV_B_OFFSET_FROM_DATA))
#define memQLinkDataGetNextFree(data)      *((byte_t **)((byte_t *)data + MEMORY_QLINK_NEXT_A_OFFSET_FROM_DATA))
#define memQLinkDataGetNextFreeMasked(data) ((byte_t *)(*((uintptr_t *)((byte_t *)data + MEMORY_QLINK_NEXT_A_OFFSET_FROM_DATA)) & MEMORY_QLINK_BLOCK_ACTIVE_MASK))
#define memQLinkDataGetBlock(data)          ((byte_t *)data + MEMORY_QLINK_BLOCK_OFFSET_FROM_DATA)

#define memQLinkDataGetActive(data)        *((uintptr_t *)((byte_t *)data + MEMORY_QLINK_FLAGS_OFFSET_FROM_DATA))
#define memQLinkDataGetActiveMasked(data) (*((uintptr_t *)((byte_t *)data + MEMORY_QLINK_FLAGS_OFFSET_FROM_DATA)) & MEMORY_QLINK_BLOCK_INACTIVE_MASK)
#define memQLinkDataGetActivePointer(data)  ((uintptr_t *)((byte_t *)data + MEMORY_QLINK_FLAGS_OFFSET_FROM_DATA))

#define memQLinkDataSetFlags(data, flag) *((uintptr_t *)((byte_t *)data + MEMORY_QLINK_FLAGS_OFFSET_FROM_DATA)) &= MEMORY_QLINK_BLOCK_ACTIVE_MASK; \
                                         *((uintptr_t *)((byte_t *)data + MEMORY_QLINK_FLAGS_OFFSET_FROM_DATA)) |= flag;

#ifdef MEMORY_QLINK_LEAN
	#define MEMORY_QLINK_ALIGN(x) x
#else
	#define MEMORY_QLINK_ALIGN(x) MEMORY_ALIGN(x)
#endif

typedef struct {
	size_t block;  // Block size.
	byte_t *free;  // Next free block pointer.
	memoryRegion *region;
} memoryQLink;

#define memQLinkBlockSize(bytes) MEMORY_QLINK_ALIGN(bytes + MEMORY_QLINK_BLOCK_HEADER_SIZE)
#define memQLinkBlockSizeUnaligned(bytes)          (bytes + MEMORY_QLINK_BLOCK_HEADER_SIZE)
#ifndef MEMORY_ALLOCATOR_ALIGNED
	#define memQLinkAlignStartBlock(start) ((byte_t *)MEMORY_QLINK_ALIGN((uintptr_t)start + MEMORY_QLINK_BLOCK_HEADER_SIZE) - MEMORY_QLINK_BLOCK_HEADER_SIZE)
	#define memQLinkAlignStartData(start)  ((byte_t *)MEMORY_QLINK_ALIGN((uintptr_t)start + MEMORY_QLINK_BLOCK_HEADER_SIZE))
#else
	#define memQLinkAlignStartBlock(start) start
	#define memQLinkAlignStartData(start)  ((byte_t *)start + MEMORY_QLINK_BLOCK_HEADER_SIZE)
#endif
#define memQLinkAllocationSize(start, bytes, length) \
	(memQLinkBlockSize(bytes) * length + (uintptr_t)memQLinkAlignStartBlock(start) - (uintptr_t)start)
	// The following can save small amounts of memory but can't be predicted as easily:
	//(memQLinkBlockSize(bytes) * (length - 1) + memQLinkBlockSizeUnaligned(bytes) + (uintptr_t)memQLinkAlignStartBlock(start) - (uintptr_t)start)

#define memQLinkFirst(region)           ((void *)memQLinkAlignStartData((region)->start))
#define memQLinkPrevA(i)                memQLinkDataGetPrevA(i)
#define memQLinkNextA(i)                memQLinkDataGetNextA(i)
#define memQLinkPrevB(i)                memQLinkDataGetPrevB(i)
#define memQLinkNextB(i)                memQLinkDataGetNextB(i)
#define memQLinkBlockStatus(block)      memQLinkDataGetActiveMasked(block)
#define memQLinkBlockNext(array, i)     (void *)((byte_t *)i + (array).block)
#define memQLinkBlockPrevious(array, i) (void *)((byte_t *)i - (array).block)

void memQLinkInit(memoryQLink *const restrict array);
void *memQLinkCreate(memoryQLink *const restrict array, void *const start, const size_t bytes, const size_t length);
void *memQLinkCreateInit(memoryQLink *const restrict array, void *const start, const size_t bytes, const size_t length, void (*func)(void *const restrict block));
void *memQLinkAllocate(memoryQLink *const restrict array);
void *memQLinkAllocateSorted(memoryQLink *const restrict array, void **const startA, void **const startB, void *const prevA, void *const nextA, void *const prevB, void *const nextB, unsigned int swapA, unsigned int swapB);
void memQLinkFree(memoryQLink *const restrict array, void *const element);
void memQLinkFreeSorted(memoryQLink *const restrict array, void **const startA, void **const startB, void *const element);
void *memQLinkSetupMemory(void *start, const size_t bytes, const size_t length);
void *memQLinkSetupMemoryInit(void *start, const size_t bytes, const size_t length, void (*func)(void *const restrict block));
void memQLinkClear(memoryQLink *const restrict array);
void memQLinkClearInit(memoryQLink *const restrict array, void (*func)(void *const restrict block));
void *memQLinkExtend(memoryQLink *const restrict array, void *const start, const size_t bytes, const size_t length);
void *memQLinkExtendInit(memoryQLink *const restrict array, void *const start, const size_t bytes, const size_t length, void (*func)(void *const restrict block));
void memQLinkDelete(memoryQLink *const restrict array);

#define MEMORY_QLINK_LOOP_BEGIN(allocator, n, type)                      \
	{                                                                    \
		const memoryRegion *__region_##n = allocator.region;             \
		do {                                                             \
			type n = memQLinkFirst(__region_##n);                        \
			while(n < (type)memAllocatorEnd(__region_##n)){              \
				const byte_t __flag_##n = memQLinkDataGetActive(n);      \
				if((__flag_##n & MEMORY_QLINK_BLOCK_INACTIVE_MASK) == 0){

#define MEMORY_QLINK_LOOP_INACTIVE_CASE(n)                               \
				}else if((__flag_##n & MEMORY_QLINK_BLOCK_INACTIVE) > 0){

#define MEMORY_QLINK_LOOP_END(allocator, n, earlyexit)                   \
				}else if((__flag_##n & MEMORY_QLINK_BLOCK_INVALID) > 0){ \
					earlyexit                                            \
				}                                                        \
				n = memQLinkBlockNext(allocator, n);                     \
			}                                                            \
			__region_##n = memAllocatorNext(__region_##n);               \
		} while(__region_##n != NULL);                                   \
	}

#endif
