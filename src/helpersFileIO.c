#include "helpersFileIO.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

return_t fileParseNextLine(FILE *const restrict file, char *const restrict buffer, size_t bufferSize, char **const line, size_t *const restrict lineLength){

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

__HINT_INLINE__ void fileGenerateFullPath(char *const restrict fullPath,
                                          const char *const restrict prgPath, const size_t prgLength,
                                          const char *const restrict resPath, const size_t resLength,
                                          const char *const restrict filePath, const size_t fileLength){

	char *path = fullPath;

	memcpy(path, prgPath, prgLength);
	path += prgLength;

	memcpy(path, resPath, resLength);
	path += resLength;

	memcpy(path, filePath, fileLength);
	path += fileLength;

	*path = '\0';

}

__HINT_INLINE__ void fileParseResourcePath(char *const restrict resPath, size_t *const restrict resLength, const char *const restrict line, const size_t length, const size_t offset){

	size_t pathBegin;
	size_t pathLength;
	const char *firstQuote = strchr(line+offset, '"');
	const char *secondQuote = NULL;
	char *delimiter;

	if(firstQuote != NULL){
		++firstQuote;
		pathBegin = firstQuote-line;
		secondQuote = strrchr(firstQuote, '"');
	}
	if(secondQuote > firstQuote){
		pathLength = secondQuote-firstQuote;
	}else{
		pathBegin = offset;
		pathLength = length-pathBegin;
	}

	strncpy(resPath, line+pathBegin, pathLength);
	resPath[pathLength] = '\0';

	// Replace Windows delimiters with Linux delimiters or vice versa.
	delimiter = resPath;
	while(*delimiter != '\0'){
		if(*delimiter == FILE_PATH_DELIMITER_CHAR_UNUSED){
			*delimiter = FILE_PATH_DELIMITER_CHAR;
		}
		++delimiter;
	}

	if(resLength != NULL){
		*resLength = pathLength;
	}

}

__HINT_INLINE__ char *fileGenerateResourceName(const char *const restrict resource, const size_t length){

	char *name;

	// Check if there's a file extension.
	// If there is, remove it.
	/*size_t extension = 0;
	char *c = resource[length-1];
	while(c != resource){
		if(*c == '.'){
			extension = resource[length-1] - c;
			break;
		}else if(*c == FILE_PATH_DELIMITER_CHAR){
			break;
		}
		--c;
	}*/

	// Allocate the name.
	name = memAllocate((length/*-extension*/+1)*sizeof(char));
	if(name != NULL){
		memcpy(name, resource, length/*-extension*/);
		name[length/*-extension*/] = '\0';
	}

	return name;

}
