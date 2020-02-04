#ifndef HELPERSMISC_H
#define HELPERSMISC_H

#include "return.h"
#include "qualifiers.h"
#include <stddef.h>

#define LTOSTR_MAX_LENGTH 20  // Max number length for ltostr (including minus sign and null terminator). 64-bit just in case.

return_t pushDynamicArray(void **vector, const void *const __RESTRICT__ element, const size_t bytes, size_t *const __RESTRICT__ size, size_t *const __RESTRICT__ capacity);
size_t ltostr(long n, char *s);

/** TEMPORARY **/
void getDelimitedString(char *const __RESTRICT__ line, const size_t lineLength, const char *__RESTRICT__ delims, char **const __RESTRICT__ strStart, size_t *const __RESTRICT__ strLength);

#endif
