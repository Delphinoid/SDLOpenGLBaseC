#include "graphicsManager.h"
#include "helpersMath.h"
#include "object.h"
#include "camera.h"
#include <math.h>

/** **/
#include "moduleRenderable.h"

/** THIS FILE IS TEMPORARY **/
/** THIS FILE IS TEMPORARY **/
/** THIS FILE IS TEMPORARY **/
/** THIS FILE IS TEMPORARY **/
/** THIS FILE IS TEMPORARY **/
/** THIS FILE IS TEMPORARY **/
/** THIS FILE IS TEMPORARY **/

/** This should not be necessary! **/
void renderModel(const objInstance *obji, const float distance, const camera *cam, const float interpT, graphicsManager *gfxMngr){

	boneIndex_t i;
	bone interpBone;

	rndrInstance *currentRndr = obji->renderables;

	/* Update the object's configuration for rendering. */
	//rndrConfigRenderUpdate(&obji->tempRndrConfig, interpT);  /** Only line that requires non-const object. **/

	/* Interpolate between the previous and last skeleton states. */
	for(i = 0; i < obji->skeletonData.skl->boneNum; ++i){

		const bone *current  = &obji->state.skeleton[i];
		const bone *previous = (obji->state.previous == NULL ? current : &obji->state.previous->skeleton[i]);

		// Interpolate between bone states.
		//boneInterpolate(&obji->skeletonState[1][i], &obji->skeletonState[0][i], interpT, &interpBone);
		boneInterpolate(previous, current, interpT, &interpBone);

		// Convert the bone to a matrix.
		//mat4SetScaleMatrix(&gfxMngr->sklTransformState[i], gfxMngr->sklAnimationState[i].scale.x, gfxMngr->sklAnimationState[i].scale.y, gfxMngr->sklAnimationState[i].scale.z);
		//mat4SetTranslationMatrix(&gfxMngr->sklTransformState[i], gfxMngr->sklAnimationState[i].position.x, gfxMngr->sklAnimationState[i].position.y, gfxMngr->sklAnimationState[i].position.z);
		mat4SetRotationMatrix(&gfxMngr->sklTransformState[i], &interpBone.orientation);
		//mat4Rotate(&gfxMngr->sklTransformState[i], &gfxMngr->sklAnimationState[i].orientation);
		//mat4Translate(&gfxMngr->sklTransformState[i], gfxMngr->sklAnimationState[i].position.x, gfxMngr->sklAnimationState[i].position.y, gfxMngr->sklAnimationState[i].position.z);
		mat4Scale(&gfxMngr->sklTransformState[i], interpBone.scale.x, interpBone.scale.y, interpBone.scale.z);
		gfxMngr->sklTransformState[i].m[3][0] = interpBone.position.x;
		gfxMngr->sklTransformState[i].m[3][1] = interpBone.position.y;
		gfxMngr->sklTransformState[i].m[3][2] = interpBone.position.z;

	}

	/* Draw each renderable. */
	while(currentRndr != NULL){

		/* Get texture information for rendering and feed it to the shader. */
		float texFrag[4];  // The x, y, width and height of the fragment of the texture being rendered.
		GLuint frameTexID;
		twiGetFrameInfo(&currentRndr->twi, &texFrag[0], &texFrag[1], &texFrag[2], &texFrag[3], &frameTexID, interpT);
		// Bind the texture (if needed).
		gfxMngrBindTexture(gfxMngr, GL_TEXTURE0, frameTexID);
		// Feed the texture coordinates to the shader.
		glUniform4fv(gfxMngr->textureFragmentID, 1, texFrag);

		/*
		** Generate the renderable configuration based off the animated skeleton, if possible.
		** The loop converts the global skeleton state in gfxMngr->sklTransformState to local
		** model space for rendering.
		*/
		if(currentRndr->mdl->skl != NULL){

			const float alpha = floatLerp(currentRndr->alphaPrevious, currentRndr->alpha, interpT);

			if(alpha > 0.f){

				boneIndex_t rndrBone;
				vec4 translation;
				mat4 transform;

				// If there is a valid animated skeleton, apply animation transformations.
				for(i = 0; i < currentRndr->mdl->skl->boneNum; ++i){

					// Accumulate the bind positions. We need to use global bone offsets.
					if(currentRndr->mdl->skl->bones[i].parent < currentRndr->mdl->skl->boneNum &&
					   i != currentRndr->mdl->skl->bones[i].parent){
						// Apply the parent's bind offsets.
						gfxMngr->sklBindAccumulator[i].x = gfxMngr->sklBindAccumulator[currentRndr->mdl->skl->bones[i].parent].x;
						gfxMngr->sklBindAccumulator[i].y = gfxMngr->sklBindAccumulator[currentRndr->mdl->skl->bones[i].parent].y;
						gfxMngr->sklBindAccumulator[i].z = gfxMngr->sklBindAccumulator[currentRndr->mdl->skl->bones[i].parent].z;
					}else{
						gfxMngr->sklBindAccumulator[i].x = 0.f;
						gfxMngr->sklBindAccumulator[i].y = 0.f;
						gfxMngr->sklBindAccumulator[i].z = 0.f;
					}

					// If the animated bone is in the model, pass in its animation transforms.
					/** Use a lookup, same in object.c. **/
					rndrBone = sklFindBone(obji->skeletonData.skl, i, currentRndr->mdl->skl->bones[i].name);
					if(rndrBone < obji->skeletonData.skl->boneNum){

						// Rotate the bind pose position by the current bone's orientation
						// and add this offset to the bind pose accumulator.
						mat4MultNByM(currentRndr->mdl->skl->bones[i].defaultState.position.x,
						             currentRndr->mdl->skl->bones[i].defaultState.position.y,
						             currentRndr->mdl->skl->bones[i].defaultState.position.z,
						             0.f,
						             &gfxMngr->sklTransformState[rndrBone],
						             &translation);
						gfxMngr->sklBindAccumulator[i].x += translation.x;
						gfxMngr->sklBindAccumulator[i].y += translation.y;
						gfxMngr->sklBindAccumulator[i].z += translation.z;

						// Translate the bone by the inverse of the accumulated bind translations.
						transform = gfxMngr->sklTransformState[rndrBone];
						transform.m[3][0] -= gfxMngr->sklBindAccumulator[i].x;
						transform.m[3][1] -= gfxMngr->sklBindAccumulator[i].y;
						transform.m[3][2] -= gfxMngr->sklBindAccumulator[i].z;

						// Feed the bone configuration to the shader.
						glUniformMatrix4fv(gfxMngr->boneArrayID[i], 1, GL_FALSE, &transform.m[0][0]);

					}else{
						// Otherwise pass in an identity bone.
						glUniformMatrix4fv(gfxMngr->boneArrayID[i], 1, GL_FALSE, &gfxMngr->identityMatrix.m[0][0]);
					}

				}

				/* Feed the translucency multiplier to the shader */
				glUniform1f(gfxMngr->alphaID, alpha);

				/* Render the model. */
				glBindVertexArray(currentRndr->mdl->vaoID);
				if(currentRndr->mdl->indexNum > 0){
					GLsizei indexNum;
					void *offset;
					mdlFindCurrentLOD(currentRndr->mdl, &indexNum, &offset, distance, gfxMngr->biasLOD);
					if(indexNum){
						glDrawElements(GL_TRIANGLES, indexNum, GL_UNSIGNED_INT, offset);
					}
				}else{
					glDrawArrays(GL_TRIANGLES, 0, currentRndr->mdl->vertexNum);
				}

			}

		}

		currentRndr = moduleRenderableInstanceNext(currentRndr);

	}

}

/** Clean this up! **/
/**void batchRenderSprites(cVector *allSprites, const camera *cam, const float interpT, graphicsManager *gfxMngr){

	objInstance *currentSpr;
	rndrInstance *currentRndr;
	size_t i;
	float texFrag[4];
	GLuint currentTexID;
	GLuint vboID;
	size_t currentVertexBatchSize = 0;

	// Reset the texture fragment.
	glUniform4f(gfxMngr->textureFragmentID, 0.f, 0.f, 1.f, 1.f);
	// Reset the root bone.
	glUniformMatrix4fv(gfxMngr->boneArrayID[0], 1, GL_FALSE, &gfxMngr->identityMatrix.m[0][0]);
	*glUniform3f(gfxMngr->bonePositionArrayID[0], 0.f, 0.f, 0.f);
	glUniform4f(gfxMngr->boneOrientationArrayID[0], 1.f, 0.f, 0.f, 1.f);
	glUniform3f(gfxMngr->boneScaleArrayID[0], 1.f, 1.f, 1.f);*
	// Reset the translucency value.
	glUniform1f(gfxMngr->alphaID, 1.f);

	// Find the first sprite with a valid renderable.
	// Bind its VAO and use its VBO to draw every other sprite.
	for(i = 0; i < allSprites->size; ++i){
		currentSpr = *((objInstance **)cvGet(allSprites, i));
		if(currentSpr != NULL){  ** Remove? **
			currentRndr = currentSpr->renderables;
			while(currentRndr != NULL){
				if(currentRndr->mdl != NULL){  ** Remove? **
					// Bind the VAO.
					glBindVertexArray(currentRndr->mdl->vaoID);
					vboID = currentRndr->mdl->vboID;
					goto RENDER_SPRITES;
				}
				currentRndr = moduleRenderableInstanceNext(currentRndr);
			}
		}
	}
	RENDER_SPRITES:

	while(i < allSprites->size){

		if(currentSpr != NULL){  ** Remove? **

			//rndrConfigRenderUpdate(&currentSpr->tempRndrConfig, interpT);

			while(currentRndr != NULL){

				if(currentRndr->mdl != NULL){  ** Remove? **

					twiGetFrameInfo(&currentRndr->twi, &texFrag[0], &texFrag[1], &texFrag[2], &texFrag[3], &currentTexID, interpT);

					// If the current texture ID differs from the last, render and clear the VBO.
					if((gfxMngr->lastTexID != currentTexID && currentVertexBatchSize >= **4**6) || currentVertexBatchSize >= SPRITE_MAX_BATCH_SIZE){
						// We need the texture width and height to get around texture offset calculations in the shader.
						** This shouldn't be necessary! **
						glBindTexture(GL_TEXTURE_2D, gfxMngr->lastTexID);
						glBindBuffer(GL_ARRAY_BUFFER, vboID);  ** Why does this need to be bound...? **
						glBufferData(GL_ARRAY_BUFFER, sizeof(gfxMngr->sprVertexBatchBuffer), &gfxMngr->sprVertexBatchBuffer[0], GL_DYNAMIC_DRAW);
						glDrawArrays(GL_TRIANGLES, 0, currentVertexBatchSize);
						currentVertexBatchSize = 0;
					}

					// Add sprite to the current batch.
					gfxMngr->lastTexID = currentTexID;
					objiGenerateSprite(currentSpr, currentRndr, interpT, texFrag, (vertex *)(&gfxMngr->sprVertexBatchBuffer[currentVertexBatchSize]));
					// Copy duplicates, since IBOs aren't working.
					gfxMngr->sprVertexBatchBuffer[currentVertexBatchSize+5] = gfxMngr->sprVertexBatchBuffer[currentVertexBatchSize];
					gfxMngr->sprVertexBatchBuffer[currentVertexBatchSize+4] = gfxMngr->sprVertexBatchBuffer[currentVertexBatchSize+2];
					gfxMngr->sprVertexBatchBuffer[currentVertexBatchSize+2] = gfxMngr->sprVertexBatchBuffer[currentVertexBatchSize+3];
					currentVertexBatchSize += **4**6;

				}

				currentRndr = moduleRenderableInstanceNext(currentRndr);

			}

		}

		++i;
		currentSpr = *((objInstance **)cvGet(allSprites, i));
		currentRndr = currentSpr->renderables;

	}

	// Renders the final batch if necessary.
	if(currentVertexBatchSize >= **4**6){
		** This STILL shouldn't be necessary! **
		glBindTexture(GL_TEXTURE_2D, gfxMngr->lastTexID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);  ** Why does this need to be bound...? **
		glBufferData(GL_ARRAY_BUFFER, sizeof(gfxMngr->sprVertexBatchBuffer), &gfxMngr->sprVertexBatchBuffer[0], GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, currentVertexBatchSize);
	}

}

void depthSortModels(cVector *allModels, cVector *mdlRenderList, const camera *cam){

	** Use quick sort and potentially some sort of linked list data structure? **

	cVector translucentModels; cvInit(&translucentModels, 1);  // Holds object pointers
	cVector distances;  cvInit(&distances, 1);  // Holds floats

	// Sort the different models into groups of those that are opaque and those that contain translucency
	size_t i;
	for(i = 0; i < allModels->size; ++i){

		objInstance *curMdl = *((objInstance **)cvGet(allModels, i));
		const gfxRenderGroup_t currentRenderGroup = 1;//objiRenderGroup(curMdl);

		if(currentRenderGroup == GFX_RENDER_GROUP_OPAQUE){
			cvPush(mdlRenderList, (void *)&curMdl, sizeof(objInstance *));
		}else if(currentRenderGroup == GFX_RENDER_GROUP_TRANSLUCENT){
			cvPush(&translucentModels, (void *)&curMdl, sizeof(objInstance *));
			float tempDistance = camDistance(cam, &curMdl->state.skeleton[0].position);
			cvPush(&distances, (void *)&tempDistance, sizeof(float));
		}

		// If currentRenderMethod is anything else (e.g. -1), the model will not be rendered at all

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

}**/

/** stateManager should be const. **/
/**void sortElements(stateManager *gameStateManager, const size_t stateID,
                  cVector *modelsScene,  cVector *modelsHUD,
                  cVector *spritesScene, cVector *spritesHUD){

	** Merge with camera updating? **
	**
	*** Use separate vectors for translucent objects, or render
	*** opaque objects after a glEnable(GL_CULL_FACE).
	**
	// Sort models and sprites into their scene and HUD vectors
	size_t i, j;
	for(i = 0; i < gameStateManager->objectType[SM_TYPE_CAMERA].capacity; ++i){
		if(camGetState(gameStateManager, i, stateID) != NULL){
			for(j = 0; j < camGetState(gameStateManager, i, stateID)->targetScene->objectNum; ++j){
				objInstance *curObj = objGetState(gameStateManager, camGetState(gameStateManager, i, stateID)->targetScene->objectIDs[j], stateID);
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

void rndrSortObjects(const size_t objectNum, objInstance **objects){

	//

}

void renderScene(graphicsManager *gfxMngr, scene *scn, camera *cam, const float interpT){

	* Update the camera's VP matrix. *
	camUpdateViewProjectionMatrix(
		cam,
		gfxMngr->windowModified,
		gfxMngr->windowAspectRatioX,
		gfxMngr->windowAspectRatioY,
		interpT
	);

	* Switch to the camera's view. *
	gfxMngrSwitchView(gfxMngr, &cam->view);

	* Find which zones should be rendered. *
	///

	* Prepare the scene for rendering. *
	scnSortObjects(scn);

	* Render the scene. *
	// Feed the view-projection matrix into the shader.
	glUniformMatrix4fv(gfxMngr->vpMatrixID, 1, GL_FALSE, &cam->viewProjectionMatrix.m[0][0]);
	// Render the scene's objects.
	for(i = 0; i < renderList.size; ++i){
		///
	}

	//glClear(GL_DEPTH_BUFFER_BIT);

}

** stateManager should be const. **
** This entire function should be part of the camera. **
**void renderScene(stateManager *gameStateManager, const size_t stateID, const float interpT, graphicsManager *gfxMngr){

	// Update cameras.
	size_t i;
	for(i = 0; i < gameStateManager->objectType[SM_TYPE_CAMERA].capacity; ++i){
		if(camGetState(gameStateManager, i, stateID) != NULL){
			camUpdateViewProjectionMatrix(camGetState(gameStateManager, i, stateID),
			                              gfxMngr->windowModified,
			                              gfxMngr->windowAspectRatioX,
			                              gfxMngr->windowAspectRatioY,
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
		glUniformMatrix4fv(gfxMngr->vpMatrixID, 1, GL_FALSE, &camGetState(gameStateManager, 0, stateID)->viewProjectionMatrix.m[0][0]);
		// Depth sort scene models.
		cVector renderList; cvInit(&renderList, 1);  // Holds model pointers; pointers to depth-sorted scene models that must be rendered.
		depthSortModels(&modelsScene, &renderList, camGetState(gameStateManager, 0, stateID));
		// Render scene models.
		for(i = 0; i < renderList.size; ++i){
			renderModel(*((objInstance **)cvGet(&renderList, i)), camGetState(gameStateManager, 0, stateID), interpT, gfxMngr);
		}
		// Batch render scene sprites.
		batchRenderSprites(&spritesScene, camGetState(gameStateManager, 0, stateID), interpT, gfxMngr);
		cvClear(&renderList);
	}

	//glClear(GL_DEPTH_BUFFER_BIT);

	// Render the HUD scene.
	if(camGetState(gameStateManager, 1, stateID) != NULL){
		// Feed the view-projection matrix into the shader.
		glUniformMatrix4fv(gfxMngr->vpMatrixID, 1, GL_FALSE, &camGetState(gameStateManager, 1, stateID)->viewProjectionMatrix.m[0][0]);
		// Render HUD models.
		** HUD camera? Streamline this to make handling different cameras easily **
		for(i = 0; i < modelsHUD.size; ++i){
			renderModel(*((objInstance **)cvGet(&modelsHUD, i)), camGetState(gameStateManager, 1, stateID), interpT, gfxMngr);
		}
		// Batch render HUD sprites.
		batchRenderSprites(&spritesHUD, camGetState(gameStateManager, 1, stateID), interpT, gfxMngr);
		cvClear(&modelsHUD);
	}

	cvClear(&modelsScene);
	cvClear(&spritesScene);
	cvClear(&spritesHUD);

}**/
