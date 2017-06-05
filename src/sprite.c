#include "sprite.h"
#include <math.h>
#include <SDL2/SDL.h>
#include "mat4.h"
#include "vec4.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define radianRatio 0.017453292  // = PI / 180, used for converting degrees to radians

unsigned char sprLoad(sprite *spr, const char *prgPath, const char *filePath, cVector *allTexWrappers){

	spr->width = 1.f;
	spr->height = 1.f;
	vec3SetS(&spr->position, 0.f);
	vec3SetS(&spr->relPivot, 0.f);
	quatSet(&spr->orientation, 1.0f, 0.f, 0.f, 0.f);
	vec3SetS(&spr->target, 0.f);
	vec3SetS(&spr->changeRot, 0.f);
	vec3SetS(&spr->scale, 1.f);
	spr->alpha = 1.f;
	spr->billboardX = 0;
	spr->billboardY = 0;
	spr->billboardZ = 0;
	spr->simpleBillboard = 0;
	spr->hudElement = 0;
	spr->hudScaleMode = 0;
	spr->texture = (textureWrapper *)cvGet(allTexWrappers, 0);
	spr->currentAnim = 0;
	spr->currentFrame = 0;
	spr->timesLooped = 0;
	spr->frameProgress = 0.f;

	return 1;

}

void sprHudElement(sprite *spr, unsigned char isHudElement){
	if(isHudElement != spr->hudElement){
		// HUD elements render rotated 180 degrees on the X axis due to the way the orthographic matrix is set up
		quatMultQByQ2(quatNewEuler(vec3New(M_PI, 0.f, 0.f)), &spr->orientation);
	}
	spr->hudElement = isHudElement;
	if(!spr->hudElement){
		spr->hudScaleMode = 0;
	}
}

void sprSetRotation(sprite *spr, float newX, float newY, float newZ){
	// HUD elements render rotated 180 degrees on the X axis due to the way the orthographic matrix is set up
	float offsetX = spr->hudElement ? M_PI : 0.f;
	spr->orientation = quatNewEuler(vec3New(newX * radianRatio + offsetX, newY * radianRatio, newZ * radianRatio));
	spr->changeRot = vec3NewS(0.f);
}

void sprRotateX(sprite *spr, float changeX){
	spr->changeRot.x += changeX;
}

void sprRotateY(sprite *spr, float changeY){
	spr->changeRot.y += changeY;
}

void sprRotateZ(sprite *spr, float changeZ){
	spr->changeRot.z += changeZ;
}

void sprAnimateTex(sprite *spr){

	if(spr->texture != NULL){
		if(spr->frameProgress == 0){
			spr->frameProgress = SDL_GetTicks();
		}
		twAnimate(spr->texture, 1.f, &spr->currentAnim, &spr->currentFrame, &spr->frameProgress, &spr->timesLooped);
	}

}

void sprCreate(sprite *spr, gfxProgram *gfxPrg, camera *cam){

	/* Set temporary position and scale vectors based on the selected HUD scaling mode */
	vec3 windowPos = spr->position;
	vec3 windowScale = spr->scale;
	if(spr->hudElement){
		if(spr->hudScaleMode == 1 || spr->hudScaleMode == 3){
			windowPos = vec3New(spr->position.x * (float)gfxPrg->windowWidth / (float)gfxPrg->biggestDimension,
			                    spr->position.y * (float)gfxPrg->windowHeight / (float)gfxPrg->biggestDimension,
			                    spr->position.z);
		}
		if(spr->hudScaleMode == 2 || spr->hudScaleMode == 3){
			windowScale = vec3New(spr->scale.x * (float)gfxPrg->windowWidth / (float)gfxPrg->biggestDimension,
			                      spr->scale.y * (float)gfxPrg->windowHeight / (float)gfxPrg->biggestDimension,
			                      spr->scale.z);
		}
	}

	vertex3D tempVert;
	float scaledWidth  = spr->width  * windowScale.x;
	float scaledHeight = spr->height * windowScale.y;
	vec3 scaledPivot = vec3New(spr->relPivot.x * windowScale.x, scaledHeight - spr->relPivot.y * windowScale.y, spr->relPivot.z * windowScale.z);

	// Create the top left vertex
	tempVert.pos.x = 0.f;
	tempVert.pos.y = 0.f;
	tempVert.pos.z = 0.f;
	tempVert.u = 0.f;
	tempVert.v = 0.f;
	spr->vertices[0] = tempVert;

	// Create the top right vertex
	tempVert.pos.x = scaledWidth;
	tempVert.pos.y = 0.f;
	tempVert.pos.z = 0.f;
	tempVert.u = 1.f;
	tempVert.v = 0.f;
	spr->vertices[1] = tempVert;

	// Create the bottom left vertex
	tempVert.pos.x = 0.f;
	tempVert.pos.y = scaledHeight;
	tempVert.pos.z = 0.f;
	tempVert.u = 0.f;
	tempVert.v = -1.f;  // Flip the y dimension so the image isn't upside down
	spr->vertices[2] = tempVert;

	// Create the bottom right vertex
	tempVert.pos.x = scaledWidth;
	tempVert.pos.y = scaledHeight;
	tempVert.pos.z = 0.f;
	tempVert.u = 1.f;
	tempVert.v = -1.f;  // Flip the y dimension so the image isn't upside down
	spr->vertices[3] = tempVert;


	/*
	** Translate the model. By translating it from the camera coordinates to begin
	** with, we can save multiplying the model matrix by the view matrix later on.
	** However, we must start with the identity matrix for HUD elements
	*/
	mat4 modelViewMatrix;
	if(spr->hudElement){
		modelViewMatrix = gfxPrg->identityMatrix;  // Start with the identity matrix
	}else{
		modelViewMatrix = gfxPrg->viewMatrix;      // Start with the view matrix
	}
	mat4Translate(&modelViewMatrix, windowPos.x, windowPos.y, windowPos.z);


	/* Billboarding */
	if(spr->billboardX || spr->billboardY || spr->billboardZ){
		vec3 axisX; vec3 axisY; vec3 axisZ;
		if(spr->simpleBillboard){
			// Use the camera's X, Y and Z axes
			axisX = vec3New(gfxPrg->viewMatrix.m[0][0], gfxPrg->viewMatrix.m[0][1], gfxPrg->viewMatrix.m[0][2]);
			axisY = vec3New(gfxPrg->viewMatrix.m[1][0], gfxPrg->viewMatrix.m[1][1], gfxPrg->viewMatrix.m[1][2]);
			axisZ = vec3New(gfxPrg->viewMatrix.m[2][0], gfxPrg->viewMatrix.m[2][1], gfxPrg->viewMatrix.m[2][2]);
		}else{
			// Generate a new view matrix for the billboard
			mat4 billboardViewMatrix;
			/** Merge model, sprite and cam **/
			mat4LookAt(&billboardViewMatrix, spr->target, spr->position, cam->up);
			axisX = vec3New(billboardViewMatrix.m[0][0], billboardViewMatrix.m[0][1], billboardViewMatrix.m[0][2]);
			axisY = vec3New(billboardViewMatrix.m[1][0], billboardViewMatrix.m[1][1], billboardViewMatrix.m[1][2]);
			axisZ = vec3New(billboardViewMatrix.m[2][0], billboardViewMatrix.m[2][1], billboardViewMatrix.m[2][2]);
		}
		// Lock certain axes if needed
		if(!spr->billboardX){
			axisX.y = 0.f;
			axisY.y = 1.f;
			axisZ.y = 0.f;
		}
		if(!spr->billboardY){
			axisX.x = 1.f;
			axisY.x = 0.f;
			axisZ.x = 0.f;
		}
		if(!spr->billboardZ){
			axisX.z = 0.f;
			axisY.z = 0.f;
			axisZ.z = 1.f;
		}
		// Rotation matrix               X axis   Y axis   Z axis
		mat4 billboardRotation = {.m = {{axisX.x, axisY.x, axisZ.x, 0.f},
		                                {axisX.y, axisY.y, axisZ.y, 0.f},
		                                {axisX.z, axisY.z, axisZ.z, 0.f},
		                                {0.f,     0.f,     0.f,     1.f}}};
		mat4MultMByM1(&modelViewMatrix, &billboardRotation);  // Apply billboard rotation
	}


	/* Rotate the model */
	// Convert the change in rotation to radians
	vec3 rotationRadians = vec3VMultS(spr->changeRot, radianRatio);
	quatMultQByQ2(quatNewEuler(rotationRadians), &spr->orientation);  // Apply the change in rotation to the current orientation
	vec3SetS(&spr->changeRot, 0.f);  // Reset the change in rotation

	// Convert orientation quaternion to its equivalent axis-angle representation
	/*float rotAngle = 0.f;
	vec3 rotAxis; vec3NewS(1.f);
	quatAxisAngle(spr->orientation, &rotAngle, &rotAxis.x, &rotAxis.y, &rotAxis.z);

	mat4RotateAA(&modelViewMatrix, rotAngle, rotAxis);*/
	mat4Rotate(&modelViewMatrix, spr->orientation);


	/*
	** Translate the model by -scaledPivot to counteract the scaledPivot in the
	** last translation. The result is the appearance of the model "pivoting"
	** around position + scaledPivot
	*/
	mat4Translate(&modelViewMatrix, -scaledPivot.x, -scaledPivot.y, -scaledPivot.z);


	/* Apply transformations to each vertex */
	vec4 vertexPos;
	unsigned int d;
	for(d = 0; d < 4; d++){
		// We need to make the vertex positions a vec4 so we can multiply them by the 4x4 modelViewProjectionMatrix
		vertexPos = vec3Extend(spr->vertices[d].pos, 1.f);
		mat4MultMByV(&modelViewMatrix, &vertexPos);
		spr->vertices[d].pos = vec4Truncate(vertexPos);
	}

}

void sprOffsetTexture(sprite *spr, float texFrag[4], float texWidth, float texHeight){
	// We can't pass unique textureFragment values for each individual sprite when batching. Therefore,
	// we have to do the offset calculations for each vertex UV here instead of in the shader
	for(unsigned int d = 0; d < 4; d++){
		spr->vertices[d].u = ((spr->vertices[d].u * texFrag[2]) + texFrag[0]) / texWidth;
		spr->vertices[d].v = ((spr->vertices[d].v * texFrag[3]) + texFrag[1]) / texHeight;
	}
}
