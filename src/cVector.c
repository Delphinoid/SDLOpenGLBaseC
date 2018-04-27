#include "cVector.h"
#include <string.h>

signed char cvInit(cVector *vec, const size_t capacity){
	vec->size = 0;
	void **tempBuffer = malloc(sizeof(void *) * capacity);
	if(capacity > 0 && tempBuffer == NULL){
		return -1;
	}
	vec->buffer = tempBuffer;
	vec->capacity = capacity;
	return 1;
}

signed char cvResize(cVector *vec, const size_t capacity){
	if(vec->capacity != capacity){
		void **tempBuffer = realloc(vec->buffer, sizeof(void *) * capacity);
		if(tempBuffer == NULL){
			return -1;
		}
		vec->buffer = tempBuffer;
		vec->capacity = capacity;
	}
	return 1;
}

signed char cvPush(cVector *vec, const void *data, const size_t bytes){
	void *tempPointer = malloc(bytes);
	if(tempPointer == NULL){
		return -1;
	}
	if(vec->size == vec->capacity){
		if(!cvResize(vec, vec->capacity << 1)){
			return -1;
		}
	}
	vec->buffer[vec->size] = tempPointer;
	memcpy(vec->buffer[vec->size++], data, bytes);
	return 1;
}

signed char cvPop(cVector *vec){
	if(vec->size > 0){
		free(vec->buffer[--vec->size]);
		return 1;
	}
	return 0;
}

signed char cvInsert(cVector *vec, const size_t pos, const void *data, const size_t bytes){
	if(pos < vec->size){
		void *tempPointer = malloc(bytes);
		if(tempPointer != NULL){
			if(vec->size == vec->capacity){
				cvResize(vec, vec->capacity << 1);
			}
			size_t i;
			for(i = pos + 1; i < vec->size; ++i){
				vec->buffer[i] = vec->buffer[i - 1];
			}
			free(vec->buffer[pos]);
			vec->buffer[pos] = tempPointer;
			memcpy(vec->buffer[pos], data, bytes);
			++vec->size;
			return 1;
		}
	}else{
		return cvPush(vec, data, bytes);
	}
	return 0;
}

signed char cvErase(cVector *vec, const size_t pos){
	if(pos < vec->size){
		free(vec->buffer[pos]);
		--vec->size;
		size_t i;
		for(i = pos; i < vec->size; ++i){
			vec->buffer[i] = vec->buffer[i + 1];
		}
		return 1;
	}
	return 0;
}

void *cvGet(cVector *vec, const size_t pos){
	if(pos < vec->size){
		return vec->buffer[pos];
	}
	return NULL;
}

signed char cvSet(cVector *vec, const size_t pos, const void *data, const size_t bytes){
	if(pos < vec->size){
		void *tempPointer = malloc(bytes);
		if(tempPointer != NULL){
			free(vec->buffer[pos]);
			vec->buffer[pos] = malloc(bytes);
			memcpy(vec->buffer[pos], data, bytes);
			return 1;
		}
	}
	return 0;
}

size_t cvSize(cVector *vec){
	return vec->size;
}

void cvClear(cVector *vec){
	if(vec->buffer != NULL){
		size_t i;
		for(i = 0; i < vec->size; ++i){
			free(vec->buffer[i]);
		}
		free(vec->buffer);
		vec->size = 0;
		vec->capacity = 0;
		vec->buffer = NULL;
	}
}
