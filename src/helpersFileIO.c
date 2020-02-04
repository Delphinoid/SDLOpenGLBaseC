#include "helpersFileIO.h"
#include "memoryManager.h"
#include <string.h>

return_t fileParseNextLine(FILE *const __RESTRICT__ file, char *const __RESTRICT__ buffer, size_t bufferSize, char **const line, size_t *const __RESTRICT__ lineLength){

	if(fgets(buffer, bufferSize, file)){

		char *commentPos;
		int doneFront = 0, doneEnd = 0;
		size_t newOffset = 0;
		size_t i;

		*line = buffer;
		*lineLength = strlen(*line);

		// Remove new line and carriage return.
		if((*line)[*lineLength-1] == '\n'){
			(*line)[--(*lineLength)] = '\0';
		}
		if((*line)[*lineLength-1] == '\r'){
			(*line)[--(*lineLength)] = '\0';
		}

		// Remove any comments from the line.
		commentPos = strstr(*line, "//");
		if(commentPos != NULL){
			*lineLength -= *lineLength-(size_t)(commentPos-*line);
			*commentPos = '\0';
		}

		// Remove any indentations from the line, as well as any trailing spaces and tabs.
		for(i = 0; i < *lineLength && (!doneFront || !doneEnd); ++i){
			if(!doneFront && (*line)[i] != '\t' && (*line)[i] != ' '){
				newOffset = i;
				doneFront = 1;
			}
			if(!doneEnd && i > 0 && (*line)[*lineLength-i] != '\t' && (*line)[*lineLength-i] != ' '){
				*lineLength -= i-1;
				(*line)[*lineLength] = '\0';
				doneEnd = 1;
			}
		}

		*line += newOffset;
		*lineLength -= newOffset;

		return 1;

	}

	return 0;

}

__HINT_INLINE__ void fileGenerateFullPath(char *const __RESTRICT__ fullPath,
                                          const char *const __RESTRICT__ prgPath, const size_t prgLength,
                                          const char *const __RESTRICT__ resPath, const size_t resLength,
                                          const char *const __RESTRICT__ filePath, const size_t fileLength){

	char *path = fullPath;

	memcpy(path, prgPath, prgLength);
	path += prgLength;

	memcpy(path, resPath, resLength);
	path += resLength;

	memcpy(path, filePath, fileLength);
	path += fileLength;

	*path = '\0';

}

__FORCE_INLINE__ void fileProcessPath(char *path){
	// Replace Windows delimiters with Linux delimiters or vice versa.
	while(*path != '\0'){
		if(*path == FILE_PATH_DELIMITER_CHAR_UNUSED){
			*path = FILE_PATH_DELIMITER_CHAR;
		}
		++path;
	}
}

__HINT_INLINE__ void fileParseResourcePath(char *const __RESTRICT__ resPath, size_t *const __RESTRICT__ resLength, const char *const __RESTRICT__ line, const size_t length){

	size_t pathBegin;
	size_t pathLength;
	const char *firstQuote = strchr(line, '"');
	const char *secondQuote = NULL;

	if(firstQuote != NULL){
		++firstQuote;
		pathBegin = firstQuote-line;
		secondQuote = strrchr(firstQuote, '"');
	}
	if(secondQuote > firstQuote){
		pathLength = secondQuote-firstQuote;
	}else{
		pathBegin = 0;
		pathLength = length-pathBegin;
	}

	strncpy(resPath, line+pathBegin, pathLength);
	resPath[pathLength] = '\0';

	fileProcessPath(resPath);

	if(resLength != NULL){
		*resLength = pathLength;
	}

}

__HINT_INLINE__ char *fileGenerateResourceName(const char *const __RESTRICT__ resource, const size_t length){

	char *name;

	// Check if there's a file extension.
	// If there is, remove it.
	/**size_t extension = 0;
	char *c = resource[length-1];
	while(c != resource){
		if(*c == '.'){
			extension = resource[length-1] - c;
			break;
		}else if(*c == FILE_PATH_DELIMITER_CHAR){
			break;
		}
		--c;
	}**/

	// Allocate the name.
	name = memAllocate((length/**-extension**/+1)*sizeof(char));
	if(name != NULL){
		memcpy(name, resource, length/**-extension**/);
		name[length/**-extension**/] = '\0';
	}

	return name;

}
