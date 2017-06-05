#include "model.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include "math.h"
#include "mat4.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define radianRatio 0.017453292  // = PI / 180, used for converting degrees to radians

unsigned char mdlLoad(model *mdl, const char *prgPath, const char *filePath, cVector *allTexWrappers){

	/* Initialize member variables */
	mdl->vertices = NULL;
	mdl->vertexNum = 0;
	mdl->indices = NULL;
	mdl->indexNum = 0;
	mdl->vaoID = 0;
	mdl->vboID = 0;
	mdl->iboID = 0;
	vec3SetS(&mdl->position, 0.f);
	vec3SetS(&mdl->relPivot, 0.f);
	quatSet(&mdl->orientation, 1.0f, 0.f, 0.f, 0.f);
	vec3SetS(&mdl->target, 0.f);
	vec3SetS(&mdl->changeRot, 0.f);
	vec3SetS(&mdl->scale, 1.f);
	mdl->alpha = 1.f;
	mdl->billboardX = 0;
	mdl->billboardY = 0;
	mdl->billboardZ = 0;
	mdl->simpleBillboard = 0;
	mdl->hudElement = 0;
	mdl->hudScaleMode = 0;
	mdl->texture = NULL;
	mdl->currentAnim = 0;
	mdl->currentFrame = 0;
	mdl->timesLooped = 0;
	mdl->frameProgress = 0;

	char *fullPath = malloc((strlen(prgPath) + strlen(filePath) + 1) * sizeof(char));
	strcpy(fullPath, prgPath);
	strcat(fullPath, filePath);
	fullPath[strlen(prgPath)+strlen(filePath)] = '\0';
	FILE *mdlInfo = fopen(fullPath, "r");
	char lineFeed[1024];
	char *line;
	char compare[1024];
	unsigned int lineLength;

	cVector tempPositions; cvInit(&tempPositions, 3);  // Holds floats; temporarily holds vertex position data before it is pushed into vertexBuffer
	cVector tempTexCoords; cvInit(&tempTexCoords, 2);  // Holds floats; temporarily holds vertex UV data before it is pushed into vertexBuffer
	cVector tempNorms;     cvInit(&tempNorms, 3);      // Holds floats; temporarily holds vertex normal data before it is pushed into vertexBuffer
	vertex3D tempVert;  // Holds a vertex before pushing it into the triangle array
	unsigned int positionIndex[3];  // Holds all the positional information for a face
	unsigned int uvIndex[3];        // Holds all the UV information for a face
	unsigned int normalIndex[3];    // Holds all the normal information for a face
	cVector allVertices; cvInit(&allVertices, 1);  // Temporarily holds all vertex data
	cVector allIndices;  cvInit(&allIndices, 1);   // Temporarily holds all index data

	if(mdlInfo != NULL){
		while(!feof(mdlInfo)){

			fgets(lineFeed, sizeof(lineFeed), mdlInfo);
			lineFeed[strcspn(lineFeed, "\r\n")] = 0;
			line = lineFeed;
			lineLength = strlen(line);

			// Remove any comments from the line
			char *commentPos = strstr(line, "//");
			if(commentPos != NULL){
				commentPos = '\0';
			}
			// Remove any indentations from the line
			unsigned int d;
			for(d = 0; d < lineLength; d++){
				if(line[d] != '\t' && line[d] != ' '){
					line += d;
					d = lineLength;
				}
			}

			// Texture to use
			if(lineLength >= 8 && strncpy(compare, line, 7) && (compare[7] = '\0') == 0 && strcmp(compare, "usemtl ") == 0){
				for(d = 0; d < allTexWrappers->size; d++){
					textureWrapper *tempTexWrap = (textureWrapper *)cvGet(allTexWrappers, d);
					if(strcmp(line+7, tempTexWrap->name) == 0){
						mdl->texture = tempTexWrap;
						d = allTexWrappers->size;
					}
				}

			// Vertex data
			}else if(lineLength >= 7 && strncpy(compare, line, 2) && (compare[2] = '\0') == 0 && strcmp(compare, "v ") == 0){
				char *token = strtok(line+2, " /");
				float curVal = strtof(token, NULL);
				cvPush(&tempPositions, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " /");
				curVal = strtof(token, NULL);
				cvPush(&tempPositions, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " /");
				curVal = strtof(token, NULL);
				cvPush(&tempPositions, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " /");

			// UV data
			}else if(lineLength >= 6 && strncpy(compare, line, 3) && (compare[3] = '\0') == 0 && strcmp(compare, "vt ") == 0){
				char *token = strtok(line+3, " /");
				float curVal = strtof(token, NULL);
				cvPush(&tempTexCoords, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " /");
				curVal = strtof(token, NULL);
				cvPush(&tempTexCoords, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " /");

			// Normal data
			}else if(lineLength >= 8 && strncpy(compare, line, 3) && (compare[3] = '\0') == 0 && strcmp(compare, "vn ") == 0){
				char *token = strtok(line+3, " /");
				float curVal = strtof(token, NULL);
				cvPush(&tempNorms, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " /");
				curVal = strtof(token, NULL);
				cvPush(&tempNorms, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " /");
				curVal = strtof(token, NULL);
				cvPush(&tempNorms, (void *)&curVal, sizeof(curVal));
				token = strtok(NULL, " /");

			// Face data
			}else if(lineLength >= 19 && strncpy(compare, line, 2) && (compare[2] = '\0') == 0 && strcmp(compare, "f ") == 0){
				char *token = strtok(line+2, " /");
				for(d = 0; d < 3; d++){

					// Load face data
					positionIndex[d] = strtoul(token, NULL, 0);
					token = strtok(NULL, " /");
					uvIndex[d] = strtoul(token, NULL, 0);
					token = strtok(NULL, " /");
					normalIndex[d] = strtoul(token, NULL, 0);
					token = strtok(NULL, " /");

					// Create a vertex from the given data
					// Vertex positional data
					void *checkVal = cvGet(&tempPositions, (positionIndex[d]-1)*3);
					if(checkVal != NULL){
						tempVert.pos.x = *((float *)checkVal);
					}else{
						tempVert.pos.x = 0.f;
					}
					checkVal = cvGet(&tempPositions, (positionIndex[d]-1)*3+1);
					if(checkVal != NULL){
						tempVert.pos.y = *((float *)checkVal);
					}else{
						tempVert.pos.y = 0.f;
					}
					checkVal = cvGet(&tempPositions, (positionIndex[d]-1)*3+2);
					if(checkVal != NULL){
						tempVert.pos.z = *((float *)checkVal);
					}else{
						tempVert.pos.z = 0.f;
					}
					// Vertex UV data
					checkVal = cvGet(&tempTexCoords, (uvIndex[d]-1)*2);
					if(checkVal != NULL){
						tempVert.u = *((float *)checkVal);
					}else{
						tempVert.u = 0.f;
					}
					checkVal = cvGet(&tempTexCoords, (uvIndex[d]-1)*2+1);
					if(checkVal != NULL){
						tempVert.v = -*((float *)checkVal);
					}else{
						tempVert.v = 0.f;
					}
					// Vertex normal data
					checkVal = cvGet(&tempNorms, (normalIndex[d]-1)*3);
					if(checkVal != NULL){
						tempVert.nx = *((float *)checkVal);
					}else{
						tempVert.nx = 0.f;
					}
					checkVal = cvGet(&tempNorms, (normalIndex[d]-1)*3+1);
					if(checkVal != NULL){
						tempVert.ny = *((float *)checkVal);
					}else{
						tempVert.ny = 0.f;
					}
					checkVal = cvGet(&tempNorms, (normalIndex[d]-1)*3+2);
					if(checkVal != NULL){
						tempVert.nz = *((float *)checkVal);
					}else{
						tempVert.nz = 0.f;
					}

					// Check if the vertex has already been loaded, and if so add an index
					unsigned char foundVertex = 0;
					unsigned int f;
					for(f = 0; f < allVertices.size; f++){
						vertex3D *checkVert = (vertex3D *)cvGet(&allVertices, f);
						if(checkVert->pos.x == tempVert.pos.x && checkVert->pos.y == tempVert.pos.y && checkVert->pos.z == tempVert.pos.z &&
						   checkVert->u     == tempVert.u     && checkVert->v     == tempVert.v     &&
						   checkVert->nx    == tempVert.nx    && checkVert->ny    == tempVert.ny    && checkVert->nz    == tempVert.nz){

							cvPush(&allIndices, (void *)&f, sizeof(f));
							f = allVertices.size;
							foundVertex = 1;
						}
					}

					// If the vertex has not yet been loaded, add it to both the vertex vector and the index vector
					if(!foundVertex){
						cvPush(&allIndices, (void *)&allVertices.size, sizeof(allVertices.size));
						cvPush(&allVertices, (void *)&tempVert, sizeof(tempVert));
					}

				}

			}

		}

		fclose(mdlInfo);

	}else{
		printf("Error loading model:\nCouldn't open %s\n", fullPath);
		cvClear(&tempPositions);
		cvClear(&tempTexCoords);
		cvClear(&tempNorms);
		free(fullPath);
		return 0;
	}

	cvClear(&tempPositions);
	cvClear(&tempTexCoords);
	cvClear(&tempNorms);
	free(fullPath);

	mdl->vertexNum = allVertices.size;
	mdl->vertices = malloc(allVertices.size * sizeof(vertex3D));
	unsigned int d;
	for(d = 0; d < allVertices.size; d++){
		mdl->vertices[d] = *((vertex3D *)cvGet(&allVertices, d));
	}
	cvClear(&allVertices);

	mdl->indexNum = allIndices.size;
	mdl->indices = malloc(allIndices.size * sizeof(unsigned int));
	for(d = 0; d < allIndices.size; d++){
		mdl->indices[d] = *((unsigned int *)cvGet(&allIndices, d));
	}
	cvClear(&allIndices);

	mdlCreateVAO(mdl);
	return 1;


}

void mdlCreateVAO(model *mdl){

	if(mdl->indexNum > 0 && (mdl->vaoID == 0 || mdl->vboID == 0 || mdl->iboID == 0)){

		// Create and bind the VAO
		glGenVertexArrays(1, &mdl->vaoID);
		glBindVertexArray(mdl->vaoID);

		// Create and bind the VBO
		glGenBuffers(1, &mdl->vboID);
		glBindBuffer(GL_ARRAY_BUFFER, mdl->vboID);
		glBufferData(GL_ARRAY_BUFFER, mdl->vertexNum * sizeof(vertex3D), &mdl->vertices[0], GL_STATIC_DRAW);

		// Create and bind the IBO
		glGenBuffers(1, &mdl->iboID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdl->iboID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mdl->indexNum * sizeof(unsigned int), &mdl->indices[0], GL_STATIC_DRAW);

		// Position offset
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex3D), (GLvoid*)offsetof(vertex3D, pos));
		glEnableVertexAttribArray(2);
		// UV offset
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex3D), (GLvoid*)offsetof(vertex3D, u));
		glEnableVertexAttribArray(1);
		// Normals offset
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex3D), (GLvoid*)offsetof(vertex3D, nx));
		glEnableVertexAttribArray(0);

		// We don't want anything else to modify the VAO
		glBindVertexArray(0);

		GLenum glError = glGetError();
		if(glError != GL_NO_ERROR){
			printf("Error creating buffers:\n%u\n", glError);
		}

	}

}

unsigned int mdlRenderMethod(model *mdl){
	if(mdl->alpha > 0.f && mdl->texture != NULL){
		if(mdl->alpha < 1.f || twContainsTranslucency(mdl->texture, mdl->currentAnim, mdl->currentFrame)){
			return 1;  // The model contains translucency
		}else{
			return 0;  // The model is fully opaque
		}
	}
	return 2;  // The model is fully transparent
}

void mdlHudElement(model *mdl, unsigned char isHudElement){
	if(isHudElement != mdl->hudElement){
		// HUD elements render rotated 180 degrees on the X axis due to the way the orthographic matrix is set up
		quatMultQByQ2(quatNewEuler(vec3New(M_PI, 0.f, 0.f)), &mdl->orientation);
	}
	mdl->hudElement = isHudElement;
	if(!mdl->hudElement){
		mdl->hudScaleMode = 0;
	}
}

void mdlSetRotation(model *mdl, float newX, float newY, float newZ){
	// HUD elements render rotated 180 degrees on the X axis due to the way the orthographic matrix is set up
	float offsetX = mdl->hudElement ? M_PI : 0.f;
	mdl->orientation = quatNewEuler(vec3New(newX * radianRatio + offsetX, newY * radianRatio, newZ * radianRatio));
	mdl->changeRot = vec3NewS(0.f);
}

void mdlRotateX(model *mdl, float changeX){
	mdl->changeRot.x += changeX;
}

void mdlRotateY(model *mdl, float changeY){
	mdl->changeRot.y += changeY;
}

void mdlRotateZ(model *mdl, float changeZ){
	mdl->changeRot.z += changeZ;
}

void mdlAnimate(model *mdl){

	// Execute the next frame of the animation

}

void mdlAnimateTex(model *mdl){

	if(mdl->texture != NULL){
		if(mdl->frameProgress == 0){
			mdl->frameProgress = SDL_GetTicks();
		}
		twAnimate(mdl->texture, 1.f, &mdl->currentAnim, &mdl->currentFrame, &mdl->frameProgress, &mdl->timesLooped);
	}

}

void mdlRender(model *mdl, gfxProgram *gfxPrg, camera *cam){

	if(mdl->texture != NULL && mdl->indexNum > 0 && mdl->vboID != 0 && mdl->alpha > 0.f){

		/* Get texture information for rendering */
		float texFrag[4];  // The x, y, width and height of the fragment of the texture being rendered
		GLuint frameTexID;
		twGetFrameInfo(mdl->texture, mdl->currentAnim, mdl->currentFrame, &texFrag[0], &texFrag[1], &texFrag[2], &texFrag[3], &frameTexID);


		/* Bind the texture (if needed) */
		glActiveTexture(GL_TEXTURE0);
		if(frameTexID != gfxPrg->lastTexID){
			gfxPrg->lastTexID = frameTexID;
			glBindTexture(GL_TEXTURE_2D, frameTexID);
		}


		/* Feed the texture coordinates to the shader */
		glUniform4f(gfxPrg->textureFragmentID, texFrag[0], texFrag[1], texFrag[2], texFrag[3]);


		/* Feed the translucency value to the shader */
		glUniform1f(gfxPrg->alphaID, mdl->alpha);


		/* Set temporary position and scale vectors based on the selected HUD scaling mode */
		vec3 windowPos = mdl->position;
		vec3 windowScale = mdl->scale;
		if(mdl->hudElement){
			if(mdl->hudScaleMode == 1 || mdl->hudScaleMode == 3){
				windowPos = vec3New(mdl->position.x * (float)gfxPrg->windowWidth  / (float)gfxPrg->biggestDimension,
				                    mdl->position.y * (float)gfxPrg->windowHeight / (float)gfxPrg->biggestDimension,
				                    mdl->position.z);
			}
			if(mdl->hudScaleMode == 2 || mdl->hudScaleMode == 3){
				windowScale = vec3New(mdl->scale.x * (float)gfxPrg->windowWidth  / (float)gfxPrg->biggestDimension,
				                      mdl->scale.y * (float)gfxPrg->windowHeight / (float)gfxPrg->biggestDimension,
				                      mdl->scale.z);
			}
		}


		/*
		** Translate the model. By translating it from the camera coordinates to begin
		** with, we can save multiplying the model matrix by the view matrix later on.
		** However, we must start with the identity matrix for HUD elements
		*/
		mat4 modelViewMatrix;
		vec3 scaledPivot = vec3VMultV(mdl->relPivot, windowScale);
		if(mdl->hudElement){
			modelViewMatrix = gfxPrg->identityMatrix;  // Start with the identity matrix
		}else{
			modelViewMatrix = gfxPrg->viewMatrix;      // Start with the view matrix
		}
		mat4Translate(&modelViewMatrix, windowPos.x+scaledPivot.x, windowPos.y+scaledPivot.y, windowPos.z+scaledPivot.z);


		/* Billboarding */
		if(mdl->billboardX || mdl->billboardY || mdl->billboardZ){
			vec3 axisX; vec3 axisY; vec3 axisZ;
			if(mdl->simpleBillboard){
				// Use the camera's X, Y and Z axes
				axisX = vec3New(gfxPrg->viewMatrix.m[0][0], gfxPrg->viewMatrix.m[0][1], gfxPrg->viewMatrix.m[0][2]);
				axisY = vec3New(gfxPrg->viewMatrix.m[1][0], gfxPrg->viewMatrix.m[1][1], gfxPrg->viewMatrix.m[1][2]);
				axisZ = vec3New(gfxPrg->viewMatrix.m[2][0], gfxPrg->viewMatrix.m[2][1], gfxPrg->viewMatrix.m[2][2]);
			}else{
				// Generate a new view matrix for the billboard
				mat4 billboardViewMatrix;
				/** Merge model, sprite and cam **/
				mat4LookAt(&billboardViewMatrix, mdl->target, mdl->position, cam->up);
				axisX = vec3New(billboardViewMatrix.m[0][0], billboardViewMatrix.m[0][1], billboardViewMatrix.m[0][2]);
				axisY = vec3New(billboardViewMatrix.m[1][0], billboardViewMatrix.m[1][1], billboardViewMatrix.m[1][2]);
				axisZ = vec3New(billboardViewMatrix.m[2][0], billboardViewMatrix.m[2][1], billboardViewMatrix.m[2][2]);
			}
			// Lock certain axes if needed
			if(!mdl->billboardX){
				axisX.y = 0.f;
				axisY.y = 1.f;
				axisZ.y = 0.f;
			}
			if(!mdl->billboardY){
				axisX.x = 1.f;
				axisY.x = 0.f;
				axisZ.x = 0.f;
			}
			if(!mdl->billboardZ){
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
		vec3 rotationRadians = vec3VMultS(mdl->changeRot, radianRatio);
		quatMultQByQ2(quatNewEuler(rotationRadians), &mdl->orientation);  // Apply the change in rotation to the current orientation
		vec3SetS(&mdl->changeRot, 0.f);  // Reset the change in rotation

		// Convert orientation quaternion to its equivalent axis-angle representation
		/*float rotAngle = 0.f;
		vec3 rotAxis; vec3NewS(1.f);
		quatAxisAngle(mdl->orientation, &rotAngle, &rotAxis.x, &rotAxis.y, &rotAxis.z);

		mat4RotateV(&modelViewMatrix, rotAngle, rotAxis);*/
		mat4Rotate(&modelViewMatrix, mdl->orientation);


		/*
		** Translate the model by -scaledPivot to counteract the scaledPivot in the
		** last translation. The result is the appearance of the model "pivoting"
		** around position + scaledPivot
		*/
		mat4Translate(&modelViewMatrix, -scaledPivot.x, -scaledPivot.y, -scaledPivot.z);


		/* Scale the model */
		mat4Scale(&modelViewMatrix, windowScale.x, windowScale.y, windowScale.z);


		/* Create the MVP matrix */
		//mat4 modelViewProjectionMatrix;
		if(mdl->hudElement){
			//modelViewProjectionMatrix = gfxPrg->projectionMatrixOrtho;    // Ortho for HUD elements
			mat4MultMByM2(&gfxPrg->projectionMatrixOrtho, &modelViewMatrix);
		}else{
			//modelViewProjectionMatrix = gfxPrg->projectionMatrixFrustum;  // Frustum for regular models
			mat4MultMByM2(&gfxPrg->projectionMatrixFrustum, &modelViewMatrix);
		}
		//mat4MultMByM1(&modelViewProjectionMatrix, &modelViewMatrix);


		/* Feed the MVP matrix to the shader */
		glUniformMatrix4fv(gfxPrg->mvpMatrixID, 1, GL_FALSE, &modelViewMatrix.m[0][0]);


		/* Render the model */
		glBindVertexArray(mdl->vaoID);
		glDrawElements(GL_TRIANGLES, mdl->indexNum, GL_UNSIGNED_INT, (void *)0);
		//glDrawArrays(GL_TRIANGLES, 0, mdl->vertexNum);

	}

}

void mdlDelete(model *mdl){
	if(mdl->vaoID != 0){
		glDeleteVertexArrays(1, &mdl->vaoID);
	}
	if(mdl->vboID != 0){
		glDeleteBuffers(1, &mdl->vboID);
	}
	if(mdl->iboID != 0){
		glDeleteBuffers(1, &mdl->iboID);
	}
	free(&mdl->vertices);
	free(&mdl->indices);
}
