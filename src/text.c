#include "text.h"
#include "memoryManager.h"
#include "moduleTexture.h"
#include "helpersFileIO.h"
/** TEMPORARY **/
#include "helpersMisc.h"
/** TEMPORARY **/

#define TEXT_RESOURCE_DIRECTORY_STRING FILE_PATH_RESOURCE_DIRECTORY_SHARED"Resources"FILE_PATH_DELIMITER_STRING"Fonts"FILE_PATH_DELIMITER_STRING
#define TEXT_RESOURCE_DIRECTORY_LENGTH 18

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

return_t txtStreamNextCharacter(const txtStream *const __RESTRICT__ stream, const byte_t **i){
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

uint32_t txtStreamParseCharacter(const txtStream *const __RESTRICT__ stream, const byte_t **i, txtFormat *const __RESTRICT__ format){

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

return_t txtFontLoad(
	txtFont *const __RESTRICT__ font, const flags_t type,
	const char *const __RESTRICT__ name, const size_t nameLength,
	const char *const __RESTRICT__ texPath0, const size_t texPathLength0,
	const char *const __RESTRICT__ texPath1, const size_t texPathLength1,
	const char *const __RESTRICT__ glyphPath, const size_t glyphPathLength,
	const char *const __RESTRICT__ cmapPath, const size_t cmapPathLength
){

	// Load the font's graphics.
	texture **const t = memAllocate(sizeof(texture*)<<1);
	t[0] = moduleTextureAllocate(); t[1] = moduleTextureAllocate();
	tLoad(t[0], texPath0, texPathLength0);
	tLoad(t[1], texPath1, texPathLength1);
	// Load the font's glyph offset information.
	font->glyphs = txtGlyphArrayLoad(glyphPath, glyphPathLength, t, 1);
	// Allocate memory for the font's character map and load it.
	font->cmap = txtCMapLoad(cmapPath, cmapPathLength);
	memFree(t);

	font->type = type;
	font->height = 48.f;
	font->name = memAllocate(nameLength*sizeof(char));
	memcpy(font->name, name, nameLength);
	font->name[nameLength] = '\0';

	return 1;

}

txtGlyph *txtGlyphArrayLoad(const char *const __RESTRICT__ glyphPath, const size_t glyphPathLength, texture *const *const atlas, const size_t atlasSize){
	// Temporary function by 8426THMY.
	txtGlyph *glyphs = NULL;

	// Load the glyph offsets!
	FILE *glyphFile = fopen(glyphPath, "r");
	if(glyphFile != NULL){
		const float invAtlasSize[2] = {1.f/atlas[0]->width, 1.f/atlas[0]->height};

		char *endPos = NULL;

		size_t lastIndex = (size_t)-1;

		char lineBuffer[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;

		while(fileParseNextLine(glyphFile, lineBuffer, sizeof(lineBuffer), &line, &lineLength)){
			// If we haven't read the character count, try and read it!
			if(lastIndex == (size_t)-1){
				lastIndex = strtoul(line, &endPos, 10);
				// If we didn't read a number, return numGlyphs to the invalid value.
				if(endPos == line){
					lastIndex = (size_t)-1;

				// Otherwise, allocate memory for our glyphs!
				}else{
					glyphs = memAllocate(sizeof(*glyphs) * (lastIndex + 1));
				}
			}else if(line[0] != '\0'){
				size_t tokenLength;
				char *token = line;

				// Load the glyph data from the file!
				const size_t char_id = (token = stringDelimited(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtoul(token, NULL, 10));
				const size_t char_index = (token += tokenLength + 1, token = stringDelimited(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtoul(token, NULL, 10));
				const uint32_t char_char = (token += tokenLength + 1, token = stringDelimited(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', 1234);
				const float char_width = (token += tokenLength + 1, token = stringDelimited(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtof(token, NULL));
				const float char_height = (token += tokenLength + 1, token = stringDelimited(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtof(token, NULL));
				const float char_xoffset = (token += tokenLength + 1, token = stringDelimited(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtof(token, NULL));
				const float char_yoffset = (token += tokenLength + 1, token = stringDelimited(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtof(token, NULL));
				const float char_xadvance = (token += tokenLength + 1, token = stringDelimited(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtof(token, NULL));
				const byte_t char_chnl = (token += tokenLength + 1, token = stringDelimited(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtoul(token, NULL, 10));
				const float char_x = (token += tokenLength + 1, token = stringDelimited(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtof(token, NULL));
				const float char_y = (token += tokenLength + 1, token = stringDelimited(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtof(token, NULL));
				const byte_t char_page = (token += tokenLength + 1, token = stringDelimited(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtoul(token, NULL, 10));
				const uint32_t info_charset = (token += tokenLength + 1, token = stringDelimited(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', 1234);

				const txtGlyph newGlyph = {
					.atlas = atlas[char_page],

					.frame.x = char_x * invAtlasSize[0],
					.frame.y = char_y * invAtlasSize[1],
					.frame.w = char_width * invAtlasSize[0],
					.frame.h = char_height * invAtlasSize[1],

					.kerningX = char_xoffset,
					.kerningY = char_yoffset,
					.advanceX = char_xadvance
				};
				// Add the new glyph to its correct position in our array of glyphs!
				glyphs[char_index] = newGlyph;
			}
		}

		fclose(glyphFile);
	}else{
		printf(
			"Unable to open glyph offsets file!\n"
			"Path: %s\n",
			glyphPath
		);
	}


	return(glyphs);
	#if 0
	textGlyph *glyphs = NULL;

	// Load the glyph offsets!
	FILE *glyphFile = fopen(glyphPath, "r");
	if(glyphFile != NULL){
		char *endPos = NULL;

		size_t numGlyphs = invalidValue(numGlyphs);

		char lineBuffer[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;

		while((line = fileReadLine(glyphFile, &lineBuffer[0], &lineLength)) != NULL){
			// If we haven't read the character count, try and read it!
			if(valueIsInvalid(numGlyphs)){
				numGlyphs = strtoul(line, &endPos, 10);
				// If we didn't read a number, return numGlyphs to the invalid value.
				if(endPos == line){
					numGlyphs = invalidValue(numGlyphs);
				}
			}else{
				const size_t curID = strtoul(line, &endPos, 10);
				// If we could load the character identifier,
				// we can try and load the rest of the glyph.
				// Otherwise, the line was probably empty.
				if(endPos != line){
					const textGlyph curGlyph = {
						.uvOffsets = {
							.x = strtof(endPos + 1, &endPos),
							.y = strtof(endPos + 1, &endPos),
							.w = strtof(endPos + 1, &endPos),
							.h = strtof(endPos + 1, &endPos)
						},

						.kerningX = strtof(endPos + 1, &endPos),
						.kerningY = strtof(endPos + 1, &endPos),
						.advanceX = strtof(endPos + 1, NULL)
					};
					printf(
						PRINTF_SIZE_T": (%f, %f, %f, %f), (%f, %f), %f\n",
						curID,
						curGlyph.uvOffsets.x, curGlyph.uvOffsets.y,
						curGlyph.uvOffsets.w, curGlyph.uvOffsets.h,
						curGlyph.kerningX, curGlyph.kerningX,
						curGlyph.advanceX
					);
				}
			}
		}
	}else{
		printf(
			"Unable to open glyph offsets file!\n"
			"Path: %s\n",
			glyphPath
		);
	}


	return(glyphs);
	#endif
}

void txtFontDelete(txtFont *const __RESTRICT__ font){
	txtGlyph *g = font->glyphs;
	const txtGlyph *const gLast = &g[font->glyphNum];
	while(g < gLast){
		if(memPoolBlockStatus(g) == MEMORY_POOL_BLOCK_ACTIVE){
			moduleTextureFree(g->atlas);
		}
	}
	/** Change these when moduleText is introduced. **/
	memFree(font->name);
	memFree(font->cmap);
	memFree(font->glyphs);
}