#include "camera.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include "vec4.h"
#include "mat4.h"

#define RADIAN_RATIO 0.017453292  // = PI / 180, used for converting degrees to radians

unsigned char rndrInit(void *rndr){
	return skliInit(&((renderable *)rndr)->skli, NULL, 0);
}

unsigned char rndrNew(void *rndr){
	((renderable *)rndr)->name = NULL;
	((renderable *)rndr)->mdl = NULL;
	iVec3Init(&((renderable *)rndr)->position, 0.f, 0.f, 0.f);
	iQuatInit(&((renderable *)rndr)->orientation);
	//vec3Set(&((renderable *)rndr)->rotation, 0.f, 0.f, 0.f);
	iVec3Init(&((renderable *)rndr)->pivot, 0.f, 0.f, 0.f);
	iVec3Init(&((renderable *)rndr)->targetPosition, 0.f, 0.f, 0.f);
	iQuatInit(&((renderable *)rndr)->targetOrientation);
	iVec3Init(&((renderable *)rndr)->scale, 1.f, 1.f, 1.f);
	iFloatInit(&((renderable *)rndr)->alpha, 1.f);
	((renderable *)rndr)->sprite = 0;
	((renderable *)rndr)->flags = 0;
	twiInit(&((renderable *)rndr)->twi, NULL);
	return rndrInit(rndr);
}

unsigned char rndrStateCopy(const void *o, void *c){
	((renderable *)c)->name = ((renderable *)o)->name;
	((renderable *)c)->mdl = ((renderable *)o)->mdl;
	((renderable *)c)->twi = ((renderable *)o)->twi;
	((renderable *)c)->position = ((renderable *)o)->position;
	((renderable *)c)->orientation = ((renderable *)o)->orientation;
	((renderable *)c)->pivot = ((renderable *)o)->pivot;
	((renderable *)c)->targetPosition = ((renderable *)o)->targetPosition;
	((renderable *)c)->targetOrientation = ((renderable *)o)->targetOrientation;
	((renderable *)c)->scale = ((renderable *)o)->scale;
	((renderable *)c)->alpha = ((renderable *)o)->alpha;
	((renderable *)c)->sprite = ((renderable *)o)->sprite;
	((renderable *)c)->flags = ((renderable *)o)->flags;
	return skliStateCopy(&((renderable *)o)->skli, &((renderable *)c)->skli);
}

void rndrResetInterpolation(void *rndr){
	iVec3ResetInterp(&((renderable *)rndr)->position);
	iQuatResetInterp(&((renderable *)rndr)->orientation);
	iVec3ResetInterp(&((renderable *)rndr)->pivot);
	iVec3ResetInterp(&((renderable *)rndr)->targetPosition);
	iQuatResetInterp(&((renderable *)rndr)->targetOrientation);
	iVec3ResetInterp(&((renderable *)rndr)->scale);
	iFloatResetInterp(&((renderable *)rndr)->alpha);
}

/** Finish this **/
unsigned char rndrLoad(renderable *rndr, const char *prgPath, const char *filePath, cVector *allModels, cVector *allTexWrappers){

	return 1;

}

/**void rndrSetRotation(renderable *rndr, const float pitch, const float yaw, const float roll){
	quatSetEuler(&rndr->orientation.value, pitch*RADIAN_RATIO, yaw*RADIAN_RATIO, roll*RADIAN_RATIO);
	vec3SetS(&rndr->rotation, 0.f);
}

void rndrRotateX(renderable *rndr, const float changeX){
	iVec3GetValue(&rndr->rotation)->x += changeX;
}

void rndrRotateY(renderable *rndr, const float changeY){
	iVec3GetValue(&rndr->rotation)->y += changeY;
}

void rndrRotateZ(renderable *rndr, const float changeZ){
	iVec3GetValue(&rndr->rotation)->z += changeZ;
}**/

unsigned char rndrRenderMethod(renderable *rndr, const float interpT){
	// Update alpha.
	iFloatUpdate(&rndr->alpha, interpT);
	if(rndr->alpha.render > 0.f){
		if(rndr->alpha.render < 1.f || twiContainsTranslucency(&rndr->twi)){
			// The model contains translucency
			return 1;
		}else{
			// The model is fully opaque
			return 0;
		}
	}
	// The model is fully transparent
	return 2;
}
unsigned char rndrRenderUpdate(renderable *rndr, const float interpT){

	// Apply the change in rotation to the current orientation.
	/*if(rndr->rotation.x != 0.f || rndr->rotation.y != 0.f || rndr->rotation.z != 0.f){
		quat changeRotation;
		quatSetEuler(&changeRotation, rndr->rotation.x*RADIAN_RATIO,
		                              rndr->rotation.y*RADIAN_RATIO,
		                              rndr->rotation.z*RADIAN_RATIO);
		quatMultQByQ2(&changeRotation, &rndr->orientation.value);
		vec3SetS(&rndr->rotation, 0.f);
	}*/

	// Return whether or not anything has changed.
	/** Remove alpha updates from here once rndrRenderMethod() is being used by everything. **/
	return iVec3Update(&rndr->position,          interpT) |
	       iQuatUpdate(&rndr->orientation,       interpT) |
	       iVec3Update(&rndr->pivot,             interpT) |
	       iVec3Update(&rndr->targetPosition,    interpT) |
	       iQuatUpdate(&rndr->targetOrientation, interpT) |
	       iVec3Update(&rndr->scale,             interpT) |
	       iFloatUpdate(&rndr->alpha,            interpT);

}

void rndrAnimateTexture(renderable *rndr, const float elapsedTime){
	twiAnimate(&rndr->twi, elapsedTime);
}

void rndrAnimateSkeleton(renderable *rndr, const float elapsedTime){
	skliAnimate(&rndr->skli, elapsedTime);
}

/** Remove #include "camera.h" **/
void rndrGenerateTransform(const renderable *rndr, const camera *cam, mat4 *transformMatrix){

	/*
	** Translate the model. By translating it from the camera coordinates to begin
	** with, we can save multiplying the model matrix by the view matrix later on
	*/
	*transformMatrix = cam->viewMatrix;  // Start with the view matrix
	mat4Translate(transformMatrix, rndr->position.render.x, rndr->position.render.y, rndr->position.render.z);

	/* Billboarding */
	// If any of the flags apart from RNDR_BILLBOARD_TARGET are set, continue
	if((rndr->flags & (RNDR_BILLBOARD_X | RNDR_BILLBOARD_Y | RNDR_BILLBOARD_Z)) > 0){
		mat4 billboardRotation;
		if((rndr->flags & RNDR_BILLBOARD_SPRITE) > 0){
			/* Sprites use a special, faster method for billboarding. */
			vec3 right, up, forward;
			// Use the camera's X, Y and Z axes for cheap sprite billboarding
			vec3Set(&right,   cam->viewMatrix.m[0][0], cam->viewMatrix.m[0][1], cam->viewMatrix.m[0][2]);
			vec3Set(&up,      cam->viewMatrix.m[1][0], cam->viewMatrix.m[1][1], cam->viewMatrix.m[1][2]);
			vec3Set(&forward, cam->viewMatrix.m[2][0], cam->viewMatrix.m[2][1], cam->viewMatrix.m[2][2]);
			// Lock certain axes if needed
			if((rndr->flags & RNDR_BILLBOARD_X) == 0){
				right.y   = 0.f;
				up.y      = 1.f;
				forward.y = 0.f;
			}
			if((rndr->flags & RNDR_BILLBOARD_Y) == 0){
				right.x   = 1.f;
				up.x      = 0.f;
				forward.x = 0.f;
			}
			if((rndr->flags & RNDR_BILLBOARD_Z) == 0){
				right.z   = 0.f;
				up.z      = 0.f;
				forward.z = 1.f;
			}
			billboardRotation.m[0][0] = right.x; billboardRotation.m[0][1] = up.x; billboardRotation.m[0][2] = forward.x; billboardRotation.m[0][3] = 0.f;
			billboardRotation.m[1][0] = right.y; billboardRotation.m[1][1] = up.y; billboardRotation.m[1][2] = forward.y; billboardRotation.m[1][3] = 0.f;
			billboardRotation.m[2][0] = right.z; billboardRotation.m[2][1] = up.z; billboardRotation.m[2][2] = forward.z; billboardRotation.m[2][3] = 0.f;
			billboardRotation.m[3][0] = 0.f;     billboardRotation.m[3][1] = 0.f;  billboardRotation.m[3][2] = 0.f;       billboardRotation.m[3][3] = 1.f;
		}else{
			vec3 eye, target, up;
			if((rndr->flags & RNDR_BILLBOARD_TARGET) > 0){
				eye = rndr->targetPosition.render;
				target = rndr->position.render;
				vec3Set(&up, 0.f, 1.f, 0.f);
				quatRotateVec3(&rndr->targetOrientation.render, &up);
			}else if((rndr->flags & RNDR_BILLBOARD_TARGET_CAMERA) > 0){
				eye = cam->position.render;
				target = rndr->position.render;
				up = cam->up.render;
			}else{
				eye = cam->position.render;
				target = cam->targetPosition.render;
				up = cam->up.render;
			}
			// Lock certain axes if needed
			if((rndr->flags & RNDR_BILLBOARD_X) == 0){
				target.y = eye.y;
			}
			if((rndr->flags & RNDR_BILLBOARD_Y) == 0){
				target.x = eye.x;
			}
			if((rndr->flags & RNDR_BILLBOARD_Z) == 0){
				vec3Set(&up, 0.f, 1.f, 0.f);
			}
			mat4RotateToFace(&billboardRotation, &eye, &target, &up);
		}
		mat4MultMByM2(&billboardRotation, transformMatrix);  // Apply billboard rotation
	}

	/* Rotate the model */
	mat4Rotate(transformMatrix, &rndr->orientation.render);

	/* Scale the model */
	mat4Scale(transformMatrix, rndr->scale.render.x, rndr->scale.render.y, rndr->scale.render.z);

	/*
	** Translate the model by -scaledPivot. The result is the appearance of the model
	** "pivoting" around position + scaledPivot
	*/
	mat4Translate(transformMatrix, -rndr->pivot.render.x, -rndr->pivot.render.y, -rndr->pivot.render.z);

}

void rndrGenerateSprite(const renderable *rndr, vertex *vertices, const mat4 *transformMatrix){

	/* Undo the initial translations in rndrGenerateTransform() and use our own */
	/** Only way to remove this is to duplicate rndrGenerateTransform(). Is it worth it? **/
	/** Might copy rndrGenerateTransform() but without matrices **/
	const float left   = -rndr->pivot.render.x * (twiGetFrameWidth(&rndr->twi) - 1.f);
	const float top    = -rndr->pivot.render.y * (twiGetFrameHeight(&rndr->twi) - 1.f);
	const float right  = left + twiGetFrameWidth(&rndr->twi);
	const float bottom = top  + twiGetFrameHeight(&rndr->twi);
	const float z      = -rndr->pivot.render.z;

	// Create the top left vertex
	vertices[0].position.x = left;
	vertices[0].position.y = top;
	vertices[0].position.z = z;
	vertices[0].u = 0.f;
	vertices[0].v = 0.f;
	vertices[0].normal.x = 0.f;
	vertices[0].normal.y = 0.f;
	vertices[0].normal.z = 0.f;
	vertices[0].bIDs[0] = -1;
	vertices[0].bIDs[1] = -1;
	vertices[0].bIDs[2] = -1;
	vertices[0].bIDs[3] = -1;
	vertices[0].bWeights[0] = 0.f;
	vertices[0].bWeights[1] = 0.f;
	vertices[0].bWeights[2] = 0.f;
	vertices[0].bWeights[3] = 0.f;

	// Create the top right vertex
	vertices[1].position.x = right;
	vertices[1].position.y = top;
	vertices[1].position.z = z;
	vertices[1].u = 1.f;
	vertices[1].v = 0.f;
	vertices[1].normal.x = 0.f;
	vertices[1].normal.y = 0.f;
	vertices[1].normal.z = 0.f;
	vertices[1].bIDs[0] = -1;
	vertices[1].bIDs[1] = -1;
	vertices[1].bIDs[2] = -1;
	vertices[1].bIDs[3] = -1;
	vertices[1].bWeights[0] = 0.f;
	vertices[1].bWeights[1] = 0.f;
	vertices[1].bWeights[2] = 0.f;
	vertices[1].bWeights[3] = 0.f;

	// Create the bottom left vertex
	vertices[2].position.x = left;
	vertices[2].position.y = bottom;
	vertices[2].position.z = z;
	vertices[2].u = 0.f;
	vertices[2].v = -1.f;  // Flip the y dimension so the image isn't upside down
	vertices[2].normal.x = 0.f;
	vertices[2].normal.y = 0.f;
	vertices[2].normal.z = 0.f;
	vertices[2].bIDs[0] = -1;
	vertices[2].bIDs[1] = -1;
	vertices[2].bIDs[2] = -1;
	vertices[2].bIDs[3] = -1;
	vertices[2].bWeights[0] = 0.f;
	vertices[2].bWeights[1] = 0.f;
	vertices[2].bWeights[2] = 0.f;
	vertices[2].bWeights[3] = 0.f;

	// Create the bottom right vertex
	vertices[3].position.x = right;
	vertices[3].position.y = bottom;
	vertices[3].position.z = z;
	vertices[3].u = 1.f;
	vertices[3].v = -1.f;  // Flip the y dimension so the image isn't upside down
	vertices[3].normal.x = 0.f;
	vertices[3].normal.y = 0.f;
	vertices[3].normal.z = 0.f;
	vertices[3].bIDs[0] = -1;
	vertices[3].bIDs[1] = -1;
	vertices[3].bIDs[2] = -1;
	vertices[3].bIDs[3] = -1;
	vertices[3].bWeights[0] = 0.f;
	vertices[3].bWeights[1] = 0.f;
	vertices[3].bWeights[2] = 0.f;
	vertices[3].bWeights[3] = 0.f;

	/* Apply transformations to each vertex */
	vec4 vertexPos;
	size_t i;
	for(i = 0; i < 4; ++i){
		// We need to make the vertex positions a vec4 so we can multiply them by the 4x4 modelViewProjectionMatrix
		vec4Set(&vertexPos, vertices[i].position.x, vertices[i].position.y, vertices[i].position.z, 1.f);
		mat4MultMByV(transformMatrix, &vertexPos);
		vec3Set(&vertices[i].position, vertexPos.x, vertexPos.y, vertexPos.z);
	}

}

void rndrOffsetSpriteTexture(vertex *vertices, const float texFrag[4], const float texWidth, const float texHeight){
	// We can't pass unique textureFragment values for each individual sprite when batching. Therefore,
	// we have to do the offset calculations for each vertex UV here instead of in the shader
	size_t i;
	for(i = 0; i < 4; ++i){
		vertices[i].u = ((vertices[i].u * texFrag[2]) + texFrag[0]) / texWidth;
		vertices[i].v = ((vertices[i].v * texFrag[3]) + texFrag[1]) / texHeight;
	}
}

void rndrDelete(void *rndr){
	if(((renderable *)rndr)->name != NULL){
		free(((renderable *)rndr)->name);
	}
	skliDelete(&((renderable *)rndr)->skli);
}
