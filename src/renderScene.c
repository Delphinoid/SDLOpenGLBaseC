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
void renderModel(object *obj, const camera *cam, const float interpT, gfxProgram *gfxPrg){

	size_t i;

	/* Update the object's configuration for rendering. */
	rndrConfigRenderUpdate(&obj->tempRndrConfig, interpT);  /** Only line that requires non-const object. **/

	/* Interpolate between the previous and last skeleton states. */
	for(i = 0; i < obj->skl->boneNum; ++i){
		// If the bone has been simulated, use its position and orientation.
		//if((obj->physicsState[i].flags & PHYSICS_BODY_SIMULATE) > 0){
			/**
			obj->skeletonState[0][i].position    = obj->physicsState[i].position;
			obj->skeletonState[0][i].orientation = obj->physicsState[i].orientation;
			**/
		//}
		boneInterpolate(&obj->skeletonState[1][i], &obj->skeletonState[0][i], interpT, &gfxPrg->sklAnimationState[i]);
	}

	/* Feed the translucency multiplier to the shader */
	glUniform1f(gfxPrg->alphaID, obj->tempRndrConfig.alpha.render);

	/* Draw each renderable. */
	for(i = 0; i < obj->renderableNum; ++i){

		if(obj->renderables[i].mdl != NULL){  /** Remove? **/

			/* Get texture information for rendering and feed it to the shader. */
			float texFrag[4];  // The x, y, width and height of the fragment of the texture being rendered.
			GLuint frameTexID;
			twiGetFrameInfo(&obj->renderables[i].twi, &texFrag[0], &texFrag[1], &texFrag[2], &texFrag[3], &frameTexID, interpT);
			// Bind the texture (if needed).
			glActiveTexture(GL_TEXTURE0);
			if(frameTexID != gfxPrg->lastTexID){
				gfxPrg->lastTexID = frameTexID;
				glBindTexture(GL_TEXTURE_2D, frameTexID);
			}
			// Feed the texture coordinates to the shader.
			glUniform4fv(gfxPrg->textureFragmentID, 1, texFrag);

			/* Generate the renderable configuration based off the animated skeleton, if possible. */
			if(obj->renderables[i].mdl->skl != NULL){

				size_t j;
				size_t rndrBone;
				bone modelSpace;

				// If there is a valid animated skeleton, apply animation transformations.
				for(j = 0; j < obj->renderables[i].mdl->skl->boneNum; ++j){

					// If the animated bone is in the model, pass in its animation transforms.
					/** Use a lookup, same in object.c. **/
					rndrBone = sklFindBone(obj->skl, obj->renderables[i].mdl->skl->bones[j].name);
					if(rndrBone < obj->skl->boneNum){

						// Check if the current bone has an associated rigid body that is currently active.
						// If it does, interpolate its configuration and use that for rendering.
						if(obj->renderables[i].physicsSimulate && (obj->renderables[i].physicsState[j].flags & PHYSICS_BODY_SIMULATE) > 0){
							// Interpolate bone's rigid body's configuration.
							boneInterpolate(&obj->renderables[i].physicsState[j].configuration,
							                &obj->renderables[i].physicsState[j].configurationLast,
							                interpT, &modelSpace);
						}else{

							// Translate the bone from global object space to global model space.
							boneTransformAppendPositionVec(&gfxPrg->sklAnimationState[rndrBone],
							                               -obj->renderables[i].mdl->skl->bones[j].defaultState.position.x,
							                               -obj->renderables[i].mdl->skl->bones[j].defaultState.position.y,
							                               -obj->renderables[i].mdl->skl->bones[j].defaultState.position.z,
							                               &modelSpace.position);
							modelSpace.orientation = gfxPrg->sklAnimationState[rndrBone].orientation;
							modelSpace.scale = gfxPrg->sklAnimationState[rndrBone].scale;
						}

						// Feed the bone configuration to the shader.
						glUniform3f(gfxPrg->bonePositionArrayID[j], modelSpace.position.x,
							                                        modelSpace.position.y,
							                                        modelSpace.position.z);
						glUniform4f(gfxPrg->boneOrientationArrayID[j], modelSpace.orientation.v.x,
						                                               modelSpace.orientation.v.y,
						                                               modelSpace.orientation.v.z,
						                                               modelSpace.orientation.w);
						glUniform3f(gfxPrg->boneScaleArrayID[j], modelSpace.scale.x,
						                                         modelSpace.scale.y,
						                                         modelSpace.scale.z);

					}else{
						// Otherwise pass in an identity bone.
						glUniform3f(gfxPrg->bonePositionArrayID[j], 0.f, 0.f, 0.f);
						glUniform4f(gfxPrg->boneOrientationArrayID[j], 1.f, 0.f, 0.f, 1.f);
						glUniform3f(gfxPrg->boneScaleArrayID[j], 1.f, 1.f, 1.f);
					}

				}

			}

			/* Render the model. */
			glBindVertexArray(obj->renderables[i].mdl->vaoID);
			if(obj->renderables[i].mdl->indexNum > 0){
				glDrawElements(GL_TRIANGLES, obj->renderables[i].mdl->indexNum, GL_UNSIGNED_INT, (void *)0);
			}else{
				glDrawArrays(GL_TRIANGLES, 0, obj->renderables[i].mdl->vertexNum);
			}

		}

	}

}

/** Clean this up! **/
void batchRenderSprites(cVector *allSprites, const camera *cam, const float interpT, gfxProgram *gfxPrg){

	object *curSpr;
	size_t i;
	size_t j = 0;
	float texFrag[4];
	GLuint currentTexID;
	GLuint vboID;
	size_t currentVertexBatchSize = 0;

	// Reset the texture fragment.
	glUniform4f(gfxPrg->textureFragmentID, 0.f, 0.f, 1.f, 1.f);
	// Reset the root bone.
	glUniform3f(gfxPrg->bonePositionArrayID[j], 0.f, 0.f, 0.f);
	glUniform4f(gfxPrg->boneOrientationArrayID[j], 1.f, 0.f, 0.f, 1.f);
	glUniform3f(gfxPrg->boneScaleArrayID[j], 1.f, 1.f, 1.f);
	// Reset the translucency value.
	glUniform1f(gfxPrg->alphaID, 1.f);

	// Find the first sprite with a valid renderable.
	// Bind its VAO and use its VBO to draw every other sprite.
	for(i = 0; i < allSprites->size; ++i){
		curSpr = *((object **)cvGet(allSprites, i));
		if(curSpr != NULL){  /** Remove? **/
			for(j = 0; j < curSpr->renderableNum; ++j){
				if(curSpr->renderables[j].mdl != NULL){  /** Remove? **/
					// Bind the VAO.
					glBindVertexArray((*((object **)cvGet(allSprites, i)))->renderables[j].mdl->vaoID);
					vboID = (*((object **)cvGet(allSprites, i)))->renderables[j].mdl->vboID;
					goto EXIT;
				}
			}
		}
	}
	EXIT:

	while(i < allSprites->size){

		curSpr = *((object **)cvGet(allSprites, i));

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
					objGenerateSprite(curSpr, j, interpT, texFrag, (vertex *)(&gfxPrg->sprVertexBatchBuffer[currentVertexBatchSize]));
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

	cVector translucentModels; cvInit(&translucentModels, 1);  // Holds object pointers
	cVector distances;  cvInit(&distances, 1);  // Holds floats

	// Sort the different models into groups of those that are opaque and those that contain translucency
	size_t i;
	for(i = 0; i < allModels->size; ++i){

		object *curMdl = *((object **)cvGet(allModels, i));
		unsigned int currentRenderMethod = objRenderMethod(curMdl, interpT);

		if(currentRenderMethod == 0){  // If the model is fully opaque, add it straight to the render list
			cvPush(mdlRenderList, (void *)&curMdl, sizeof(object *));
		}else if(currentRenderMethod == 1){  // If the model contains translucency, it'll need to be depth sorted
			cvPush(&translucentModels, (void *)&curMdl, sizeof(object *));
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

				object *tempModel = *((object **)cvGet(&translucentModels, j-1));
				cvSet(&translucentModels, j-1, cvGet(&translucentModels, j), sizeof(object *));
				cvSet(&translucentModels, j, (void *)&tempModel, sizeof(object *));

			}

		}
	}


	// Combine the three vectors
	cvResize(mdlRenderList, mdlRenderList->size + translucentModels.size);
	for(i = 0; i < translucentModels.size; ++i){
		cvPush(mdlRenderList, cvGet(&translucentModels, i), sizeof(object *));
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
				object *curObj = objGetState(gameStateManager, (*camGetState(gameStateManager, i, stateID)->targetScene)->objectIDs[j], stateID);
				if(curObj != NULL){
					if(!curObj->tempRndrConfig.sprite){
						if(i == 0){
							cvPush(modelsScene, (void *)&curObj, sizeof(object *));
						}else{
							cvPush(modelsHUD, (void *)&curObj, sizeof(object *));
						}
					}else{
						if(i == 0){
							cvPush(spritesScene, (void *)&curObj, sizeof(object *));
						}else{
							cvPush(spritesHUD, (void *)&curObj, sizeof(object *));
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
			renderModel(*((object **)cvGet(&renderList, i)), camGetState(gameStateManager, 0, stateID), interpT, gfxPrg);
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
			renderModel(*((object **)cvGet(&modelsHUD, i)), camGetState(gameStateManager, 1, stateID), interpT, gfxPrg);
		}
		// Batch render HUD sprites.
		batchRenderSprites(&spritesHUD, camGetState(gameStateManager, 1, stateID), interpT, gfxPrg);
		cvClear(&modelsHUD);
	}

	cvClear(&modelsScene);
	cvClear(&spritesScene);
	cvClear(&spritesHUD);

}
