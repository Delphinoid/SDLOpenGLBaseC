#include "helpersFileIO.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

return_t fileParseNextLine(FILE *file, char *buffer, size_t bufferSize, char **line, size_t *lineLength){

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

__FORCE_INLINE__ void fileGenerateFullPath(char *fullPath,
                                           const char *prgPath, const size_t prgLength,
                                           const char *resPath, const size_t resLength,
                                           const char *filePath, const size_t fileLength){

	char *path = fullPath;

	memcpy(path, prgPath, prgLength);
	path += prgLength;

	memcpy(path, resPath, resLength);
	path += resLength;

	memcpy(path, filePath, fileLength);
	path += fileLength;

	*path = '\0';

}

__FORCE_INLINE__ char *fileGenerateResourceName(const char *resource, const size_t length){

	char *name;

	// Check if there's a file extension.
	// If there is, remove it.
	/*size_t extension = 0;
	char *c = resource[length-1];
	while(c != resource){
		if(*c == '.'){
			extension = resource[length-1] - c;
			break;
		}else if(*c == '\\'){
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
