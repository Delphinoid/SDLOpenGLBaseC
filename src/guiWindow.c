#include "graphicsManager.h"
#include "gui.h"
#include "sprite.h"

void guiElementRenderWindow(const guiElement *const restrict element, graphicsManager *const restrict gfxMngr, const camera *const restrict cam, const float distance, const float interpT){

	const guiWindow window = element->data.window;
	const bone root = element->root;

	const twFrame *const restrict frameBody = twiState(&window.body, interpT);
	const twFrame *const restrict frameBorder = twiState(&window.border, interpT);

	const rectangle *offsets = &window.offsets[0];
	// Transformed root location.
	const bone transform = {
		.position = {
			// Move the origin to the top left corner.
			.x = root.position.x + root.scale.x*0.5f + offsets->w * frameBorder->image->width,
			.y = root.position.y - root.scale.y*0.5f - offsets->h * frameBorder->image->height,
			.z = root.position.z
		},
		.orientation = root.orientation,
		.scale = root.scale
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
	// Consists of a scale and a translation.
	state->transformation.m[0][0] = size.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = transform.position.x + 0.5f * (transform.scale.x + size.x);
	state->transformation.m[0][1] = 0.f;    state->transformation.m[1][1] = size.y; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = transform.position.y - 0.5f * (transform.scale.y + size.y);
	state->transformation.m[0][2] = 0.f;    state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = transform.position.z;
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
	// Consists of a scale and a translation.
	state->transformation.m[0][0] = size.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = transform.position.x + 0.5f * (transform.scale.x + size.x);
	state->transformation.m[0][1] = 0.f;    state->transformation.m[1][1] = size.y; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = transform.position.y + 0.5f * (transform.scale.y + size.y);
	state->transformation.m[0][2] = 0.f;    state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = transform.position.z;
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
	// Consists of a scale and a translation.
	state->transformation.m[0][0] = size.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = transform.position.x - 0.5f * (transform.scale.x + size.x);
	state->transformation.m[0][1] = 0.f;    state->transformation.m[1][1] = size.y; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = transform.position.y + 0.5f * (transform.scale.y + size.y);
	state->transformation.m[0][2] = 0.f;    state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = transform.position.z;
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
	// Consists of a scale and a translation.
	state->transformation.m[0][0] = size.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = transform.position.x - 0.5f * (transform.scale.x + size.x);
	state->transformation.m[0][1] = 0.f;    state->transformation.m[1][1] = size.y; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = transform.position.y - 0.5f * (transform.scale.y + size.y);
	state->transformation.m[0][2] = 0.f;    state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = transform.position.z;
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
	// Consists of a scale, a rotation and a translation.
	state->transformation.m[0][0] = 0.f;                state->transformation.m[1][0] = size.x; state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = transform.position.x + 0.5f * (transform.scale.x + size.x);
	state->transformation.m[0][1] = -transform.scale.y; state->transformation.m[1][1] = 0.f;    state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = transform.position.y;
	state->transformation.m[0][2] = 0.f;                state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = transform.position.z;
	state->transformation.m[0][3] = 0.f;                state->transformation.m[1][3] = 0.f;    state->transformation.m[2][3] = 0.f; state->transformation.m[3][3] = 1.f;
	// Texture fragment.
	state->frame.x = frameBorder->subframe.x + offsets->x;
	state->frame.y = frameBorder->subframe.y + offsets->y;
	state->frame.w = flagsAreSet(window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBorder->subframe.w * offsets->w : -2.f*transform.scale.y*inverseWidth;
	state->frame.h = frameBorder->subframe.h * offsets->h;

	// Draw the top side.
	++offsets, ++state;
	// Border side thickness, in pixels.
	size.x = offsets->h * frameBorder->image->height;
	// Transformation matrix.
	// Consists of a scale, a rotation and a translation.
	state->transformation.m[0][0] = transform.scale.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = transform.position.x;
	state->transformation.m[0][1] = 0.f;               state->transformation.m[1][1] = size.x; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = transform.position.y + 0.5f * (transform.scale.y + size.x);
	state->transformation.m[0][2] = 0.f;               state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = transform.position.z;
	state->transformation.m[0][3] = 0.f;               state->transformation.m[1][3] = 0.f;    state->transformation.m[2][3] = 0.f; state->transformation.m[3][3] = 1.f;
	// Texture fragment.
	state->frame.x = frameBorder->subframe.x + offsets->x;
	state->frame.y = frameBorder->subframe.y + offsets->y;
	state->frame.w = flagsAreSet(window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBorder->subframe.w * offsets->w : -2.f*transform.scale.x*inverseWidth;
	state->frame.h = frameBorder->subframe.h * offsets->h;

	// Draw the left side.
	++offsets, ++state;
	// Border side thickness, in pixels.
	size.x = offsets->h * frameBorder->image->height;
	// Transformation matrix.
	// Consists of a scale, a rotation and a translation.
	state->transformation.m[0][0] = 0.f;               state->transformation.m[1][0] = -size.x; state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = transform.position.x - 0.5f * (transform.scale.x + size.x);
	state->transformation.m[0][1] = transform.scale.y; state->transformation.m[1][1] = 0.f;     state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = transform.position.y;
	state->transformation.m[0][2] = 0.f;               state->transformation.m[1][2] = 0.f;     state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = transform.position.z;
	state->transformation.m[0][3] = 0.f;               state->transformation.m[1][3] = 0.f;     state->transformation.m[2][3] = 0.f; state->transformation.m[3][3] = 1.f;
	// Texture fragment.
	state->frame.x = flagsAreSet(window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBorder->subframe.x + offsets->x : frameBorder->subframe.x + offsets->x + 2.f*transform.scale.y*inverseWidth;
	state->frame.y = frameBorder->subframe.y + offsets->y;
	state->frame.w = flagsAreSet(window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBorder->subframe.w * offsets->w : 2.f*transform.scale.y*inverseWidth;
	state->frame.h = frameBorder->subframe.h * offsets->h;

	// Draw the bottom side.
	++offsets, ++state;
	// Border side thickness, in pixels.
	size.x = offsets->h * frameBorder->image->height;
	// Transformation matrix.
	// Consists of a scale, a rotation and a translation.
	state->transformation.m[0][0] = -transform.scale.x; state->transformation.m[1][0] = 0.f;     state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = transform.position.x;
	state->transformation.m[0][1] = 0.f;                state->transformation.m[1][1] = -size.x; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = transform.position.y - 0.5f * (transform.scale.y + size.x);
	state->transformation.m[0][2] = 0.f;                state->transformation.m[1][2] = 0.f;     state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = transform.position.z;
	state->transformation.m[0][3] = 0.f;                state->transformation.m[1][3] = 0.f;     state->transformation.m[2][3] = 0.f; state->transformation.m[3][3] = 1.f;
	// Texture fragment.
	state->frame.x = flagsAreSet(window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBorder->subframe.x + offsets->x : frameBorder->subframe.x + offsets->x + 2.f*transform.scale.x*inverseWidth;
	state->frame.y = frameBorder->subframe.y + offsets->y;
	state->frame.w = flagsAreSet(window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBorder->subframe.w * offsets->w : 2.f*transform.scale.x*inverseWidth;
	state->frame.h = frameBorder->subframe.h * offsets->h;

	// Upload the state data to the shader and render the model.
	glBindVertexArray(g_meshSprite.vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, g_sprStateBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 8*sizeof(spriteState), &gfxMngr->shdrData.spriteTransformState[0]);
	glDrawElementsInstanced(GL_TRIANGLES, g_meshSprite.indexNum, GL_UNSIGNED_INT, 0, 8);

	// Draw the body.
	state = &gfxMngr->shdrData.spriteTransformState[0];
	// Transformation matrix.
	state->transformation = boneMatrix(transform);
	// Texture fragment.
	state->frame.x = frameBody->subframe.x;
	state->frame.y = frameBody->subframe.y;
	state->frame.w = flagsAreSet(window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBody->subframe.w : transform.scale.x/frameBody->image->width;
	state->frame.h = flagsAreSet(window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBody->subframe.h : transform.scale.y*inverseHeight;
	// Bind the texture.
	gfxMngrBindTexture(gfxMngr, GL_TEXTURE0, frameBody->image->diffuseID);
	// Upload the state data to the shader and render the model.
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spriteState), &gfxMngr->shdrData.spriteTransformState[0]);
	glDrawElementsInstanced(GL_TRIANGLES, g_meshSprite.indexNum, GL_UNSIGNED_INT, 0, 1);

}