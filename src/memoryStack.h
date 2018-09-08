#ifndef MEMORYSTACK_H
#define MEMORYSTACK_H

#include "memoryShared.h"

/*
** Stack allocator.
**
** Relies on user remembering how much memory
** they've allocated if they wish to make a
** deallocation. In my opinion it's good
** practice (and even necessary in many cases)
** to keep track of how much memory you're
** allocating. This also eliminates overhead
** for cases that don't need it, i.e. stacks
** that will only ever be added to or cleared.
*/

#define MEMORY_STACK_UNSPECIFIED_LENGTH MEMORY_UNSPECIFIED_LENGTH

typedef struct {
	byte_t *next;
	byte_t *start;
	byte_t *end;
} memoryStack;

#define memStackBlockSize(bytes) bytes
#define memStackAllocationSize(start, bytes, length) (length > 0 ? memStackBlockSize(bytes) * length : bytes)

byte_t *memStackInit(memoryStack *stack, byte_t *start, const size_t bytes, const size_t length);
byte_t *memStackPush(memoryStack *stack, const size_t bytes);
void memStackPop(memoryStack *stack, const size_t bytes);
void memStackShrink(memoryStack *stack, const size_t bytes);
void memStackClear(memoryStack *stack);

#endif
