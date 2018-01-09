#ifndef CVECTOR_H
#define CVECTOR_H

#include <stdlib.h>

typedef struct {
	size_t size;
	size_t capacity;
	void **buffer;
} cVector;

signed char cvInit(cVector *vec, const size_t capacity);
signed char cvResize(cVector *vec, const size_t capacity);
signed char cvPush(cVector *vec, const void *data, size_t bytes);
signed char cvPop(cVector *vec);
signed char cvInsert(cVector *vec, const size_t pos, const void *data, const size_t bytes);
signed char cvErase(cVector *vec, const size_t pos);
void *cvGet(cVector *vec, const size_t pos);
signed char cvSet(cVector *vec, const size_t pos, const void *data, const size_t bytes);
size_t cvSize(cVector *vec);
void cvClear(cVector *vec);

#endif
