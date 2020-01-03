#include "graphicsManager.h"
#include "gui.h"
#include "vertex.h"
#include "texture.h"
#include "skeleton.h"
#include "sprite.h"
#include "constantsMath.h"
#include "helpersMath.h"
#include "inline.h"

void guiElementTick(guiElement *const restrict element, const float elapsedTime){
	if(flagsAreSet(element->flags, GUI_ELEMENT_TYPE_WINDOW)){
		twiTick(&element->data.window.body, elapsedTime);
		twiTick(&element->data.window.border, elapsedTime);
	}
}

guiElement *guiElementAddChild(guiElement *const restrict element){
	//
}

static void guiElementRenderText(const guiElement *const restrict element, graphicsManager *const restrict gfxMngr, const camera *const restrict cam, const float distance, const float interpT){

	//

}

static void guiElementRenderWindow(const guiElement *const restrict element, graphicsManager *const restrict gfxMngr, const camera *const restrict cam, const float distance, const float interpT){

	const twFrame *const restrict frameBody = twiState(&element->data.window.body, interpT);
	const twFrame *const restrict frameBorder = twiState(&element->data.window.border, interpT);

	// Generate bone states for the window corners.
	const rectangle *offsets = &element->data.window.offsets[0];
	const bone root = {
		.position = {
			// Move the origin to the top left corner.
			.x = element->root.position.x + element->root.scale.x*0.5f + offsets->w * frameBorder->image->width,
			.y = element->root.position.y - element->root.scale.y*0.5f - offsets->h * frameBorder->image->height,
			.z = element->root.position.z
		},
		.orientation = element->root.orientation,
		.scale = element->root.scale
	};

	const float inverseWidth = 1.f/frameBorder->image->width;
	const float inverseHeight = 1.f/frameBody->image->height;

	vec2 size;
	spriteState *state = &gfxMngr->shdrData.spriteTransformState[0];

	// Bind the texture.
	gfxMngrBindTexture(gfxMngr, GL_TEXTURE0, frameBorder->image->diffuseID);
	// Feed the translucency multiplier to the shader
	glUniform1f(gfxMngr->shdrPrgObj.alphaID, 1.f);

	// Draw the bottom right corner.
	// Border corner size, in pixels.
	size.x = offsets->w * frameBorder->image->width;
	size.y = offsets->h * frameBorder->image->height;
	// Transformation matrix.
	state->transformation.m[0][0] = size.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = root.position.x + 0.5f * (root.scale.x + size.x);
	state->transformation.m[0][1] = 0.f;    state->transformation.m[1][1] = size.y; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = root.position.y - 0.5f * (root.scale.y + size.y);
	state->transformation.m[0][2] = 0.f;    state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = root.position.z;
	state->transformation.m[0][3] = 0.f;    state->transformation.m[1][3] = 0.f;    state->transformation.m[2][3] = 0.f; state->transformation.m[3][3] = 1.f;
	// Texture fragment.
	state->frame.x = frameBorder->subframe.x + offsets->x;
	state->frame.y = frameBorder->subframe.y + offsets->y;
	state->frame.w = frameBorder->subframe.w * offsets->w;
	state->frame.h = frameBorder->subframe.h * offsets->h;

	// Draw the top right corner.
	++offsets, ++state;
	// Border corner size, in pixels.
	size.x = offsets->w * frameBorder->image->width;
	size.y = offsets->h * frameBorder->image->height;
	// Transformation matrix.
	state->transformation.m[0][0] = size.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = root.position.x + 0.5f * (root.scale.x + size.x);
	state->transformation.m[0][1] = 0.f;    state->transformation.m[1][1] = size.y; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = root.position.y + 0.5f * (root.scale.y + size.y);
	state->transformation.m[0][2] = 0.f;    state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = root.position.z;
	state->transformation.m[0][3] = 0.f;    state->transformation.m[1][3] = 0.f;    state->transformation.m[2][3] = 0.f; state->transformation.m[3][3] = 1.f;
	// Texture fragment.
	state->frame.x = frameBorder->subframe.x + offsets->x;
	state->frame.y = frameBorder->subframe.y + offsets->y;
	state->frame.w = frameBorder->subframe.w * offsets->w;
	state->frame.h = frameBorder->subframe.h * offsets->h;

	// Draw the top left corner.
	++offsets, ++state;
	// Border corner size, in pixels.
	size.x = offsets->w * frameBorder->image->width;
	size.y = offsets->h * frameBorder->image->height;
	// Transformation matrix.
	state->transformation.m[0][0] = size.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = root.position.x - 0.5f * (root.scale.x + size.x);
	state->transformation.m[0][1] = 0.f;    state->transformation.m[1][1] = size.y; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = root.position.y + 0.5f * (root.scale.y + size.y);
	state->transformation.m[0][2] = 0.f;    state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = root.position.z;
	state->transformation.m[0][3] = 0.f;    state->transformation.m[1][3] = 0.f;    state->transformation.m[2][3] = 0.f; state->transformation.m[3][3] = 1.f;
	// Texture fragment.
	state->frame.x = frameBorder->subframe.x + offsets->x;
	state->frame.y = frameBorder->subframe.y + offsets->y;
	state->frame.w = frameBorder->subframe.w * offsets->w;
	state->frame.h = frameBorder->subframe.h * offsets->h;

	// Draw the bottom left corner.
	++offsets, ++state;
	// Border corner size, in pixels.
	size.x = offsets->w * frameBorder->image->width;
	size.y = offsets->h * frameBorder->image->height;
	// Transformation matrix.
	state->transformation.m[0][0] = size.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = root.position.x - 0.5f * (root.scale.x + size.x);
	state->transformation.m[0][1] = 0.f;    state->transformation.m[1][1] = size.y; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = root.position.y - 0.5f * (root.scale.y + size.y);
	state->transformation.m[0][2] = 0.f;    state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = root.position.z;
	state->transformation.m[0][3] = 0.f;    state->transformation.m[1][3] = 0.f;    state->transformation.m[2][3] = 0.f; state->transformation.m[3][3] = 1.f;
	// Texture fragment.
	state->frame.x = frameBorder->subframe.x + offsets->x;
	state->frame.y = frameBorder->subframe.y + offsets->y;
	state->frame.w = frameBorder->subframe.w * offsets->w;
	state->frame.h = frameBorder->subframe.h * offsets->h;

	// Draw the right side.
	++offsets, ++state;
	// Border side thickness, in pixels.
	size.x = offsets->h * frameBorder->image->height;
	// Transformation matrix.
	state->transformation.m[0][0] = 0.f;           state->transformation.m[1][0] = size.x; state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = root.position.x + 0.5f * (root.scale.x + size.x);
	state->transformation.m[0][1] = -root.scale.y; state->transformation.m[1][1] = 0.f;    state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = root.position.y;
	state->transformation.m[0][2] = 0.f;           state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = root.position.z;
	state->transformation.m[0][3] = 0.f;           state->transformation.m[1][3] = 0.f;    state->transformation.m[2][3] = 0.f; state->transformation.m[3][3] = 1.f;
	// Texture fragment.
	state->frame.x = frameBorder->subframe.x + offsets->x;
	state->frame.y = frameBorder->subframe.y + offsets->y;
	state->frame.w = flagsAreSet(element->data.window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBorder->subframe.w * offsets->w : -2.f*root.scale.y*inverseWidth;
	state->frame.h = frameBorder->subframe.h * offsets->h;

	// Draw the top side.
	++offsets, ++state;
	// Border side thickness, in pixels.
	size.x = offsets->h * frameBorder->image->height;
	// Transformation matrix.
	state->transformation.m[0][0] = root.scale.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = root.position.x;
	state->transformation.m[0][1] = 0.f;          state->transformation.m[1][1] = size.x; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = root.position.y + 0.5f * (root.scale.y + size.x);
	state->transformation.m[0][2] = 0.f;          state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = root.position.z;
	state->transformation.m[0][3] = 0.f;          state->transformation.m[1][3] = 0.f;    state->transformation.m[2][3] = 0.f; state->transformation.m[3][3] = 1.f;
	// Texture fragment.
	state->frame.x = frameBorder->subframe.x + offsets->x;
	state->frame.y = frameBorder->subframe.y + offsets->y;
	state->frame.w = flagsAreSet(element->data.window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBorder->subframe.w * offsets->w : -2.f*root.scale.x*inverseWidth;
	state->frame.h = frameBorder->subframe.h * offsets->h;

	// Draw the left side.
	++offsets, ++state;
	// Border side thickness, in pixels.
	size.x = offsets->h * frameBorder->image->height;
	// Transformation matrix.
	state->transformation.m[0][0] = 0.f;          state->transformation.m[1][0] = -size.x; state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = root.position.x - 0.5f * (root.scale.x + size.x);
	state->transformation.m[0][1] = root.scale.y; state->transformation.m[1][1] = 0.f;    state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = root.position.y;
	state->transformation.m[0][2] = 0.f;          state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = root.position.z;
	state->transformation.m[0][3] = 0.f;          state->transformation.m[1][3] = 0.f;    state->transformation.m[2][3] = 0.f; state->transformation.m[3][3] = 1.f;
	// Texture fragment.
	state->frame.x = flagsAreSet(element->data.window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBorder->subframe.x + offsets->x : frameBorder->subframe.x + offsets->x + 2.f*root.scale.y*inverseWidth;
	state->frame.y = frameBorder->subframe.y + offsets->y;
	state->frame.w = flagsAreSet(element->data.window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBorder->subframe.w * offsets->w : 2.f*root.scale.y*inverseWidth;
	state->frame.h = frameBorder->subframe.h * offsets->h;

	// Draw the bottom side.
	++offsets, ++state;
	// Border side thickness, in pixels.
	size.x = offsets->h * frameBorder->image->height;
	// Transformation matrix.
	state->transformation.m[0][0] = -root.scale.x; state->transformation.m[1][0] = 0.f;     state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = root.position.x;
	state->transformation.m[0][1] = 0.f;           state->transformation.m[1][1] = -size.x; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = root.position.y - 0.5f * (root.scale.y + size.x);
	state->transformation.m[0][2] = 0.f;           state->transformation.m[1][2] = 0.f;     state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = root.position.z;
	state->transformation.m[0][3] = 0.f;           state->transformation.m[1][3] = 0.f;     state->transformation.m[2][3] = 0.f; state->transformation.m[3][3] = 1.f;
	// Texture fragment.
	state->frame.x = flagsAreSet(element->data.window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBorder->subframe.x + offsets->x : frameBorder->subframe.x + offsets->x + 2.f*root.scale.x*inverseWidth;
	state->frame.y = frameBorder->subframe.y + offsets->y;
	state->frame.w = flagsAreSet(element->data.window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBorder->subframe.w * offsets->w : 2.f*root.scale.x*inverseWidth;
	state->frame.h = frameBorder->subframe.h * offsets->h;

	// Upload the state data to the shader and render the model.
	glBindVertexArray(meshSprite.vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, sprStateBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 8*sizeof(spriteState), &gfxMngr->shdrData.spriteTransformState[0]);
	glDrawElementsInstanced(GL_TRIANGLES, meshSprite.indexNum, GL_UNSIGNED_INT, 0, 8);

	// Draw the body.
	state = &gfxMngr->shdrData.spriteTransformState[0];
	// Transformation matrix.
	state->transformation = boneMatrix(root);
	// Texture fragment.
	state->frame.x = frameBody->subframe.x;
	state->frame.y = flagsAreSet(element->data.window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBody->subframe.y : frameBody->subframe.y + root.scale.y*inverseHeight;
	state->frame.w = flagsAreSet(element->data.window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBody->subframe.w : root.scale.x/frameBody->image->width;
	state->frame.h = flagsAreSet(element->data.window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBody->subframe.h : root.scale.y*inverseHeight;
	// Bind the texture.
	gfxMngrBindTexture(gfxMngr, GL_TEXTURE0, frameBody->image->diffuseID);
	// Upload the state data to the shader and render the model.
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spriteState), &gfxMngr->shdrData.spriteTransformState[0]);
	glDrawElementsInstanced(GL_TRIANGLES, meshSprite.indexNum, GL_UNSIGNED_INT, 0, 1);

}

/** The lines below should eventually be removed. **/
#define guiElementRenderRenderable NULL
#define guiElementRenderObject     NULL

void (* const guiElementRenderJumpTable[4])(
	const guiElement *const restrict element, graphicsManager *const restrict gfxMngr, const camera *const restrict cam, const float distance, const float interpT
) = {
	guiElementRenderText,
	guiElementRenderWindow,
	guiElementRenderRenderable,
	guiElementRenderObject
};
__FORCE_INLINE__ void guiElementRender(const guiElement *const restrict element, graphicsManager *const restrict gfxMngr, const camera *const restrict cam, const float distance, const float interpT){
	guiElementRenderJumpTable[element->flags & GUI_ELEMENT_TYPE_MASK](element, gfxMngr, cam, distance, interpT);
}

void guiElementDelete(guiElement *element){
	//
}