#include "gfxProgram.h"
#include "stateManagerHelpers.h"
#include <stdio.h>

#define RADIAN_RATIO 0.017453292  // = PI / 180, used for converting degrees to radians

/** stateManager should be const. **/
void renderScene(stateManager *gameStateManager, const size_t stateID, const float interpT, gfxProgram *gfxPrg);
void cleanup(gfxProgram *gfxPrg, stateManager *gameStateManager,
             cVector *allTextures, cVector *allTexWrappers,
             cVector *allSkeletons, cVector *allSklAnimations,
             cVector *allModels, cVector *allObjects);

/** Remember to do regular searches for these important comments when possible **/
int main(int argc, char *argv[]){

	char *prgPath = (char*)argv[0];
	prgPath[strrchr(prgPath, '\\') - prgPath + 1] = '\0';  // Removes program name (everything after the last backslash) from the path
	/** prgPath can be replaced with ".\\", but it may present some problems when running directly from Code::Blocks. **/

	gfxProgram gfxPrg;
	if(!gfxInitProgram(&gfxPrg, prgPath)){
		return 1;
	}

	/** Configs should be loaded here **/
	//

	/** Most of the code below this comment will be removed eventually **/
	stateManager gameStateManager;
	smInit(&gameStateManager);
	smObjectTypeNew(&gameStateManager, &scnInit,  &scnStateCopy,  &scnResetInterpolation,  &scnDelete,  sizeof(scene),  2, 1);
	smObjectTypeNew(&gameStateManager, &camInit,  &camStateCopy,  &camResetInterpolation,  &camDelete,  sizeof(camera), 2, 1);
	smObjectTypeNew(&gameStateManager, &objiInit, &objiStateCopy, &objiResetInterpolation, &objiDelete, sizeof(objInstance), 8, 1);

	cVector allTextures; cvInit(&allTextures, 1);            // Holds textures.
	cVector allTexWrappers; cvInit(&allTexWrappers, 1);      // Holds textureWrappers.
	cVector allSkeletons; cvInit(&allSkeletons, 1);          // Holds skeletons.
	cVector allSklAnimations; cvInit(&allSklAnimations, 1);  // Holds sklAnims.
	cVector allModels; cvInit(&allModels, 1);                // Holds models.
	cVector allObjects; cvInit(&allObjects, 1);              // Holds objects.

	/* Textures */
	texture tempTex;
	tDefault(&tempTex);
	cvPush(&allTextures, (void *)&tempTex, sizeof(tempTex));
	tLoad(&tempTex, prgPath, "Resources\\Images\\Misc\\Kobold.png");
	cvPush(&allTextures, (void *)&tempTex, sizeof(tempTex));
	tLoad(&tempTex, prgPath, "Resources\\Images\\Misc\\Avatar.png");
	cvPush(&allTextures, (void *)&tempTex, sizeof(tempTex));

	/* Skeletons */
	skeleton tempSkl;
	sklDefault(&tempSkl);
	cvPush(&allSkeletons, (void *)&tempSkl, sizeof(tempSkl));

	/* Texture Wrappers */
	textureWrapper tempTexWrap;
	twDefault(&tempTexWrap, &allTextures);
	cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));
	twLoad(&tempTexWrap, prgPath, "Resources\\Textures\\Static\\AvatarStatic.tdt", &allTextures);
	cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));
	twLoad(&tempTexWrap, prgPath, "Resources\\Textures\\Static\\KoboldStatic.tdt", &allTextures);
	cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));

	/* Models */
	model tempMdl;
	mdlDefault(&tempMdl, &allSkeletons);
	cvPush(&allModels, (void *)&tempMdl, sizeof(tempMdl));
	mdlCreateSprite(&tempMdl, &allSkeletons);
	cvPush(&allModels, (void *)&tempMdl, sizeof(tempMdl));

	/* Objects */
	object tempObj;
	objLoad(&tempObj, prgPath, "Resources\\Objects\\CubeTest.tdo", &allTextures, &allTexWrappers, &allSkeletons, &allSklAnimations, &allModels);
	cvPush(&allObjects, (void *)&tempObj, sizeof(tempObj));

	/* Object Instances */
	size_t tempID;
	smObjectNew(&gameStateManager, SM_TYPE_OBJECT, &tempID);
	objiInstantiate(objGetState(&gameStateManager, tempID, 0), (object *)cvGet(&allObjects, 0));
	objiChangeAnimation(objGetState(&gameStateManager, tempID, 0), 0, objGetState(&gameStateManager, tempID, 0)->base->animations[0], 0, 0.f);
	//objNewRenderable(objGetState(&gameStateManager, tempID, 0));
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl = (model *)cvGet(&allModels, 1);
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 1);
	//objInitSkeleton(objGetState(&gameStateManager, tempID, 0), (skeleton *)cvGet(&allSkeletons, 1));
	//
	smObjectNew(&gameStateManager, SM_TYPE_OBJECT, &tempID);
	objiInstantiate(objGetState(&gameStateManager, tempID, 0), (object *)cvGet(&allObjects, 0));
	//objiChangeAnimation(objGetState(&gameStateManager, tempID, 0), 0, objGetState(&gameStateManager, tempID, 0)->base->animations[0], 0, 0.f);
	//objNewRenderable(objGetState(&gameStateManager, tempID, 0));
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl = (model *)cvGet(&allModels, 1);
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 2);
	//objInitSkeleton(objGetState(&gameStateManager, tempID, 0), objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl->skl);
	objGetState(&gameStateManager, tempID, 0)->configuration[0].position.x = 0.5f;
	objGetState(&gameStateManager, tempID, 0)->configuration[0].position.y = 0.5f;
	vec3SetS(&objGetState(&gameStateManager, tempID, 0)->configuration[0].scale, 0.15f);
	quat changeRotation;
	quatSetEuler(&changeRotation, 45.f*RADIAN_RATIO, 45.f*RADIAN_RATIO, 0.f);
	quatMultQByQ2(&changeRotation, &objGetState(&gameStateManager, tempID, 0)->configuration[0].orientation);
	objGetState(&gameStateManager, tempID, 0)->tempRndrConfig.alpha.value = 0.5f;

	/* Sprite Object Instances */
	smObjectNew(&gameStateManager, SM_TYPE_OBJECT, &tempID);
	objiNewRenderable(objGetState(&gameStateManager, tempID, 0), (model *)cvGet(&allModels, 1), (textureWrapper *)cvGet(&allTexWrappers, 1));
	objiInitSkeleton(objGetState(&gameStateManager, tempID, 0), objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl->skl);
	objGetState(&gameStateManager, tempID, 0)->tempRndrConfig.sprite = 1;
	objGetState(&gameStateManager, tempID, 0)->configuration[0].scale.x = 0.026f;
	objGetState(&gameStateManager, tempID, 0)->configuration[0].scale.y = 0.026f;
	//
	smObjectNew(&gameStateManager, SM_TYPE_OBJECT, &tempID);
	objiNewRenderable(objGetState(&gameStateManager, tempID, 0), (model *)cvGet(&allModels, 1), (textureWrapper *)cvGet(&allTexWrappers, 1));
	objiInitSkeleton(objGetState(&gameStateManager, tempID, 0), objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl->skl);
	objGetState(&gameStateManager, tempID, 0)->tempRndrConfig.sprite = 1;
	objGetState(&gameStateManager, tempID, 0)->configuration[0].position.y = 1.f;
	objGetState(&gameStateManager, tempID, 0)->tempRndrConfig.pivot.value.x = -0.5f;
	objGetState(&gameStateManager, tempID, 0)->tempRndrConfig.pivot.value.y = 0.5f;
	objGetState(&gameStateManager, tempID, 0)->configuration[0].scale.x = 0.0026f;
	objGetState(&gameStateManager, tempID, 0)->configuration[0].scale.y = 0.0026f;
	//
	smObjectNew(&gameStateManager, SM_TYPE_OBJECT, &tempID);
	objiNewRenderable(objGetState(&gameStateManager, tempID, 0), (model *)cvGet(&allModels, 1), (textureWrapper *)cvGet(&allTexWrappers, 1));
	objiInitSkeleton(objGetState(&gameStateManager, tempID, 0), objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl->skl);
	objGetState(&gameStateManager, tempID, 0)->tempRndrConfig.sprite = 1;
	objGetState(&gameStateManager, tempID, 0)->configuration[0].position.x = 4.f;
	objGetState(&gameStateManager, tempID, 0)->configuration[0].position.z = -3.f;
	objGetState(&gameStateManager, tempID, 0)->configuration[0].scale.x = 0.026f;
	objGetState(&gameStateManager, tempID, 0)->configuration[0].scale.y = 0.026f;
	objGetState(&gameStateManager, tempID, 0)->tempRndrConfig.flags |= RNDR_BILLBOARD_TARGET | RNDR_BILLBOARD_Y;
	//
	smObjectNew(&gameStateManager, SM_TYPE_OBJECT, &tempID);
	objiNewRenderable(objGetState(&gameStateManager, tempID, 0), (model *)cvGet(&allModels, 1), (textureWrapper *)cvGet(&allTexWrappers, 2));
	objiInitSkeleton(objGetState(&gameStateManager, tempID, 0), objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl->skl);
	objGetState(&gameStateManager, tempID, 0)->tempRndrConfig.sprite = 1;
	objGetState(&gameStateManager, tempID, 0)->configuration[0].position.x = -3.f;
	objGetState(&gameStateManager, tempID, 0)->configuration[0].position.y = -2.f;
	objGetState(&gameStateManager, tempID, 0)->configuration[0].position.z = -3.f;
	objGetState(&gameStateManager, tempID, 0)->configuration[0].scale.x = 0.0085f;
	objGetState(&gameStateManager, tempID, 0)->configuration[0].scale.y = 0.0065f;

	/* Scenes */
	smObjectNew(&gameStateManager, SM_TYPE_SCENE, &tempID);
	scnGetState(&gameStateManager, tempID, 0)->objectNum = 4;
	scnGetState(&gameStateManager, tempID, 0)->objectCapacity = 4;
	scnGetState(&gameStateManager, tempID, 0)->objectIDs = malloc(scnGetState(&gameStateManager, tempID, 0)->objectCapacity * sizeof(size_t));
	scnGetState(&gameStateManager, tempID, 0)->objectIDs[0] = 0;
	scnGetState(&gameStateManager, tempID, 0)->objectIDs[1] = 2;
	scnGetState(&gameStateManager, tempID, 0)->objectIDs[2] = 4;
	scnGetState(&gameStateManager, tempID, 0)->objectIDs[3] = 5;
	//
	smObjectNew(&gameStateManager, SM_TYPE_SCENE, &tempID);
	scnGetState(&gameStateManager, tempID, 0)->objectNum = 2;
	scnGetState(&gameStateManager, tempID, 0)->objectCapacity = 2;
	scnGetState(&gameStateManager, tempID, 0)->objectIDs = malloc(scnGetState(&gameStateManager, tempID, 0)->objectCapacity * sizeof(size_t));
	scnGetState(&gameStateManager, tempID, 0)->objectIDs[0] = 1;
	scnGetState(&gameStateManager, tempID, 0)->objectIDs[1] = 3;

	/* Cameras */
	smObjectNew(&gameStateManager, SM_TYPE_CAMERA, &tempID);
	vec3Set(&camGetState(&gameStateManager, tempID, 0)->position.value, 0.f, 2.f, 7.f);
	camGetState(&gameStateManager, tempID, 0)->targetScene = (scene *)gameStateManager.objectType[SM_TYPE_SCENE].instance[0].state[0];
	//
	smObjectNew(&gameStateManager, SM_TYPE_CAMERA, &tempID);
	camGetState(&gameStateManager, tempID, 0)->flags |= CAM_PROJECTION_ORTHO;
	camGetState(&gameStateManager, tempID, 0)->targetScene = (scene *)gameStateManager.objectType[SM_TYPE_SCENE].instance[1].state[0];


	signed char prgRunning = 1;

	float globalTimeMod = 1.f;
	float framerate = 1000.f / 125.f;  // Desired renders per millisecond
	float tickrate = 1000.f / 125.f;  // Desired updates per millisecond
	float tickratio = tickrate / 1000.f;  // 1 / updates per second.

	float nextUpdate = 0.f;
	float nextRender = 0.f;

	uint32_t updates = 0;
	uint32_t renders = 0;
	uint32_t lastPrint = 0;

	SDL_Event prgEventHandler;

	signed char UP    = 0;
	signed char DOWN  = 0;
	signed char LEFT  = 0;
	signed char RIGHT = 0;

	signed char lockMouse = 0;
	int mouseRelX;
	int mouseRelY;

	size_t i;

    while(prgRunning){

		gfxUpdateWindow(&gfxPrg);


		/* Take input */
		/** Use input queuing system. **/
		// Detect input
		SDL_PollEvent(&prgEventHandler);

		// Exit
		if(prgEventHandler.type == SDL_QUIT){
			prgRunning = 0;
		}

		// Key presses
		if(prgEventHandler.type == SDL_KEYDOWN){
			if(prgEventHandler.key.keysym.sym == SDLK_ESCAPE){
				prgRunning = 0;
			}
			if(prgEventHandler.key.keysym.sym == SDLK_l){
				lockMouse = !lockMouse;
				SDL_SetRelativeMouseMode(lockMouse);
			}
			if(prgEventHandler.key.keysym.sym == SDLK_UP){
				UP = 1;
			}
			if(prgEventHandler.key.keysym.sym == SDLK_DOWN){
				DOWN = 1;
			}
			if(prgEventHandler.key.keysym.sym == SDLK_LEFT){
				LEFT = 1;
			}
			if(prgEventHandler.key.keysym.sym == SDLK_RIGHT){
				RIGHT = 1;
			}
		}

		// Key releases
		if(prgEventHandler.type == SDL_KEYUP){
			if(prgEventHandler.key.keysym.sym == SDLK_UP){
				UP = 0;
			}
			if(prgEventHandler.key.keysym.sym == SDLK_DOWN){
				DOWN = 0;
			}
			if(prgEventHandler.key.keysym.sym == SDLK_LEFT){
				LEFT = 0;
			}
			if(prgEventHandler.key.keysym.sym == SDLK_RIGHT){
				RIGHT = 0;
			}
		}

		// Get mouse position relative to its position in the last call
		SDL_GetRelativeMouseState(&mouseRelX, &mouseRelY);


		const float updateStart = (float)SDL_GetTicks();
		while(updateStart >= nextUpdate){

			/* Prepare the next game state. */
			smPrepareNextState(&gameStateManager);

			/* Handle inputs */
			if(UP){
				globalTimeMod = 1.f;
				/** changeRotation is created to initialize the second renderable. **/
				quatSetEuler(&changeRotation, -90.f*RADIAN_RATIO, 0.f, 0.f);
				quatRotate(&objGetState(&gameStateManager, 0, 0)->configuration[0].orientation, &changeRotation, tickratio, &objGetState(&gameStateManager, 0, 0)->configuration[0].orientation);
				camGetState(&gameStateManager, 0, 0)->position.value.z += -5.f * tickratio;
				objGetState(&gameStateManager, 4, 0)->tempRndrConfig.targetPosition.value = camGetState(&gameStateManager, 0, 0)->position.value;
			}
			if(DOWN){
				globalTimeMod = -1.f;
				/** changeRotation is created to initialize the second renderable. **/
				quatSetEuler(&changeRotation, 90.f*RADIAN_RATIO, 0.f, 0.f);
				quatRotate(&objGetState(&gameStateManager, 0, 0)->configuration[0].orientation, &changeRotation, tickratio, &objGetState(&gameStateManager, 0, 0)->configuration[0].orientation);
				camGetState(&gameStateManager, 0, 0)->position.value.z += 5.f * tickratio;
				objGetState(&gameStateManager, 4, 0)->tempRndrConfig.targetPosition.value = camGetState(&gameStateManager, 0, 0)->position.value;
			}
			if(LEFT){
				/** changeRotation is created to initialize the second renderable. **/
				quatSetEuler(&changeRotation, 0.f, -90.f*RADIAN_RATIO, 0.f);
				quatRotate(&objGetState(&gameStateManager, 0, 0)->configuration[0].orientation, &changeRotation, tickratio, &objGetState(&gameStateManager, 0, 0)->configuration[0].orientation);
				quatSetEuler(&changeRotation, 0.f, 0.f, -90.f*RADIAN_RATIO);
				quatRotate(&objGetState(&gameStateManager, 3, 0)->configuration[0].orientation, &changeRotation, tickratio, &objGetState(&gameStateManager, 3, 0)->configuration[0].orientation);
				camGetState(&gameStateManager, 0, 0)->position.value.x += -5.f * tickratio;
				objGetState(&gameStateManager, 4, 0)->tempRndrConfig.targetPosition.value = camGetState(&gameStateManager, 0, 0)->position.value;
			}
			if(RIGHT){
				/** changeRotation is created to initialize the second renderable. **/
				quatSetEuler(&changeRotation, 0.f, 90.f*RADIAN_RATIO, 0.f);
				quatRotate(&objGetState(&gameStateManager, 0, 0)->configuration[0].orientation, &changeRotation, tickratio, &objGetState(&gameStateManager, 0, 0)->configuration[0].orientation);
				quatSetEuler(&changeRotation, 0.f, 0.f, 90.f*RADIAN_RATIO);
				quatRotate(&objGetState(&gameStateManager, 3, 0)->configuration[0].orientation, &changeRotation, tickratio, &objGetState(&gameStateManager, 3, 0)->configuration[0].orientation);
				camGetState(&gameStateManager, 0, 0)->position.value.x += 5.f * tickratio;
				objGetState(&gameStateManager, 4, 0)->tempRndrConfig.targetPosition.value = camGetState(&gameStateManager, 0, 0)->position.value;
			}

			/* Animate */
			/** Could be merged with the update function but animating should be done in here. **/
			for(i = 0; i < gameStateManager.objectType[SM_TYPE_OBJECT].capacity; ++i){
				/* Update the renderable for rendering */
				if(objGetState(&gameStateManager, i, 0) != NULL){
					objiUpdate(objGetState(&gameStateManager, i, 0), camGetState(&gameStateManager, 0, 0), tickrate*globalTimeMod, tickratio);
				}
			}

			/* Next frame */
			nextUpdate += tickrate;
			++updates;

		}


		/* Render the scene */
		const float renderStart = (float)SDL_GetTicks();
		if(renderStart >= nextRender){

			/* Progress between current and next frame. */
			float interpT = (renderStart - (nextUpdate - tickrate)) / tickrate;
			if(interpT < 0.f){
				interpT = 0.f;
			}else if(interpT > 1.f){
				interpT = 1.f;
			}

			/* Render */
			/** Remove later **/
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// Render the scene
			renderScene(&gameStateManager, 0, interpT, &gfxPrg);
			// Update the window
			SDL_GL_SwapWindow(gfxPrg.window);

			/* Next frame */
			nextRender = renderStart + framerate;
			++renders;

		}


		if(SDL_GetTicks() - lastPrint > 1000){
			printf("Updates: %u\n", updates);
			printf("Renders: %u\n", renders);
			lastPrint = SDL_GetTicks();
			updates = 0;
			renders = 0;
		}

    }


	cleanup(&gfxPrg, &gameStateManager, &allTextures, &allTexWrappers, &allSkeletons, &allSklAnimations, &allModels, &allObjects);
	return 0;

}

void cleanup(gfxProgram *gfxPrg, stateManager *gameStateManager,
             cVector *allTextures, cVector *allTexWrappers,
             cVector *allSkeletons, cVector *allSklAnimations,
             cVector *allModels, cVector *allObjects){

	size_t i;
	for(i = 0; i < allTextures->size; ++i){
		tDelete((texture *)cvGet(allTextures, i));
	}
	cvClear(allTextures);

	for(i = 0; i < allTexWrappers->size; ++i){
		twDelete((textureWrapper *)cvGet(allTexWrappers, i));
	}
	cvClear(allTexWrappers);

	for(i = 0; i < allSkeletons->size; ++i){
		sklDelete((skeleton *)cvGet(allSkeletons, i));
	}
	cvClear(allSkeletons);

	for(i = 0; i < allSklAnimations->size; ++i){
		sklaDelete((sklAnim *)cvGet(allSklAnimations, i));
	}
	cvClear(allSklAnimations);

	for(i = 0; i < allModels->size; ++i){
		mdlDelete((model *)cvGet(allModels, i));
	}
	cvClear(allModels);

	for(i = 0; i < allObjects->size; ++i){
		objDelete((object *)cvGet(allObjects, i));
	}
	cvClear(allObjects);

	smDelete(gameStateManager);
	gfxDestroyProgram(gfxPrg);

}
