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
void renderModel(const object *const restrict obj, const float distance, const camera *const restrict cam, const float interpT, graphicsManager *const restrict gfxMngr){

	const rndrInstance *currentRndr = obj->renderables;

	mat4 *transformCurrent = gfxMngr->sklTransformState;
	const bone *bCurrent = obj->state.skeleton;
	const bone *bPrevious = (obj->state.previous == NULL ? bCurrent : obj->state.previous->skeleton);
	const bone *const bLast = &bCurrent[obj->skeletonData.skl->boneNum];

	// Update the object's configuration for rendering.
	//rndrConfigRenderUpdate(&obj->tempRndrConfig, interpT);  /** Only line that requires non-const object. **/

	// Interpolate between the previous and last skeleton states.
	for(; bCurrent < bLast; ++bCurrent, ++bPrevious, ++transformCurrent){

		// Interpolate between bone states and
		// convert the interpolated bone to a
		// matrix for the shader.
		*transformCurrent = boneMatrix(
			boneInterpolate(*bPrevious, *bCurrent, interpT)
		);

	}

	// Draw each renderable.
	while(currentRndr != NULL){

		// Get texture information for rendering and feed it to the shader.
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

				const boneIndex_t boneNum = currentRndr->mdl->skl->boneNum;

				boneIndex_t i;

				sklNode *nLayout = currentRndr->mdl->skl->bones;
				GLuint *bArray = gfxMngr->boneArrayID;
				vec3 *bAccumulator = gfxMngr->sklBindAccumulator;

				// If there is a valid animated skeleton, apply animation transformations.
				for(i = 0; i < boneNum; ++i, ++bAccumulator, ++bArray, ++nLayout){

					const boneIndex_t rndrBone = sklFindBone(obj->skeletonData.skl, i, nLayout->name);

					// Accumulate the bind positions. We need to use global bone offsets.
					// Also make sure the bone's parent isn't itself (that is, make sure it has a parent).
					if(nLayout->parent < boneNum && i != nLayout->parent){
						// Apply the parent's bind offsets.
						*bAccumulator = gfxMngr->sklBindAccumulator[nLayout->parent];
					}else{
						vec3ZeroP(bAccumulator);
					}

					// If the animated bone is in the model, pass in its animation transforms.
					/** Use a lookup, same in object.c. **/
					if(rndrBone < obj->skeletonData.skl->boneNum){

						mat4 transform = gfxMngr->sklTransformState[rndrBone];

						// Rotate the bind pose position by the current bone's orientation
						// and add this offset to the bind pose accumulator. Treats the
						// vector as a bra vector so that column-major multiplication is
						// invoked. OpenGL prefers column-major matrices.
						const vec4 translation = mat4MMultNKet(transform,
						                                       nLayout->defaultState.position.x,
						                                       nLayout->defaultState.position.y,
						                                       nLayout->defaultState.position.z,
						                                       0.f);
						bAccumulator->x += translation.x;
						bAccumulator->y += translation.y;
						bAccumulator->z += translation.z;

						// Translate the bone by the inverse of the accumulated bind translations.
						transform.m[3][0] -= bAccumulator->x;
						transform.m[3][1] -= bAccumulator->y;
						transform.m[3][2] -= bAccumulator->z;

						// Feed the bone configuration to the shader.
						glUniformMatrix4fv(*bArray, 1, GL_FALSE, &transform.m[0][0]);

					}else{
						// Otherwise pass in an identity bone.
						glUniformMatrix4fv(*bArray, 1, GL_FALSE, &gfxMngr->identityMatrix.m[0][0]);
					}

				}

				// Feed the translucency multiplier to the shader
				glUniform1f(gfxMngr->alphaID, alpha);

				// Render the model.
				glBindVertexArray(currentRndr->mdl->vaoID);
				if(currentRndr->mdl->indexNum > 0){
					GLsizei indexNum;
					const void *offset;
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

	object *currentSpr;
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
		currentSpr = *((object **)cvGet(allSprites, i));
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
					objGenerateSprite(currentSpr, currentRndr, interpT, texFrag, (vertex *)(&gfxMngr->sprVertexBatchBuffer[currentVertexBatchSize]));
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
		currentSpr = *((object **)cvGet(allSprites, i));
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

		object *curMdl = *((object **)cvGet(allModels, i));
		const gfxRenderGroup_t currentRenderGroup = 1;//objRenderGroup(curMdl);

		if(currentRenderGroup == GFX_RENDER_GROUP_OPAQUE){
			cvPush(mdlRenderList, (void *)&curMdl, sizeof(object *));
		}else if(currentRenderGroup == GFX_RENDER_GROUP_TRANSLUCENT){
			cvPush(&translucentModels, (void *)&curMdl, sizeof(object *));
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
				object *curObj = objGetState(gameStateManager, camGetState(gameStateManager, i, stateID)->targetScene->objectIDs[j], stateID);
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

void rndrSortObjects(const size_t objectNum, object **objects){

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
			renderModel(*((object **)cvGet(&renderList, i)), camGetState(gameStateManager, 0, stateID), interpT, gfxMngr);
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
			renderModel(*((object **)cvGet(&modelsHUD, i)), camGetState(gameStateManager, 1, stateID), interpT, gfxMngr);
		}
		// Batch render HUD sprites.
		batchRenderSprites(&spritesHUD, camGetState(gameStateManager, 1, stateID), interpT, gfxMngr);
		cvClear(&modelsHUD);
	}

	cvClear(&modelsScene);
	cvClear(&spritesScene);
	cvClear(&spritesHUD);

}**/
