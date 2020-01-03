#ifndef TEXT_H
#define TEXT_H

#include "memoryShared.h"
#include "texture.h"
#include "rectangle.h"
#include "flags.h"

#define TEXT_FONT_STYLES 3
#define TEXT_FONT_STYLE_NORMAL  0
#define TEXT_FONT_STYLE_ITALICS 1
#define TEXT_FONT_STYLE_BOLD    2

// Whether or not the first byte of a character
// is less than these values determines how many
// bytes long it is.
#define TEXT_CHARACTER_UTF8_1_BYTE_LIMIT 0x80
#define TEXT_CHARACTER_UTF8_2_BYTE_LIMIT 0xE0
#define TEXT_CHARACTER_UTF8_3_BYTE_LIMIT 0xF0
#define TEXT_CHARACTER_UTF8_4_BYTE_LIMIT 0xF8

// The n-1 bytes following the first byte of
// an n-byte character must be less than 0xC0.
#define TEXT_CHARACTER_UTF8_MULTIBYTE_LIMIT 0xC0

typedef uint32_t txtCode;

typedef struct {
	// The Unicode for the glyph.
	txtCode code;
	// Area in the texture map.
	rectangle bounds;
	// Physical dimensions of the glyph, in pixels.
	// This may be smaller or larger than the area
	// in the texture map for tightly packed or
	// monospace fonts.
	rectangle dimensions;
} txtGlyph;

/** Need to think of a new way of storing glyphs and fonts. **/
typedef struct {
	const texture *map;
	txtGlyph *glyphs;
	txtCode glyphNum;
	// The height, in pixels, of the font.
	float size;
	flags_t style;
} txtFont;

typedef struct {
	txtFont *styles;
	size_t styleNum;
} txtTypeface;

typedef struct {
	// Where to start reading text from.
	byte_t *offset;
	// Buffer beginning and end.
	byte_t *front;
	byte_t *back;
	// Typeface information.
	const txtTypeface *typeface;
} txtBuffer;

/** I don't really like these searches. Surely there's a better way? **/
const txtGlyph *txtFontFindGlyph(const txtFont *const restrict font, const txtCode c);
const txtFont *txtTypefaceFindFont(const txtTypeface *const restrict typeface, const flags_t style, const float size);

return_t txtBufferNextCharacter(const txtBuffer *const restrict buffer, const byte_t **i);
txtCode txtBufferParseCharacter(const txtBuffer *const restrict buffer, const byte_t **i, flags_t *const restrict style, float *const restrict size);

#endif