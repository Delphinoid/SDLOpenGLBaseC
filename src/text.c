#include "text.h"

// Whether or not the first byte of a character
// is less than these values determines how many
// bytes long it is.
#define TEXT_CHARACTER_UTF8_1_BYTE_LIMIT 0x80
#define TEXT_CHARACTER_UTF8_2_BYTE_LIMIT 0xE0
#define TEXT_CHARACTER_UTF8_3_BYTE_LIMIT 0xF0
#define TEXT_CHARACTER_UTF8_4_BYTE_LIMIT 0xF8

// The n-1 bytes following the first byte of
// an n-byte character must be less than 0xC0.
#define TEXT_CHARACTER_UTF8_MULTIBYTE_MASK  0xC0
#define TEXT_CHARACTER_UTF8_MULTIBYTE_VALUE 0x80

// Composite bytes must end with 1 as the most significant bit, preceded by a 0.
#define txtBufferInvalidByte(c) (c & TEXT_CHARACTER_UTF8_MULTIBYTE_MASK) != TEXT_CHARACTER_UTF8_MULTIBYTE_VALUE

return_t txtBufferNextCharacter(const txtBuffer *const restrict buffer, const byte_t **i){
	if(*i >= buffer->back){
		*i = buffer->front;
	}else{
		++(*i);
	}
	if(*i == buffer->offset){
		return 0;
	}
	return 1;
}

uint32_t txtBufferParseCharacter(const txtBuffer *const restrict buffer, const byte_t **i, flags_t *const restrict style, float *const restrict size){

	// Parse the first byte.
	txtCodeUnit_t code = {.byte = {._1 = **i, ._2 = 0, ._3 = 0, ._4 = 0}};

	if(code.byte._1 >= TEXT_CHARACTER_UTF8_1_BYTE_LIMIT){

		// Parse the second byte.
		byte_t nextByte;
		if(
			txtBufferNextCharacter(buffer, i) == 0 ||
			txtBufferInvalidByte((nextByte = **i))
		){
			// Invalid character code.
			return TEXT_CMAP_INVALID_CODEUNIT_ID;
		}
		// Append the character to the full width character code.
		code.byte._2 = nextByte;

		if(code.byte._1 >= TEXT_CHARACTER_UTF8_2_BYTE_LIMIT){

			// Parse the third byte.
			if(
				txtBufferNextCharacter(buffer, i) == 0 ||
				txtBufferInvalidByte((nextByte = **i))
			){
				// Invalid character code.
				return TEXT_CMAP_INVALID_CODEUNIT_ID;
			}
			// Append the character to the full width character code.
			code.byte._3 = nextByte;

			if(code.byte._1 >= TEXT_CHARACTER_UTF8_3_BYTE_LIMIT){

				// Parse the fourth byte.
				if(
					code.byte._1 >= TEXT_CHARACTER_UTF8_4_BYTE_LIMIT ||
					txtBufferNextCharacter(buffer, i) == 0 ||
					txtBufferInvalidByte((nextByte = **i))
				){
					// Invalid character code.
					return TEXT_CMAP_INVALID_CODEUNIT_ID;
				}
				// Append the character to the full width character code.
				code.byte._4 = nextByte;

			}

		}

	}

	// Return the full width character code.
	return code._32;

}

void txtGlyphRender(const txtGlyph *const restrict glyph, flags_t *const restrict style, float *const restrict size){

	//

}