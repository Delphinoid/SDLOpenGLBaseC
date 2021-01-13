#include "helpersFileIO.h"
#include "memoryManager.h"
#include <string.h>

return_t fileChangeDirectory(char *const program, size_t *const length){
	// Removes program name (everything after the last backslash) from the path.
	return_t r;
	char *const delim = strrchr(program, FILE_PATH_DELIMITER_CHAR) + 1;
	const char old = *delim;
	*delim = '\0';
	if(length != NULL){
		*length = delim-program;
	}
	// Change the working directory.
	r = chdir(program);
	*delim = old;
	return r;
}

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

__HINT_INLINE__ size_t fileGenerateFullPath(
	char *const __RESTRICT__ fullPath,
	const char *const __RESTRICT__ resPath, const size_t resPathLength,
	const char *const __RESTRICT__ filePath, const size_t filePathLength
){

	char *path = fullPath;

	memcpy(path, resPath, resPathLength);
	path += resPathLength;

	memcpy(path, filePath, filePathLength);
	path += filePathLength;

	*path = '\0';

	return (size_t)(path - fullPath);

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

__HINT_INLINE__ size_t fileParseResourcePath(char *const __RESTRICT__ resPath, const char *const __RESTRICT__ line, const size_t lineLength){

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
		pathLength = lineLength-pathBegin;
	}

	memcpy(resPath, line+pathBegin, pathLength);
	resPath[pathLength] = '\0';

	fileProcessPath(resPath);

	return pathLength;

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
