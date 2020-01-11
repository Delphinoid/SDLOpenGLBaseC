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
		const txtCMap2 *const cmap2 = (txtCMap2 *)cmap;
		const txtCMap2SubHeader subHeader = *((txtCMap2SubHeader *)(((byte_t *)cmap2->subHeaders) + cmap2->subHeaderKeys[code.byte._2]));
		const uint16_t *const subarray = (uint16_t *)(((byte_t *)cmap2->glyphIndexArray) + subHeader.idRangeOffset);
		const uint16_t subarrayIndex = code._16 - subHeader.firstCode;  //const uint8_t subarrayIndex = code.byte._1 - subHeader.firstCode;
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
		// This uses some weird pointer arithmetic tricks to reduce
		// the number of increments each iteration in the while loop.
		const txtCMap4 *const cmap4 = (txtCMap4 *)cmap;
		uintptr_t offset = 0;
		const uint16_t *end = cmap4->endCode;
		while(*end < code._16){
			 offset += sizeof(uint16_t);
			 end = (uint16_t *)(((byte_t *)end) + offset);
		}
		{
			const uint16_t start = *((uint16_t *)(((byte_t *)cmap4->startCode) + offset));
			if(code._16 <= start){
				return TEXT_CMAP_MISSING_GLYPH_ID;
			}else{
				const uint16_t *const rangeOffset = (uint16_t *)(((byte_t *)cmap4->idRangeOffset) + offset);
				uint16_t glyphIndex = *(((*rangeOffset) >> 2) + (code._16 - start) + rangeOffset);
				if(glyphIndex != TEXT_CMAP_MISSING_GLYPH_ID){
					glyphIndex += *((uint16_t *)(((byte_t *)cmap4->idDelta) + offset));
				}
				return glyphIndex;
			}
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