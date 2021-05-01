#include "console.h"
#include <string.h>

void conInit(console *const __RESTRICT__ con){
	con->buffer[0] = '\0';
	con->start = con->buffer;
	con->end = con->buffer;
}
/** This function is poorly written and doesn't work properly. **/
void conAppend(console *const __RESTRICT__ con, const byte_t *str, const size_t strLength){

	if(str != NULL && strLength > 0){

		const size_t maxLength = CONSOLE_BUFFER_SIZE-1;

		if(strLength >= maxLength){
			// Handle the easy case.
			memcpy(con->buffer, &str[strLength-maxLength], maxLength);
			con->start = con->buffer;
			con->end = &con->buffer[maxLength];
		}else{

			if(con->start == con->buffer){

				// Note that con->start < con->end if and
				// only if con->start == con->buffer.
				const size_t bufferSize = con->end - con->start;
				if(bufferSize + strLength > CONSOLE_BUFFER_SIZE){
					// Split the string into two copies.
					const size_t fill = CONSOLE_BUFFER_SIZE-bufferSize;
					const size_t offset = strLength-fill;
					memcpy(con->end, str, fill);
					memcpy(con->buffer, &str[fill], offset);
					con->start += offset;
					con->end -= CONSOLE_BUFFER_SIZE-offset;
				}else{
					// Just append it. Nice and easy.
					memcpy(con->end, str, strLength);
					con->end += strLength;
				}
				if(con->end >= &con->buffer[CONSOLE_BUFFER_SIZE]){
					// Make sure we wrap around correctly.
					con->end -= CONSOLE_BUFFER_SIZE;
					con->start = con->end+1;
				}

			}else{

				// We know bufferSize = CONSOLE_BUFFER_SIZE
				// and con->start > con->end.
				const size_t fill = &con->buffer[CONSOLE_BUFFER_SIZE] - con->end;
				if(strLength < fill){
					// No need to wrap con->end.
					// We may need to wrap con->start.
					memcpy(con->end, str, strLength);
					con->end += strLength;
					con->start += strLength;
					if(con->start == &con->buffer[CONSOLE_BUFFER_SIZE]){
						con->start -= CONSOLE_BUFFER_SIZE;
					}
				}else{
					// We need to wrap both con->start and con->end.
					memcpy(con->end, str, fill);
					memcpy(con->buffer, &str[fill], strLength-fill);
					con->start -= CONSOLE_BUFFER_SIZE-strLength;
					con->end -= CONSOLE_BUFFER_SIZE-strLength;
				}

			}

		}

		// Add a NULL terminator.
		*con->end = '\0';

	}

}