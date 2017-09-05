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
void renderModel(renderable *rndr, const camera *cam, gfxProgram *gfxPrg){

	// Get texture information for rendering
	float texFrag[4];  // The x, y, width and height of the fragment of the texture being rendered
	GLuint frameTexID;
	twiGetFrameInfo(&rndr->twi, &texFrag[0], &texFrag[1], &texFrag[2], &texFrag[3], &frameTexID);
	// Bind the texture (if needed)
	glActiveTexture(GL_TEXTURE0);
	if(frameTexID != gfxPrg->lastTexID){
		gfxPrg->lastTexID = frameTexID;
		glBindTexture(GL_TEXTURE_2D, frameTexID);
	}
	// Feed the texture coordinates to the shader
	glUniform4fv(gfxPrg->textureFragmentID, 1, texFrag);

	/** **/
	skeleton *skel = malloc(sizeof(skeleton));
	skel->root = malloc(sizeof(sklNode));
	skel->root->name = malloc(5*sizeof(char));
	memcpy(skel->root->name, "root\0", 5);
	skel->root->defaultState.position = vec3New(0.5f, -1.f, 0.5f);
	skel->root->defaultState.orientation = quatNew(1.f, 0.f, 0.f, 0.f);
	skel->root->defaultState.scale = vec3New(1.f, 1.f, 1.f);
	skel->root->parent = NULL;
	skel->root->childNum = 1;
	skel->root->children = malloc(sizeof(sklNode));
	skel->root->children[0].parent = skel->root;
	skel->root->children[0].childNum = 0;
	skel->root->children[0].name = malloc(4*sizeof(char));
	memcpy(skel->root->children[0].name, "top\0", 4);
	skel->root->children[0].defaultState.position = vec3New(0.f, 2.f, 0.f);
	skel->root->children[0].defaultState.orientation = quatNew(1.f, 0.f, 0.f, 0.f);
	skel->root->children[0].defaultState.scale = vec3New(1.f, 1.f, 1.f);
	skel->boneNum = 2;
	mat4 *skeletonState = malloc(2*sizeof(mat4));

	/* Feed the skeleton state to the shader */
	if(rndr->skli.skl != NULL){
		// Generate a state for the model skeleton, transformed into the animated skeleton's space
		skliGenerateState(&rndr->skli, skeletonState, skel);
		size_t i;
		for(i = 0; i < rndr->skli.skl->boneNum; ++i){
			/*mat4 boneMatrixTemp;
			memcpy(&boneMatrixTemp.m[0][0], &rndr->skli.skeletonState[i].m[0][0], sizeof(boneMatrixTemp));*/
			/** Translates each skeleton bone by the inverse model's default bone positions **/
			/*if(i==0){
				mat4Translate(&boneMatrixTemp, -mdlBonesTemp[0].x, -mdlBonesTemp[0].y, -mdlBonesTemp[0].z);
			}else if(i==1){
				mat4 boneTranslationTemp = mat4TranslationMatrix(-mdlBonesTemp[0].x, -mdlBonesTemp[0].y, -mdlBonesTemp[0].z);
				mat4MultMByM1(&boneMatrixTemp, &boneTranslationTemp);
				mat4Translate(&boneMatrixTemp, -mdlBonesTemp[1].x, -mdlBonesTemp[1].y, -mdlBonesTemp[1].z);
			}*/
			glUniformMatrix4fv(gfxPrg->boneArrayID[i], 1, GL_FALSE, &skeletonState[i].m[0][0]);
		}
	}

	/** **/
	free(skeletonState);
	free(skel->root->children[0].name);
	free(skel->root->children);
	free(skel->root->name);
	free(skel->root);
	free(skel);

	/* Feed the translucency multiplier to the shader */
	glUniform1f(gfxPrg->alphaID, rndr->rTrans.alpha);

	/* Generate the MVP matrix and feed it to the shader */
	mat4 mvpMatrix;
	rndrGenerateTransform(rndr, cam, &mvpMatrix);
	mat4MultMByM1(&mvpMatrix, &cam->projectionMatrix);
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
void batchRenderSprites(cVector *allSprites, const camera *cam, gfxProgram *gfxPrg){

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
	size_t i;
	for(i = 0; i < allSprites->size; ++i){

		curSpr = *((renderable **)cvGet(allSprites, i));

		if(curSpr != NULL){

			twiGetFrameInfo(&curSpr->twi, &texFrag[0], &texFrag[1], &texFrag[2], &texFrag[3], &currentTexID);

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
			texWidth  = twiGetTexWidth(&curSpr->twi);
			texHeight = twiGetTexHeight(&curSpr->twi);

			// Add sprite to the current batch
			gfxPrg->lastTexID = currentTexID;
			rndrGenerateTransform(curSpr, cam, &mvMatrix);
			rndrGenerateSprite(curSpr, (vertex *)(&currentVertexBatch[currentVertexBatchSize]), &mvMatrix);
			rndrOffsetSpriteTexture((vertex *)(&currentVertexBatch[currentVertexBatchSize]), texFrag, texWidth, texHeight);
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

void depthSortModels(cVector *allModels, cVector *mdlRenderList, const camera *cam){

	cVector translucentModels; cvInit(&translucentModels, 1);  // Holds renderable pointers
	cVector distances;  cvInit(&distances, 1);  // Holds floats

	// Sort the different models into groups of those that are opaque and those that contain translucency
	size_t i;
	for(i = 0; i < allModels->size; ++i){

		renderable *curMdl = *((renderable **)cvGet(allModels, i));
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
	size_t j;
	for(i = 0; i < translucentModels.size; ++i){
		for(j = 1; j < translucentModels.size - i; ++j){

			if(*((float *)cvGet(&distances, j-1)) < *((float *)cvGet(&distances, j))){

				float tempDistance = *((float *)cvGet(&distances, j-1));
				cvSet(&distances, j-1, cvGet(&distances, j), sizeof(float));
				cvSet(&distances, j, (void *)&tempDistance, sizeof(float));

				renderable *tempModel = *((renderable **)cvGet(&translucentModels, j-1));
				cvSet(&translucentModels, j-1, cvGet(&translucentModels, j), sizeof(renderable *));
				cvSet(&translucentModels, j, (void *)&tempModel, sizeof(renderable *));

			}

		}
	}


	// Combine the three vectors
	cvResize(mdlRenderList, mdlRenderList->size + translucentModels.size);
	for(i = 0; i < translucentModels.size; ++i){
		cvPush(mdlRenderList, cvGet(&translucentModels, i), sizeof(renderable *));
	}
	cvClear(&translucentModels);
	cvClear(&distances);

}

void sortElements(cVector *allRenderables,
                  cVector *modelsScene,  cVector *modelsHUD,
                  cVector *spritesScene, cVector *spritesHUD){

	// Sort models and sprites into their scene and HUD vectors
	size_t i;
	for(i = 0; i < allRenderables->size; ++i){
		renderable *curRndr = (renderable *)cvGet(allRenderables, i);
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

void renderScene(cVector *allRenderables, cVector *allCameras, gfxProgram *gfxPrg){

	// Vector initialization
	cVector modelsScene;  cvInit(&modelsScene, 1);   // Holds renderable pointers; pointers to scene models
	cVector modelsHUD;    cvInit(&modelsHUD, 1);     // Holds renderable pointers; pointers to HUD models
	cVector spritesScene; cvInit(&spritesScene, 1);  // Holds renderable pointers; pointers to scene sprites
	cVector spritesHUD;   cvInit(&spritesHUD, 1);    // Holds renderable pointers; pointers to HUD sprites
	sortElements(allRenderables, &modelsScene, &modelsHUD, &spritesScene, &spritesHUD);

	// Depth sort scene models
	cVector renderList; cvInit(&renderList, 1);  // Holds model pointers; pointers to depth-sorted scene models that must be rendered
	depthSortModels(&modelsScene, &renderList, (camera *)cvGet(allCameras, 0));
	// Render scene models
	size_t i;
	for(i = 0; i < renderList.size; ++i){
		renderModel(*((renderable **)cvGet(&renderList, i)), (camera *)cvGet(allCameras, 0), gfxPrg);
	}
	// Batch render scene sprites
	// Change the MVP matrix to the frustum projection matrix, as other sprite vertex transformations are done on the CPU through sprCreate()
	glUniformMatrix4fv(gfxPrg->mvpMatrixID, 1, GL_FALSE, &((camera *)cvGet(allCameras, 0))->projectionMatrix.m[0][0]);
	batchRenderSprites(&spritesScene, (camera *)cvGet(allCameras, 0), gfxPrg);


	//glClear(GL_DEPTH_BUFFER_BIT);

	// Render HUD models
	/** HUD camera? Streamline this to make handling different cameras easily **/
	for(i = 0; i < modelsHUD.size; ++i){
		renderModel(*((renderable **)cvGet(&modelsHUD, i)), (camera *)cvGet(allCameras, 1), gfxPrg);
	}
	// Batch render HUD sprites
	// Change the MVP matrix to the orthographic projection matrix, as other sprite vertex transformations are done on the CPU through sprCreate()
	glUniformMatrix4fv(gfxPrg->mvpMatrixID, 1, GL_FALSE, &((camera *)cvGet(allCameras, 1))->projectionMatrix.m[0][0]);
	batchRenderSprites(&spritesHUD, (camera *)cvGet(allCameras, 1), gfxPrg);

	cvClear(&renderList);
	cvClear(&modelsScene);
	cvClear(&modelsHUD);
	cvClear(&spritesScene);
	cvClear(&spritesHUD);

}
