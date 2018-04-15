#ifndef HELPERSMISC_H
#define HELPERSMISC_H

#include <stdlib.h>
#include <stdio.h>

#define LTOSTR_MAX_LENGTH 20  // Max number length for ltostr (including minus sign and null terminator). 64-bit just in case.

signed char pushDynamicArray(void **vector, const void *element, const size_t bytes, size_t *size, size_t *capacity);
size_t ltostr(long n, char *s);
char *fileParseNextLine(FILE *file, char *buffer, size_t bufferSize, char **line, size_t *lineLength);

/** TEMPORARY **/
void getDelimitedString(char *line, const size_t lineLength, const char *delims, char **strStart, size_t *strLength);

#endif
