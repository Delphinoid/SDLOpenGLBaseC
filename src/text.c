#include "text.h"

// Whether or not the first byte of a character
// is less than these values determines how many
// bytes long it is.
#define TEXT_UTF8_CHARACTER_1_BYTE_LIMIT 0x80
#define TEXT_UTF8_CHARACTER_2_BYTE_LIMIT 0xE0
#define TEXT_UTF8_CHARACTER_3_BYTE_LIMIT 0xF0
#define TEXT_UTF8_CHARACTER_4_BYTE_LIMIT 0xF8

// The n-1 bytes following the first byte of
// an n-byte character must be less than 0xC0.
#define TEXT_UTF8_CHARACTER_MULTIBYTE_MASK  0xC0
#define TEXT_UTF8_CHARACTER_MULTIBYTE_VALUE 0x80

// Composite bytes must end with 1 as the most significant bit, preceded by a 0.
#define txtInvalidByteUTF8(c) ((c & TEXT_UTF8_CHARACTER_MULTIBYTE_MASK) != TEXT_UTF8_CHARACTER_MULTIBYTE_VALUE)

return_t txtStreamNextCharacter(const txtStream *const restrict stream, const byte_t **i){
	// Returns 0 at the end of the stream.
	if(*i >= stream->back){
		*i = stream->front;
	}else{
		++(*i);
	}
	if(*i == stream->offset){
		return 0;
	}
	return 1;
}

uint32_t txtStreamParseCharacter(const txtStream *const restrict stream, const byte_t **i, txtFormat *const restrict format){

	// Parse the first byte.
	txtCodeUnit_t code = {.byte = {._1 = **i, ._2 = 0, ._3 = 0, ._4 = 0}};

	/// Check for formatting commands.

	if(code.byte._1 >= TEXT_UTF8_CHARACTER_1_BYTE_LIMIT){

		// Parse the second byte.
		byte_t nextByte;
		if(txtStreamNextCharacter(stream, i) == 0){
			// End of stream.
			return 0;
		}else if(txtInvalidByteUTF8((nextByte = **i))){
			// Invalid character code.
			return TEXT_UTF8_INVALID_CODEUNIT;
		}
		// Append the character to the full width character code.
		code.byte._2 = nextByte;

		if(code.byte._1 >= TEXT_UTF8_CHARACTER_2_BYTE_LIMIT){

			// Parse the third byte.
			if(txtStreamNextCharacter(stream, i) == 0){
				// End of stream.
				return 0;
			}else if(txtInvalidByteUTF8((nextByte = **i))){
				// Invalid character code.
				return TEXT_UTF8_INVALID_CODEUNIT;
			}
			// Append the character to the full width character code.
			code.byte._3 = nextByte;

			if(code.byte._1 >= TEXT_UTF8_CHARACTER_3_BYTE_LIMIT){

				// Parse the fourth byte.
				if(txtStreamNextCharacter(stream, i) == 0){
					// End of stream.
					return 0;
				}else if(code.byte._1 >= TEXT_UTF8_CHARACTER_4_BYTE_LIMIT || txtInvalidByteUTF8((nextByte = **i))){
					// Invalid character code.
					return TEXT_UTF8_INVALID_CODEUNIT;
				}
				// Append the character to the full width character code.
				code.byte._4 = nextByte;

			}

		}

	}

	// Return the full width character code.
	return code._32;

}

return_t txtCursorAdvance(txtCursor *const restrict cursor, const float advanceX, const float advanceY, const float boxWidth){
	// When using SDF fonts, make sure advanceX and advanceY
	// are correctly scaled by the size of the font.
	// Return value 1 indicates that the next glyph will be
	// on a new line and to adjust advanceY.
	if(advanceX > 0){
		cursor->x += advanceX;
		if(cursor->x > boxWidth){
			cursor->x = 0;
			cursor->y += advanceY;
			return 1;
		}
	}
	return 0;
}