#ifndef TEXTCMAP_H
#define TEXTCMAP_H

#include "memoryShared.h"
#include <stdint.h>

// Standard missing glyph ID.
#define TEXT_CMAP_MISSING_GLYPH_ID 0

// Currently only formats 0, 2 and 4 are supported,
// as we only really use the basic multilingual plane.
#define TEXT_CMAP_FORMAT_NUM 3

// Format code unit limits.
// Format 0 deals with 8-bit code units, formats
// 2, 4 and 6 deal with 16-bit code units and most
// other formats support full 32-bit code units.
#define TEXT_CMAP_FORMAT_0_CODEUNIT_LIMIT 256
#define TEXT_CMAP_FORMAT_2_CODEUNIT_LIMIT 65536
#define TEXT_CMAP_FORMAT_4_CODEUNIT_LIMIT 65536

// 32-bit code unit structure.
// This represents an encoded code point.
// The byte struct allows convenient access to
// each of the four bytes of the code unit.
typedef union {
	uint8_t _8;
	uint16_t _16;
	uint32_t _32;
	struct {
		byte_t _1;
		byte_t _2;
		byte_t _3;
		byte_t _4;
	} byte;
} txtCodeUnit_t;

typedef struct {
	// Header information.
	// The format is 16-bit in the standard, but
	// only needs to be 8-bit for our purposes.
	// We also use a linear map for formats rather
	// than powers of 2 for the jump table.
	uint16_t format;
	uint16_t length;
	uint16_t language;
} txtCMap;

typedef struct {

	// This is effectively an implementation of
	// the standard OpenType format 0 subtable.
	// Unnecessary header data has been removed.

	// Header information.
	// The format is 16-bit in the standard, but
	// only needs to be 8-bit for our purposes.
	// We also use a linear map for formats rather
	// than powers of 2 for the jump table.
	uint16_t format;
	uint16_t length;
	uint16_t language;

	// Simple 1:1 mapping from code units to glyph indices.
	uint8_t glyphIdArray[TEXT_CMAP_FORMAT_0_CODEUNIT_LIMIT];

} txtCMap0;

typedef struct {

	// SubHeaders map the second byte of a 16-bit
	// code unit to a glyph under cmap format 2.
	//
	// Each SubHeader defines a subrange: a sequence
	// of adjacent code units all beginning with the
	// same byte. As such, subranges stay within the
	// [0, 255] range of the first (low) byte.
	//
	// Single byte code units will always be mapped to
	// SubHeader 0.
	//
	// The offset of the desired code unit in the
	// subrange is used as the index of a subarray in
	// txtCMap2's glyphIndexArray, which is the same
	// length as the SubHeader's subrange (entryCount).
	//
	// See txtCMap2's comment for an explanation in
	// code on how the system works.

	// The first code unit in the subrange.
	// This is 16-bit in the standard, but only needs to
	// be 8-bit and store the second byte of the code unit.
	// The first byte is used to index the SubHeader.
	uint16_t firstCode;
	// The number of code units mapped in the subrange.
	// In other words, the length of the subrange.
	// This is 16-bit in the standard, but only needs to
	// be 8-bit, as the maximum length is 255.
	uint16_t entryCount;
	// If the value obtained from the subarray does
	// not indicate a missing glyph, idDelta is added.
	// This permits multiple subarrays to use the same
	// SubHeader. idDelta is modulo 65536.
	int16_t idDelta;
	// The number of bytes that the element of txtCMap2's
	// glyphIndexArray corresponding to firstCode is offset
	// from &idRangeOffset. So if firstCode corresponds to
	// element n, we have
	// idRangeOffset = &glyphIndexArray[n] - &idRangeOffset.
	uint16_t idRangeOffset;

} txtCMap2SubHeader;

typedef struct {

	// This is effectively an implementation of
	// the standard OpenType format 2 subtable.
	// Unnecessary header data has been removed.
	//
	// Suppose we have a code unit, with high and
	// low bytes code.high and code.low respectively,
	// that we wish to map to a particular glyph. We
	// may do this using the following code:
	//
	// txtGlyph glyph;
	// txtCMap2SubHeader subHeader = subHeaders[subHeaderKeys[code.high]];
	// uint16_t *subarray = (uint16_t *)(((byte_t *)glyphIndexArray) + subHeader.idRangeOffset);
	// uint8_t subarrayIndex = code.low - subHeader.firstCode;
	// if(subarrayIndex >= subHeader.entryCount){
	//     glyph = glyphs[TEXT_CMAP_MISSING_GLYPH_ID];
	// }else{
	//     uint16_t glyphIndex = subarray[subarrayIndex];
	//     if(glyphIndex != TEXT_CMAP_MISSING_GLYPH_ID){
	//         glyphIndex += subHeader.idDelta;
	//     }
	//     glyph = glyphs[glyphIndex];
	// }

	// Header information.
	// The format is 16-bit in the standard, but
	// only needs to be 8-bit for our purposes.
	// We also use a linear map for formats rather
	// than powers of 2 for the jump table.
	uint16_t format;
	uint16_t length;
	uint16_t language;

	// Maps the first byte of a code unit to a SubHeader.
	// Contains a byte offset of the corresponding
	// SubHeader in subHeaders. Element 0 will always be 0.
	uint16_t subHeaderKeys[256];

	// SubHeaders map the second byte to a glyph.
	// A second byte is not needed for SubHeader 0.
	//txtCMap2SubHeader *subHeaders;
	// This array sits in the area of memory directly
	// following that used by all of the SubHeaders.
	//uint16_t *glyphIndexArray;

	// First element of subHeaders. The other elements and
	// glyphIndexArray are stored directly after this in
	// memory.
	uint16_t data;

} txtCMap2;

typedef struct {

	// This is effectively an implementation of
	// the standard OpenType format 4 subtable.
	// Unnecessary header data has been removed.
	//
	// Format 4 works somewhat similarly to format 2.
	// Rather than using SubHeaders, however, we have
	// segments. These segments are sorted in order
	// of increasing endCode.
	//
	// Suppose we have a code unit that we wish to
	// map to a particular glyph. We may do this using
	// the following code:
	//
	// txtGlyph glyph;
	// uint16_t *start = startCode;
	// uint16_t *end = endCode;
	// uint16_t *delta = idDelta;
	// uint16_t *rangeOffset = idRangeOffset;
	// while(*end < c){
	//      ++start; ++end; ++delta; ++rangeOffset;
	// }
	// if(c <= *start){
	//     glyph = glyphs[TEXT_CMAP_MISSING_GLYPH_ID];
	// }else{
	//     uint16_t glyphIndex = *((*rangeOffset >> 2) + (code - *start) + rangeOffset);
	//     if(glyphIndex != TEXT_CMAP_MISSING_GLYPH_ID){
	//         glyphIndex += *delta;
	//     }
	//     glyph = glyphs[glyphIndex];
	// }

	// Header information.
	// The format is 16-bit in the standard, but
	// only needs to be 8-bit for our purposes.
	// We also use a linear map for formats rather
	// than powers of 2 for the jump table.
	uint16_t format;
	uint16_t length;
	uint16_t language;

	// Allows convenient indexing of segment arrays.
	uint16_t segCountX2;
	// These are totally useless and ignored.
	uint16_t searchRange;
	uint16_t entrySelector;
	uint16_t rangeShift;

	// Array of start code units for each segment.
	//uint16_t *startCode;
	// Array of end code units for each segment.
	// Because of how segments are sorted, these will
	// be in increasing order.
	// This array also has an extra element at the end
	// containing 0xFFFF, for loop termination.
	//uint16_t *endCode;
	// Array of deltas for each segment. These serve
	// the same purpose as the SubHeader deltas.
	//int16_t *idDelta;
	// Array of range offsets for each segment. Again,
	// these serve a similar purpose to the SubHeader
	// range offsets.
	//uint16_t *idRangeOffset;
	// This array sits in the area of memory directly
	// following that used by the idRangeOffset array.
	//uint16_t *glyphIdArray;

	// First element of startCode. The other elements and
	// all of the other arrays are stored directly after
	// this in memory.
	uint16_t data;

} txtCMap4;

// Forward declaration for inlining.
extern uint32_t (* const txtCMapIndexJumpTable[TEXT_CMAP_FORMAT_NUM])(
	const txtCMap *const cmap,
	const txtCodeUnit_t code
);

uint32_t txtCMapIndex(const txtCMap *const cmap, const txtCodeUnit_t code);

#endif
