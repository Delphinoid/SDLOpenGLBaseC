#ifndef HELPERSFILEIO_H
#define HELPERSFILEIO_H

#include "return.h"
#include "qualifiers.h"
#include <stdio.h>

#ifdef _WIN32

	#include <windows.h>

	#define FILE_PATH_DELIMITER_CHAR   '\\'
	#define FILE_PATH_DELIMITER_STRING "\\"
	#define FILE_PATH_DELIMITER_CHAR_UNUSED   '/'
	#define FILE_PATH_DELIMITER_STRING_UNUSED "/"

	#define S_IREAD _S_IREAD
	#define S_IWRITE _S_IWRITE
	#define S_IRUSR _S_IREAD
	#define S_IWUSR _S_IWRITE

	#define mkdir(path, mode) _mkdir(path)
	#define rmdir(path) _rmdir(path)
	#define chmod(path, mode) _chmod(path, mode)

#else

	#define FILE_PATH_DELIMITER_CHAR   '/'
	#define FILE_PATH_DELIMITER_STRING "/"
	#define FILE_PATH_DELIMITER_CHAR_UNUSED   '\\'
	#define FILE_PATH_DELIMITER_STRING_UNUSED "\\"

#endif

#define FILE_MAX_PATH_LENGTH 4096
#define FILE_MAX_LINE_LENGTH 1024

typedef unsigned int fileLine_t;

return_t fileParseNextLine(FILE *const __RESTRICT__ file, char *const __RESTRICT__ buffer, size_t bufferSize, char **const line, size_t *const __RESTRICT__ lineLength);
void fileGenerateFullPath(char *const __RESTRICT__ fullPath, const char *const __RESTRICT__ prgPath, const size_t prgLength, const char *const __RESTRICT__ resPath, const size_t resLength, const char *const __RESTRICT__ filePath, const size_t fileLength);
void fileProcessPath(char *path);
void fileParseResourcePath(char *const __RESTRICT__ resPath, size_t *const __RESTRICT__ resLength, const char *const __RESTRICT__ line, const size_t length);
char *fileGenerateResourceName(const char *const __RESTRICT__ resource, const size_t length);

#endif
