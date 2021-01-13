#ifndef CONSOLE_H
#define CONSOLE_H

#include <stddef.h>

#define CONSOLE_BUFFER_SIZE 65536

typedef struct {
	char buffer[CONSOLE_BUFFER_SIZE];
	size_t start;  // The current beginning of the buffer.
} console;

#endif
