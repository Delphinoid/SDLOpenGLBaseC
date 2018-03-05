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
	mat4 mvpMatrix;

	/* Update the object's configuration for rendering. */
	rndrConfigRenderUpdate(&obj->configuration, interpT);  /** Only line that requires non-const object. **/

	/* Interpolate between the previous and last skeleton states. */
	if(obj->skl != NULL){  /** Remove? **/
		for(i = 0; i < obj->skl->boneNum; ++i){
			// If the bone has been simulated, use its position and orientation.
			if((obj->physicsState[i].flags & PHYSICS_BODY_SIMULATE) > 0){
				/**
				obj->skeletonState[0][i].position    = obj->physicsState[i].position;
				obj->skeletonState[0][i].orientation = obj->physicsState[i].orientation;
				**/
			}
			boneInterpolate(&obj->skeletonState[1][i], &obj->skeletonState[0][i], interpT, &gfxPrg->sklAnimationState[i]);
		}
	}

	/* Feed the translucency multiplier to the shader */
	glUniform1f(gfxPrg->alphaID, obj->configuration.alpha.render);

	/* Generate the MVP matrix and feed it to the shader. */
	rndrConfigGenerateTransform(&obj->configuration, cam, &mvpMatrix);
	mat4MultMByM1(&mvpMatrix, &cam->projectionMatrix);
	glUniformMatrix4fv(gfxPrg->mvpMatrixID, 1, GL_FALSE, &mvpMatrix.m[0][0]);

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
				bone identity;
				boneInit(&identity);
				if(obj->skl != NULL){
					// If there is a valid animated skeleton, apply animation transformations.
					for(j = 0; j < obj->renderables[i].mdl->skl->boneNum; ++j){
						size_t animBone = sklFindBone(obj->skl, obj->renderables[i].mdl->skl->bones[j].name);
						if(animBone < obj->skl->boneNum){
							// If the animated bone is in the model, pass in its animation transforms.
							glUniform3f(gfxPrg->bonePositionArrayID[j], gfxPrg->sklAnimationState[animBone].position.x,
							                                            gfxPrg->sklAnimationState[animBone].position.y,
							                                            gfxPrg->sklAnimationState[animBone].position.z);
							glUniform4f(gfxPrg->boneOrientationArrayID[j], gfxPrg->sklAnimationState[animBone].orientation.v.x,
							                                               gfxPrg->sklAnimationState[animBone].orientation.v.y,
							                                               gfxPrg->sklAnimationState[animBone].orientation.v.z,
							                                               gfxPrg->sklAnimationState[animBone].orientation.w);
							glUniform3f(gfxPrg->boneScaleArrayID[j], gfxPrg->sklAnimationState[animBone].scale.x,
							                                         gfxPrg->sklAnimationState[animBone].scale.y,
							                                         gfxPrg->sklAnimationState[animBone].scale.z);
						}else{
							// Otherwise pass in an identity bone.
							glUniform3f(gfxPrg->bonePositionArrayID[j], identity.position.x, identity.position.y, identity.position.z);
							glUniform4f(gfxPrg->boneOrientationArrayID[j], identity.orientation.v.x, identity.orientation.v.y, identity.orientation.v.z, identity.orientation.w);
							glUniform3f(gfxPrg->boneScaleArrayID[j], identity.scale.x, identity.scale.y, identity.scale.z);
						}
						//skliGenerateBoneStateFromGlobal(&gfxPrg->sklAnimationState[0], obj->skl, obj->renderables[i].mdl->skl, &gfxPrg->sklTransformBuffer[0], j);
						//glUniformMatrix4fv(gfxPrg->boneArrayID[j], 1, GL_FALSE, &gfxPrg->sklTransformBuffer[j].m[0][0]);
					}
				}else{
					// If there is no animated skeleton, pass in some identity bones.
					for(j = 0; j < obj->renderables[i].mdl->skl->boneNum; ++j){
						glUniform3f(gfxPrg->bonePositionArrayID[j], identity.position.x, identity.position.y, identity.position.z);
						glUniform4f(gfxPrg->boneOrientationArrayID[j], identity.orientation.v.x, identity.orientation.v.y, identity.orientation.v.z, identity.orientation.w);
						glUniform3f(gfxPrg->boneScaleArrayID[j], identity.scale.x, identity.scale.y, identity.scale.z);
						//skliGenerateDefaultState(obj->renderables[i].mdl->skl, &gfxPrg->sklTransformBuffer[0], j);
						//glUniformMatrix4fv(gfxPrg->boneArrayID[j], 1, GL_FALSE, &gfxPrg->sklTransformBuffer[j].m[0][0]);
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
	mat4 mvMatrix;
	float texFrag[4];
	GLuint texWidth;
	GLuint texHeight;
	GLuint currentTexID;
	GLuint vboID;
	size_t currentVertexBatchSize = 0;
	/**size_t currentIndexBatch[allSprites->size * 6];
	size_t currentIndexBatchSize = 0;**/

	// Find the first sprite with a valid renderable.
	// Bind its VAO and use its VBO to draw every other sprite.
	for(i = 0; i < allSprites->size; ++i){
		curSpr = *((object **)cvGet(allSprites, i));
		if(curSpr != NULL){  /** Remove? **/
			for(j = 0; j < curSpr->renderableNum; ++j){
				if(curSpr->renderables[j].mdl != NULL){  /** Remove? **/
					// Bind the VAO
					glBindVertexArray((*((object **)cvGet(allSprites, i)))->renderables[j].mdl->vaoID);
					vboID = (*((object **)cvGet(allSprites, i)))->renderables[j].mdl->vboID;
					goto EXIT;
				}
			}
		}
	}
	EXIT:

	// Reset the translucency value
	glUniform1f(gfxPrg->alphaID, 1.f);

	while(i < allSprites->size){

		curSpr = *((object **)cvGet(allSprites, i));

		if(curSpr != NULL){  /** Remove? **/

			rndrConfigRenderUpdate(&curSpr->configuration, interpT);
			rndrConfigGenerateTransform(&curSpr->configuration, cam, &mvMatrix);

			while(j < curSpr->renderableNum){

				if(curSpr->renderables[j].mdl != NULL){  /** Remove? **/

					twiGetFrameInfo(&curSpr->renderables[j].twi, &texFrag[0], &texFrag[1], &texFrag[2], &texFrag[3], &currentTexID, interpT);

					// If the current texture ID differs from the last, render and clear the VBO
					if((gfxPrg->lastTexID != currentTexID && currentVertexBatchSize >= /**4**/6) || currentVertexBatchSize >= SPR_MAX_BATCH_SIZE){
						// We need the texture width and height to get around texture offset calculations in the shader
						/** This shouldn't be necessary! **/
						glUniform4f(gfxPrg->textureFragmentID, 0.f, 0.f, texWidth, texHeight);

						glBindTexture(GL_TEXTURE_2D, gfxPrg->lastTexID);
						glBindBuffer(GL_ARRAY_BUFFER, vboID);  /** Why does this need to be bound...? **/
						glBufferData(GL_ARRAY_BUFFER, sizeof(gfxPrg->sprVertexBatchBuffer), &gfxPrg->sprVertexBatchBuffer[0], GL_DYNAMIC_DRAW);
						/**glBindBuffer(GL_ARRAY_BUFFER, curSpr->renderables[j].mdl->iboID);
						glBufferData(GL_ARRAY_BUFFER, sizeof(currentIndexBatch), &currentIndexBatch[0], GL_DYNAMIC_DRAW);
						glDrawElements(GL_TRIANGLES, currentIndexBatchSize, GL_UNSIGNED_INT, (void *)0);**/
						glDrawArrays(GL_TRIANGLES, 0, currentVertexBatchSize);
						currentVertexBatchSize = 0;
						/**currentIndexBatchSize = 0;**/
					}

					// Get the texture's width and height for calculating the texture's UV offsets outside of the shader
					texWidth  = twiGetTexWidth(&curSpr->renderables[j].twi);
					texHeight = twiGetTexHeight(&curSpr->renderables[j].twi);

					// Add sprite to the current batch
					gfxPrg->lastTexID = currentTexID;
					rndrConfigGenerateSprite(&curSpr->configuration, &curSpr->renderables[j].twi, (vertex *)(&gfxPrg->sprVertexBatchBuffer[currentVertexBatchSize]), &mvMatrix);
					rndrConfigOffsetSpriteTexture((vertex *)(&gfxPrg->sprVertexBatchBuffer[currentVertexBatchSize]), texFrag, texWidth, texHeight);
					gfxPrg->sprVertexBatchBuffer[currentVertexBatchSize+5] = gfxPrg->sprVertexBatchBuffer[currentVertexBatchSize];
					gfxPrg->sprVertexBatchBuffer[currentVertexBatchSize+4] = gfxPrg->sprVertexBatchBuffer[currentVertexBatchSize+2];
					gfxPrg->sprVertexBatchBuffer[currentVertexBatchSize+2] = gfxPrg->sprVertexBatchBuffer[currentVertexBatchSize+3];
					currentVertexBatchSize += /**4**/6;
					/**currentIndexBatch[currentIndexBatchSize]   = 0;
					currentIndexBatch[currentIndexBatchSize+1] = 1;
					currentIndexBatch[currentIndexBatchSize+2] = 3;
					currentIndexBatch[currentIndexBatchSize+3] = 3;
					currentIndexBatch[currentIndexBatchSize+4] = 2;
					currentIndexBatch[currentIndexBatchSize+5] = 0;
					currentIndexBatchSize += 6;**/

				}

				++j;

			}

			j = 0;

		}

		++i;

	}

	// Renders the final batch if necessary
	if(currentVertexBatchSize >= /**4**/6){
		/** This STILL shouldn't be necessary! **/
		glUniform4f(gfxPrg->textureFragmentID, 0.f, 0.f, texWidth, texHeight);

		glBindTexture(GL_TEXTURE_2D, gfxPrg->lastTexID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);  /** Why does this need to be bound...? **/
		glBufferData(GL_ARRAY_BUFFER, sizeof(gfxPrg->sprVertexBatchBuffer), &gfxPrg->sprVertexBatchBuffer[0], GL_DYNAMIC_DRAW);
		/**glBindBuffer(GL_ARRAY_BUFFER, curSpr->mdl->iboID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(currentIndexBatch), &currentIndexBatch[0], GL_DYNAMIC_DRAW);
		glDrawElements(GL_TRIANGLES, currentIndexBatchSize, GL_UNSIGNED_INT, (void *)0);**/
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
			float tempDistance = (sqrt(abs((curMdl->configuration.position.render.x - cam->position.render.x) * (curMdl->configuration.position.render.x - cam->position.render.x) +
			                               (curMdl->configuration.position.render.y - cam->position.render.y) * (curMdl->configuration.position.render.y - cam->position.render.y) +
			                               (curMdl->configuration.position.render.z - cam->position.render.z) * (curMdl->configuration.position.render.z - cam->position.render.z))));
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
					if(!curObj->configuration.sprite){
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
void renderScene(stateManager *gameStateManager, const size_t stateID, const float interpT, gfxProgram *gfxPrg){

	// Update cameras.
	size_t i;
	for(i = 0; i < gameStateManager->objectType[SM_TYPE_CAMERA].capacity; ++i){
		if(camGetState(gameStateManager, i, stateID) != NULL){
			camUpdateViewMatrix(camGetState(gameStateManager, i, stateID), interpT);
			if(gfxPrg->windowChanged){
				// If the window size changed, update the camera projection matrices as well.
				camGetState(gameStateManager, i, stateID)->flags |= CAM_UPDATE_PROJECTION;
			}
			camUpdateProjectionMatrix(camGetState(gameStateManager, i, stateID), gfxPrg->aspectRatioX, gfxPrg->aspectRatioY, interpT);
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
		// Depth sort scene models.
		cVector renderList; cvInit(&renderList, 1);  // Holds model pointers; pointers to depth-sorted scene models that must be rendered.
		depthSortModels(&modelsScene, &renderList, camGetState(gameStateManager, 0, stateID), interpT);
		// Render scene models.
		for(i = 0; i < renderList.size; ++i){
			renderModel(*((object **)cvGet(&renderList, i)), camGetState(gameStateManager, 0, stateID), interpT, gfxPrg);
		}
		// Batch render scene sprites.
		// Change the MVP matrix to the frustum projection matrix, as other sprite vertex transformations are done on the CPU through sprCreate().
		glUniformMatrix4fv(gfxPrg->mvpMatrixID, 1, GL_FALSE, &camGetState(gameStateManager, 0, stateID)->projectionMatrix.m[0][0]);
		batchRenderSprites(&spritesScene, camGetState(gameStateManager, 0, stateID), interpT, gfxPrg);
		cvClear(&renderList);
	}

	//glClear(GL_DEPTH_BUFFER_BIT);

	// Render the HUD scene.
	if(camGetState(gameStateManager, 1, stateID) != NULL){
		// Render HUD models.
		/** HUD camera? Streamline this to make handling different cameras easily **/
		for(i = 0; i < modelsHUD.size; ++i){
			renderModel(*((object **)cvGet(&modelsHUD, i)), camGetState(gameStateManager, 1, stateID), interpT, gfxPrg);
		}
		// Batch render HUD sprites.
		// Change the MVP matrix to the orthographic projection matrix, as other sprite vertex transformations are done on the CPU through sprCreate().
		glUniformMatrix4fv(gfxPrg->mvpMatrixID, 1, GL_FALSE, &camGetState(gameStateManager, 1, stateID)->projectionMatrix.m[0][0]);
		batchRenderSprites(&spritesHUD, camGetState(gameStateManager, 1, stateID), interpT, gfxPrg);
		cvClear(&modelsHUD);
	}

	cvClear(&modelsScene);
	cvClear(&spritesScene);
	cvClear(&spritesHUD);

}
