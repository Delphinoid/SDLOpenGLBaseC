#include "graphicsManager.h"
#include "gui.h"
#include "sprite.h"
#include "mesh.h"

void guiElementRenderText(const guiElement *const __RESTRICT__ element, graphicsManager *const __RESTRICT__ gfxMngr, const camera *const __RESTRICT__ cam, const float distance, const float interpT){

	const guiText text = element->data.text;
	const bone root = element->root;
	const mat4 rootTransform = boneMatrix(root);

	txtFormat format = {.font = 0, .size = 0, .style = 0};
	txtFont font = *text.stream.typeface->styles;
	const texture *atlas = NULL;

	txtCursor cursor = {.x = 0.f, .y = 0.f};
	float advanceY = 0.f;

	size_t bufferSize = 0;
	spriteState *state = &gfxMngr->shdrData.spriteTransformState[0];

	const byte_t *currentCharacter = text.stream.offset;

	// Scaled root transform.
	mat4 transform = rootTransform;

	// Initialize rendering state.
	glBindVertexArray(g_meshSprite.vaoID);

	// Loop through every character in the stream.
	do {

		const txtCodeUnit_t code = {._32 = txtStreamParseCharacter(&text.stream, &currentCharacter, &format)};
		if(code._32 == 0){
			// End of stream.
			break;
		}

		if(code._32 == TEXT_UTF8_INVALID_CODEUNIT){

			// Invalid UTF-8 code unit. This could
			// mean that the formatting was updated.
			if(flagsAreSet(format.style, TEXT_FORMAT_UPDATED)){

				// Handle updated formatting.
				if(flagsAreSet(format.style, TEXT_FORMAT_FONT_UPDATED)){
					font = text.stream.typeface->styles[format.font];
					gfxMngrBindTexture(gfxMngr, GL_TEXTURE0, atlas->diffuseID);
				}
				if(flagsAreSet(format.style, TEXT_FORMAT_SIZE_UPDATED)){
					transform = mat4Scale(rootTransform, format.size, format.size, format.size);
				}
				if(flagsAreSet(format.style, TEXT_FORMAT_STYLE_UPDATED)){
					/// Send SDF style changes to the shader.
				}
				flagsUnset(format.style, TEXT_FORMAT_FONT_UPDATED);

				// If the state buffer is not empty, render.
				// We must do this to avoid formatting changes
				// affecting past characters.
				if(bufferSize > 0){
					state = &gfxMngr->shdrData.spriteTransformState[0];
					// Upload the state data to the shader.
					glBindBuffer(GL_ARRAY_BUFFER, g_sprStateBufferID);
					glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize*sizeof(spriteState), state);
					// Render.
					glDrawElementsInstanced(GL_TRIANGLES, g_meshSprite.indexNum, GL_UNSIGNED_INT, NULL, bufferSize);
					bufferSize = 0;
				}

			}

			continue;

		}else if(code._32 == '\n'){

			// New line.
			cursor.x = 0.f;
			cursor.y += advanceY*format.size;
			advanceY = font.height;

		}else{

			// Obtain the glyph for this code unit.
			const txtGlyph glyph = font.glyphs[txtCMapIndex(font.cmap, code)];

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

			// Generate glyph transform and add it to the state buffer.
			/// Temporary transformation. There must be a better way.
			state->transformation = mat4Translate(transform, cursor.x + glyph.kerningX, cursor.y + glyph.kerningY, 0.f);
			state->frame = glyph.frame;
			++state;

			if(txtCursorAdvance(&cursor, glyph.advanceX*format.size, advanceY*format.size, text.bounds.w) || font.height > advanceY){
				// Reset font.height after the new line.
				// Also make sure that advanceY is the maximum
				// height of every font on the current line.
				advanceY = font.height;
			}

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

}