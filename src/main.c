#include "stateManagerHelpers.h"
#include <stdio.h>

#define RADIAN_RATIO 0.017453292  // = PI / 180, used for converting degrees to radians

/** stateManager should be const. **/
void renderScene(stateManager *gameStateManager, const size_t stateID, const float interpT, gfxProgram *gfxPrg);
void cleanup(gfxProgram *gfxPrg,    stateManager *gameStateManager,
             cVector *allTextures,  cVector *allTexWrappers, cVector *allModels,
             cVector *allSkeletons, cVector *allSklAnimations);

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
	smInit(&gameStateManager, 2);
	smObjectTypeNew(&gameStateManager, &rndrInit, &rndrNew, &rndrStateCopy, &rndrResetInterpolation, &rndrDelete, sizeof(renderable), 6);
	smObjectTypeNew(&gameStateManager, &scnInit,  &scnInit, &scnStateCopy,  &scnResetInterpolation,  &scnDelete,  sizeof(scene),      2);
	smObjectTypeNew(&gameStateManager, &camInit,  &camNew,  &camStateCopy,  &camResetInterpolation,  &camDelete,  sizeof(camera),     2);

	cVector allTextures; cvInit(&allTextures, 1);            // Holds textures
	cVector allTexWrappers; cvInit(&allTexWrappers, 1);      // Holds textureWrappers
	cVector allModels; cvInit(&allModels, 1);                // Holds models
	cVector allSkeletons; cvInit(&allSkeletons, 1);          // Holds skeletons
	cVector allSklAnimations; cvInit(&allSklAnimations, 1);  // Holds sklAnims

	/* Textures */
	texture tempTex;
	tLoad(&tempTex, prgPath, "Resources\\Images\\Guy\\Guy.png");
	cvPush(&allTextures, (void *)&tempTex, sizeof(tempTex));
	tLoad(&tempTex, prgPath, "Resources\\Images\\Misc\\Kobold.png");
	cvPush(&allTextures, (void *)&tempTex, sizeof(tempTex));
	tLoad(&tempTex, prgPath, "Resources\\Images\\Misc\\Avatar.png");
	cvPush(&allTextures, (void *)&tempTex, sizeof(tempTex));

	/* Texture Wrappers */
	textureWrapper tempTexWrap;
	twLoad(&tempTexWrap, prgPath, "Resources\\Textures\\Static\\AvatarStatic.tdt", &allTextures);
	cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));
	twLoad(&tempTexWrap, prgPath, "Resources\\Textures\\Animated\\GuySpr.tdt", &allTextures);
	cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));
	twLoad(&tempTexWrap, prgPath, "Resources\\Textures\\Animated\\GuyMulti.tdt", &allTextures);
	cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));
	twLoad(&tempTexWrap, prgPath, "Resources\\Textures\\Static\\KoboldStatic.tdt", &allTextures);
	cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));

	/* Models */
	model tempMdl;
	mdlCreateSprite(&tempMdl, "sprite");
	cvPush(&allModels, (void *)&tempMdl, sizeof(tempMdl));
	mdlLoad(&tempMdl, prgPath, "Resources\\Models\\CubeTest.obj");
	cvPush(&allModels, (void *)&tempMdl, sizeof(tempMdl));

	/* Renderables */
	size_t tempID;
	smObjectNew(&gameStateManager, SM_TYPE_RENDERABLE, &tempID);
	rndrGetState(&gameStateManager, tempID, 0)->mdl = (model *)cvGet(&allModels, 1);
	rndrGetState(&gameStateManager, tempID, 0)->twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 1);
	//
	smObjectNew(&gameStateManager, SM_TYPE_RENDERABLE, &tempID);
	rndrGetState(&gameStateManager, tempID, 0)->mdl = (model *)cvGet(&allModels, 1);
	rndrGetState(&gameStateManager, tempID, 0)->twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 2);
	rndrGetState(&gameStateManager, tempID, 0)->position.value.x = 0.5f;
	rndrGetState(&gameStateManager, tempID, 0)->position.value.y = 0.5f;
	vec3SetS(&rndrGetState(&gameStateManager, tempID, 0)->scale.value, 0.15f);
	quat changeRotation;
	quatSetEuler(&changeRotation, 45.f, 45.f, 0.f);
	quatMultQByQ2(&changeRotation, &rndrGetState(&gameStateManager, tempID, 0)->orientation.value);
	rndrGetState(&gameStateManager, tempID, 0)->alpha.value = 0.5f;

	/* Sprite Renderables */
	smObjectNew(&gameStateManager, SM_TYPE_RENDERABLE, &tempID);
	rndrGetState(&gameStateManager, tempID, 0)->sprite = 1;
	rndrGetState(&gameStateManager, tempID, 0)->mdl = (model *)cvGet(&allModels, 0);
	rndrGetState(&gameStateManager, tempID, 0)->twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 0);
	rndrGetState(&gameStateManager, tempID, 0)->pivot.value.x = 0.5f;
	rndrGetState(&gameStateManager, tempID, 0)->pivot.value.y = 0.5f;
	rndrGetState(&gameStateManager, tempID, 0)->scale.value.x = 0.026f;
	rndrGetState(&gameStateManager, tempID, 0)->scale.value.y = 0.026f;
	//
	smObjectNew(&gameStateManager, SM_TYPE_RENDERABLE, &tempID);
	rndrInit(rndrGetState(&gameStateManager, tempID, 0));
	rndrGetState(&gameStateManager, tempID, 0)->sprite = 1;
	rndrGetState(&gameStateManager, tempID, 0)->mdl = (model *)cvGet(&allModels, 0);
	rndrGetState(&gameStateManager, tempID, 0)->twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 0);
	rndrGetState(&gameStateManager, tempID, 0)->position.value.y = 1.f;
	rndrGetState(&gameStateManager, tempID, 0)->pivot.value.y = 1.f;
	rndrGetState(&gameStateManager, tempID, 0)->scale.value.x = 0.0026f;
	rndrGetState(&gameStateManager, tempID, 0)->scale.value.y = 0.0026f;
	//
	smObjectNew(&gameStateManager, SM_TYPE_RENDERABLE, &tempID);
	rndrInit(rndrGetState(&gameStateManager, tempID, 0));
	rndrGetState(&gameStateManager, tempID, 0)->sprite = 1;
	rndrGetState(&gameStateManager, tempID, 0)->mdl = (model *)cvGet(&allModels, 0);
	rndrGetState(&gameStateManager, tempID, 0)->twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 0);
	rndrGetState(&gameStateManager, tempID, 0)->position.value.x = 4.f;
	rndrGetState(&gameStateManager, tempID, 0)->position.value.z = -3.f;
	rndrGetState(&gameStateManager, tempID, 0)->pivot.value.x = 0.5f;
	rndrGetState(&gameStateManager, tempID, 0)->pivot.value.y = 0.5f;
	rndrGetState(&gameStateManager, tempID, 0)->scale.value.x = 0.026f;
	rndrGetState(&gameStateManager, tempID, 0)->scale.value.y = 0.026f;
	rndrGetState(&gameStateManager, tempID, 0)->flags |= RNDR_BILLBOARD_TARGET | RNDR_BILLBOARD_Y;
	//
	smObjectNew(&gameStateManager, SM_TYPE_RENDERABLE, &tempID);
	rndrInit(rndrGetState(&gameStateManager, tempID, 0));
	rndrGetState(&gameStateManager, tempID, 0)->sprite = 1;
	rndrGetState(&gameStateManager, tempID, 0)->mdl = (model *)cvGet(&allModels, 0);
	rndrGetState(&gameStateManager, tempID, 0)->twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 3);
	rndrGetState(&gameStateManager, tempID, 0)->position.value.x = -3.f;
	rndrGetState(&gameStateManager, tempID, 0)->position.value.y = -2.f;
	rndrGetState(&gameStateManager, tempID, 0)->position.value.z = -3.f;
	rndrGetState(&gameStateManager, tempID, 0)->pivot.value.x = 0.5f;
	rndrGetState(&gameStateManager, tempID, 0)->pivot.value.y = 0.5f;
	rndrGetState(&gameStateManager, tempID, 0)->scale.value.x = 0.0085f;
	rndrGetState(&gameStateManager, tempID, 0)->scale.value.y = 0.0065f;

	/* Scenes */
	smObjectNew(&gameStateManager, SM_TYPE_SCENE, &tempID);
	scnGetState(&gameStateManager, tempID, 0)->renderableNum = 4;
	scnGetState(&gameStateManager, tempID, 0)->renderableCapacity = 4;
	scnGetState(&gameStateManager, tempID, 0)->renderableIDs = malloc(scnGetState(&gameStateManager, tempID, 0)->renderableNum * sizeof(size_t));
	scnGetState(&gameStateManager, tempID, 0)->renderableIDs[0] = 0;
	scnGetState(&gameStateManager, tempID, 0)->renderableIDs[1] = 2;
	scnGetState(&gameStateManager, tempID, 0)->renderableIDs[2] = 4;
	scnGetState(&gameStateManager, tempID, 0)->renderableIDs[3] = 5;
	//
	smObjectNew(&gameStateManager, SM_TYPE_SCENE, &tempID);
	scnGetState(&gameStateManager, tempID, 0)->renderableNum = 2;
	scnGetState(&gameStateManager, tempID, 0)->renderableCapacity = 2;
	scnGetState(&gameStateManager, tempID, 0)->renderableIDs = malloc(scnGetState(&gameStateManager, tempID, 0)->renderableNum * sizeof(size_t));
	scnGetState(&gameStateManager, tempID, 0)->renderableIDs[0] = 1;
	scnGetState(&gameStateManager, tempID, 0)->renderableIDs[1] = 3;

	/* Cameras */
	smObjectNew(&gameStateManager, SM_TYPE_CAMERA, &tempID);
	vec3Set(&camGetState(&gameStateManager, tempID, 0)->position.value, 0.f, 2.f, 7.f);
	camGetState(&gameStateManager, tempID, 0)->targetScene = (scene **)&gameStateManager.objectType[SM_TYPE_SCENE].instance[0].state[0];
	//
	smObjectNew(&gameStateManager, SM_TYPE_CAMERA, &tempID);
	camGetState(&gameStateManager, tempID, 0)->flags |= CAM_PROJECTION_ORTHO;
	camGetState(&gameStateManager, tempID, 0)->targetScene = (scene **)&gameStateManager.objectType[SM_TYPE_SCENE].instance[1].state[0];

	/** Remove the special deletion code below the main loop as well. **/
	skliLoad(&rndrGetState(&gameStateManager, 0, 0)->skli, prgPath, "Resources\\Skeletons\\CubeTestSkeleton.tds");
	rndrGetState(&gameStateManager, 0, 0)->mdl->skl = rndrGetState(&gameStateManager, 0, 0)->skli.skl;
	skliLoad(&rndrGetState(&gameStateManager, 1, 0)->skli, prgPath, "Resources\\Skeletons\\CubeTestSkeleton.tds");
	rndrGetState(&gameStateManager, 1, 0)->mdl->skl = rndrGetState(&gameStateManager, 1, 0)->skli.skl;


	unsigned char prgRunning = 1;

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

	unsigned char UP    = 0;
	unsigned char DOWN  = 0;
	unsigned char LEFT  = 0;
	unsigned char RIGHT = 0;

	unsigned char lockMouse = 0;
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
				quatRotate(&rndrGetState(&gameStateManager, 0, 0)->orientation.value, &changeRotation, tickratio, &rndrGetState(&gameStateManager, 0, 0)->orientation.value);
				camGetState(&gameStateManager, 0, 0)->position.value.z += -5.f * tickratio;
				rndrGetState(&gameStateManager, 4, 0)->targetPosition.value = camGetState(&gameStateManager, 0, 0)->position.value;
			}
			if(DOWN){
				globalTimeMod = -1.f;
				/** changeRotation is created to initialize the second renderable. **/
				quatSetEuler(&changeRotation, 90.f*RADIAN_RATIO, 0.f, 0.f);
				quatRotate(&rndrGetState(&gameStateManager, 0, 0)->orientation.value, &changeRotation, tickratio, &rndrGetState(&gameStateManager, 0, 0)->orientation.value);
				camGetState(&gameStateManager, 0, 0)->position.value.z += 5.f * tickratio;
				rndrGetState(&gameStateManager, 4, 0)->targetPosition.value = camGetState(&gameStateManager, 0, 0)->position.value;
			}
			if(LEFT){
				/** changeRotation is created to initialize the second renderable. **/
				quatSetEuler(&changeRotation, 0.f, -90.f*RADIAN_RATIO, 0.f);
				quatRotate(&rndrGetState(&gameStateManager, 0, 0)->orientation.value, &changeRotation, tickratio, &rndrGetState(&gameStateManager, 0, 0)->orientation.value);
				quatSetEuler(&changeRotation, 0.f, 0.f, -90.f*RADIAN_RATIO);
				quatRotate(&rndrGetState(&gameStateManager, 3, 0)->orientation.value, &changeRotation, tickratio, &rndrGetState(&gameStateManager, 3, 0)->orientation.value);
				camGetState(&gameStateManager, 0, 0)->position.value.x += -5.f * tickratio;
				rndrGetState(&gameStateManager, 4, 0)->targetPosition.value = camGetState(&gameStateManager, 0, 0)->position.value;
			}
			if(RIGHT){
				/** changeRotation is created to initialize the second renderable. **/
				quatSetEuler(&changeRotation, 0.f, 90.f*RADIAN_RATIO, 0.f);
				quatRotate(&rndrGetState(&gameStateManager, 0, 0)->orientation.value, &changeRotation, tickratio, &rndrGetState(&gameStateManager, 0, 0)->orientation.value);
				quatSetEuler(&changeRotation, 0.f, 0.f, 90.f*RADIAN_RATIO);
				quatRotate(&rndrGetState(&gameStateManager, 3, 0)->orientation.value, &changeRotation, tickratio, &rndrGetState(&gameStateManager, 3, 0)->orientation.value);
				camGetState(&gameStateManager, 0, 0)->position.value.x += 5.f * tickratio;
				rndrGetState(&gameStateManager, 4, 0)->targetPosition.value = camGetState(&gameStateManager, 0, 0)->position.value;
			}

			/* Animate */
			/** Could be merged with the update function but animating should be done in here. **/
			for(i = 0; i < gameStateManager.objectType[SM_TYPE_RENDERABLE].capacity; ++i){
				/* Update the renderable for rendering */
				if(rndrGetState(&gameStateManager, i, 0) != NULL){
					rndrAnimateTexture(rndrGetState(&gameStateManager, i, 0), tickrate * globalTimeMod);
					rndrAnimateSkeleton(rndrGetState(&gameStateManager, i, 0), tickrate * globalTimeMod);
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









	/** Special deletion code **/
	sklDelete(rndrGetState(&gameStateManager, 0, 0)->skli.skl);
	sklDelete(rndrGetState(&gameStateManager, 1, 0)->skli.skl);
	sklaDelete(rndrGetState(&gameStateManager, 0, 0)->skli.animations[0].animFrags[0].currentAnim);
	sklaDelete(rndrGetState(&gameStateManager, 1, 0)->skli.animations[0].animFrags[0].currentAnim);
	cleanup(&gfxPrg, &gameStateManager, &allTextures, &allTexWrappers, &allModels, &allSkeletons, &allSklAnimations);

	return 0;

}

void cleanup(gfxProgram *gfxPrg,    stateManager *gameStateManager,
             cVector *allTextures,  cVector *allTexWrappers, cVector *allModels,
             cVector *allSkeletons, cVector *allSklAnimations){

	size_t i;
	for(i = 0; i < allTextures->size; ++i){
		tDelete((texture *)cvGet(allTextures, i));
	}
	cvClear(allTextures);

	cvClear(allTexWrappers);

	for(i = 0; i < allModels->size; ++i){
		mdlDelete((model *)cvGet(allModels, i));
	}
	cvClear(allModels);

	for(i = 0; i < allSkeletons->size; ++i){
		sklDelete((skeleton *)cvGet(allSkeletons, i));
	}
	cvClear(allSkeletons);

	for(i = 0; i < allSklAnimations->size; ++i){
		sklaDelete((sklAnim *)cvGet(allSklAnimations, i));
	}
	cvClear(allSklAnimations);

	smDelete(gameStateManager);
	gfxDestroyProgram(gfxPrg);

}
