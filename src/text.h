#ifndef TEXT_H
#define TEXT_H

#include "textCMap.h"
#include "texture.h"
#include "rectangle.h"
#include "flags.h"

// Text strings can invoke the following commands, which
// will be processed during rendering / texture generation.
//
//     <sn> - Change the size of the text that follows,
//            where 'n' is the desired size.
//
//     <fn> - Change the font, where 'n' is the index in
//            the parent typeface or font family.
//
//     <bn> - If 'n' = 1, bold the following characters
//            in the GPU, otherwise do not bold them.
//
//     <in> - If 'n' = 1, italicize the following characters
//            in the GPU, otherwise do not italicize them.
//
// Note that the '<' character can be escaped with a '\',
// and that 'n' in each of these commands is encoded as a
// character.

// Maximum value for an unsigned 32-bit integer.
#define TEXT_UTF8_INVALID_CODEUNIT ((uint32_t)-1)

// Font types.
#define TEXT_FONT_TYPE_BMP 0x00
#define TEXT_FONT_TYPE_SDF 0x01

// Format flags.
#define TEXT_FORMAT_FONT_UPDATED  0x01
#define TEXT_FORMAT_SIZE_UPDATED  0x02
#define TEXT_FORMAT_STYLE_UPDATED 0x04
#define TEXT_FORMAT_UPDATED       0x07

typedef struct {
	float x;
	float y;
} txtCursor;

typedef struct {
	// The Unicode code unit for the glyph.
	txtCodeUnit_t code;
	// Area in the font atlas.
	rectangle frame;
	// Kerning for the glyph.  This may be
	// smaller or larger than the area in
	// the texture for tightly packed or
	// monospace fonts.
	float kerningX;
	float kerningY;
	// How much to advance the cursor by.
	float advanceX;
} txtGlyph;

typedef struct {
	const texture *atlas;
	// Array of glyphs.
	// The first glyph should be the missing / invalid glyph.
	const txtGlyph *glyphs;
	// Maps code units to glyphs.
	// Check the format to know what to cast it to.
	const txtCMap *cmap;
	// Line height. That is, how much to
	// advance the cursor by on each new line.
	// The maximum height value of each font
	// in a line is taken to be advanceY.
	float height;
	// BMP or SDF.
	flags_t type;
} txtFont;

typedef struct {
	txtFont *styles;
} txtTypeface;

typedef struct {
	size_t font;
	float size;
	flags_t style;
} txtFormat;

typedef struct {
	// Where to start reading text from.
	byte_t *offset;
	// Stream beginning and end.
	byte_t *front;
	byte_t *back;
	// Typeface information.
	const txtTypeface *typeface;
} txtStream;

return_t txtStreamNextCharacter(const txtStream *const restrict stream, const byte_t **i);
uint32_t txtStreamParseCharacter(const txtStream *const restrict stream, const byte_t **i, txtFormat *const restrict format);

return_t txtCursorAdvance(txtCursor *const restrict cursor, const float advanceX, const float advanceY, const float boxWidth);

#endif