#include "graphicsManager.h"
#include "gui.h"
#include "sprite.h"
#include "mesh.h"

void guiTextTick(guiElement *const element, const float dt_ms){
	//
}

/**static void guiRenderTextNextLine(){



}**/

__HINT_INLINE__ mat4 tempmat4Translate(const float x, const float y, const float z, const mat4 m){

}
void guiTextRender(const guiElement *const element, graphicsManager *const gfxMngr, const camera *const cam, const float distance, const float interpT){

	const guiText text = element->data.text;
	const transform root = (element->parent == NULL ? element->root : tfMultiply(element->parent->root, element->root));
	const mat4 rootTransform = tfMatrix4(root);

	txtFont font = *text.format.font;
	const texture *atlas = NULL;

	txtCursor cursor = {.x = 0.f, .y = 0.f};
	float advanceY = 0.f;

	size_t bufferSize = 0;
	spriteState *state = &gfxMngr->shdrData.spriteTransformState[0];

	const byte_t *currentCharacter = text.stream.offset;

	// Scaled root transform.
	mat4 transform = mat4Scale(text.format.size, text.format.size, text.format.size, rootTransform);

	// Initialize rendering state.
	glBindVertexArray(g_meshSprite.vaoID);
	glUniform1ui(gfxMngr->shdrPrgSpr.sdfTypeID, font.type);
	glUniform4fv(gfxMngr->shdrPrgSpr.sdfColourID, 1, (GLfloat *)&text.format.colour);
	glUniform4fv(gfxMngr->shdrPrgSpr.sdfBackgroundID, 1, (GLfloat *)&text.format.background);

	// Loop through every character in the stream.
	do {

		const txtCodeUnit_t code = {._32 = txtStreamParseCharacter(&text.stream, &currentCharacter)};

		if(code._32 == '\0'){
			// End of stream.
			break;
		}else if(code._32 == '\n'){
			// New line.
			cursor.x = 0.f;
			cursor.y += advanceY;
			advanceY = font.height;
		/**
		}else if(code._32 == TEXT_UTF8_INVALID_CODEUNIT){

			// Invalid UTF-8 code unit. This could
			// mean that the formatting was updated.
			if(flagsAreSet(text.format.style, TEXT_FORMAT_UPDATED)){

				// If the state buffer is not empty, render.
				// We must do this to avoid formatting changes
				// affecting past characters.
				/// Check if we actually need to do this.
				if(bufferSize > 0){
					state = &gfxMngr->shdrData.spriteTransformState[0];
					// Upload the state data to the shader.
					glBindBuffer(GL_ARRAY_BUFFER, g_sprStateBufferID);
					glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize*sizeof(spriteState), state);
					// Render.
					glDrawElementsInstanced(GL_TRIANGLES, g_meshSprite.indexNum, GL_UNSIGNED_INT, NULL, bufferSize);
					bufferSize = 0;
				}

				// Handle updated formatting.
				if(flagsAreSet(text.format.style, TEXT_FORMAT_FONT_UPDATED)){
					///font = text.stream.typeface->styles[text.format.font];
					///gfxMngrBindTexture(gfxMngr, GL_TEXTURE0, atlas->diffuseID);
				}
				if(flagsAreSet(text.format.style, TEXT_FORMAT_SIZE_UPDATED)){
					transform = mat4Scale(rootTransform, text.format.size, text.format.size, text.format.size);
				}
				if(flagsAreSet(text.format.style, TEXT_FORMAT_STYLE_UPDATED)){
					/// Send SDF style changes to the shader.
				}
				flagsUnset(text.format.style, TEXT_FORMAT_FONT_UPDATED);

			}


			continue;
		**/
		}else{

			// Obtain the glyph for this code unit.
			const txtGlyph glyph = font.glyphs[txtCMapIndex(font.cmap, code)];
			const float glyphWidth = glyph.frame.w*glyph.atlas->width;
			const float glyphHeight = glyph.frame.h*glyph.atlas->height;
			float glyphX = cursor.x + glyphWidth*0.5f + glyph.kerningX;
			float glyphY = -glyphHeight*0.5f - glyph.kerningY;

			// If the state buffer is full or the atlas must change, render.
			if(bufferSize >= SPRITE_STATE_BUFFER_SIZE || atlas != glyph.atlas){
				if(bufferSize != 0){
					state = &gfxMngr->shdrData.spriteTransformState[0];
					// Upload the state data to the shader.
					glBindBuffer(GL_ARRAY_BUFFER, g_sprStateBufferID);
					glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize*sizeof(spriteState), state);
					// Render.
					glDrawElementsInstanced(GL_TRIANGLES, g_meshSprite.indexNum, GL_UNSIGNED_INT, NULL, bufferSize);
					bufferSize = 0;
				}
				// Bind the atlas texture for the following glyphs.
				atlas = glyph.atlas;
				gfxMngrBindTexture(gfxMngr, GL_TEXTURE0, atlas->diffuseID);
			}

			/** Maybe support vertical text rendering? **/
			// Adjust advanceY.
			if(font.height > advanceY){
				// Reset font.height after the new line.
				// Also make sure that advanceY is the maximum
				// height of every font on the current line.
				advanceY = font.height;
			}

			// Check if the glyph will fit.
			if((cursor.x + glyphWidth + glyph.kerningX)*text.format.size*root.scale.x > text.width){
				// New line.
				glyphX -= cursor.x;
				cursor.x = 0.f;
				cursor.y += advanceY;
				advanceY = font.height;
			}
			glyphY -= cursor.y;

			// Generate glyph transform and add it to the state buffer.
			/// Temporary transformation. There must be a better way.
			state->transformation = mat4TranslatePre(transform, glyphX, glyphY, 0.f);
			state->transformation = mat4Scale(glyphWidth, glyphHeight, 1.f, state->transformation);
			state->frame = glyph.frame;
			++state; ++bufferSize;

			cursor.x += glyph.advanceX;

		}

	} while(txtStreamNextCharacter(&text.stream, &currentCharacter));

	// Render any leftover elements in the buffer.
	if(bufferSize > 0){
		// Upload the state data to the shader.
		glBindBuffer(GL_ARRAY_BUFFER, g_sprStateBufferID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize*sizeof(spriteState), &gfxMngr->shdrData.spriteTransformState[0]);
		// Render.
		glDrawElementsInstanced(GL_TRIANGLES, g_meshSprite.indexNum, GL_UNSIGNED_INT, NULL, bufferSize);
	}

	// Reset the SDF type flag so future non-text rendering isn't messed up.
	if(font.type != 0){
		glUniform1ui(gfxMngr->shdrPrgSpr.sdfTypeID, 0);
	}

}
