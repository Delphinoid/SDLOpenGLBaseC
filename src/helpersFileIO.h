#ifndef HELPERSFILEIO_H
#define HELPERSFILEIO_H

#include "return.h"
#include <stdio.h>

#define FILE_MAX_PATH_LENGTH 1024
#define FILE_MAX_LINE_LENGTH 1024

typedef unsigned int fileLine_t;

return_t fileParseNextLine(FILE *file, char *buffer, size_t bufferSize, char **line, size_t *lineLength);
void fileGenerateFullPath(char *fullPath, const char *prgPath, const size_t prgLength, const char *resPath, const size_t resLength, const char *filePath, const size_t fileLength);
char *fileGenerateResourceName(const char *resource, const size_t length);

#endif
