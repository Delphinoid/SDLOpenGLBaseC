#include "gfxProgram.h"
#include "sprite.h"
#include "camera.h"
#include "model.h"
#include "math.h"

void batchRenderSprites(gfxProgram *gfxPrg, camera *cam, cVector *allSprites){

	// Reset the translucency value
	glUniform1f(gfxPrg->alphaID, 1);
	// Bind the VAO and VBO
	glBindVertexArray(gfxPrg->spriteVaoID);
	glBindBuffer(GL_ARRAY_BUFFER, gfxPrg->spriteVboID);

	float texFrag[4];
	GLuint texWidth;
	GLuint texHeight;
	GLuint currentTexID;
	vertex3D currentBatch[allSprites->size * 6];
	unsigned int currentBatchSize = 0;

	unsigned int d;
	for(d = 0; d < allSprites->size; d++){

		sprite *curSpr = *((sprite **)cvGet(allSprites, d));

		if(curSpr != NULL){

			twGetFrameInfo(curSpr->texture, curSpr->currentAnim, curSpr->currentFrame,
			               &texFrag[0], &texFrag[1], &texFrag[2], &texFrag[3], &currentTexID);

			// If the current texture ID differs from the last, render and clear the VBO
			if(gfxPrg->lastTexID != currentTexID && currentBatchSize >= 6){
				// We need the texture width and height to get around texture offset calculations in the shader
				/** This shouldn't be necessary! **/
				glUniform4f(gfxPrg->textureFragmentID, 0.f, 0.f, texWidth, texHeight);

				glBufferData(GL_ARRAY_BUFFER, sizeof(currentBatch), &currentBatch[0], GL_DYNAMIC_DRAW);
				glBindTexture(GL_TEXTURE_2D, gfxPrg->lastTexID);
				glDrawArrays(GL_TRIANGLES, 0, currentBatchSize);
				currentBatchSize = 0;
			}

			// Get the texture's width and height for calculating the texture's UV offsets outside of the shader
			texWidth = twGetTexWidth(curSpr->texture, curSpr->currentAnim, curSpr->currentFrame);
			texHeight = twGetTexHeight(curSpr->texture, curSpr->currentAnim, curSpr->currentFrame);

			// Add sprite to the current batch
			gfxPrg->lastTexID = currentTexID;
			sprCreate(curSpr, gfxPrg, cam);
			sprOffsetTexture(curSpr, texFrag, texWidth, texHeight);
			currentBatch[currentBatchSize]   = curSpr->vertices[0];
			currentBatch[currentBatchSize+1] = curSpr->vertices[1];
			currentBatch[currentBatchSize+2] = curSpr->vertices[3];
			currentBatch[currentBatchSize+3] = curSpr->vertices[3];
			currentBatch[currentBatchSize+4] = curSpr->vertices[2];
			currentBatch[currentBatchSize+5] = curSpr->vertices[0];
			currentBatchSize += 6;

		}

	}

	// Renders the final batch if necessary
	if(currentBatchSize >= 6){
		/** This STILL shouldn't be necessary! **/
		glUniform4f(gfxPrg->textureFragmentID, 0.f, 0.f, texWidth, texHeight);

		glBufferData(GL_ARRAY_BUFFER, sizeof(currentBatch), &currentBatch[0], GL_DYNAMIC_DRAW);
		glBindTexture(GL_TEXTURE_2D, gfxPrg->lastTexID);
		glDrawArrays(GL_TRIANGLES, 0, currentBatchSize);
	}

}

void depthSortModels(camera *cam, cVector *allModels, cVector *mdlRenderList){

	cVector translucentModels; cvInit(&translucentModels, 1);  // Holds model pointers
	cVector distances;  cvInit(&distances, 1);  // Holds floats

	// Sort the different models into groups of those that are opaque and those that contain translucency
	unsigned int d;
	for(d = 0; d < allModels->size; d++){

		model *curMdl = *((model **)cvGet(allModels, d));
		unsigned int currentRenderMethod = mdlRenderMethod(curMdl);

		if(currentRenderMethod == 0){  // If the model is fully opaque, add it straight to the render list
			cvPush(mdlRenderList, (void *)&curMdl, sizeof(*curMdl));
		}else if(currentRenderMethod == 1){  // If the model contains translucency, it'll need to be depth sorted
			cvPush(&translucentModels, (void *)&curMdl, sizeof(*curMdl));
			float tempDistance = (sqrt(abs((curMdl->position.x - cam->position.x) * (curMdl->position.x - cam->position.x) +
			                               (curMdl->position.y - cam->position.y) * (curMdl->position.y - cam->position.y) +
			                               (curMdl->position.z - cam->position.z) * (curMdl->position.z - cam->position.z))));
			cvPush(&distances, (void *)&tempDistance, sizeof(float));
		}
		// If currentRenderMethod is anything else (e.g. 2), the model will not be rendered at all

	}


	// Simple bubblesort (for now) to sort models with translucency by depth
	for(d = 0; d < translucentModels.size; d++){
		unsigned int f;
		for(f = 1; f < translucentModels.size - d; f++){

			if(*((float *)cvGet(&distances, f-1)) < *((float *)cvGet(&distances, f))){

				float tempDistance = *((float *)cvGet(&distances, f-1));
				cvSet(&distances, f-1, cvGet(&distances, f), sizeof(float));
				cvSet(&distances, f, (void *)&tempDistance, sizeof(float));

				model *tempModel = *((model **)cvGet(&translucentModels, f-1));
				cvSet(&translucentModels, f-1, cvGet(&translucentModels, f), sizeof(model *));
				cvSet(&translucentModels, f, (void *)&tempModel, sizeof(model *));

			}

		}
	}


	// Combine the three vectors
	cvResize(mdlRenderList, mdlRenderList->size + translucentModels.size);
	for(d = 0; d < translucentModels.size; d++){
		cvPush(mdlRenderList, cvGet(&translucentModels, d), sizeof(model *));
	}
	cvClear(&translucentModels);
	cvClear(&distances);

}

void sortElements(cVector *allModels,  cVector *modelsScene,  cVector *modelsHUD,
                  cVector *allSprites, cVector *spritesScene, cVector *spritesHUD){

	// Sort models into scene and HUD vectors
	unsigned int d;
	for(d = 0; d < allModels->size; d++){
		model *curMdl = (model *)cvGet(allModels, d);
		if(curMdl->hudElement){
			cvPush(modelsHUD, (void *)&curMdl, sizeof(model *));
		}else{
			cvPush(modelsScene, (void *)&curMdl, sizeof(model *));
		}
	}

	// Sort sprites into scene and HUD vectors
	for(d = 0; d < allSprites->size; d++){
		sprite *curSpr = (sprite *)cvGet(allSprites, d);
		if(curSpr->hudElement){
			cvPush(spritesHUD, (void *)&curSpr, sizeof(sprite *));
		}else{
			cvPush(spritesScene, (void *)&curSpr, sizeof(sprite *));
		}
	}

}

void renderScene(gfxProgram *gfxPrg, camera *cam, cVector *allModels, cVector *allSprites){

	// Vector initialization
	cVector modelsScene;  cvInit(&modelsScene, 1);   // Holds model pointers; pointers to scene models
	cVector modelsHUD;    cvInit(&modelsHUD, 1);     // Holds model pointers; pointers to HUD models
	cVector spritesScene; cvInit(&spritesScene, 1);  // Holds sprite pointers; pointers to scene sprites
	cVector spritesHUD;   cvInit(&spritesHUD, 1);    // Holds sprite pointers; pointers to HUD sprites
	sortElements(allModels, &modelsScene, &modelsHUD, allSprites, &spritesScene, &spritesHUD);


	// Depth sort scene models
	cVector renderList; cvInit(&renderList, 1);  // Holds model pointers; pointers to depth-sorted scene models that must be rendered
	depthSortModels(cam, &modelsScene, &renderList);
	// Render scene models
	unsigned int d;
	for(d = 0; d < renderList.size; d++){
		mdlRender(*((model **)cvGet(&renderList, d)), gfxPrg, cam);
	}
	// Batch render scene sprites
	// Change the MVP matrix to the frustum projection matrix, as other sprite vertex transformations are done on the CPU through sprCreate()
	glUniformMatrix4fv(gfxPrg->mvpMatrixID, 1, GL_FALSE, &gfxPrg->projectionMatrixFrustum.m[0][0]);
	batchRenderSprites(gfxPrg, cam, &spritesScene);


	glClear(GL_DEPTH_BUFFER_BIT);

	// Render HUD models
	for(d = 0; d < modelsHUD.size; d++){
		mdlRender(*((model **)cvGet(&modelsHUD, d)), gfxPrg, cam);
	}
	// Batch render HUD sprites
	// Change the MVP matrix to the orthographic projection matrix, as other sprite vertex transformations are done on the CPU through sprCreate()
	glUniformMatrix4fv(gfxPrg->mvpMatrixID, 1, GL_FALSE, &gfxPrg->projectionMatrixOrtho.m[0][0]);
	batchRenderSprites(gfxPrg, cam, &spritesHUD);


	cvClear(&renderList);
	cvClear(&modelsScene);
	cvClear(&modelsHUD);
	cvClear(&spritesScene);
	cvClear(&spritesHUD);

}
