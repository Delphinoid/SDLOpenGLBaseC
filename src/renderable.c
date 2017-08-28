#include "renderable.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include "vec4.h"
#include "mat4.h"

#define RADIAN_RATIO 0.017453292  // = PI / 180, used for converting degrees to radians

void rndrInit(renderable *rndr){
	rndr->name = NULL;
	rndr->mdl = NULL;
	skliInit(&rndr->skli, NULL);
	twiInit(&rndr->twi, NULL);
	stInit(&rndr->sTrans);
	rtInit(&rndr->rTrans);
	rndr->sprite = 0;
	rndr->flags = 0;
	rndr->hudElement = 0;
}

/** Finish this **/
unsigned char rndrLoad(renderable *rndr, const char *prgPath, const char *filePath, cVector *allModels, cVector *allTexWrappers){

	rndrInit(rndr);
	return 1;

}

unsigned char rndrRenderMethod(renderable *rndr){
	if(rndr->rTrans.alpha > 0.f){
		if(rndr->rTrans.alpha < 1.f || twiContainsTranslucency(&rndr->twi)){
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

void rndrSetRotation(renderable *rndr, const float newX, const float newY, const float newZ){
	quatSetEuler(&rndr->sTrans.orientation, newX*RADIAN_RATIO, newY*RADIAN_RATIO, newZ*RADIAN_RATIO);
	vec3SetS(&rndr->sTrans.changeRot, 0.f);
}

void rndrRotateX(renderable *rndr, const float changeX){
	rndr->sTrans.changeRot.x += changeX;
}

void rndrRotateY(renderable *rndr, const float changeY){
	rndr->sTrans.changeRot.y += changeY;
}

void rndrRotateZ(renderable *rndr, const float changeZ){
	rndr->sTrans.changeRot.z += changeZ;
}

void rndrAnimateTexture(renderable *rndr, const uint32_t currentTick, const float globalDelayMod){
	twiAnimate(&rndr->twi, currentTick, globalDelayMod);
}

void rndrAnimateSkeleton(renderable *rndr, const uint32_t currentTick, const float globalDelayMod){
	skliAnimate(&rndr->skli, currentTick, globalDelayMod);
}

void rndrGenerateTransform(renderable *rndr, const camera *cam, mat4 *transformMatrix){

	/*
	** Translate the model. By translating it from the camera coordinates to begin
	** with, we can save multiplying the model matrix by the view matrix later on
	*/
	*transformMatrix = cam->viewMatrix;  // Start with the view matrix
	mat4Translate(transformMatrix, rndr->sTrans.position.x, rndr->sTrans.position.y, rndr->sTrans.position.z);

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
				eye = rndr->rTrans.targetPosition;
				target = rndr->sTrans.position;
				vec3Set(&up, 0.f, 1.f, 0.f);
				quatRotateVec3(rndr->rTrans.targetOrientation, &up);
			}else if((rndr->flags & RNDR_BILLBOARD_TARGET_CAMERA) > 0){
				eye = cam->position;
				target = rndr->sTrans.position;
				up = cam->up;
			}else{
				eye = cam->position;
				target = cam->target;
				up = cam->up;
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
			mat4RotateToFace(&billboardRotation, eye, target, up);
		}
		mat4MultMByM2(&billboardRotation, transformMatrix);  // Apply billboard rotation
	}

	/* Rotate the model */
	// Apply the change in rotation to the current orientation
	quatMultQByQ2(quatNewEuler(rndr->sTrans.changeRot.x*RADIAN_RATIO,
	                           rndr->sTrans.changeRot.y*RADIAN_RATIO,
	                           rndr->sTrans.changeRot.z*RADIAN_RATIO),
	              &rndr->sTrans.orientation);
	vec3SetS(&rndr->sTrans.changeRot, 0.f);  // Reset the change in rotation
	mat4Rotate(transformMatrix, rndr->sTrans.orientation);

	/* Scale the model */
	mat4Scale(transformMatrix, rndr->rTrans.scale.x, rndr->rTrans.scale.y, rndr->rTrans.scale.z);

	/*
	** Translate the model by -scaledPivot. The result is the appearance of the model
	** "pivoting" around position + scaledPivot
	*/
	mat4Translate(transformMatrix, -rndr->sTrans.relPivot.x, -rndr->sTrans.relPivot.y, -rndr->sTrans.relPivot.z);

}

void rndrGenerateSprite(const renderable *rndr, vertex *vertices, const mat4 *transformMatrix){

	/* Generate the base sprite quad */
	vertex tempVert;

	/* Undo the initial translations in rndrGenerateTransform() and use our own */
	/** Only way to remove this is to duplicate rndrGenerateTransform(). Is it worth it? **/
	/** Might copy rndrGenerateTransform() but without matrices **/
	const float left   = -rndr->sTrans.relPivot.x * (twiGetFrameWidth(&rndr->twi) - 1.f);
	const float top    = -rndr->sTrans.relPivot.y * (twiGetFrameHeight(&rndr->twi) - 1.f);
	const float right  = left + twiGetFrameWidth(&rndr->twi);
	const float bottom = top  + twiGetFrameHeight(&rndr->twi);
	const float z      = -rndr->sTrans.relPivot.z;

	// Create the top left vertex
	tempVert.pos.x = left;
	tempVert.pos.y = top;
	tempVert.pos.z = z;
	tempVert.u = 0.f;
	tempVert.v = 0.f;
	tempVert.nx = 0.f;
	tempVert.ny = 0.f;
	tempVert.nz = 0.f;
	tempVert.bIDs[0] = -1;
	tempVert.bIDs[1] = -1;
	tempVert.bIDs[2] = -1;
	tempVert.bIDs[3] = -1;
	tempVert.bWeights[0] = 0.f;
	tempVert.bWeights[1] = 0.f;
	tempVert.bWeights[2] = 0.f;
	tempVert.bWeights[3] = 0.f;
	vertices[0] = tempVert;

	// Create the top right vertex
	tempVert.pos.x = right;
	tempVert.pos.y = top;
	tempVert.pos.z = z;
	tempVert.u = 1.f;
	tempVert.v = 0.f;
	tempVert.nx = 0.f;
	tempVert.ny = 0.f;
	tempVert.nz = 0.f;
	tempVert.bIDs[0] = -1;
	tempVert.bIDs[1] = -1;
	tempVert.bIDs[2] = -1;
	tempVert.bIDs[3] = -1;
	tempVert.bWeights[0] = 0.f;
	tempVert.bWeights[1] = 0.f;
	tempVert.bWeights[2] = 0.f;
	tempVert.bWeights[3] = 0.f;
	vertices[1] = tempVert;

	// Create the bottom left vertex
	tempVert.pos.x = left;
	tempVert.pos.y = bottom;
	tempVert.pos.z = z;
	tempVert.u = 0.f;
	tempVert.v = -1.f;  // Flip the y dimension so the image isn't upside down
	tempVert.nx = 0.f;
	tempVert.ny = 0.f;
	tempVert.nz = 0.f;
	tempVert.bIDs[0] = -1;
	tempVert.bIDs[1] = -1;
	tempVert.bIDs[2] = -1;
	tempVert.bIDs[3] = -1;
	tempVert.bWeights[0] = 0.f;
	tempVert.bWeights[1] = 0.f;
	tempVert.bWeights[2] = 0.f;
	tempVert.bWeights[3] = 0.f;
	vertices[2] = tempVert;

	// Create the bottom right vertex
	tempVert.pos.x = right;
	tempVert.pos.y = bottom;
	tempVert.pos.z = z;
	tempVert.u = 1.f;
	tempVert.v = -1.f;  // Flip the y dimension so the image isn't upside down
	tempVert.nx = 0.f;
	tempVert.ny = 0.f;
	tempVert.nz = 0.f;
	tempVert.bIDs[0] = -1;
	tempVert.bIDs[1] = -1;
	tempVert.bIDs[2] = -1;
	tempVert.bIDs[3] = -1;
	tempVert.bWeights[0] = 0.f;
	tempVert.bWeights[1] = 0.f;
	tempVert.bWeights[2] = 0.f;
	tempVert.bWeights[3] = 0.f;
	vertices[3] = tempVert;

	/* Apply transformations to each vertex */
	vec4 vertexPos;
	size_t i;
	for(i = 0; i < 4; ++i){
		// We need to make the vertex positions a vec4 so we can multiply them by the 4x4 modelViewProjectionMatrix
		vec4Set(&vertexPos, vertices[i].pos.x, vertices[i].pos.y, vertices[i].pos.z, 1.f);
		mat4MultMByV(transformMatrix, &vertexPos);
		vec3Set(&vertices[i].pos, vertexPos.x, vertexPos.y, vertexPos.z);
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

void rndrDelete(renderable *rndr){
	if(rndr->name != NULL){
		free(rndr->name);
	}
	skliDelete(&rndr->skli);
}
