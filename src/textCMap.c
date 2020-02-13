#include "textCMap.h"
#include "memoryManager.h"
#include <string.h>
#include <stdio.h>

#define TEXT_CMAP_PLATFORM_UNICODE 0
#define TEXT_CMAP_PLATFORM_MAC     1
#define TEXT_CMAP_PLATFORM_ISO     2
#define TEXT_CMAP_PLATFORM_WINDOWS 3
#define TEXT_CMAP_PLATFORM_CUSTOM  4

#define TEXT_CMAP_ENCODING_1_0      0
#define TEXT_CMAP_ENCODING_1_1      1
#define TEXT_CMAP_ENCODING_ISO      2
#define TEXT_CMAP_ENCODING_2_0_BMP  3
#define TEXT_CMAP_ENCODING_2_0_FULL 4
#define TEXT_CMAP_ENCODING_VAR      5
#define TEXT_CMAP_ENCODING_FULL     6

#define TEXT_CMAP_HEAD_CHECKSUM_ADJUSTMENT_OFFSET (2*sizeof(uint16_t) + sizeof(uint32_t))

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
	txtCMapIndexFormat0, NULL, txtCMapIndexFormat2, NULL, txtCMapIndexFormat4, NULL, NULL,
	NULL,                NULL, NULL,                NULL, NULL,                NULL, NULL
};
__FORCE_INLINE__ uint32_t txtCMapIndex(const txtCMap *const cmap, const txtCodeUnit_t code){
	return txtCMapIndexJumpTable[cmap->format](cmap, code);
}


/** EVERYTHING BELOW IS TEMPORARY **/
static __FORCE_INLINE__ byte_t readByte(FILE *file){
	byte_t val;
	fread((void *)&val, sizeof(val), 1, file);
	return(val);
}

static __FORCE_INLINE__ uint16_t readUint16LE(FILE *file){
	byte_t bytes[2];
	fread((void *)bytes, sizeof(bytes), 1, file);
	return(
		((uint16_t)bytes[0] << 8) |
		(uint16_t)bytes[1]
	);
}

static __FORCE_INLINE__ uint32_t readUint32LE(FILE *file){
	byte_t bytes[4];
	fread((void *)bytes, sizeof(bytes), 1, file);
	return(
		((uint32_t)bytes[0] << 24) |
		((uint32_t)bytes[1] << 16) |
		((uint32_t)bytes[2] << 8)  |
		(uint32_t)bytes[3]
	);
}

static txtCMap *txtCMapLoadFormat0(FILE *file){
	const uint16_t length = readUint16LE(file);

	txtCMap0 *cmap = memAllocate(length);
	if(cmap == NULL){
		/** MALLOC FAILED **/
	}


	cmap->format   = 0;
	cmap->length   = length;
	cmap->language = readUint16LE(file);

	// Read the glyph indices into the character map's array.
	fread(cmap->glyphIdArray, sizeof(*cmap->glyphIdArray), 256, file);


	return((txtCMap *)cmap);
}

static txtCMap *txtCMapLoadFormat2(FILE *file){
	const uint16_t length = readUint16LE(file);
	uint16_t *curInt;
	const uint16_t *lastInt;

	txtCMap2 *cmap = memAllocate(length);
	if(cmap == NULL){
		/** MALLOC FAILED **/
	}


	cmap->format = 2;
	cmap->length = length;

	curInt = &cmap->language;
	lastInt = (uint16_t *)((byte_t *)cmap + length);
	// Load the character map's data.
	for(; curInt < lastInt; ++curInt){
		*curInt = readUint16LE(file);
	}


	return((txtCMap *)cmap);
}

static txtCMap *txtCMapLoadFormat4(FILE *file){
	const uint16_t length = readUint16LE(file);
	uint16_t *curInt;
	uint16_t *lastInt;

	txtCMap4 *cmap = memAllocate(length);
	if(cmap == NULL){
		/** MALLOC FAILED **/
	}


	cmap->format = 4;
	cmap->length = length;

	curInt = &cmap->language;
	lastInt = (uint16_t *)((byte_t *)cmap + length);
	// Read the character map's segments and glyph indices.
	for(; curInt < lastInt; ++curInt){
		*curInt = readUint16LE(file);
	}


	return((txtCMap *)cmap);
}

static txtCMap *(* const txtCMapLoadJumpTable[TEXT_CMAP_FORMAT_NUM])(
	FILE *const __RESTRICT__ file
) = {
	txtCMapLoadFormat0, NULL, txtCMapLoadFormat2, NULL, txtCMapLoadFormat4, NULL, NULL,
	NULL,               NULL, NULL,               NULL, NULL,               NULL, NULL
};

static __FORCE_INLINE__ uint32_t calculateTableChecksum(FILE *file, const uint32_t tableOffset, const uint32_t tableLength){
	uint32_t sum = 0;
	uint32_t numUint32s = ((tableLength + 3) & ~3) / sizeof(uint32_t);

	const int32_t oldPos = ftell(file);
	// Set the file pointer to the beginning of the table's data.
	fseek(file, tableOffset, SEEK_SET);

	// Sum the 32-bit integers in the table.
	for(; numUint32s > 0; --numUint32s){
		sum += readUint32LE(file);
	}

	// Return the file pointer to its original position.
	fseek(file, oldPos, SEEK_SET);


	return(sum);
}

static __FORCE_INLINE__ txtCMap *readCMapTable(FILE *file){
	const long tableStart = ftell(file);
	uint16_t numTables;

	// Load the character map's table information.
	// We only really need the number of subtables.
	readUint16LE(file); // version
	numTables = readUint16LE(file);

	for(; numTables > 0; --numTables){
		const uint16_t platformID = readUint16LE(file);
		const uint16_t encodingID = readUint16LE(file);
		const uint32_t offset     = readUint32LE(file);

		// We only load character map subtables for the Unicode platform
		// that are limited to supporting the basic multilingual plane.
		if(platformID == TEXT_CMAP_PLATFORM_UNICODE && encodingID <= TEXT_CMAP_ENCODING_2_0_BMP){
			const long oldPos = ftell(file);
			const uint16_t format = (fseek(file, tableStart + offset, SEEK_SET), readUint16LE(file));
			// If the subtable has a supported format, we can load the map!
			if(txtCMapLoadJumpTable[format]){
				txtCMap *cmap = txtCMapLoadJumpTable[format](file);
				// Return the map if it was loaded successfully!
				if(cmap != NULL){
					return(cmap);
				}
			}
			fseek(file, oldPos, SEEK_SET);
		}
	}

	return(NULL);
}

txtCMap *txtCMapLoad(const char *const __RESTRICT__ cmapPath, const size_t cmapPathLength){
	FILE *ttfFile;

	ttfFile = fopen(cmapPath, "rb");
	if(ttfFile != NULL){
		uint16_t numTables;
		uint32_t cmapOffset = 0;
		txtCMap *cmap = NULL;

		// Load the font's offset table information.
		// We only really need the number of tables.
		readUint32LE(ttfFile); // scalerType
		numTables = readUint16LE(ttfFile);
		readUint16LE(ttfFile); // searchRange
		readUint16LE(ttfFile); // entrySelector
		readUint16LE(ttfFile); // rangeShift

		// Search the table directory for the "cmap" table.
		// We also need to check each subtable's checksum.
		for(; numTables > 0; --numTables){
			// Load the current table directory entry's information.
			const char tag[5]     = {readByte(ttfFile), readByte(ttfFile), readByte(ttfFile), readByte(ttfFile), '\0'};
			uint32_t checkSum     = readUint32LE(ttfFile);
			const uint32_t offset = readUint32LE(ttfFile);
			const uint32_t length = readUint32LE(ttfFile);

			checkSum -= calculateTableChecksum(ttfFile, offset, length);
			// If this is the head table, we need to add the value of
			// "checkSumAdjustment", since we accidentally removed it.
			if(strcmp(tag, "head") == 0){
				const long oldPos = ftell(ttfFile);
				fseek(ttfFile, offset + TEXT_CMAP_HEAD_CHECKSUM_ADJUSTMENT_OFFSET, SEEK_SET);
				checkSum += readUint32LE(ttfFile);
				fseek(ttfFile, oldPos, SEEK_SET);

			// Remember the character map's table offset for when
			// we've finished verifying the other tables' checksums.
			}else if(strcmp(tag, "cmap") == 0){
				cmapOffset = offset;
			}

			// If the table's checksum doesn't
			// match, stop trying to load the font.
			if(checkSum != 0){
				printf(
					"Checksum mismatch for font table %s!\n"
					"Path: %s\n",
					tag, cmapPath
				);
				fclose(ttfFile);

				return(NULL);
			}
		}

		// If we found a character map table, load its data!
		if(cmapOffset != 0){
			fseek(ttfFile, cmapOffset, SEEK_SET);
			cmap = readCMapTable(ttfFile);
			if(cmap == NULL){
				printf(
					"Font file does not contain a supported character map format!\n"
					"Path: %s\n",
					cmapPath
				);
			}
		}else{
			printf(
				"Font file does not contain a character map table!\n"
				"Path: %s\n",
				cmapPath
			);
		}

		fclose(ttfFile);


		return(cmap);
	}else{
		printf(
			"Unable to open character map!\n"
			"Path: %s\n",
			cmapPath
		);
	}


	return(NULL);
}