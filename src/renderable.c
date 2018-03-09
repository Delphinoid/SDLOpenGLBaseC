#include "renderable.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include "vec4.h"

#define RADIAN_RATIO 0.017453292  // = PI / 180, used for converting degrees to radians

void rndrInit(renderable *rndr){
	rndr->mdl = NULL;
	twiInit(&rndr->twi, NULL);
	rndr->parentBoneLookup = NULL;
	rndr->physicsSimulate = 0;
	rndr->physicsState = NULL;
	//rndr->hitboxState = NULL;
}

/*signed char rndrCreate(renderable *rndr, model *mdl, textureWrapper *tw, const skl *oskl){

	size_t arraySizeS = mdl->skl->boneNum*sizeof(size_t);
	size_t arraySizeB = mdl->skl->boneNum*sizeof(bone);
	size_t arraySizeP = mdl->skl->boneNum*sizeof(prbInstance);
	//size_t arraySizeH = mdl->skl->boneNum*sizeof(hitbox *);

	* Allocate memory for various arrays. *
    rndr->skeletonState[0] = malloc(arraySizeB);
	if(rndr->skeletonState[0] == NULL){
		** Memory allocation failure. **
		return 0;
	}
	rndr->skeletonState[1] = malloc(arraySizeB);
	if(rndr->skeletonState[1] == NULL){
		** Memory allocation failure. **
		free(rndr->skeletonState[0]);
		return 0;
	}
	rndr->physicsState = malloc(arraySizeP);
	if(rndr->physicsState == NULL){
		** Memory allocation failure. **
		free(rndr->skeletonState[1]);
		free(rndr->skeletonState[0]);
		return 0;
	}

	** Create bone lookup with respect to oskl. **
	** Construct prbInstance array with constraints and collisions. **
	** Add references in physics island. **

	rndr->mdl = mdl;
	twiInit(&rndr->twi, tw);
	return 1;

}

signed char rndrCopy(renderable *o, renderable *c){

	if(o->skeletonState[0] == NULL){
		if(c->skeletonState[0] != NULL){
            free(c->skeletonState[0]);
		}
		c->skeletonState[0] = NULL;
	}else if( NULL){
	}

	c->mdl = o->mdl;
	c->twi = o->twi;

}*/

void rndrDelete(renderable *rndr){
	size_t i;
	if(rndr->parentBoneLookup != NULL){
		free(rndr->parentBoneLookup);
	}
	if(rndr->physicsState != NULL){
		for(i = 0; i < rndr->mdl->skl->boneNum; ++i){
			prbiDelete(&rndr->physicsState[i]);
		}
		free(rndr->physicsState);
	}
	/*if(rndr->hitboxState != NULL){
		for(i = 0; i < rndr->mdl->skl->boneNum; ++i){
			if(rndr->hitboxState[i] != NULL){
				free(rndr->hitboxState[i]);
			}
		}
		free(rndr->hitboxState);
	}*/
}



void rndrConfigInit(rndrConfig *rc){
	iVec3Init(&rc->position, 0.f, 0.f, 0.f);
	iQuatInit(&rc->orientation);
	iVec3Init(&rc->pivot, 0.f, 0.f, 0.f);
	iVec3Init(&rc->targetPosition, 0.f, 0.f, 0.f);
	iQuatInit(&rc->targetOrientation);
	iVec3Init(&rc->scale, 1.f, 1.f, 1.f);
	iFloatInit(&rc->alpha, 1.f);
	rc->sprite = 0;
	rc->flags = 0;
}

void rndrConfigStateCopy(rndrConfig *o, rndrConfig *c){
	c->position = o->position;
	c->orientation = o->orientation;
	c->pivot = o->pivot;
	c->targetPosition = o->targetPosition;
	c->targetOrientation = o->targetOrientation;
	c->scale = o->scale;
	c->alpha = o->alpha;
	c->sprite = o->sprite;
	c->flags = o->flags;
}

void rndrConfigResetInterpolation(rndrConfig *rc){
	iVec3ResetInterp(&rc->position);
	iQuatResetInterp(&rc->orientation);
	iVec3ResetInterp(&rc->pivot);
	iVec3ResetInterp(&rc->targetPosition);
	iQuatResetInterp(&rc->targetOrientation);
	iVec3ResetInterp(&rc->scale);
	iFloatResetInterp(&rc->alpha);
}

signed char rndrConfigRenderUpdate(rndrConfig *rc, const float interpT){

	// Return whether or not anything has changed.
	/** Remove alpha updates from here once rndrRenderMethod() is being used by everything. **/
	return iVec3Update(&rc->position,          interpT) |
	       iQuatUpdate(&rc->orientation,       interpT) |
	       iVec3Update(&rc->pivot,             interpT) |
	       iVec3Update(&rc->targetPosition,    interpT) |
	       iQuatUpdate(&rc->targetOrientation, interpT) |
	       iVec3Update(&rc->scale,             interpT) |
	       iFloatUpdate(&rc->alpha,            interpT);

}

/** Remove #include "camera.h" **/
void rndrConfigGenerateTransform(const rndrConfig *rc, const camera *cam, mat4 *transformMatrix){

	/*
	** Translate the model. By translating it from the camera coordinates to begin
	** with, we can save multiplying the model matrix by the view matrix later on
	*/
	*transformMatrix = cam->viewMatrix;  // Start with the view matrix
	mat4Translate(transformMatrix, rc->position.render.x, rc->position.render.y, rc->position.render.z);

	/* Billboarding */
	// If any of the flags apart from RNDR_BILLBOARD_TARGET are set, continue
	/*if((rc->flags & (RNDR_BILLBOARD_X | RNDR_BILLBOARD_Y | RNDR_BILLBOARD_Z)) > 0){
		mat4 billboardRotation;
		if((rc->flags & RNDR_BILLBOARD_SPRITE) > 0){
			* Sprites use a special, faster method for billboarding. *
			vec3 right, up, forward;
			// Use the camera's X, Y and Z axes for cheap sprite billboarding
			vec3Set(&right,   cam->viewMatrix.m[0][0], cam->viewMatrix.m[0][1], cam->viewMatrix.m[0][2]);
			vec3Set(&up,      cam->viewMatrix.m[1][0], cam->viewMatrix.m[1][1], cam->viewMatrix.m[1][2]);
			vec3Set(&forward, cam->viewMatrix.m[2][0], cam->viewMatrix.m[2][1], cam->viewMatrix.m[2][2]);
			// Lock certain axes if needed
			if((rc->flags & RNDR_BILLBOARD_X) == 0){
				right.y   = 0.f;
				up.y      = 1.f;
				forward.y = 0.f;
			}
			if((rc->flags & RNDR_BILLBOARD_Y) == 0){
				right.x   = 1.f;
				up.x      = 0.f;
				forward.x = 0.f;
			}
			if((rc->flags & RNDR_BILLBOARD_Z) == 0){
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
			if((rc->flags & RNDR_BILLBOARD_TARGET) > 0){
				eye = rc->targetPosition.render;
				target = rc->position.render;
				vec3Set(&up, 0.f, 1.f, 0.f);
				quatRotateVec3(&rc->targetOrientation.render, &up);
			}else if((rc->flags & RNDR_BILLBOARD_TARGET_CAMERA) > 0){
				eye = cam->position.render;
				target = rc->position.render;
				up = cam->up.render;
			}else{
				eye = cam->position.render;
				target = cam->targetPosition.render;
				up = cam->up.render;
			}
			// Lock certain axes if needed
			if((rc->flags & RNDR_BILLBOARD_X) == 0){
				target.y = eye.y;
			}
			if((rc->flags & RNDR_BILLBOARD_Y) == 0){
				target.x = eye.x;
			}
			if((rc->flags & RNDR_BILLBOARD_Z) == 0){
				vec3Set(&up, 0.f, 1.f, 0.f);
			}
			mat4RotateToFace(&billboardRotation, &eye, &target, &up);
		}
		mat4MultMByM2(&billboardRotation, transformMatrix);  // Apply billboard rotation
	}*/

	/* Rotate the model */
	mat4Rotate(transformMatrix, &rc->orientation.render);

	/* Scale the model */
	mat4Scale(transformMatrix, rc->scale.render.x, rc->scale.render.y, rc->scale.render.z);

	/*
	** Translate the model by -scaledPivot. The result is the appearance of the model
	** "pivoting" around position + scaledPivot
	*/
	mat4Translate(transformMatrix, -rc->pivot.render.x, -rc->pivot.render.y, -rc->pivot.render.z);

}

void rndrConfigGenerateSprite(const rndrConfig *rc, const twInstance *twi, vertex *vertices, const mat4 *transformMatrix){

	/* Undo the initial translations in rndrGenerateTransform() and use our own */
	/** Only way to remove this is to duplicate rndrGenerateTransform(). Is it worth it? **/
	/** Might copy rndrGenerateTransform() but without matrices **/
	const float left   = -rc->pivot.render.x * (twiGetFrameWidth(twi) - 1.f);
	const float top    = -rc->pivot.render.y * (twiGetFrameHeight(twi) - 1.f);
	const float right  = left + twiGetFrameWidth(twi);
	const float bottom = top  + twiGetFrameHeight(twi);
	const float z = -rc->pivot.render.z;

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

void rndrConfigOffsetSpriteTexture(vertex *vertices, const float texFrag[4], const float texWidth, const float texHeight){
	// We can't pass unique textureFragment values for each individual sprite when batching. Therefore,
	// we have to do the offset calculations for each vertex UV here instead of in the shader
	size_t i;
	for(i = 0; i < 4; ++i){
		vertices[i].u = ((vertices[i].u * texFrag[2]) + texFrag[0]) / texWidth;
		vertices[i].v = ((vertices[i].v * texFrag[3]) + texFrag[1]) / texHeight;
	}
}
