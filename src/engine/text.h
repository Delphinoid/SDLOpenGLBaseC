#ifndef TEXT_H
#define TEXT_H

#include "textCMap.h"
#include "texture.h"
#include "rectangle.h"
#include "vec4.h"
#include "flags.h"

// Text strings can invoke the following commands, which
// will be processed during rendering / texture generation.
///
//     <sn> - Change the size of the text that follows,
//            where 'n' is the desired size.
///
//     <fn> - Change the font, where 'n' is the index in
//            the parent typeface or font family.
///
//     <bn> - If 'n' = 1, bold the following characters
//            in the GPU, otherwise do not bold them.
///
//     <in> - If 'n' = 1, italicize the following characters
//            in the GPU, otherwise do not italicize them.
///
// Note that the '<' character can be escaped with a '\',
// and that 'n' in each of these commands is encoded as a
// character.

// Maximum value for an unsigned 32-bit integer.
#define TEXT_UTF8_INVALID_CODEUNIT ((uint32_t)-1)

// Font types.
#define TEXT_FONT_TYPE_BMP  0x00
#define TEXT_FONT_TYPE_SDF  0x01
#define TEXT_FONT_TYPE_MSDF 0x02

// Format flags.
#define TEXT_FORMAT_FONT_UPDATED  0x01
#define TEXT_FORMAT_SIZE_UPDATED  0x02
#define TEXT_FORMAT_STYLE_UPDATED 0x04
#define TEXT_FORMAT_UPDATED       0x07

// Wraps words to the next line. Width and length
// properties can be used for adjusting the kerning.
// Uses oidashi kinsoku shori for Japanese text.
#define TEXT_LINE_AUTOMATIC_LINE_BREAKS 0x01

typedef struct {
	float x;
	float y;
} txtCursor;

typedef struct {
	// Pointer to the atlas containing the glyph.
	texture *atlas;
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
	// Array of glyphs.
	// The first glyph should be the missing / invalid glyph.
	txtGlyph *glyphs;
	// Maps code units to glyphs.
	// Check the format to know what to cast it to.
	txtCMap *cmap;
	// Total number of glyphs.
	size_t glyphNum;
	// Line height. That is, how much to
	// advance the cursor by on each new line.
	// The maximum height value of each font
	// in a line is taken to be advanceY.
	float height;
	// Name of the font.
	char *name;
	// BMP or SDF.
	flags_t type;
} txtFont;

typedef struct {
	const txtFont *font;
	float size;
	vec4 colour;
	vec4 background;
	flags_t style;
} txtFormat;

typedef struct {
	// Where to start reading text from.
	byte_t *offset;
	// Stream beginning and end.
	byte_t *front;
	byte_t *back;
} txtStream;

/**typedef struct {
	const txtStream *stream;
	const byte_t *start;
	const byte_t *end;
	unsigned int length;
	float width;
} txtLine;**/

txtGlyph *txtGlyphArrayLoad(const char *const __RESTRICT__ glyphPath, const size_t glyphPathLength, texture *const *const atlas, const size_t atlasSize);
return_t txtFontLoad(txtFont *const __RESTRICT__ font, const flags_t type, const char *const __RESTRICT__ name, const size_t nameLength, const char *const __RESTRICT__ texPath0, const size_t texPathLength0, const char *const __RESTRICT__ texPath1, const size_t texPathLength1, const char *const __RESTRICT__ glyphPath, const size_t glyphPathLength, const char *const __RESTRICT__ cmapPath, const size_t cmapPathLength);
void txtFontDelete(txtFont *const __RESTRICT__ font);

return_t txtStreamNextCharacter(const txtStream *const __RESTRICT__ stream, const byte_t **i);
uint32_t txtStreamParseCharacter(const txtStream *const __RESTRICT__ stream, const byte_t **i);

///void txtLinePrepare(txtLine *const __RESTRICT__ line, const txtStream *const stream, const txtFont *const font);
///void txtLineParseNext(txtLine *const __RESTRICT__ line, const txtFormat format, const float maxWidth, const flags_t style);

#endif
