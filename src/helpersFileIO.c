#include "helpersFileIO.h"
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

__FORCE_INLINE__ void fileGenerateFullPath(char *fullPath, const char *prgPath, const size_t prgLength, const char *filePath, const size_t fileLength){

	memcpy(fullPath, prgPath, prgLength);
	memcpy(fullPath+prgLength, filePath, fileLength);
	fullPath[prgLength+fileLength] = '\0';

}
