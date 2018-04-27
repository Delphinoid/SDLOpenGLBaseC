#include "gfxProgram.h"
#include "object.h"
#include "stateManagerHelpers.h"
#include <math.h>

/** THIS FILE IS TEMPORARY **/
/** THIS FILE IS TEMPORARY **/
/** THIS FILE IS TEMPORARY **/
/** THIS FILE IS TEMPORARY **/
/** THIS FILE IS TEMPORARY **/
/** THIS FILE IS TEMPORARY **/
/** THIS FILE IS TEMPORARY **/

/** This should not be necessary! **/
void renderModel(objInstance *obji, const camera *cam, const float interpT, gfxProgram *gfxPrg){

	size_t i;
	bone interpBone;

	/* Update the object's configuration for rendering. */
	rndrConfigRenderUpdate(&obji->tempRndrConfig, interpT);  /** Only line that requires non-const object. **/

	/* Interpolate between the previous and last skeleton states. */
	for(i = 0; i < obji->skl->boneNum; ++i){

		// Interpolate between bone states.
		boneInterpolate(&obji->skeletonState[1][i], &obji->skeletonState[0][i], interpT, &interpBone);

		// Convert the bone to a matrix.
		//mat4SetScaleMatrix(&gfxPrg->sklTransformState[i], gfxPrg->sklAnimationState[i].scale.x, gfxPrg->sklAnimationState[i].scale.y, gfxPrg->sklAnimationState[i].scale.z);
		//mat4SetTranslationMatrix(&gfxPrg->sklTransformState[i], gfxPrg->sklAnimationState[i].position.x, gfxPrg->sklAnimationState[i].position.y, gfxPrg->sklAnimationState[i].position.z);
		mat4SetRotationMatrix(&gfxPrg->sklTransformState[i], &interpBone.orientation);
		//mat4Rotate(&gfxPrg->sklTransformState[i], &gfxPrg->sklAnimationState[i].orientation);
		//mat4Translate(&gfxPrg->sklTransformState[i], gfxPrg->sklAnimationState[i].position.x, gfxPrg->sklAnimationState[i].position.y, gfxPrg->sklAnimationState[i].position.z);
		mat4Scale(&gfxPrg->sklTransformState[i], interpBone.scale.x, interpBone.scale.y, interpBone.scale.z);
		gfxPrg->sklTransformState[i].m[3][0] = interpBone.position.x;
		gfxPrg->sklTransformState[i].m[3][1] = interpBone.position.y;
		gfxPrg->sklTransformState[i].m[3][2] = interpBone.position.z;

	}

	/* Feed the translucency multiplier to the shader */
	glUniform1f(gfxPrg->alphaID, obji->tempRndrConfig.alpha.render);

	/* Draw each renderable. */
	for(i = 0; i < obji->renderableNum; ++i){

		if(obji->renderables[i].mdl != NULL){  /** Remove? **/


			/* Get texture information for rendering and feed it to the shader. */
			float texFrag[4];  // The x, y, width and height of the fragment of the texture being rendered.
			GLuint frameTexID;
			twiGetFrameInfo(&obji->renderables[i].twi, &texFrag[0], &texFrag[1], &texFrag[2], &texFrag[3], &frameTexID, interpT);
			// Bind the texture (if needed).
			glActiveTexture(GL_TEXTURE0);
			if(frameTexID != gfxPrg->lastTexID){
				gfxPrg->lastTexID = frameTexID;
				glBindTexture(GL_TEXTURE_2D, frameTexID);
			}
			// Feed the texture coordinates to the shader.
			glUniform4fv(gfxPrg->textureFragmentID, 1, texFrag);


			/*
			** Generate the renderable configuration based off the animated skeleton, if possible.
			** The loop converts the global skeleton state in gfxPrg->sklTransformState to local
			** model space for rendering.
			*/
			if(obji->renderables[i].mdl->skl != NULL){

				size_t j;
				size_t rndrBone;
				vec4 translation;
				mat4 transform;

				// If there is a valid animated skeleton, apply animation transformations.
				for(j = 0; j < obji->renderables[i].mdl->skl->boneNum; ++j){

					// Accumulate the bind positions. We need to use global bone offsets
					if(obji->renderables[i].mdl->skl->bones[j].parent < obji->renderables[i].mdl->skl->boneNum &&
					   j != obji->renderables[i].mdl->skl->bones[j].parent){
						// Apply the parent's bind offsets.
						gfxPrg->sklBindAccumulator[j].x = gfxPrg->sklBindAccumulator[obji->renderables[i].mdl->skl->bones[j].parent].x;
						gfxPrg->sklBindAccumulator[j].y = gfxPrg->sklBindAccumulator[obji->renderables[i].mdl->skl->bones[j].parent].y;
						gfxPrg->sklBindAccumulator[j].z = gfxPrg->sklBindAccumulator[obji->renderables[i].mdl->skl->bones[j].parent].z;
					}else{
						gfxPrg->sklBindAccumulator[j].x = 0.f;
						gfxPrg->sklBindAccumulator[j].y = 0.f;
						gfxPrg->sklBindAccumulator[j].z = 0.f;
					}

					// If the animated bone is in the model, pass in its animation transforms.
					/** Use a lookup, same in object.c. **/
					rndrBone = sklFindBone(obji->skl, obji->renderables[i].mdl->skl->bones[j].name);
					if(rndrBone < obji->skl->boneNum){

						// Rotate the bind pose position by the current bone's orientation
						// and add this offset to the bind pose accumulator.
						mat4MultNByM(obji->renderables[i].mdl->skl->bones[j].defaultState.position.x,
						             obji->renderables[i].mdl->skl->bones[j].defaultState.position.y,
						             obji->renderables[i].mdl->skl->bones[j].defaultState.position.z,
						             0.f,
						             &gfxPrg->sklTransformState[rndrBone],
						             &translation);
						gfxPrg->sklBindAccumulator[j].x += translation.x;
						gfxPrg->sklBindAccumulator[j].y += translation.y;
						gfxPrg->sklBindAccumulator[j].z += translation.z;

						// Translate the bone by the inverse of the accumulated bind translations.
						transform = gfxPrg->sklTransformState[rndrBone];
						transform.m[3][0] -= gfxPrg->sklBindAccumulator[j].x;
						transform.m[3][1] -= gfxPrg->sklBindAccumulator[j].y;
						transform.m[3][2] -= gfxPrg->sklBindAccumulator[j].z;

						// Feed the bone configuration to the shader.
						glUniformMatrix4fv(gfxPrg->boneArrayID[j], 1, GL_FALSE, &transform.m[0][0]);

					}else{
						// Otherwise pass in an identity bone.
						glUniformMatrix4fv(gfxPrg->boneArrayID[j], 1, GL_FALSE, &gfxPrg->identityMatrix.m[0][0]);
					}

				}

			}


			/* Render the model. */
			glBindVertexArray(obji->renderables[i].mdl->vaoID);
			if(obji->renderables[i].mdl->indexNum > 0){
				glDrawElements(GL_TRIANGLES, obji->renderables[i].mdl->indexNum, GL_UNSIGNED_INT, (void *)0);
			}else{
				glDrawArrays(GL_TRIANGLES, 0, obji->renderables[i].mdl->vertexNum);
			}


		}

	}

}

/** Clean this up! **/
void batchRenderSprites(cVector *allSprites, const camera *cam, const float interpT, gfxProgram *gfxPrg){

	objInstance *curSpr;
	size_t i;
	size_t j = 0;
	float texFrag[4];
	GLuint currentTexID;
	GLuint vboID;
	size_t currentVertexBatchSize = 0;

	// Reset the texture fragment.
	glUniform4f(gfxPrg->textureFragmentID, 0.f, 0.f, 1.f, 1.f);
	// Reset the root bone.
	glUniformMatrix4fv(gfxPrg->boneArrayID[0], 1, GL_FALSE, &gfxPrg->identityMatrix.m[0][0]);
	/*glUniform3f(gfxPrg->bonePositionArrayID[0], 0.f, 0.f, 0.f);
	glUniform4f(gfxPrg->boneOrientationArrayID[0], 1.f, 0.f, 0.f, 1.f);
	glUniform3f(gfxPrg->boneScaleArrayID[0], 1.f, 1.f, 1.f);*/
	// Reset the translucency value.
	glUniform1f(gfxPrg->alphaID, 1.f);

	// Find the first sprite with a valid renderable.
	// Bind its VAO and use its VBO to draw every other sprite.
	for(i = 0; i < allSprites->size; ++i){
		curSpr = *((objInstance **)cvGet(allSprites, i));
		if(curSpr != NULL){  /** Remove? **/
			for(j = 0; j < curSpr->renderableNum; ++j){
				if(curSpr->renderables[j].mdl != NULL){  /** Remove? **/
					// Bind the VAO.
					glBindVertexArray((*((objInstance **)cvGet(allSprites, i)))->renderables[j].mdl->vaoID);
					vboID = (*((objInstance **)cvGet(allSprites, i)))->renderables[j].mdl->vboID;
					goto EXIT;
				}
			}
		}
	}
	EXIT:

	while(i < allSprites->size){

		curSpr = *((objInstance **)cvGet(allSprites, i));

		if(curSpr != NULL){  /** Remove? **/

			rndrConfigRenderUpdate(&curSpr->tempRndrConfig, interpT);

			while(j < curSpr->renderableNum){

				if(curSpr->renderables[j].mdl != NULL){  /** Remove? **/

					twiGetFrameInfo(&curSpr->renderables[j].twi, &texFrag[0], &texFrag[1], &texFrag[2], &texFrag[3], &currentTexID, interpT);

					// If the current texture ID differs from the last, render and clear the VBO.
					if((gfxPrg->lastTexID != currentTexID && currentVertexBatchSize >= /**4**/6) || currentVertexBatchSize >= SPR_MAX_BATCH_SIZE){
						// We need the texture width and height to get around texture offset calculations in the shader.
						/** This shouldn't be necessary! **/
						glBindTexture(GL_TEXTURE_2D, gfxPrg->lastTexID);
						glBindBuffer(GL_ARRAY_BUFFER, vboID);  /** Why does this need to be bound...? **/
						glBufferData(GL_ARRAY_BUFFER, sizeof(gfxPrg->sprVertexBatchBuffer), &gfxPrg->sprVertexBatchBuffer[0], GL_DYNAMIC_DRAW);
						glDrawArrays(GL_TRIANGLES, 0, currentVertexBatchSize);
						currentVertexBatchSize = 0;
					}

					// Add sprite to the current batch.
					gfxPrg->lastTexID = currentTexID;
					objiGenerateSprite(curSpr, j, interpT, texFrag, (vertex *)(&gfxPrg->sprVertexBatchBuffer[currentVertexBatchSize]));
					// Copy duplicates, since IBOs aren't working.
					gfxPrg->sprVertexBatchBuffer[currentVertexBatchSize+5] = gfxPrg->sprVertexBatchBuffer[currentVertexBatchSize];
					gfxPrg->sprVertexBatchBuffer[currentVertexBatchSize+4] = gfxPrg->sprVertexBatchBuffer[currentVertexBatchSize+2];
					gfxPrg->sprVertexBatchBuffer[currentVertexBatchSize+2] = gfxPrg->sprVertexBatchBuffer[currentVertexBatchSize+3];
					currentVertexBatchSize += /**4**/6;

				}

				++j;

			}

			j = 0;

		}

		++i;

	}

	// Renders the final batch if necessary.
	if(currentVertexBatchSize >= /**4**/6){
		/** This STILL shouldn't be necessary! **/
		glBindTexture(GL_TEXTURE_2D, gfxPrg->lastTexID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);  /** Why does this need to be bound...? **/
		glBufferData(GL_ARRAY_BUFFER, sizeof(gfxPrg->sprVertexBatchBuffer), &gfxPrg->sprVertexBatchBuffer[0], GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, currentVertexBatchSize);
	}

}

void depthSortModels(cVector *allModels, cVector *mdlRenderList, const camera *cam, const float interpT){

	/** Use quick sort and potentially some sort of linked list data structure? **/

	cVector translucentModels; cvInit(&translucentModels, 1);  // Holds object pointers
	cVector distances;  cvInit(&distances, 1);  // Holds floats

	// Sort the different models into groups of those that are opaque and those that contain translucency
	size_t i;
	for(i = 0; i < allModels->size; ++i){

		objInstance *curMdl = *((objInstance **)cvGet(allModels, i));
		unsigned int currentRenderMethod = objiRenderMethod(curMdl, interpT);

		if(currentRenderMethod == 0){  // If the model is fully opaque, add it straight to the render list
			cvPush(mdlRenderList, (void *)&curMdl, sizeof(objInstance *));
		}else if(currentRenderMethod == 1){  // If the model contains translucency, it'll need to be depth sorted
			cvPush(&translucentModels, (void *)&curMdl, sizeof(objInstance *));
			float tempDistance = camDistance(cam, &curMdl->tempRndrConfig.position.render);
			cvPush(&distances, (void *)&tempDistance, sizeof(float));
		}

		// If currentRenderMethod is anything else (e.g. 2), the model will not be rendered at all

	}


	// Simple bubblesort (for now) to sort models with translucency by depth
	size_t j;
	for(i = 0; i < translucentModels.size; ++i){
		for(j = 1; j < translucentModels.size - i; ++j){

			if(*((float *)cvGet(&distances, j-1)) < *((float *)cvGet(&distances, j))){

				float tempDistance = *((float *)cvGet(&distances, j-1));
				cvSet(&distances, j-1, cvGet(&distances, j), sizeof(float));
				cvSet(&distances, j, (void *)&tempDistance, sizeof(float));

				objInstance *tempModel = *((objInstance **)cvGet(&translucentModels, j-1));
				cvSet(&translucentModels, j-1, cvGet(&translucentModels, j), sizeof(objInstance *));
				cvSet(&translucentModels, j, (void *)&tempModel, sizeof(objInstance *));

			}

		}
	}


	// Combine the three vectors
	cvResize(mdlRenderList, mdlRenderList->size + translucentModels.size);
	for(i = 0; i < translucentModels.size; ++i){
		cvPush(mdlRenderList, cvGet(&translucentModels, i), sizeof(objInstance *));
	}
	cvClear(&translucentModels);
	cvClear(&distances);

}

/** stateManager should be const. **/
void sortElements(stateManager *gameStateManager, const size_t stateID,
                  cVector *modelsScene,  cVector *modelsHUD,
                  cVector *spritesScene, cVector *spritesHUD){

	/** Merge with camera updating? **/
	/**
	*** Use separate vectors for translucent objects, or render
	*** opaque objects after a glEnable(GL_CULL_FACE).
	**/
	// Sort models and sprites into their scene and HUD vectors
	size_t i, j;
	for(i = 0; i < gameStateManager->objectType[SM_TYPE_CAMERA].capacity; ++i){
		if(camGetState(gameStateManager, i, stateID) != NULL){
			for(j = 0; j < (*camGetState(gameStateManager, i, stateID)->targetScene)->objectNum; ++j){
				objInstance *curObj = objGetState(gameStateManager, (*camGetState(gameStateManager, i, stateID)->targetScene)->objectIDs[j], stateID);
				if(curObj != NULL){
					if(!curObj->tempRndrConfig.sprite){
						if(i == 0){
							cvPush(modelsScene, (void *)&curObj, sizeof(objInstance *));
						}else{
							cvPush(modelsHUD, (void *)&curObj, sizeof(objInstance *));
						}
					}else{
						if(i == 0){
							cvPush(spritesScene, (void *)&curObj, sizeof(objInstance *));
						}else{
							cvPush(spritesHUD, (void *)&curObj, sizeof(objInstance *));
						}
					}
				}
			}
		}
	}

}

/** stateManager should be const. **/
/** This entire function should be part of the camera. **/
void renderScene(stateManager *gameStateManager, const size_t stateID, const float interpT, gfxProgram *gfxPrg){

	// Update cameras.
	size_t i;
	for(i = 0; i < gameStateManager->objectType[SM_TYPE_CAMERA].capacity; ++i){
		if(camGetState(gameStateManager, i, stateID) != NULL){
			camUpdateViewProjectionMatrix(camGetState(gameStateManager, i, stateID),
			                              gfxPrg->windowChanged,
			                              gfxPrg->aspectRatioX,
			                              gfxPrg->aspectRatioY,
			                              interpT);
		}
	}

	// Vector initialization.
	cVector modelsScene;  cvInit(&modelsScene, 1);   // Holds object pointers; pointers to scene models.
	cVector modelsHUD;    cvInit(&modelsHUD, 1);     // Holds object pointers; pointers to HUD models.
	cVector spritesScene; cvInit(&spritesScene, 1);  // Holds object pointers; pointers to scene sprites.
	cVector spritesHUD;   cvInit(&spritesHUD, 1);    // Holds object pointers; pointers to HUD sprites.
	sortElements(gameStateManager, stateID, &modelsScene, &modelsHUD, &spritesScene, &spritesHUD);

	// Render the main scene.
	if(camGetState(gameStateManager, 0, stateID) != NULL){
		// Feed the view-projection matrix into the shader.
		glUniformMatrix4fv(gfxPrg->vpMatrixID, 1, GL_FALSE, &camGetState(gameStateManager, 0, stateID)->viewProjectionMatrix.m[0][0]);
		// Depth sort scene models.
		cVector renderList; cvInit(&renderList, 1);  // Holds model pointers; pointers to depth-sorted scene models that must be rendered.
		depthSortModels(&modelsScene, &renderList, camGetState(gameStateManager, 0, stateID), interpT);
		// Render scene models.
		for(i = 0; i < renderList.size; ++i){
			renderModel(*((objInstance **)cvGet(&renderList, i)), camGetState(gameStateManager, 0, stateID), interpT, gfxPrg);
		}
		// Batch render scene sprites.
		batchRenderSprites(&spritesScene, camGetState(gameStateManager, 0, stateID), interpT, gfxPrg);
		cvClear(&renderList);
	}

	//glClear(GL_DEPTH_BUFFER_BIT);

	// Render the HUD scene.
	if(camGetState(gameStateManager, 1, stateID) != NULL){
		// Feed the view-projection matrix into the shader.
		glUniformMatrix4fv(gfxPrg->vpMatrixID, 1, GL_FALSE, &camGetState(gameStateManager, 1, stateID)->viewProjectionMatrix.m[0][0]);
		// Render HUD models.
		/** HUD camera? Streamline this to make handling different cameras easily **/
		for(i = 0; i < modelsHUD.size; ++i){
			renderModel(*((objInstance **)cvGet(&modelsHUD, i)), camGetState(gameStateManager, 1, stateID), interpT, gfxPrg);
		}
		// Batch render HUD sprites.
		batchRenderSprites(&spritesHUD, camGetState(gameStateManager, 1, stateID), interpT, gfxPrg);
		cvClear(&modelsHUD);
	}

	cvClear(&modelsScene);
	cvClear(&spritesScene);
	cvClear(&spritesHUD);

}
