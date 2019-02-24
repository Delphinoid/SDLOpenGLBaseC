#ifndef HELPERSFILEIO_H
#define HELPERSFILEIO_H

#include "return.h"
#include <stdio.h>

#define FILE_MAX_PATH_LENGTH 1024
#define FILE_MAX_LINE_LENGTH 1024

#ifdef _WIN32
	#define FILE_PATH_DELIMITER_CHAR   '\\'
	#define FILE_PATH_DELIMITER_STRING "\\"
	#define FILE_PATH_DELIMITER_CHAR_UNUSED   '/'
	#define FILE_PATH_DELIMITER_STRING_UNUSED "/"
#else
	#define FILE_PATH_DELIMITER_CHAR   '/'
	#define FILE_PATH_DELIMITER_STRING "/"
	#define FILE_PATH_DELIMITER_CHAR_UNUSED   '\\'
	#define FILE_PATH_DELIMITER_STRING_UNUSED "\\"
#endif

typedef unsigned int fileLine_t;

return_t fileParseNextLine(FILE *const restrict file, char *const restrict buffer, size_t bufferSize, char **const line, size_t *const restrict lineLength);
void fileGenerateFullPath(char *const restrict fullPath, const char *const restrict prgPath, const size_t prgLength, const char *const restrict resPath, const size_t resLength, const char *const restrict filePath, const size_t fileLength);
void fileParseResourcePath(char *const restrict resPath, size_t *const restrict resLength, const char *const restrict line, const size_t length, const size_t offset);
char *fileGenerateResourceName(const char *const restrict resource, const size_t length);

#endif
