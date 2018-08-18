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

typedef struct {
	byte_t *start;
	byte_t *next;
	byte_t *end;
} memoryStack;

byte_t *memStackInit(memoryStack *stack, byte_t *start, const size_t bytes);
byte_t *memStackPush(memoryStack *stack, const size_t bytes);
void memStackPop(memoryStack *stack, const size_t bytes);
void memStackShrink(memoryStack *stack, const size_t bytes);
void memStackClear(memoryStack *stack);

#endif
