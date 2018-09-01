#include "memoryStack.h"

inline size_t memStackAllocationOverhead(const byte_t *start, const size_t bytes){
	/*
	** Returns the total allocation overhead.
	*/
	return 0;
}

byte_t *memStackInit(memoryStack *stack, byte_t *start, const size_t bytes){
	stack->start = start;
	stack->next = start;
	stack->end = start + bytes;
	return start;
}

byte_t *memStackPush(memoryStack *stack, const size_t bytes){
	byte_t *r = stack->next;
	stack->next += bytes;
	if(stack->next > stack->end){
		stack->next -= bytes;
		return NULL;
	}
	return r;
}

void memStackPop(memoryStack *stack, const size_t bytes){
	stack->next -= bytes;
	if(stack->next < stack->start){
		stack->next = stack->start;
	}
}

void memStackShrink(memoryStack *stack, const size_t bytes){
	stack->end = stack->start + bytes;
}

inline void memStackClear(memoryStack *stack){
	stack->next = stack->start;
}
