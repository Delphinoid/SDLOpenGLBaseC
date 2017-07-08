#ifndef CVECTOR_H
#define CVECTOR_H

#include <stdlib.h>

typedef struct {
	size_t size;
	size_t capacity;
	void **buffer;
} cVector;

void cvInit(cVector *vec, const size_t capacity);
void cvResize(cVector *vec, const size_t capacity);
void cvPush(cVector *vec, const void *data, size_t bytes);
void cvPop(cVector *vec);
void cvInsert(cVector *vec, const size_t pos, const void *data, const size_t bytes);
void cvErase(cVector *vec, const size_t pos);
void *cvGet(cVector *vec, const size_t pos);
void cvSet(cVector *vec, const size_t pos, const void *data, const size_t bytes);
size_t cvSize(cVector *vec);
void cvClear(cVector *vec);

#endif
