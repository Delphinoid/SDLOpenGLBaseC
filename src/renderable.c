#include "renderable.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include "vec4.h"
#include "mat4.h"

#define radianRatio 0.017453292  // = PI / 180, used for converting degrees to radians

void rndrInit(renderable *rndr){
	rndr->name = NULL;
	rndr->mdl = NULL;
	//skliInit(&rndr->skl, NULL);
	twiInit(&rndr->tex, NULL);
	stInit(&rndr->sTrans);
	rtInit(&rndr->rTrans);
	rndr->sprite = 0;
	rndr->width = 0;
	rndr->height = 0;
	rndr->billboardFlags = RNDR_BILLBOARD_TARGET;
	rndr->hudElement = 0;
	rndr->hudScaleMode = 0;
}

/** Finish this **/
unsigned char rndrLoad(renderable *rndr, const char *prgPath, const char *filePath, cVector *allModels, cVector *allTexWrappers){

	rndrInit(rndr);
	return 1;

}

unsigned char rndrRenderMethod(renderable *rndr){
	if(rndr->rTrans.alpha > 0.f){
		if(rndr->rTrans.alpha < 1.f || twiContainsTranslucency(&rndr->tex)){
			return 1;  // The model contains translucency
		}else{
			return 0;  // The model is fully opaque
		}
	}
	return 2;  // The model is fully transparent
}

/*size_t rndrBoneNum(renderable *rndr){
	/** **
	return rndr->skl.skl->boneNum;
}

unsigned char rndrGenerateSkeletonState(renderable *rndr){
	// Check if state has changed
	/** **
	return skliGenerateState(&rndr->skl);
}*/

void rndrHudElement(renderable *rndr, unsigned char isHudElement){
	if(isHudElement != rndr->hudElement){
		// HUD elements render rotated 180 degrees on the X axis due to the way the orthographic matrix is set up
		quatMultQByQ2(quatNewEuler(M_PI, 0.f, 0.f), &rndr->sTrans.orientation);
	}
	rndr->hudElement = isHudElement;
	if(!rndr->hudElement){
		rndr->hudScaleMode = 0;
	}
}

void rndrSetRotation(renderable *rndr, float newX, float newY, float newZ){
	// HUD elements render rotated 180 degrees on the X axis due to the way the orthographic matrix is set up
	float offsetX = rndr->hudElement ? M_PI : 0.f;
	quatSetEuler(&rndr->sTrans.orientation, newX*radianRatio+offsetX, newY*radianRatio, newZ*radianRatio);
	vec3SetS(&rndr->sTrans.changeRot, 0.f);
}

void rndrRotateX(renderable *rndr, float changeX){
	rndr->sTrans.changeRot.x += changeX;
}

void rndrRotateY(renderable *rndr, float changeY){
	rndr->sTrans.changeRot.y += changeY;
}

void rndrRotateZ(renderable *rndr, float changeZ){
	rndr->sTrans.changeRot.z += changeZ;
}

void rndrAnimateTex(renderable *rndr, uint32_t currentTick, float globalDelayMod){
	twiAnimate(&rndr->tex, currentTick, globalDelayMod);
}

void rndrAnimateSkel(renderable *rndr, uint32_t currentTick, float globalDelayMod){
	skliAnimate(&rndr->skli, currentTick, globalDelayMod);
}

/** Shouldn't need to take in isSprite. Also, are gfxPrg and cam needed? **/
void rndrGenerateTransform(renderable *rndr, mat4 *transformMatrix, gfxProgram *gfxPrg, camera *cam){

	/** Shouldn't need these safety checks **/
	//if(rndr->tex != NULL && rndr->mdl->vertexNum > 0 && rndr->mdl->vboID != 0 && rndr->rTrans.alpha > 0.f){

	/* Get texture information for rendering */
	/**float texFrag[4];  // The x, y, width and height of the fragment of the texture being rendered
	GLuint frameTexID;
	twiGetFrameInfo(rndr->texture, rndr->rTrans.currentAnim, rndr->rTrans.currentFrame,
				   &texFrag[0], &texFrag[1], &texFrag[2], &texFrag[3], &frameTexID);**/


	/* Bind the texture (if needed) */
	/**glActiveTexture(GL_TEXTURE0);
	if(frameTexID != gfxPrg->lastTexID){
		gfxPrg->lastTexID = frameTexID;
		glBindTexture(GL_TEXTURE_2D, frameTexID);
	}**/


	/* Feed the texture coordinates to the shader */
	/**glUniform4f(gfxPrg->textureFragmentID, texFrag[0], texFrag[1], texFrag[2], texFrag[3]);**/


	/* Feed the translucency value to the shader */
	/**glUniform1f(gfxPrg->alphaID, rndr->rTrans.alpha);**/


	/* Set temporary position and scale vectors based on the selected HUD scaling mode */
	vec3 windowPos = rndr->sTrans.position;
	vec3 windowScale = rndr->rTrans.scale;
	if(rndr->hudElement){
		if(rndr->hudScaleMode == 1 || rndr->hudScaleMode == 3){
			vec3Set(&windowPos, rndr->sTrans.position.x * (float)gfxPrg->windowWidth  / (float)gfxPrg->biggestDimension,
			                    rndr->sTrans.position.y * (float)gfxPrg->windowHeight / (float)gfxPrg->biggestDimension,
			                    rndr->sTrans.position.z);
		}
		if(rndr->hudScaleMode == 2 || rndr->hudScaleMode == 3){
			vec3Set(&windowScale, rndr->rTrans.scale.x * (float)gfxPrg->windowWidth  / (float)gfxPrg->biggestDimension,
			                      rndr->rTrans.scale.y * (float)gfxPrg->windowHeight / (float)gfxPrg->biggestDimension,
			                      rndr->rTrans.scale.z);
		}
	}


	/*
	** Translate the model. By translating it from the camera coordinates to begin
	** with, we can save multiplying the model matrix by the view matrix later on.
	** However, we must start with the identity matrix for HUD elements
	*/
	if(rndr->hudElement){
		mat4Identity(transformMatrix);          // Start with the identity matrix
	}else{
		*transformMatrix = gfxPrg->viewMatrix;  // Start with the view matrix
	}
	vec3 scaledPivot;
	if(rndr->sprite){
		/** Why height-relPivot.y? **/
		vec3Set(&scaledPivot, rndr->sTrans.relPivot.x*windowScale.x, (rndr->height-rndr->sTrans.relPivot.y)*windowScale.y, rndr->sTrans.relPivot.z*windowScale.z);
		mat4Translate(transformMatrix, windowPos.x, windowPos.y, windowPos.z);
	}else{
		vec3Set(&scaledPivot, rndr->sTrans.relPivot.x*windowScale.x, rndr->sTrans.relPivot.y*windowScale.y, rndr->sTrans.relPivot.z*windowScale.z);
		mat4Translate(transformMatrix, windowPos.x+scaledPivot.x, windowPos.y+scaledPivot.y, windowPos.z+scaledPivot.z);
	}


	/* Billboarding */
	// If any of the flags apart from RNDR_BILLBOARD_TARGET are set, continue
	if((rndr->billboardFlags & ~RNDR_BILLBOARD_TARGET) > 0){
		vec3 axisX; vec3 axisY; vec3 axisZ;
		if((rndr->billboardFlags & RNDR_BILLBOARD_TARGET) > 0){
			// Generate a new view matrix for the billboard
			mat4 billboardViewMatrix;
			/** Merge cam? **/
			mat4LookAt(&billboardViewMatrix, rndr->rTrans.target, rndr->sTrans.position, cam->up);
			vec3Set(&axisX, billboardViewMatrix.m[0][0], billboardViewMatrix.m[0][1], billboardViewMatrix.m[0][2]);
			vec3Set(&axisY, billboardViewMatrix.m[1][0], billboardViewMatrix.m[1][1], billboardViewMatrix.m[1][2]);
			vec3Set(&axisZ, billboardViewMatrix.m[2][0], billboardViewMatrix.m[2][1], billboardViewMatrix.m[2][2]);
		}else{
			// Use the camera's X, Y and Z axes
			vec3Set(&axisX, gfxPrg->viewMatrix.m[0][0], gfxPrg->viewMatrix.m[0][1], gfxPrg->viewMatrix.m[0][2]);
			vec3Set(&axisY, gfxPrg->viewMatrix.m[1][0], gfxPrg->viewMatrix.m[1][1], gfxPrg->viewMatrix.m[1][2]);
			vec3Set(&axisZ, gfxPrg->viewMatrix.m[2][0], gfxPrg->viewMatrix.m[2][1], gfxPrg->viewMatrix.m[2][2]);
		}
		// Lock certain axes if needed
		if((rndr->billboardFlags & RNDR_BILLBOARD_X) == 0){
			axisX.y = 0.f;
			axisY.y = 1.f;
			axisZ.y = 0.f;
		}
		if((rndr->billboardFlags & RNDR_BILLBOARD_Y) == 0){
			axisX.x = 1.f;
			axisY.x = 0.f;
			axisZ.x = 0.f;
		}
		if((rndr->billboardFlags & RNDR_BILLBOARD_Z) == 0){
			axisX.z = 0.f;
			axisY.z = 0.f;
			axisZ.z = 1.f;
		}
		// Rotation matrix               X axis   Y axis   Z axis
		mat4 billboardRotation = {.m = {{axisX.x, axisY.x, axisZ.x, 0.f},
		                                {axisX.y, axisY.y, axisZ.y, 0.f},
		                                {axisX.z, axisY.z, axisZ.z, 0.f},
		                                {0.f,     0.f,     0.f,     1.f}}};
		mat4MultMByM1(transformMatrix, &billboardRotation);  // Apply billboard rotation
	}


	/* Rotate the model */
	// Apply the change in rotation to the current orientation
	quatMultQByQ2(quatNewEuler(rndr->sTrans.changeRot.x*radianRatio,
	                           rndr->sTrans.changeRot.y*radianRatio,
	                           rndr->sTrans.changeRot.z*radianRatio),
	              &rndr->sTrans.orientation);
	vec3SetS(&rndr->sTrans.changeRot, 0.f);  // Reset the change in rotation

	// Convert orientation quaternion to its equivalent axis-angle representation
	/*float rotAngle = 0.f;
	vec3 rotAxis; vec3NewS(1.f);
	quatAxisAngle(rndr->sTrans.orientation, &rotAngle, &rotAxis.x, &rotAxis.y, &rotAxis.z);

	mat4RotateV(transformMatrix, rotAngle, rotAxis);*/
	mat4Rotate(transformMatrix, rndr->sTrans.orientation);


	/*
	** Translate the model by -scaledPivot to counteract the scaledPivot in the
	** last translation. The result is the appearance of the model "pivoting"
	** around position + scaledPivot
	*/
	mat4Translate(transformMatrix, -scaledPivot.x, -scaledPivot.y, -scaledPivot.z);


	/*
	** We don't need to scale sprites, and since the vertices are multiplied by the
	** model view matrix on the CPU and the projection matrix is passed to the GPU,
	** we only need to generate a model view matrix here.
	*/
	if(!rndr->sprite){

		/* Scale the model */
		mat4Scale(transformMatrix, windowScale.x, windowScale.y, windowScale.z);

		/* Create the MVP matrix by multiplying the model view matrix by the projection matrix */
		//mat4 modelViewProjectionMatrix;
		if(rndr->hudElement){
			//modelViewProjectionMatrix = gfxPrg->projectionMatrixOrtho;    // Ortho for HUD elements
			mat4MultMByM2(&gfxPrg->projectionMatrixOrtho, transformMatrix);
		}else{
			//modelViewProjectionMatrix = gfxPrg->projectionMatrixFrustum;  // Frustum for regular models
			mat4MultMByM2(&gfxPrg->projectionMatrixFrustum, transformMatrix);
		}
		//mat4MultMByM1(&modelViewProjectionMatrix, mvpMatrix);

	}


	/* Feed the MVP matrix to the shader */
	/**glUniformMatrix4fv(gfxPrg->mvpMatrixID, 1, GL_FALSE, &mvpMatrix->m[0][0]);**/


	/* Render the model */
	//glBindVertexArray(mdl->vaoID);
	/**if(rndr->mdl->indexNum > 0){
		glDrawElements(GL_TRIANGLES, rndr->mdl->indexNum, GL_UNSIGNED_INT, (void *)0);
	}else{
		glDrawArrays(GL_TRIANGLES, 0, rndr->mdl->vertexNum);
	}**/

	//}

}

/** gfxPrg should definitely not be needed. Review this part **/
void rndrGenerateSprite(renderable *rndr, vertex *vertices, mat4 *transformMatrix, gfxProgram *gfxPrg){

	vec3 windowScale;
	if(rndr->hudElement && (rndr->hudScaleMode == 2 || rndr->hudScaleMode == 3)){
		vec3Set(&windowScale, rndr->rTrans.scale.x * (float)gfxPrg->windowWidth  / (float)gfxPrg->biggestDimension,
		                      rndr->rTrans.scale.y * (float)gfxPrg->windowHeight / (float)gfxPrg->biggestDimension,
		                      rndr->rTrans.scale.z);
	}else{
		windowScale = rndr->rTrans.scale;
	}

	float scaledWidth  = rndr->width  * windowScale.x;
	float scaledHeight = rndr->height * windowScale.y;

	/* Generate the base sprite quad */
	vertex tempVert;

	// Create the top left vertex
	tempVert.pos.x = 0.f;
	tempVert.pos.y = 0.f;
	tempVert.pos.z = 0.f;
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
	tempVert.pos.x = scaledWidth;
	tempVert.pos.y = 0.f;
	tempVert.pos.z = 0.f;
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
	tempVert.pos.x = 0.f;
	tempVert.pos.y = scaledHeight;
	tempVert.pos.z = 0.f;
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
	tempVert.pos.x = scaledWidth;
	tempVert.pos.y = scaledHeight;
	tempVert.pos.z = 0.f;
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
	for(i = 0; i < 4; i++){
		// We need to make the vertex positions a vec4 so we can multiply them by the 4x4 modelViewProjectionMatrix
		vec4Set(&vertexPos, vertices[i].pos.x, vertices[i].pos.y, vertices[i].pos.z, 1.f);
		mat4MultMByV(transformMatrix, &vertexPos);
		vec3Set(&vertices[i].pos, vertexPos.x, vertexPos.y, vertexPos.z);
	}

}

void rndrOffsetSpriteTexture(vertex *vertices, float texFrag[4], float texWidth, float texHeight){
	// We can't pass unique textureFragment values for each individual sprite when batching. Therefore,
	// we have to do the offset calculations for each vertex UV here instead of in the shader
	size_t i;
	for(i = 0; i < 4; i++){
		vertices[i].u = ((vertices[i].u * texFrag[2]) + texFrag[0]) / texWidth;
		vertices[i].v = ((vertices[i].v * texFrag[3]) + texFrag[1]) / texHeight;
	}
}

void rndrDelete(renderable *rndr){
	if(rndr->name != NULL){
		free(rndr->name);
	}
}
