#include "textCMap.h"
#include "inline.h"

uint32_t txtCMapIndexFormat0(const txtCMap *const cmap, const txtCodeUnit_t code){
	// The code unit must fit into 8 bits.
	if(code._32 >= TEXT_CMAP_FORMAT_0_CODEUNIT_LIMIT){
		return TEXT_CMAP_MISSING_GLYPH_ID;
	}else{
		// See textCMap.h for an explanation.
		return ((txtCMap0 *)cmap)->glyphIdArray[code._8];
	}
}

uint32_t txtCMapIndexFormat2(const txtCMap *const cmap, const txtCodeUnit_t code){
	// The code unit must fit into 16 bits.
	if(code._32 >= TEXT_CMAP_FORMAT_2_CODEUNIT_LIMIT){
		return TEXT_CMAP_MISSING_GLYPH_ID;
	}else{
		// See textCMap.h for an explanation.
		const txtCMap2 cmap2 = *((txtCMap2 *)cmap);
		const txtCMap2SubHeader subHeader = cmap2.subHeaders[cmap2.subHeaderKeys[code.byte._1]];
		const uint16_t *const subarray = (uint16_t *)(((byte_t *)cmap2.glyphIndexArray) + subHeader.idRangeOffset);
		const uint8_t subarrayIndex = code.byte._2 - subHeader.firstCode;
		if(subarrayIndex >= subHeader.entryCount){
			return TEXT_CMAP_MISSING_GLYPH_ID;
		}else{
			uint16_t glyphIndex = subarray[subarrayIndex];
			if(glyphIndex != TEXT_CMAP_MISSING_GLYPH_ID){
				glyphIndex += subHeader.idDelta;
			}
			return glyphIndex;
		}
	}
}

uint32_t txtCMapIndexFormat4(const txtCMap *const cmap, const txtCodeUnit_t code){
	// The code unit must fit into 16 bits.
	if(code._32 >= TEXT_CMAP_FORMAT_4_CODEUNIT_LIMIT){
		return TEXT_CMAP_MISSING_GLYPH_ID;
	}else{
		// See textCMap.h for an explanation.
		const txtCMap4 cmap4 = *((txtCMap4 *)cmap);
		const uint16_t *start = cmap4.startCode;
		const uint16_t *end = cmap4.endCode;
		const int16_t *delta = cmap4.idDelta;
		const uint16_t *offset = cmap4.idRangeOffset;
		while(*end < code._16){
			 ++start; ++end; ++delta; ++offset;
		}
		if(code._16 <= *start){
			return TEXT_CMAP_MISSING_GLYPH_ID;
		}else{
			uint16_t glyphIndex = *(((*offset) >> 2) + (code._16 - *start) + offset);
			if(glyphIndex != TEXT_CMAP_MISSING_GLYPH_ID){
				glyphIndex += *delta;
			}
			return glyphIndex;
		}
	}
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