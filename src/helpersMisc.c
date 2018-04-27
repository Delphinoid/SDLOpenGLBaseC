#include "helpersMisc.h"
#include <string.h>

/*signed char nextToken(const char *str, const char *delims, size_t *tokenOffset, size_t *tokenLength){
	* Replacement for strtok() because it's a horrible function. *
	char *tokenStart = NULL;
	const char *currentChar = str+(*tokenOffset);
	while(*currentChar != '\0'){
		const char *currentDelim = delims;
		while(*currentDelim != '\0'){
			if(*currentChar == *currentDelim){
				if(tokenStart == NULL){
					tokenStart = (char *)(currentChar-str-(*tokenOffset));
				}else{
					*tokenLength = currentChar-str-(*tokenOffset)-(size_t)tokenStart;
					*tokenOffset = (size_t)tokenStart;
					return 1;
				}
			}
			++currentDelim;
		}
		++currentChar;
	}
	if(tokenStart != NULL){
		*tokenLength = currentChar-str-(*tokenOffset)-(size_t)tokenStart;
		*tokenOffset = (size_t)tokenStart;
		return 1;
	}
	return 0;
}*/

signed char pushDynamicArray(void **vector, const void *element, const size_t bytes, size_t *size, size_t *capacity){
	/* Push an element into a dynamic array. */
	if(*size == *capacity){
		if(*capacity == 0){
			*capacity = 1;
		}else{
			*capacity *= 2;
		}
		void *tempBuffer = realloc(*vector, (*capacity)*bytes);
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		*vector = tempBuffer;
	}
	memcpy(((char *)(*vector))+(*size)*bytes, element, bytes);
	++(*size);
	return 1;
}

/*size_t ltostr(long n, char **s){
	* Converts a long to a C-string. *
	size_t l;  // Length of the ouput (discluding null terminator).
	if(n == 0){
		*s = malloc(2*sizeof(char));
		**s = '0';
		*(*s+1) = '\0';
		l = 1;
	}else{
		const size_t m = 12;  // Max number length (including minus sign and null terminator).
		l = m;
		*s = malloc(m*sizeof(char));
		// Add a minus sign to the beginning if necessary.
		if(n < 0){
			**s = '-';
			n = -n;
		}else{
			**s = '\0';
		}
		// Loop through the number backwards.
		while(n > 0){
			*(*s+--l) = '0' + n % 10;
			n /= 10;
		}
		// Shift everything from the end of the array over to the beginning.
		if(*s[0] == '-'){
			memcpy(*s+1, *s+l, m-l);
		}else{
			memcpy(*s, *s+l, m-l);
		}
		// Resize *s to fit the string and add a null terminator.
		l = m-l+1;
		realloc(s, l);
		*(*s+l--) = '\0';
	}
	return l;
}*/

size_t ltostr(long n, char *s){
	/* Converts a long to a C-string. */
	size_t l;  // Length of the ouput (discluding null terminator).
	if(n == 0){
		s[0] = '0';
		l = 1;
	}else{
		const size_t m = LTOSTR_MAX_LENGTH;
		l = m;
		// Add a minus sign to the beginning if necessary.
		if(n < 0){
			s[0] = '-';
			n = -n;
		}else{
			s[0] = '\0';
		}
		// Loop through the number backwards.
		while(n > 0){
			--l;
			s[l] = '0' + n % 10;
			n /= 10;
		}
		// Shift everything from the end of the array over to the beginning.
		if(s[0] == '-'){
			memcpy(&s[1], &s[l], m-l);
		}else{
			memcpy(&s[0], &s[l], m-l);
		}
		// Add a null terminator.
		l = m-l;
		s[l+1] = '\0';
	}
	return l;
}

char *fileParseNextLine(FILE *file, char *buffer, size_t bufferSize, char **line, size_t *lineLength){

	char *r = fgets(buffer, bufferSize, file);

	if(r){

		char *commentPos;
		unsigned char doneFront = 0, doneEnd = 0;
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

	}

	return r;

}

/** TEMPORARY **/
void getDelimitedString(char *line, const size_t lineLength, const char *delims, char **strStart, size_t *strLength){
	/*
	** Temporary function by 8426THMY.
	*/
	char *tempStart = NULL;
	//Find the beginning of the string!
	while(tempStart == NULL && *delims != '\0'){
		tempStart = strchr(line, *delims);
		++delims;
	}
	--delims;

	const char *tempEnd = NULL;
	//If we could find a starting delimiter, try and find a closing one!
	if(tempStart != NULL){
		++tempStart;
		tempEnd = strchr(tempStart, *delims);

		//If we can't find a closing delimiter, just use everything up until the first one.
		if(tempEnd == NULL){
			tempEnd = tempStart - 1;
			tempStart = line;
		}

		//Get the string between our delimiters!
		*strStart = tempStart;
		*strLength = tempEnd - tempStart;

		//If we couldn't find any delimiters, use the whole string!
	}else{
		*strStart = line;
		*strLength = lineLength;
	}
}
