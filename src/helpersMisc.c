#include <stdlib.h>
#include <string.h>

/* Replacement for strtok() because it's a horrible function. */
/*unsigned char nextToken(const char *str, const char *delims, size_t *tokenOffset, size_t *tokenLength){
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


/* Push an element into a dynamic array. */
unsigned char pushDynamicArray(void **vector, const void *element, const size_t bytes, size_t *size, size_t *capacity){
	if(*size == *capacity){
		*capacity *= 2;
		void *tempBuffer = realloc(*vector, (*capacity)*bytes);
		if(tempBuffer != NULL){
			*vector = tempBuffer;
		}else{
			return 0;
		}
	}
	memcpy((*vector)+(*size)*bytes, element, bytes);
	++(*size);
	return 1;
}

/* Converts a long to a C-string. */
size_t ltostr(long n, char **s){
	size_t l;  // Length of the ouput (discluding null terminator).
	if(n == 0){
		*s = malloc(2*sizeof(char));
		**s = '0';
		*(*s+1) = '\0';
		l = 1;
	}else{
		size_t m = 12;  // Max number length (including minus sign and null terminator).
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
}
