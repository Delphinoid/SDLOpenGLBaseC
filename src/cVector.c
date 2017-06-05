#include "cVector.h"
#include <string.h>

void cvInit(cVector *vec, const size_t capacity){
	vec->size = 0;
	vec->capacity = capacity;
	void **tempBuffer = malloc(sizeof(void *) * capacity);
	if(tempBuffer != NULL){
		vec->buffer = tempBuffer;
	}
}

void cvResize(cVector *vec, const size_t capacity){
	if(vec->capacity != capacity){
		void **tempBuffer = realloc(vec->buffer, sizeof(void *) * capacity);
		if(tempBuffer != NULL){
			void **newData = tempBuffer;
			vec->buffer = newData;
			vec->capacity = capacity;
		}
	}
}

void cvPush(cVector *vec, const void *data, const size_t bytes){
	void *tempPointer = malloc(bytes);
	if(tempPointer != NULL){
		if(vec->size == vec->capacity){
			cvResize(vec, vec->capacity * 2);
		}
		vec->buffer[vec->size] = tempPointer;
		memcpy(vec->buffer[vec->size++], data, bytes);
	}
}

void cvPop(cVector *vec){
	free(vec->buffer[--vec->size]);
}

void cvInsert(cVector *vec, const size_t pos, const void *data, const size_t bytes){
	if(pos < vec->size){
		void *tempPointer = malloc(bytes);
		if(tempPointer != NULL){
			if(vec->size == vec->capacity){
				cvResize(vec, vec->capacity * 2);
			}
			size_t i;
			for(i = pos + 1; i < vec->size; i++){
				vec->buffer[i] = vec->buffer[i - 1];
			}
			free(vec->buffer[pos]);
			vec->buffer[pos] = tempPointer;
			memcpy(vec->buffer[pos], data, bytes);
			vec->size++;
		}
	}else{
		cvPush(vec, data, bytes);
	}
}

void cvErase(cVector *vec, const size_t pos){
	if(pos < vec->size){
		free(vec->buffer[pos]);
		size_t i;
		for(i = pos; i < vec->size - 1; i++){
			vec->buffer[i] = vec->buffer[i + 1];
		}
		free(vec->buffer[--vec->size]);
		if(vec->size > 0 && vec->size == vec->capacity / 4){
			cvResize(vec, vec->capacity / 2);
		}
	}
}

void *cvGet(cVector *vec, const size_t pos){
	if(pos < vec->size){
		return(vec->buffer[pos]);
	}
	return(NULL);
}

void cvSet(cVector *vec, const size_t pos, const void *data, const size_t bytes){
	if(pos < vec->size){
		void *tempPointer = malloc(bytes);
		if(tempPointer != NULL){
			free(vec->buffer[pos]);
			vec->buffer[pos] = malloc(bytes);
			memcpy(vec->buffer[pos], data, bytes);
		}
	}
}

size_t cvSize(cVector *vec){
	return(vec->size);
}

void cvClear(cVector *vec){
	if(vec->buffer != NULL){
		size_t i;
		for(i = 0; i < vec->size; i++){
			free(vec->buffer[i]);
		}
		free(vec->buffer);
		vec->size = 0;
		vec->capacity = 0;
		vec->buffer = NULL;
	}
}
