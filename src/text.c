#include "text.h"

const txtGlyph *txtFontFindGlyph(const txtFont *const restrict font, const txtCode c){
	/** I don't really like these searches. Surely there's a better way? **/
	const txtGlyph *g = font->glyphs;
	const txtGlyph *const gLast = &g[font->glyphNum];
	for(; g < gLast; ++g){
		if(g->code == c){
			return g;
		}
	}
	return 0;
}

const txtFont *txtTypefaceFindFont(const txtTypeface *const restrict typeface, const flags_t style, const float size){
	/** I don't really like these searches. Surely there's a better way? **/
	const txtFont *s = typeface->styles;
	const txtFont *const sLast = &s[typeface->styleNum];
	for(; s < sLast; ++s){
		if(s->style == style && s->size == size){
			return s;
		}
	}
	return 0;
}

return_t txtBufferNextCharacter(const txtBuffer *const restrict buffer, const byte_t **i){
	if(*i == buffer->back){
		*i = buffer->front;
	}else{
		++(*i);
	}
	if(*i == buffer->offset){
		return 0;
	}
	return 1;
}

txtCode txtBufferParseCharacter(const txtBuffer *const restrict buffer, const byte_t **i, flags_t *const restrict style, float *const restrict size){

	txtCode c = **i;
	unsigned int extra = 0;

	if(c == 0){
		// NULL terminator.
		return 0;
	}

	if(c >= TEXT_CHARACTER_UTF8_1_BYTE_LIMIT){
		++extra;
		if(c >= TEXT_CHARACTER_UTF8_2_BYTE_LIMIT){
			++extra;
			if(c >= TEXT_CHARACTER_UTF8_3_BYTE_LIMIT){
				++extra;
				if(c >= TEXT_CHARACTER_UTF8_4_BYTE_LIMIT){
					// Invalid character code.
					return 0;
				}
			}
		}
	}

	while(extra){
		txtCode e;
		if(txtBufferNextCharacter(buffer, i) && (e = **i) < TEXT_CHARACTER_UTF8_MULTIBYTE_LIMIT){
			// Append the character to the full width character code.
			c += e << (extra * 8);
			--extra;
		}else{
			// Invalid character code.
			return 0;
		}
	}

	return c;

}