#ifndef HELPERSFILEIO_H
#define HELPERSFILEIO_H

#include "return.h"
#include <stdio.h>

#define FILE_MAX_PATH_LENGTH 1024
#define FILE_MAX_LINE_LENGTH 1024

typedef unsigned int fileLine_t;

return_t fileParseNextLine(FILE *const restrict file, char *const restrict buffer, size_t bufferSize, char **const line, size_t *const restrict lineLength);
void fileGenerateFullPath(char *const restrict fullPath, const char *const restrict prgPath, const size_t prgLength, const char *const restrict resPath, const size_t resLength, const char *const restrict filePath, const size_t fileLength);
char *fileGenerateResourceName(const char *const restrict resource, const size_t length);

#endif
