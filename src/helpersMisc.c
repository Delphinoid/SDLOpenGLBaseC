#include "helpersMisc.h"
#include "memoryManager.h"
#include <string.h>

/*return_t nextToken(const char *str, const char *delims, size_t *tokenOffset, size_t *tokenLength){
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

return_t pushDynamicArray(void **vector, const void *const restrict element, const size_t bytes, size_t *const restrict size, size_t *const restrict capacity){
	// Push an element into a dynamic array.
	if(*size == *capacity){
		void *tempBuffer;
		if(*capacity == 0){
			*capacity = 1;
		}else{
			*capacity *= 2;
		}
		tempBuffer = memReallocate(*vector, (*capacity)*bytes);
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		*vector = tempBuffer;
	}
	memcpy((byte_t *)*vector + (*size)*bytes, element, bytes);
	++(*size);
	return 1;
}

size_t ltostr(long n, char *const restrict s){
	// Converts a long to a C-string.
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

/** TEMPORARY **/
void getDelimitedString(char *const restrict line, const size_t lineLength, const char *restrict delims, char **const restrict strStart, size_t *const restrict strLength){
	// Temporary function by 8426THMY.
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
