#include "graphicsManager.h"
#include "gui.h"
#include "sprite.h"

void guiWindowTick(guiElement *const element, const float dt_ms){
	twiTick(&element->data.window.body, dt_ms);
	twiTick(&element->data.window.border, dt_ms);
}

void guiWindowRender(const guiElement *const element, graphicsManager *const gfxMngr, const camera *const cam, const float distance, const float interpT){

	const guiWindow window = element->data.window;
	const transform root = element->root;

	const twFrame *const frameBody = twiState(&window.body, interpT);
	const twFrame *const frameBorder = twiState(&window.border, interpT);

	const rectangle *offsets = &window.offsets[0];
	vec2 size;

	// Transformed root location.
	float inverseWidthX;
	float inverseWidthY;
	transform tf = {
		.position = {
			// Move the origin to the top left corner.
			#ifdef TRANSFORM_MATRIX_SHEAR
			.x = root.position.x + root.scale.m[0][0]*0.5f + offsets->w * frameBorder->image->width,
			.y = root.position.y - root.scale.m[1][1]*0.5f - offsets->h * frameBorder->image->height,
			#else
			.x = root.position.x + root.scale.x*0.5f + offsets->w * frameBorder->image->width,
			.y = root.position.y - root.scale.y*0.5f - offsets->h * frameBorder->image->height,
			#endif
			.z = root.position.z
		},
		.orientation = root.orientation,
		.scale = root.scale
	};
	if(element->parent != NULL){
		// Append to parent position.
		tf = tfMultiply(element->parent->root, tf);
	}
	#ifdef TRANSFORM_MATRIX_SHEAR
	inverseWidthX = tf.scale.m[0][0]/frameBorder->image->width;
	inverseWidthY = tf.scale.m[1][1]/frameBorder->image->width;
	#else
	inverseWidthX = tf.scale.x/frameBorder->image->width;
	inverseWidthY = tf.scale.y/frameBorder->image->width;
	#endif

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
	#ifdef TRANSFORM_MATRIX_SHEAR
	state->transformation.m[0][0] = size.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = tf.position.x + 0.5f * (tf.scale.m[0][0] + size.x);
	state->transformation.m[0][1] = 0.f;    state->transformation.m[1][1] = size.y; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = tf.position.y - 0.5f * (tf.scale.m[1][1] + size.y);
	#else
	state->transformation.m[0][0] = size.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = tf.position.x + 0.5f * (tf.scale.x + size.x);
	state->transformation.m[0][1] = 0.f;    state->transformation.m[1][1] = size.y; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = tf.position.y - 0.5f * (tf.scale.y + size.y);
	#endif
	state->transformation.m[0][2] = 0.f;    state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = tf.position.z;
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
	#ifdef TRANSFORM_MATRIX_SHEAR
	state->transformation.m[0][0] = size.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = tf.position.x + 0.5f * (tf.scale.m[0][0] + size.x);
	state->transformation.m[0][1] = 0.f;    state->transformation.m[1][1] = size.y; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = tf.position.y + 0.5f * (tf.scale.m[1][1] + size.y);
	#else
	state->transformation.m[0][0] = size.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = tf.position.x + 0.5f * (tf.scale.x + size.x);
	state->transformation.m[0][1] = 0.f;    state->transformation.m[1][1] = size.y; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = tf.position.y + 0.5f * (tf.scale.y + size.y);
	#endif
	state->transformation.m[0][2] = 0.f;    state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = tf.position.z;
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
	#ifdef TRANSFORM_MATRIX_SHEAR
	state->transformation.m[0][0] = size.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = tf.position.x - 0.5f * (tf.scale.m[0][0] + size.x);
	state->transformation.m[0][1] = 0.f;    state->transformation.m[1][1] = size.y; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = tf.position.y + 0.5f * (tf.scale.m[1][1] + size.y);
	#else
	state->transformation.m[0][0] = size.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = tf.position.x - 0.5f * (tf.scale.x + size.x);
	state->transformation.m[0][1] = 0.f;    state->transformation.m[1][1] = size.y; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = tf.position.y + 0.5f * (tf.scale.y + size.y);
	#endif
	state->transformation.m[0][2] = 0.f;    state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = tf.position.z;
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
	#ifdef TRANSFORM_MATRIX_SHEAR
	state->transformation.m[0][0] = size.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = tf.position.x - 0.5f * (tf.scale.m[0][0] + size.x);
	state->transformation.m[0][1] = 0.f;    state->transformation.m[1][1] = size.y; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = tf.position.y - 0.5f * (tf.scale.m[1][1] + size.y);
	#else
	state->transformation.m[0][0] = size.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = tf.position.x - 0.5f * (tf.scale.x + size.x);
	state->transformation.m[0][1] = 0.f;    state->transformation.m[1][1] = size.y; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = tf.position.y - 0.5f * (tf.scale.y + size.y);
	#endif
	state->transformation.m[0][2] = 0.f;    state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = tf.position.z;
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
	#ifdef TRANSFORM_MATRIX_SHEAR
	state->transformation.m[0][0] = 0.f;               state->transformation.m[1][0] = size.x; state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = tf.position.x + 0.5f * (tf.scale.m[0][0] + size.x);
	state->transformation.m[0][1] = -tf.scale.m[1][1]; state->transformation.m[1][1] = 0.f;    state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = tf.position.y;
	#else
	state->transformation.m[0][0] = 0.f;         state->transformation.m[1][0] = size.x; state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = tf.position.x + 0.5f * (tf.scale.x + size.x);
	state->transformation.m[0][1] = -tf.scale.y; state->transformation.m[1][1] = 0.f;    state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = tf.position.y;
	#endif
	state->transformation.m[0][2] = 0.f;         state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = tf.position.z;
	// Texture fragment.
	state->frame.x = frameBorder->subframe.x + offsets->x;
	state->frame.y = frameBorder->subframe.y + offsets->y;
	state->frame.w = flagsAreSet(window.flags, GUI_WINDOW_STRETCH_BORDER) ? frameBorder->subframe.w * offsets->w : inverseWidthY;
	state->frame.h = frameBorder->subframe.h * offsets->h;

	// Draw the top side.
	++offsets, ++state;
	// Border side thickness, in pixels.
	size.x = offsets->h * frameBorder->image->height;
	// Transformation matrix.
	// Consists of a scale, a rotation and a translation.
	#ifdef TRANSFORM_MATRIX_SHEAR
	state->transformation.m[0][0] = tf.scale.m[0][0]; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = tf.position.x;
	state->transformation.m[0][1] = 0.f;              state->transformation.m[1][1] = size.x; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = tf.position.y + 0.5f * (tf.scale.m[1][1] + size.x);
	#else
	state->transformation.m[0][0] = tf.scale.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = tf.position.x;
	state->transformation.m[0][1] = 0.f;        state->transformation.m[1][1] = size.x; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = tf.position.y + 0.5f * (tf.scale.y + size.x);
	#endif
	state->transformation.m[0][2] = 0.f;        state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = tf.position.z;
	// Texture fragment.
	state->frame.x = frameBorder->subframe.x + offsets->x;
	state->frame.y = frameBorder->subframe.y + offsets->y;
	state->frame.w = flagsAreSet(window.flags, GUI_WINDOW_STRETCH_BORDER) ? frameBorder->subframe.w * offsets->w : inverseWidthX;
	state->frame.h = frameBorder->subframe.h * offsets->h;

	// Draw the left side.
	++offsets, ++state;
	// Border side thickness, in pixels.
	size.x = offsets->h * frameBorder->image->height;
	// Transformation matrix.
	// Consists of a scale, a rotation and a translation.
	#ifdef TRANSFORM_MATRIX_SHEAR
	state->transformation.m[0][0] = 0.f;               state->transformation.m[1][0] = size.x; state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = tf.position.x - 0.5f * (tf.scale.m[0][0] + size.x);
	state->transformation.m[0][1] = -tf.scale.m[1][1]; state->transformation.m[1][1] = 0.f;    state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = tf.position.y;
	#else
	state->transformation.m[0][0] = 0.f;         state->transformation.m[1][0] = size.x; state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = tf.position.x - 0.5f * (tf.scale.x + size.x);
	state->transformation.m[0][1] = -tf.scale.y; state->transformation.m[1][1] = 0.f;    state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = tf.position.y;
	#endif
	state->transformation.m[0][2] = 0.f;         state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = tf.position.z;
	// Texture fragment.
	state->frame.x = frameBorder->subframe.x + offsets->x;
	state->frame.y = frameBorder->subframe.y + offsets->y;
	state->frame.w = flagsAreSet(window.flags, GUI_WINDOW_STRETCH_BORDER) ? frameBorder->subframe.w * offsets->w : inverseWidthY;
	state->frame.h = frameBorder->subframe.h * offsets->h;

	// Draw the bottom side.
	++offsets, ++state;
	// Border side thickness, in pixels.
	size.x = offsets->h * frameBorder->image->height;
	// Transformation matrix.
	// Consists of a scale, a rotation and a translation.
	#ifdef TRANSFORM_MATRIX_SHEAR
	state->transformation.m[0][0] = tf.scale.m[0][0]; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = tf.position.x;
	state->transformation.m[0][1] = 0.f;              state->transformation.m[1][1] = size.x; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = tf.position.y - 0.5f * (tf.scale.m[1][1] + size.x);
	#else
	state->transformation.m[0][0] = tf.scale.x; state->transformation.m[1][0] = 0.f;    state->transformation.m[2][0] = 0.f; state->transformation.m[3][0] = tf.position.x;
	state->transformation.m[0][1] = 0.f;        state->transformation.m[1][1] = size.x; state->transformation.m[2][1] = 0.f; state->transformation.m[3][1] = tf.position.y - 0.5f * (tf.scale.y + size.x);
	#endif
	state->transformation.m[0][2] = 0.f;        state->transformation.m[1][2] = 0.f;    state->transformation.m[2][2] = 1.f; state->transformation.m[3][2] = tf.position.z;
	// Texture fragment.
	state->frame.x = frameBorder->subframe.x + offsets->x;
	state->frame.y = frameBorder->subframe.y + offsets->y;
	state->frame.w = flagsAreSet(window.flags, GUI_WINDOW_STRETCH_BORDER) ? frameBorder->subframe.w * offsets->w : inverseWidthX;
	state->frame.h = frameBorder->subframe.h * offsets->h;

	// Upload the state data to the shader and render the model.
	glBindVertexArray(g_meshSprite.vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, g_sprStateBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 8*sizeof(spriteState), &gfxMngr->shdrData.spriteTransformState[0]);
	glDrawElementsInstanced(GL_TRIANGLES, g_meshSprite.indexNum, GL_UNSIGNED_INT, 0, 8);

	// Draw the body.
	state = &gfxMngr->shdrData.spriteTransformState[0];
	// Transformation matrix.
	state->transformation = tfMatrix3x4(tf);
	// Texture fragment.
	state->frame.x = frameBody->subframe.x;
	state->frame.y = frameBody->subframe.y;
	if(flagsAreSet(window.flags, GUI_WINDOW_STRETCH_BODY)){
		state->frame.w = frameBody->subframe.w;
		state->frame.h = frameBody->subframe.h;
	}else{
		#ifdef TRANSFORM_MATRIX_SHEAR
		state->frame.w = tf.scale.m[0][0]/frameBody->image->width;
		state->frame.h = tf.scale.m[1][1]/frameBody->image->height;
		#else
		state->frame.w = tf.scale.x/frameBody->image->width;
		state->frame.h = tf.scale.y/frameBody->image->height;
		#endif
	}
	// Bind the texture.
	gfxMngrBindTexture(gfxMngr, GL_TEXTURE0, frameBody->image->diffuseID);
	// Upload the state data to the shader and render the model.
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spriteState), &gfxMngr->shdrData.spriteTransformState[0]);
	glDrawElementsInstanced(GL_TRIANGLES, g_meshSprite.indexNum, GL_UNSIGNED_INT, 0, 1);

}
