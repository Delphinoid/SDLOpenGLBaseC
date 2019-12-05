#include "renderable.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include "vec4.h"
#include "inline.h"

void rndrBaseInit(renderableBase *const restrict rndr){
	rndr->mdl = NULL;
	rndr->tw = NULL;
}

void rndrStateInit(rndrState *const restrict state){
	state->flags = BILLBOARD_DISABLED;
	state->alpha = 1.f;
	state->alphaCurrent = 1.f;
	state->alphaPrevious = 1.f;
	state->axis = NULL;
	state->target = NULL;
	state->scale = 1.f/BILLBOARD_SCALE_CALIBRATION_DISTANCE;
}

void rndrInit(renderable *const restrict rndr){
	rndr->mdl = NULL;
	twiInit(&rndr->twi, NULL);
	rndrStateInit(&rndr->state);
}

void rndrInstantiate(renderable *const restrict rndr, const renderableBase *const base){
	rndr->mdl = base->mdl;
	twiInit(&rndr->twi, base->tw);
	rndrStateInit(&rndr->state);
}

__FORCE_INLINE__ void rndrUpdate(renderable *const restrict rndr, const float elapsedTime){
	twiAnimate(&rndr->twi, elapsedTime);
	rndr->state.alphaPrevious = rndr->state.alphaCurrent;
	rndr->state.alphaCurrent = rndr->state.alpha;
}

mat4 rndrStateBillboard(const rndrState state, const camera *const restrict cam, const vec3 centroid, mat4 configuration){

	// Generates a billboard transformation matrix.
	// If no flags are set, returns the identity matrix.

	// Use a faster method for billboarding. Doesn't support individual axis locking.
	if(flagsAreSet(state.flags, BILLBOARD_SPRITE)){

		// Use the camera's X, Y and Z axes for cheap sprite billboarding.
		// This is technically an inverse matrix, so we need rows rather than columns.
		*((vec3 *)&configuration.m[0][0]) = vec3New(cam->viewMatrix.m[0][0], cam->viewMatrix.m[1][0], cam->viewMatrix.m[2][0]);

		if(state.axis != NULL){
			*((vec3 *)&configuration.m[1][0]) = *state.axis;
		}else{
			*((vec3 *)&configuration.m[1][0]) = vec3New(cam->viewMatrix.m[0][1], cam->viewMatrix.m[1][1], cam->viewMatrix.m[2][1]);
		}

		*((vec3 *)&configuration.m[2][0]) = vec3New(cam->viewMatrix.m[0][2], cam->viewMatrix.m[1][2], cam->viewMatrix.m[2][2]);

		configuration.m[0][3] = 0.f; configuration.m[1][3] = 0.f; configuration.m[2][3] = 0.f; configuration.m[3][3] = 1.f;

		if(flagsAreSet(state.flags, BILLBOARD_SCALE)){
			const float distance = camDistance(cam, centroid) * state.scale;
			configuration = mat4Scale(configuration, distance, distance, distance);
		}

	}else if(flagsAreSet(state.flags, BILLBOARD_LOCK_XYZ)){

		vec3 eye, target, up;

		// Set the up vector. This corresponds
		// to the billboard rotation axis.
		if(state.axis != NULL){
			up = *state.axis;
		}else{
			up = cam->up.render;
		}

		// Set the eye and the target.
		if(flagsAreSet(state.flags, BILLBOARD_TARGET_SPRITE)){
			eye = cam->target.render;
			target = cam->position.render;
		}else if(state.target != NULL){
			eye = centroid;
			target = *state.target;
		}else{
			eye = centroid;
			target = cam->position.render;
		}

		// Lock certain axes if needed.
		if(flagsAreUnset(state.flags, BILLBOARD_LOCK_X)){
			target.y = eye.y;
		}
		if(flagsAreUnset(state.flags, BILLBOARD_LOCK_Y)){
			target.x = eye.x;
		}
		if(flagsAreUnset(state.flags, BILLBOARD_LOCK_Z)){
			up = vec3New(0.f, 1.f, 0.f);
		}

		// Translate the transformation to the "origin".
		configuration = mat4TranslatePost(configuration, -centroid.x, -centroid.y, -centroid.z);

		// Scale based on distance if necessary.
		if(flagsAreSet(state.flags, BILLBOARD_SCALE)){
			const float distance = camDistance(cam, centroid) * state.scale;
			configuration = mat4ScalePost(configuration, distance, distance, distance);
		}

		// Rotate to face and translate back.
		configuration = mat4TranslatePost(
			mat4MMultM(
				mat4RotateToFace(eye, target, up),
				configuration
			),
			centroid.x, centroid.y, centroid.z
		);

	}else{

		if(flagsAreSet(state.flags, BILLBOARD_SCALE)){
			const float distance = camDistance(cam, centroid) * state.scale;
			configuration = mat4Scale(configuration, distance, distance, distance);
		}

	}

	return configuration;

}