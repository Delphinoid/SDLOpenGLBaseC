#include "graphicsRenderer.h"
#include "hitboxConvexMesh.h"
#include "constantsMath.h"
#include <stdio.h>

/****/
#include "moduleTexture.h"
#include "moduleTextureWrapper.h"
#include "moduleSkeleton.h"
#include "moduleModel.h"
#include "moduleObject.h"
#include "moduleScene.h"
#include "moduleCamera.h"

/** Remember to do regular searches for these important comments when possible **/
int main(int argc, char *argv[]){

	char *prgPath = (char*)argv[0];

	//memoryManager memMngr;
	graphicsManager gfxMngr;

	// Removes program name (everything after the last backslash) from the path.
	prgPath[strrchr(prgPath, '\\') - prgPath + 1] = '\0';
	/** prgPath can be replaced with ".\\", but it may present some problems when running directly from Code::Blocks. **/

	// Initialize the memory manager.
	if(memMngrInit(MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE, 1) == -1){
		return 1;
	}
	// Initialize the graphics subsystem.
	if(!gfxMngrInit(&gfxMngr, prgPath)){
		return 1;
	}

	/** Configs should be loaded here **/
	//

	/** Most of the code below this comment will be removed eventually **/
	cVector allTextures; cvInit(&allTextures, 1);            // Holds textures.
	cVector allTexWrappers; cvInit(&allTexWrappers, 1);      // Holds textureWrappers.
	cVector allSkeletons; cvInit(&allSkeletons, 1);          // Holds skeletons.
	cVector allSklAnimations; cvInit(&allSklAnimations, 1);  // Holds sklAnims.
	cVector allModels; cvInit(&allModels, 1);                // Holds models.
	cVector allObjects; cvInit(&allObjects, 1);              // Holds objects.

	/****/
	moduleTextureResourcesInit();
	moduleTextureWrapperResourcesInit();
	moduleSkeletonResourcesInit();
	moduleModelResourcesInit();
	moduleObjectResourcesInit();
	moduleSceneResourcesInit();
	moduleCameraResourcesInit();

	/* Textures */
	texture *tempTex = moduleTextureAllocate();
	tDefault(tempTex);
	tempTex = moduleTextureAllocate();
	tLoad(tempTex, prgPath, "Resources\\Images\\Misc\\Kobold.png");
	tempTex = moduleTextureAllocate();
	tLoad(tempTex, prgPath, "Resources\\Images\\Misc\\Avatar.png");

	/* Skeletons */
	skeleton *tempSkl = moduleSkeletonAllocate();
	sklDefault(tempSkl);

	/* Texture Wrappers */
	textureWrapper *tempTexWrap = moduleTextureWrapperAllocate();
	twDefault(tempTexWrap);
	tempTexWrap = moduleTextureWrapperAllocate();
	twLoad(tempTexWrap, prgPath, "Resources\\Textures\\Static\\AvatarStatic.tdt");
	tempTexWrap = moduleTextureWrapperAllocate();
	twLoad(tempTexWrap, prgPath, "Resources\\Textures\\Static\\KoboldStatic.tdt");

	/* Models */
	model *tempMdl = moduleModelAllocate();
	mdlDefault(tempMdl);
	tempMdl = moduleModelAllocate();
	mdlCreateSprite(tempMdl);

	/* Objects */
	object *tempObj = moduleObjectAllocate();
	objLoad(tempObj, prgPath, "Resources\\Objects\\CubeTest.tdo", &allTextures, &allTexWrappers, &allSkeletons, &allSklAnimations, &allModels);

	/* Object Instances */
	objInstance *tempObji = moduleObjectInstanceAllocate();
	objiInstantiate(tempObji, moduleObjectFind("CubeTest"));
	objiChangeAnimation(tempObji, 0, tempObji->base->animations[0], 0, 0.f);
	//objNewRenderable(objGetState(&gameStateManager, tempID, 0));
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl = (model *)cvGet(&allModels, 1);
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 1);
	//objInitSkeleton(objGetState(&gameStateManager, tempID, 0), (skeleton *)cvGet(&allSkeletons, 1));
	//
	tempObji = moduleObjectInstanceAllocate();
	objiInstantiate(tempObji, moduleObjectFind("CubeTest"));
	//objiChangeAnimation(objGetState(&gameStateManager, tempID, 0), 0, objGetState(&gameStateManager, tempID, 0)->base->animations[0], 0, 0.f);
	//objNewRenderable(objGetState(&gameStateManager, tempID, 0));
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl = (model *)cvGet(&allModels, 1);
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 2);
	//objInitSkeleton(objGetState(&gameStateManager, tempID, 0), objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl->skl);
	tempObji->configuration[0].position.x = 0.5f;
	tempObji->configuration[0].position.y = 0.5f;
	vec3SetS(&tempObji->configuration[0].scale, 0.15f);
	quat changeRotation;
	quatSetEuler(&changeRotation, 45.f*RADIAN_RATIO, 45.f*RADIAN_RATIO, 0.f);
	quatMultQByQ2(&changeRotation, &tempObji->configuration[0].orientation);
	tempObji->renderables[0].alpha = 0.5f;

	/* Sprite Object Instances */
	tempObji = moduleObjectInstanceAllocate();
	objiNewRenderable(tempObji, tempMdl, moduleTextureWrapperFind("AvatarStatic"));
	objiInitSkeleton(tempObji, tempObji->renderables[0].mdl->skl);
	///tempObji->tempRndrConfig.sprite = 1;
	tempObji->configuration[0].scale.x = 0.026f;
	tempObji->configuration[0].scale.y = 0.026f;
	//
	tempObji = moduleObjectInstanceAllocate();
	objiNewRenderable(tempObji, tempMdl, moduleTextureWrapperFind("AvatarStatic"));
	objiInitSkeleton(tempObji, tempObji->renderables[0].mdl->skl);
	///tempObji->tempRndrConfig.sprite = 1;
	tempObji->configuration[0].position.y = 1.f;
	///tempObji->tempRndrConfig.pivot.value.x = -0.5f;
	///tempObji->tempRndrConfig.pivot.value.y = 0.5f;
	tempObji->configuration[0].scale.x = 0.0026f;
	tempObji->configuration[0].scale.y = 0.0026f;
	//
	tempObji = moduleObjectInstanceAllocate();
	objiNewRenderable(tempObji, tempMdl, moduleTextureWrapperFind("AvatarStatic"));
	objiInitSkeleton(tempObji, tempObji->renderables[0].mdl->skl);
	///tempObji->tempRndrConfig.sprite = 1;
	tempObji->configuration[0].position.x = 4.f;
	tempObji->configuration[0].position.z = -3.f;
	tempObji->configuration[0].scale.x = 0.026f;
	tempObji->configuration[0].scale.y = 0.026f;
	///tempObji->tempRndrConfig.flags |= RNDR_BILLBOARD_TARGET | RNDR_BILLBOARD_Y;
	//
	tempObji = moduleObjectInstanceAllocate();
	objiNewRenderable(tempObji, tempMdl, moduleTextureWrapperFind("KoboldStatic"));
	objiInitSkeleton(tempObji, tempObji->renderables[0].mdl->skl);
	///tempObji->tempRndrConfig.sprite = 1;
	tempObji->configuration[0].position.x = -3.f;
	tempObji->configuration[0].position.y = -2.f;
	tempObji->configuration[0].position.z = -3.f;
	tempObji->configuration[0].scale.x = 0.0085f;
	tempObji->configuration[0].scale.y = 0.0065f;

	/* Scenes */
	scene *scnMain = moduleSceneAllocate();
	scnInit(scnMain, 4);
	*scnAllocate(scnMain) = moduleObjectInstanceIndex(0);
	*scnAllocate(scnMain) = moduleObjectInstanceIndex(2);
	*scnAllocate(scnMain) = moduleObjectInstanceIndex(4);
	*scnAllocate(scnMain) = moduleObjectInstanceIndex(5);
	//
	scene *scnHUD = moduleSceneAllocate();
	scnInit(scnHUD, 2);
	*scnAllocate(scnHUD) = moduleObjectInstanceIndex(1);
	*scnAllocate(scnHUD) = moduleObjectInstanceIndex(3);

	/* Cameras */
	camera *camMain = moduleCameraAllocate();
	camInit(camMain);
	vec3Set(&camMain->position.value, 0.f, 2.f, 7.f);
	//
	camera *camHUD = moduleCameraAllocate();
	camInit(camHUD);
	vec3Set(&camHUD->position.value, 0.f, 0.f, 0.f);
	flagsSet(camHUD->flags, CAM_PROJECTION_ORTHO);


	//memPrintAllBlocks();
	//memPrintFreeBlocks(0);


	signed char prgRunning = 1;

	float timeMod = 1.f;
	float updateMod = 1.f;

	float framerate = 1000.f / 125.f;  // Desired milliseconds per render.
	float tickrate = 1000.f / 125.f;   // Desired milliseconds per update.
	float tickratio = tickrate / 1000.f;  // 1 / updates per second.

	float tickrateTimeMod = tickrate * timeMod;
	float tickratioTimeMod = tickratio * timeMod;
	float tickrateUpdateMod = tickrate / updateMod;

	float startUpdate;
	float nextUpdate = (float)SDL_GetTicks();
	float startRender;
	float nextRender = (float)SDL_GetTicks();

	uint_least32_t updates = 0;
	uint_least32_t renders = 0;
	uint_least32_t lastPrint = 0;

	SDL_Event prgEventHandler;

	signed char UP    = 0;
	signed char DOWN  = 0;
	signed char LEFT  = 0;
	signed char RIGHT = 0;

	signed char lockMouse = 0;
	int mouseRelX;
	int mouseRelY;

    while(prgRunning){

		gfxMngrUpdateWindow(&gfxMngr);


		/* Take input */
		/** Use command queuing system and poll input on another thread. **/
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


		startUpdate = (float)SDL_GetTicks();
		while(startUpdate >= nextUpdate){

			/* Prepare the next game state. */
			/**smPrepareNextState(&gameStateManager);**/
			camResetInterpolation((void *)camMain);
			camResetInterpolation((void *)camHUD);

			/* Handle inputs */
			if(UP){
				timeMod = 1.f;
				tickrateTimeMod = tickrate*timeMod;
				tickratioTimeMod = tickratio*timeMod;
				/** changeRotation is created to initialize the second renderable. **/
				quatSetEuler(&changeRotation, -90.f*RADIAN_RATIO, 0.f, 0.f);
				quatRotate(&moduleObjectInstanceIndex(0)->configuration[0].orientation, &changeRotation, tickratio, &moduleObjectInstanceIndex(0)->configuration[0].orientation);
				camMain->position.value.z += -5.f * tickratio;
				//objGetState(&gameStateManager, 4, 0)->tempRndrConfig.targetPosition.value = camGetState(&gameStateManager, 0, 0)->position.value;
			}
			if(DOWN){
				timeMod = -1.f;
				tickrateTimeMod = tickrate*timeMod;
				tickratioTimeMod = tickratio*timeMod;
				/** changeRotation is created to initialize the second renderable. **/
				quatSetEuler(&changeRotation, 90.f*RADIAN_RATIO, 0.f, 0.f);
				quatRotate(&moduleObjectInstanceIndex(0)->configuration[0].orientation, &changeRotation, tickratio, &moduleObjectInstanceIndex(0)->configuration[0].orientation);
				camMain->position.value.z += 5.f * tickratio;
				//objGetState(&gameStateManager, 4, 0)->tempRndrConfig.targetPosition.value = camGetState(&gameStateManager, 0, 0)->position.value;
			}
			if(LEFT){
				/** changeRotation is created to initialize the second renderable. **/
				quatSetEuler(&changeRotation, 0.f, -90.f*RADIAN_RATIO, 0.f);
				quatRotate(&moduleObjectInstanceIndex(0)->configuration[0].orientation, &changeRotation, tickratio, &moduleObjectInstanceIndex(0)->configuration[0].orientation);
				quatSetEuler(&changeRotation, 0.f, 0.f, -90.f*RADIAN_RATIO);
				quatRotate(&moduleObjectInstanceIndex(3)->configuration[0].orientation, &changeRotation, tickratio, &moduleObjectInstanceIndex(3)->configuration[0].orientation);
				camMain->position.value.x += -5.f * tickratio;
				//objGetState(&gameStateManager, 4, 0)->tempRndrConfig.targetPosition.value = camGetState(&gameStateManager, 0, 0)->position.value;
			}
			if(RIGHT){
				/** changeRotation is created to initialize the second renderable. **/
				quatSetEuler(&changeRotation, 0.f, 90.f*RADIAN_RATIO, 0.f);
				quatRotate(&moduleObjectInstanceIndex(0)->configuration[0].orientation, &changeRotation, tickratio, &moduleObjectInstanceIndex(0)->configuration[0].orientation);
				quatSetEuler(&changeRotation, 0.f, 0.f, 90.f*RADIAN_RATIO);
				quatRotate(&moduleObjectInstanceIndex(3)->configuration[0].orientation, &changeRotation, tickratio, &moduleObjectInstanceIndex(3)->configuration[0].orientation);
				camMain->position.value.x += 5.f * tickratio;
				//objGetState(&gameStateManager, 4, 0)->tempRndrConfig.targetPosition.value = camGetState(&gameStateManager, 0, 0)->position.value;
			}

			/* Update. */
			moduleSceneUpdate(tickrateTimeMod, tickratioTimeMod);

			/* Next frame */
			nextUpdate += tickrateUpdateMod;
			++updates;

		}


		/* Render the scene */
		startRender = (float)SDL_GetTicks();
		if(startRender >= nextRender){

			/* Progress between current and next frame. */
			const float interpT = (startRender - (nextUpdate - tickrateUpdateMod)) / tickrateUpdateMod;

			/* Render */
			/** Remove later **/
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// Render the scene
			gfxRendererDrawScene(&gfxMngr, camMain, scnMain, interpT);
			// Render the HUD
			gfxRendererDrawScene(&gfxMngr, camHUD, scnHUD, interpT);
			// Update the window
			SDL_GL_SwapWindow(gfxMngr.window);

			/* Next frame */
			//nextRender = startRender + framerate;
			nextRender += framerate;
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


	moduleCameraResourcesDelete();
	moduleSceneResourcesDelete();
	moduleObjectResourcesDelete();
	moduleModelResourcesDelete();
	moduleSkeletonResourcesDelete();
	moduleTextureWrapperResourcesDelete();
	moduleTextureResourcesDelete();

	gfxMngrDestroyProgram(&gfxMngr);

	memPrintAllBlocks();
	memPrintFreeBlocks(0);
	memMngrDelete();

	return 0;

}