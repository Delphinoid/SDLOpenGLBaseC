#include "textCMap.h"

// Take the remainder of x modulo 65,536.
#define txtCMapMod16(x) ((x) & 0x0000FFFF)

// Find a particular subheader using the high byte of a code unit.
#define txtCMapFormat2GetSubHeader(cmap, high) *((const txtCMap2SubHeader *)((const byte_t *)(&((const txtCMap2 *)(cmap))->data) + ((const txtCMap2 *)(cmap))->subHeaderKeys[high]));
// Find the glyph index for a format 2 subtable.
#define txtCMapFormat2GetGlyphIndex(subHeader, subArrayIndex) ((const uint16_t *)((const byte_t *)((const byte_t *)(&(subHeader) + 1)) + (subHeader).idRangeOffset))[subArrayIndex];

// Get an element in the character map's end code array.
#define txtCMapFormat4GetEndCodes(cmap) ((const uint16_t *)(&((const txtCMap4 *)(cmap))->data))
#define txtCMapFormat4GetEndCode(cmap, offset) (const uint16_t *)(((const byte_t *)txtCMapFormat4GetEndCodes(cmap)) + offset)
// Get an element in the character map's start code array.
#define txtCMapFormat4GetStartCodes(cmap) ((const uint16_t *)((const byte_t *)&((const txtCMap4 *)(cmap))->data + ((const txtCMap4 *)(cmap))->segCountX2 + sizeof(uint16_t)))
#define txtCMapFormat4GetStartCode(cmap, offset) (const uint16_t *)(((const byte_t *)txtCMapFormat4GetStartCodes(cmap)) + offset)
// Get an element in the character map's delta array.
#define txtCMapFormat4GetDeltas(cmap) ((const int16_t *)((const byte_t *)&((const txtCMap4 *)(cmap))->data + ((const txtCMap4 *)(cmap))->segCountX2 + ((const txtCMap4 *)(cmap))->segCountX2 + sizeof(uint16_t)))
#define txtCMapFormat4GetDelta(cmap, offset) (const int16_t *)(((const byte_t *)txtCMapFormat4GetDeltas(cmap)) + offset)
// Get an element in the character map's offset array.
#define txtCMapFormat4GetRangeOffsets(cmap) ((const uint16_t *)((const byte_t *)&((const txtCMap4 *)(cmap))->data + ((const txtCMap4 *)(cmap))->segCountX2 + ((const txtCMap4 *)(cmap))->segCountX2 + ((const txtCMap4 *)(cmap))->segCountX2 + sizeof(uint16_t)))
#define txtCMapFormat4GetRangeOffset(cmap, offset) (const uint16_t *)(((const byte_t *)txtCMapFormat4GetRangeOffsets(cmap)) + offset)
// Find the glyph index for a format 4 subtable.
#define txtCMapFormat4GetGlyphIndex(start, offset, code) *(((*(offset)) >> 1) + ((code) - (start)) + (offset))

uint32_t txtCMapIndexFormat0(const txtCMap *const cmap, const txtCodeUnit_t code){
	// The code unit must fit into 8 bits.
	if(code._32 >= TEXT_CMAP_FORMAT_0_CODEUNIT_LIMIT){
		return TEXT_CMAP_MISSING_GLYPH_ID;
	}
	// See textCMap.h for an explanation.
	return (uint32_t)((txtCMap0 *)cmap)->glyphIdArray[code._8];
}

uint32_t txtCMapIndexFormat2(const txtCMap *const cmap, const txtCodeUnit_t code){
	// The code unit must fit into 16 bits.
	if(code._32 < TEXT_CMAP_FORMAT_2_CODEUNIT_LIMIT){
		// See textCMap.h for an explanation.
		const txtCMap2SubHeader subHeader = txtCMapFormat2GetSubHeader(cmap, code.byte._2);
		const uint16_t subArrayIndex = code._16 - subHeader.firstCode;
		if(subArrayIndex < subHeader.entryCount){
			const uint16_t glyphIndex = txtCMapFormat2GetGlyphIndex(subHeader, subArrayIndex);
			if(glyphIndex != TEXT_CMAP_MISSING_GLYPH_ID){
				return txtCMapMod16(glyphIndex + subHeader.idDelta);
			}
		}
	}
	return TEXT_CMAP_MISSING_GLYPH_ID;
}

uint32_t txtCMapIndexFormat4(const txtCMap *const cmap, const txtCodeUnit_t code){
	// The code unit must fit into 16 bits.
	if(code._32 < TEXT_CMAP_FORMAT_4_CODEUNIT_LIMIT){
		// See textCMap.h for an explanation.
		// This uses some weird pointer arithmetic tricks to reduce
		// the number of increments each iteration in the while loop.
		const uint16_t *end = txtCMapFormat4GetEndCodes(cmap);
		uint16_t start;
		uintptr_t offset;
		for(offset = 0; *end < code._16; ++end, offset += sizeof(uint16_t));
		start = *txtCMapFormat4GetStartCode(cmap, offset);
		if(code._16 > start){
			const uint16_t *const rangeOffset = txtCMapFormat4GetRangeOffset(cmap, offset);
			const int16_t idDelta = *txtCMapFormat4GetDelta(cmap, offset);
			if(*rangeOffset == 0){
				return txtCMapMod16(code._16 + idDelta);
			}else{
				const uint16_t glyphIndex = txtCMapFormat4GetGlyphIndex(start, rangeOffset, code._16);
				if(glyphIndex != TEXT_CMAP_MISSING_GLYPH_ID){
					return txtCMapMod16(glyphIndex + idDelta);
				}
			}
		}
	}
	return TEXT_CMAP_MISSING_GLYPH_ID;
}

uint32_t (* const txtCMapIndexJumpTable[TEXT_CMAP_FORMAT_NUM])(
	const txtCMap *const cmap,
	const txtCodeUnit_t code
) = {
	txtCMapIndexFormat0,
	txtCMapIndexFormat2,
	txtCMapIndexFormat4
};
__FORCE_INLINE__ uint32_t txtCMapIndex(const txtCMap *const cmap, const txtCodeUnit_t code){
	return txtCMapIndexJumpTable[cmap->format](cmap, code);
}