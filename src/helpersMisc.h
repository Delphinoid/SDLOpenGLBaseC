#ifndef HELPERSMISC_H
#define HELPERSMISC_H

#include "typedefs.h"
#include <stdlib.h>

#define LTOSTR_MAX_LENGTH 20  // Max number length for ltostr (including minus sign and null terminator). 64-bit just in case.

return_t pushDynamicArray(void **vector, const void *element, const size_t bytes, size_t *size, size_t *capacity);
size_t ltostr(long n, char *s);

/** TEMPORARY **/
void getDelimitedString(char *line, const size_t lineLength, const char *delims, char **strStart, size_t *strLength);

#endif
