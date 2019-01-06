#ifndef HELPERSMISC_H
#define HELPERSMISC_H

#include "return.h"
#include <stddef.h>

#define LTOSTR_MAX_LENGTH 20  // Max number length for ltostr (including minus sign and null terminator). 64-bit just in case.

return_t pushDynamicArray(void **const restrict vector, const void *const restrict element, const size_t bytes, size_t *const restrict size, size_t *const restrict capacity);
size_t ltostr(long n, char *s);

/** TEMPORARY **/
void getDelimitedString(char *const restrict line, const size_t lineLength, const char *restrict delims, char **const restrict strStart, size_t *const restrict strLength);

#endif
