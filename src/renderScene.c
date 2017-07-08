#include "renderable.h"
#include "camera.h"
#include <math.h>

/** THIS FILE IS TEMPORARY **/
/** THIS FILE IS TEMPORARY **/
/** THIS FILE IS TEMPORARY **/
/** THIS FILE IS TEMPORARY **/
/** THIS FILE IS TEMPORARY **/
/** THIS FILE IS TEMPORARY **/
/** THIS FILE IS TEMPORARY **/

/** This should not be necessary! **/
void renderModel(renderable *rndr, gfxProgram *gfxPrg, camera *cam){

	/* Get texture information for rendering */
	float texFrag[4];  // The x, y, width and height of the fragment of the texture being rendered
	GLuint frameTexID;
	twiGetFrameInfo(&rndr->tex, &texFrag[0], &texFrag[1], &texFrag[2], &texFrag[3], &frameTexID);
	/* Bind the texture (if needed) */
	glActiveTexture(GL_TEXTURE0);
	if(frameTexID != gfxPrg->lastTexID){
		gfxPrg->lastTexID = frameTexID;
		glBindTexture(GL_TEXTURE_2D, frameTexID);
	}
	/* Feed the texture coordinates to the shader */
	glUniform4fv(gfxPrg->textureFragmentID, 1, texFrag);

	/* Generate a new skeleton state and feed it to the shader */
	/*rndrGenerateSkeletonState(rndr);
	size_t d;
	for(d = 0; d < rndrBoneNum(rndr); d++){
		// Feed position
		glUniform3f(gfxPrg->bonePositionArrayID[d], rndr->sklState[d].position.x,
		                                            rndr->sklState[d].position.y,
		                                            rndr->sklState[d].position.z);
		// Feed orientation
		glUniform4f(gfxPrg->boneOrientationArrayID[d], rndr->sklState[d].orientation.w,
		                                               rndr->sklState[d].orientation.v.x,
		                                               rndr->sklState[d].orientation.v.y,
		                                               rndr->sklState[d].orientation.v.z);
	}*/

	//sklGenerateState(&rndr->skl);
	//sklFeedStateToShader();

	/* Feed the translucency value to the shader */
	glUniform1f(gfxPrg->alphaID, rndr->rTrans.alpha);

	/* Feed the MVP matrix to the shader */
	mat4 mvpMatrix;
	rndrGenerateTransform(rndr, &mvpMatrix, gfxPrg, cam);
	glUniformMatrix4fv(gfxPrg->mvpMatrixID, 1, GL_FALSE, &mvpMatrix.m[0][0]);

	/* Render the model */
	glBindVertexArray(rndr->mdl->vaoID);
	if(rndr->mdl->indexNum > 0){
		glDrawElements(GL_TRIANGLES, rndr->mdl->indexNum, GL_UNSIGNED_INT, (void *)0);
	}else{
		glDrawArrays(GL_TRIANGLES, 0, rndr->mdl->vertexNum);
	}

}

/** Clean this up! **/
void batchRenderSprites(cVector *allSprites, gfxProgram *gfxPrg, camera *cam){

	// Reset the translucency value
	glUniform1f(gfxPrg->alphaID, 1);
	// Bind the VAO
	glBindVertexArray((*((renderable **)cvGet(allSprites, 0)))->mdl->vaoID);

	mat4 mvMatrix;
	float texFrag[4];
	GLuint texWidth;
	GLuint texHeight;
	GLuint currentTexID;
	vertex currentVertexBatch[allSprites->size * /**4**/6];
	size_t currentVertexBatchSize = 0;
	/**size_t currentIndexBatch[allSprites->size * 6];
	size_t currentIndexBatchSize = 0;**/

	renderable *curSpr;
	size_t d;
	for(d = 0; d < allSprites->size; d++){

		curSpr = *((renderable **)cvGet(allSprites, d));

		if(curSpr != NULL){

			twiGetFrameInfo(&curSpr->tex, &texFrag[0], &texFrag[1], &texFrag[2], &texFrag[3], &currentTexID);

			// If the current texture ID differs from the last, render and clear the VBO
			if(gfxPrg->lastTexID != currentTexID && currentVertexBatchSize >= /**4**/6){
				// We need the texture width and height to get around texture offset calculations in the shader
				/** This shouldn't be necessary! **/
				glUniform4f(gfxPrg->textureFragmentID, 0.f, 0.f, texWidth, texHeight);

				glBindTexture(GL_TEXTURE_2D, gfxPrg->lastTexID);
				glBindBuffer(GL_ARRAY_BUFFER, curSpr->mdl->vboID);  /** Why does this need to be bound...? **/
				glBufferData(GL_ARRAY_BUFFER, sizeof(currentVertexBatch), &currentVertexBatch[0], GL_DYNAMIC_DRAW);
				/**glBindBuffer(GL_ARRAY_BUFFER, curSpr->mdl->iboID);
				glBufferData(GL_ARRAY_BUFFER, sizeof(currentIndexBatch), &currentIndexBatch[0], GL_DYNAMIC_DRAW);
				glDrawElements(GL_TRIANGLES, currentIndexBatchSize, GL_UNSIGNED_INT, (void *)0);**/
				glDrawArrays(GL_TRIANGLES, 0, currentVertexBatchSize);
				currentVertexBatchSize = 0;
				/**currentIndexBatchSize = 0;**/
			}

			// Get the texture's width and height for calculating the texture's UV offsets outside of the shader
			texWidth  = twiGetTexWidth(&curSpr->tex);
			texHeight = twiGetTexHeight(&curSpr->tex);

			// Add sprite to the current batch
			gfxPrg->lastTexID = currentTexID;
			rndrGenerateTransform(curSpr, &mvMatrix, gfxPrg, cam);
			rndrGenerateSprite(curSpr, (vertex *)(&currentVertexBatch[0]+currentVertexBatchSize), &mvMatrix, gfxPrg);
			rndrOffsetSpriteTexture((vertex *)(&currentVertexBatch[0]+currentVertexBatchSize), texFrag, texWidth, texHeight);
			currentVertexBatch[currentVertexBatchSize+5] = currentVertexBatch[currentVertexBatchSize];
			currentVertexBatch[currentVertexBatchSize+4] = currentVertexBatch[currentVertexBatchSize+2];
			currentVertexBatch[currentVertexBatchSize+2] = currentVertexBatch[currentVertexBatchSize+3];
			currentVertexBatchSize += /**4**/6;
			/**currentIndexBatch[currentIndexBatchSize]   = 0;
			currentIndexBatch[currentIndexBatchSize+1] = 1;
			currentIndexBatch[currentIndexBatchSize+2] = 3;
			currentIndexBatch[currentIndexBatchSize+3] = 3;
			currentIndexBatch[currentIndexBatchSize+4] = 2;
			currentIndexBatch[currentIndexBatchSize+5] = 0;
			currentIndexBatchSize += 6;**/

		}

	}

	// Renders the final batch if necessary
	if(currentVertexBatchSize >= /**4**/6){
		/** This STILL shouldn't be necessary! **/
		glUniform4f(gfxPrg->textureFragmentID, 0.f, 0.f, texWidth, texHeight);

		glBindTexture(GL_TEXTURE_2D, gfxPrg->lastTexID);
		glBindBuffer(GL_ARRAY_BUFFER, curSpr->mdl->vboID);  /** Why does this need to be bound...? **/
		glBufferData(GL_ARRAY_BUFFER, sizeof(currentVertexBatch), &currentVertexBatch[0], GL_DYNAMIC_DRAW);
		/**glBindBuffer(GL_ARRAY_BUFFER, curSpr->mdl->iboID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(currentIndexBatch), &currentIndexBatch[0], GL_DYNAMIC_DRAW);
		glDrawElements(GL_TRIANGLES, currentIndexBatchSize, GL_UNSIGNED_INT, (void *)0);**/
		glDrawArrays(GL_TRIANGLES, 0, currentVertexBatchSize);
	}

}

void depthSortModels(cVector *allModels, cVector *mdlRenderList, camera *cam){

	cVector translucentModels; cvInit(&translucentModels, 1);  // Holds renderable pointers
	cVector distances;  cvInit(&distances, 1);  // Holds floats

	// Sort the different models into groups of those that are opaque and those that contain translucency
	size_t d;
	for(d = 0; d < allModels->size; d++){

		renderable *curMdl = *((renderable **)cvGet(allModels, d));
		unsigned int currentRenderMethod = rndrRenderMethod(curMdl);

		if(currentRenderMethod == 0){  // If the model is fully opaque, add it straight to the render list
			cvPush(mdlRenderList, (void *)&curMdl, sizeof(renderable *));
		}else if(currentRenderMethod == 1){  // If the model contains translucency, it'll need to be depth sorted
			cvPush(&translucentModels, (void *)&curMdl, sizeof(renderable *));
			float tempDistance = (sqrt(abs((curMdl->sTrans.position.x - cam->position.x) * (curMdl->sTrans.position.x - cam->position.x) +
			                               (curMdl->sTrans.position.y - cam->position.y) * (curMdl->sTrans.position.y - cam->position.y) +
			                               (curMdl->sTrans.position.z - cam->position.z) * (curMdl->sTrans.position.z - cam->position.z))));
			cvPush(&distances, (void *)&tempDistance, sizeof(float));
		}
		// If currentRenderMethod is anything else (e.g. 2), the model will not be rendered at all

	}


	// Simple bubblesort (for now) to sort models with translucency by depth
	for(d = 0; d < translucentModels.size; d++){
		size_t f;
		for(f = 1; f < translucentModels.size - d; f++){

			if(*((float *)cvGet(&distances, f-1)) < *((float *)cvGet(&distances, f))){

				float tempDistance = *((float *)cvGet(&distances, f-1));
				cvSet(&distances, f-1, cvGet(&distances, f), sizeof(float));
				cvSet(&distances, f, (void *)&tempDistance, sizeof(float));

				renderable *tempModel = *((renderable **)cvGet(&translucentModels, f-1));
				cvSet(&translucentModels, f-1, cvGet(&translucentModels, f), sizeof(renderable *));
				cvSet(&translucentModels, f, (void *)&tempModel, sizeof(renderable *));

			}

		}
	}


	// Combine the three vectors
	cvResize(mdlRenderList, mdlRenderList->size + translucentModels.size);
	for(d = 0; d < translucentModels.size; d++){
		cvPush(mdlRenderList, cvGet(&translucentModels, d), sizeof(renderable *));
	}
	cvClear(&translucentModels);
	cvClear(&distances);

}

void sortElements(cVector *allRenderables,
                  cVector *modelsScene,  cVector *modelsHUD,
                  cVector *spritesScene, cVector *spritesHUD){

	// Sort models and sprites into their scene and HUD vectors
	size_t d;
	for(d = 0; d < allRenderables->size; d++){
		renderable *curRndr = (renderable *)cvGet(allRenderables, d);
		if(!curRndr->sprite){
			if(curRndr->hudElement){
				cvPush(modelsHUD, (void *)&curRndr, sizeof(renderable *));
			}else{
				cvPush(modelsScene, (void *)&curRndr, sizeof(renderable *));
			}
		}else{
			if(curRndr->hudElement){
				cvPush(spritesHUD, (void *)&curRndr, sizeof(renderable *));
			}else{
				cvPush(spritesScene, (void *)&curRndr, sizeof(renderable *));
			}
		}
	}

}

void renderScene(cVector *allRenderables, gfxProgram *gfxPrg, camera *cam){

	// Vector initialization
	cVector modelsScene;  cvInit(&modelsScene, 1);   // Holds renderable pointers; pointers to scene models
	cVector modelsHUD;    cvInit(&modelsHUD, 1);     // Holds renderable pointers; pointers to HUD models
	cVector spritesScene; cvInit(&spritesScene, 1);  // Holds renderable pointers; pointers to scene sprites
	cVector spritesHUD;   cvInit(&spritesHUD, 1);    // Holds renderable pointers; pointers to HUD sprites
	sortElements(allRenderables, &modelsScene, &modelsHUD, &spritesScene, &spritesHUD);

	// Depth sort scene models
	cVector renderList; cvInit(&renderList, 1);  // Holds model pointers; pointers to depth-sorted scene models that must be rendered
	depthSortModels(&modelsScene, &renderList, cam);
	// Render scene models
	size_t d;
	for(d = 0; d < renderList.size; d++){
		renderModel(*((renderable **)cvGet(&renderList, d)), gfxPrg, cam);
	}
	// Batch render scene sprites
	// Change the MVP matrix to the frustum projection matrix, as other sprite vertex transformations are done on the CPU through sprCreate()
	glUniformMatrix4fv(gfxPrg->mvpMatrixID, 1, GL_FALSE, &gfxPrg->projectionMatrixFrustum.m[0][0]);
	batchRenderSprites(&spritesScene, gfxPrg, cam);


	glClear(GL_DEPTH_BUFFER_BIT);

	// Render HUD models
	for(d = 0; d < modelsHUD.size; d++){
		renderModel(*((renderable **)cvGet(&modelsHUD, d)), gfxPrg, cam);
	}
	// Batch render HUD sprites
	// Change the MVP matrix to the orthographic projection matrix, as other sprite vertex transformations are done on the CPU through sprCreate()
	glUniformMatrix4fv(gfxPrg->mvpMatrixID, 1, GL_FALSE, &gfxPrg->projectionMatrixOrtho.m[0][0]);
	batchRenderSprites(&spritesHUD, gfxPrg, cam);

	cvClear(&renderList);
	cvClear(&modelsScene);
	cvClear(&modelsHUD);
	cvClear(&spritesScene);
	cvClear(&spritesHUD);

}
