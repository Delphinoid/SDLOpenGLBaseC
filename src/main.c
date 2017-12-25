#include "stateManager.h"
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
	smInit(&gameStateManager, 6, 2, 2, 2);

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
	smRenderableNew(&gameStateManager, &tempID);
	rndrInit(gameStateManager.renderables[tempID].state[0]);
	gameStateManager.renderables[tempID].state[0]->mdl = (model *)cvGet(&allModels, 1);
	gameStateManager.renderables[tempID].state[0]->twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 1);
	//
	smRenderableNew(&gameStateManager, &tempID);
	rndrInit(gameStateManager.renderables[tempID].state[0]);
	gameStateManager.renderables[tempID].state[0]->mdl = (model *)cvGet(&allModels, 1);
	gameStateManager.renderables[tempID].state[0]->twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 2);
	gameStateManager.renderables[tempID].state[0]->position.value.x = 0.5f;
	gameStateManager.renderables[tempID].state[0]->position.value.y = 0.5f;
	vec3SetS(&gameStateManager.renderables[tempID].state[0]->scale.value, 0.15f);
	quat changeRotation;
	quatSetEuler(&changeRotation, 45.f, 45.f, 0.f);
	quatMultQByQ2(&changeRotation, &gameStateManager.renderables[tempID].state[0]->orientation.value);
	gameStateManager.renderables[tempID].state[0]->alpha.value = 0.5f;

	/* Sprite Renderables */
	smRenderableNew(&gameStateManager, &tempID);
	rndrInit(gameStateManager.renderables[tempID].state[0]);
	gameStateManager.renderables[tempID].state[0]->sprite = 1;
	gameStateManager.renderables[tempID].state[0]->mdl = (model *)cvGet(&allModels, 0);
	gameStateManager.renderables[tempID].state[0]->twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 0);
	gameStateManager.renderables[tempID].state[0]->pivot.value.x = 0.5f;
	gameStateManager.renderables[tempID].state[0]->pivot.value.y = 0.5f;
	gameStateManager.renderables[tempID].state[0]->scale.value.x = 0.026f;
	gameStateManager.renderables[tempID].state[0]->scale.value.y = 0.026f;
	//
	smRenderableNew(&gameStateManager, &tempID);
	rndrInit(gameStateManager.renderables[tempID].state[0]);
	gameStateManager.renderables[tempID].state[0]->sprite = 1;
	gameStateManager.renderables[tempID].state[0]->mdl = (model *)cvGet(&allModels, 0);
	gameStateManager.renderables[tempID].state[0]->twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 0);
	gameStateManager.renderables[tempID].state[0]->position.value.y = 1.f;
	gameStateManager.renderables[tempID].state[0]->pivot.value.y = 1.f;
	gameStateManager.renderables[tempID].state[0]->scale.value.x = 0.0026f;
	gameStateManager.renderables[tempID].state[0]->scale.value.y = 0.0026f;
	//
	smRenderableNew(&gameStateManager, &tempID);
	rndrInit(gameStateManager.renderables[tempID].state[0]);
	gameStateManager.renderables[tempID].state[0]->sprite = 1;
	gameStateManager.renderables[tempID].state[0]->mdl = (model *)cvGet(&allModels, 0);
	gameStateManager.renderables[tempID].state[0]->twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 0);
	gameStateManager.renderables[tempID].state[0]->position.value.x = 4.f;
	gameStateManager.renderables[tempID].state[0]->position.value.z = -3.f;
	gameStateManager.renderables[tempID].state[0]->pivot.value.x = 0.5f;
	gameStateManager.renderables[tempID].state[0]->pivot.value.y = 0.5f;
	gameStateManager.renderables[tempID].state[0]->scale.value.x = 0.026f;
	gameStateManager.renderables[tempID].state[0]->scale.value.y = 0.026f;
	gameStateManager.renderables[tempID].state[0]->flags |= RNDR_BILLBOARD_TARGET | RNDR_BILLBOARD_Y;
	//
	smRenderableNew(&gameStateManager, &tempID);
	rndrInit(gameStateManager.renderables[tempID].state[0]);
	gameStateManager.renderables[tempID].state[0]->sprite = 1;
	gameStateManager.renderables[tempID].state[0]->mdl = (model *)cvGet(&allModels, 0);
	gameStateManager.renderables[tempID].state[0]->twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 3);
	gameStateManager.renderables[tempID].state[0]->position.value.x = -3.f;
	gameStateManager.renderables[tempID].state[0]->position.value.y = -2.f;
	gameStateManager.renderables[tempID].state[0]->position.value.z = -3.f;
	gameStateManager.renderables[tempID].state[0]->pivot.value.x = 0.5f;
	gameStateManager.renderables[tempID].state[0]->pivot.value.y = 0.5f;
	gameStateManager.renderables[tempID].state[0]->scale.value.x = 0.0085f;
	gameStateManager.renderables[tempID].state[0]->scale.value.y = 0.0065f;

	/* Scenes */
	smSceneNew(&gameStateManager, &tempID);
	gameStateManager.scenes[tempID].state[0]->renderableNum = 4;
	gameStateManager.scenes[tempID].state[0]->renderableCapacity = 4;
	gameStateManager.scenes[tempID].state[0]->renderableIDs = malloc(gameStateManager.scenes[tempID].state[0]->renderableNum * sizeof(size_t));
	gameStateManager.scenes[tempID].state[0]->renderableIDs[0] = 0;
	gameStateManager.scenes[tempID].state[0]->renderableIDs[1] = 2;
	gameStateManager.scenes[tempID].state[0]->renderableIDs[2] = 4;
	gameStateManager.scenes[tempID].state[0]->renderableIDs[3] = 5;
	//
	smSceneNew(&gameStateManager, &tempID);
	gameStateManager.scenes[tempID].state[0]->renderableNum = 2;
	gameStateManager.scenes[tempID].state[0]->renderableCapacity = 2;
	gameStateManager.scenes[tempID].state[0]->renderableIDs = malloc(gameStateManager.scenes[tempID].state[0]->renderableNum * sizeof(size_t));
	gameStateManager.scenes[tempID].state[0]->renderableIDs[0] = 1;
	gameStateManager.scenes[tempID].state[0]->renderableIDs[1] = 3;

	/* Cameras */
	smCameraNew(&gameStateManager, &tempID);
	camInit(gameStateManager.cameras[tempID].state[0]);
	vec3Set(&gameStateManager.cameras[tempID].state[0]->position.value, 0.f, 2.f, 7.f);
	gameStateManager.cameras[tempID].state[0]->targetScene = &gameStateManager.scenes[0].state[0];
	//
	smCameraNew(&gameStateManager, &tempID);
	camInit(gameStateManager.cameras[tempID].state[0]);
	gameStateManager.cameras[tempID].state[0]->flags |= CAM_PROJECTION_ORTHO;
	gameStateManager.cameras[tempID].state[0]->targetScene = &gameStateManager.scenes[1].state[0];

	/** Remove the special deletion code below the main loop as well. **/
	skliLoad(&gameStateManager.renderables[0].state[0]->skli, prgPath, "Resources\\Skeletons\\CubeTestSkeleton.tds");
	gameStateManager.renderables[0].state[0]->mdl->skl = gameStateManager.renderables[0].state[0]->skli.skl;
	skliLoad(&gameStateManager.renderables[1].state[0]->skli, prgPath, "Resources\\Skeletons\\CubeTestSkeleton.tds");
	gameStateManager.renderables[1].state[0]->mdl->skl = gameStateManager.renderables[1].state[0]->skli.skl;


	unsigned char prgRunning = 1;

	float globalTimeMod = 1.f;
	float framerate = 1000.f / 125.f;  // Desired renders per millisecond
	float tickrate = 1000.f / 125.f;  // Desired updates per millisecond
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
				quatSetEuler(&changeRotation, -0.1f*tickrate*RADIAN_RATIO, 0.f, 0.f);
				quatMultQByQ2(&changeRotation, &gameStateManager.renderables[0].state[0]->orientation.value);
				gameStateManager.cameras[0].state[0]->position.value.z += -0.005f * tickrate;
				gameStateManager.renderables[4].state[0]->targetPosition.value = gameStateManager.cameras[0].state[0]->position.value;
			}
			if(DOWN){
				globalTimeMod = -1.f;
				/** changeRotation is created to initialize the second renderable. **/
				quatSetEuler(&changeRotation, 0.1f*tickrate*RADIAN_RATIO, 0.f, 0.f);
				quatMultQByQ2(&changeRotation, &gameStateManager.renderables[0].state[0]->orientation.value);
				gameStateManager.cameras[0].state[0]->position.value.z += 0.005f * tickrate;
				gameStateManager.renderables[4].state[0]->targetPosition.value = gameStateManager.cameras[0].state[0]->position.value;
			}
			if(LEFT){
				/** changeRotation is created to initialize the second renderable. **/
				quatSetEuler(&changeRotation, 0.f, -0.1f*tickrate*RADIAN_RATIO, 0.f);
				quatMultQByQ2(&changeRotation, &gameStateManager.renderables[0].state[0]->orientation.value);
				quatSetEuler(&changeRotation, 0.f, 0.f, -0.1f*tickrate*RADIAN_RATIO);
				quatMultQByQ2(&changeRotation, &gameStateManager.renderables[3].state[0]->orientation.value);
				gameStateManager.cameras[0].state[0]->position.value.x += -0.005f * tickrate;
				gameStateManager.renderables[4].state[0]->targetPosition.value = gameStateManager.cameras[0].state[0]->position.value;
			}
			if(RIGHT){
				/** changeRotation is created to initialize the second renderable. **/
				quatSetEuler(&changeRotation, 0.f, 0.1f*tickrate*RADIAN_RATIO, 0.f);
				quatMultQByQ2(&changeRotation, &gameStateManager.renderables[0].state[0]->orientation.value);
				quatSetEuler(&changeRotation, 0.f, 0.f, 0.1f*tickrate*RADIAN_RATIO);
				quatMultQByQ2(&changeRotation, &gameStateManager.renderables[3].state[0]->orientation.value);
				gameStateManager.cameras[0].state[0]->position.value.x += 0.005f * tickrate;
				gameStateManager.renderables[4].state[0]->targetPosition.value = gameStateManager.cameras[0].state[0]->position.value;
			}

			/* Animate */
			/** Could be merged with the update function but animating should be done in here. **/
			for(i = 0; i < gameStateManager.renderableCapacity; ++i){
				/* Update the renderable for rendering */
				if(gameStateManager.renderables[i].state[0] != NULL){
					rndrAnimateTexture(gameStateManager.renderables[i].state[0], tickrate * globalTimeMod);
					rndrAnimateSkeleton(gameStateManager.renderables[i].state[0], tickrate * globalTimeMod);
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
			if(renderStart > nextRender + framerate){
				// If we fell behind, skip forward so we don't render unnecessarily.
				nextRender = renderStart + framerate;
			}else{
				nextRender += framerate;
			}
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
	sklDelete(gameStateManager.renderables[0].state[0]->skli.skl);
	sklDelete(gameStateManager.renderables[1].state[0]->skli.skl);
	sklaDelete(gameStateManager.renderables[0].state[0]->skli.animations[0].animListHead->currentAnim);
	sklaDelete(gameStateManager.renderables[1].state[0]->skli.animations[0].animListHead->currentAnim);
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
