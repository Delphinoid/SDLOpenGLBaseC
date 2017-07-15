#ifndef CVECTOR_H
#define CVECTOR_H

#include <stdlib.h>

typedef struct {
	size_t size;
	size_t capacity;
	void **buffer;
} cVector;

unsigned char cvInit(cVector *vec, const size_t capacity);
unsigned char cvResize(cVector *vec, const size_t capacity);
unsigned char cvPush(cVector *vec, const void *data, size_t bytes);
unsigned char cvPop(cVector *vec);
unsigned char cvInsert(cVector *vec, const size_t pos, const void *data, const size_t bytes);
unsigned char cvErase(cVector *vec, const size_t pos);
void *cvGet(cVector *vec, const size_t pos);
unsigned char cvSet(cVector *vec, const size_t pos, const void *data, const size_t bytes);
size_t cvSize(cVector *vec);
void cvClear(cVector *vec);

#endif
