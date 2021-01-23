#ifndef CONSOLE_H
#define CONSOLE_H

#include "memoryShared.h"
#include <stddef.h>

#define CONSOLE_BUFFER_SIZE 200

typedef struct {
	byte_t buffer[CONSOLE_BUFFER_SIZE];
	byte_t *start;
	byte_t *end;
} console;

void conInit(console *const __RESTRICT__ con);
/// This should probably become part of txtStream.
void conAppend(console *const __RESTRICT__ con, const byte_t *str, const size_t strLength);

#endif
