#include "camera.h"
#include <stdio.h>

#define RADIAN_RATIO 0.017453292  // = PI / 180, used for converting degrees to radians

void renderScene(cVector *allCameras, const size_t state, const float interpT, gfxProgram *gfxPrg);
void cleanup(cVector *allTextures,  cVector *allTexWrappers,   cVector *allModels,      cVector *allCameras,
             cVector *allSkeletons, cVector *allSklAnimations, cVector *allRenderables, cVector *allScenes,
             gfxProgram *gfxPrg);

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
	const size_t stateNum = 1;

	cVector allTextures; cvInit(&allTextures, 1);            // Holds textures
	cVector allTexWrappers; cvInit(&allTexWrappers, 1);      // Holds textureWrappers
	cVector allModels; cvInit(&allModels, 1);                // Holds models
	cVector allCameras; cvInit(&allCameras, 2);              // Holds cameras
	cVector allSkeletons; cvInit(&allSkeletons, 1);          // Holds skeletons
	cVector allSklAnimations; cvInit(&allSklAnimations, 1);  // Holds sklAnims
	cVector allRenderables; cvInit(&allRenderables, 1);      // Holds renderables
	cVector allScenes; cvInit(&allScenes, 2);                // Holds scenes

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
	mdlLoadWavefrontObj(&tempMdl, prgPath, "Resources\\Models\\CubeTest.obj");
	cvPush(&allModels, (void *)&tempMdl, sizeof(tempMdl));

	/* Renderables */
	renderable tempRndr;
	rndrInit(&tempRndr, stateNum);
	tempRndr.mdl = (model *)cvGet(&allModels, 1);
	tempRndr.twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 1);
	cvPush(&allRenderables, (void *)&tempRndr, sizeof(tempRndr));
	//
	rndrInit(&tempRndr, stateNum);
	tempRndr.mdl = (model *)cvGet(&allModels, 1);
	tempRndr.twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 2);
	tempRndr.position.value->x = 0.5f;
	tempRndr.position.value->y = 0.5f;
	vec3SetS(tempRndr.scale.value, 0.15f);
	tempRndr.rotation.x = 45.f;
	tempRndr.rotation.y = 45.f;
	*tempRndr.alpha.value = 0.5f;
	cvPush(&allRenderables, (void *)&tempRndr, sizeof(tempRndr));

	/* Sprite Renderables */
	rndrInit(&tempRndr, stateNum);
	tempRndr.sprite = 1;
	tempRndr.mdl = (model *)cvGet(&allModels, 0);
	tempRndr.twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 0);
	tempRndr.pivot.value->x = 0.5f;
	tempRndr.pivot.value->y = 0.5f;
	tempRndr.scale.value->x = 0.026f;
	tempRndr.scale.value->y = 0.026f;
	cvPush(&allRenderables, (void *)&tempRndr, sizeof(tempRndr));
	//
	rndrInit(&tempRndr, stateNum);
	tempRndr.sprite = 1;
	tempRndr.mdl = (model *)cvGet(&allModels, 0);
	tempRndr.twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 0);
	tempRndr.position.value->y = 1.f;
	tempRndr.pivot.value->y = 1.f;
	tempRndr.scale.value->x = 0.0026f;
	tempRndr.scale.value->y = 0.0026f;
	cvPush(&allRenderables, (void *)&tempRndr, sizeof(tempRndr));
	//
	rndrInit(&tempRndr, stateNum);
	tempRndr.sprite = 1;
	tempRndr.mdl = (model *)cvGet(&allModels, 0);
	tempRndr.twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 0);
	tempRndr.position.value->x = 4.f;
	tempRndr.position.value->z = -3.f;
	tempRndr.pivot.value->x = 0.5f;
	tempRndr.pivot.value->y = 0.5f;
	tempRndr.scale.value->x = 0.026f;
	tempRndr.scale.value->y = 0.026f;
	tempRndr.flags |= RNDR_BILLBOARD_TARGET | RNDR_BILLBOARD_Y;
	cvPush(&allRenderables, (void *)&tempRndr, sizeof(tempRndr));
	//
	rndrInit(&tempRndr, stateNum);
	tempRndr.sprite = 1;
	tempRndr.mdl = (model *)cvGet(&allModels, 0);
	tempRndr.twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 3);
	tempRndr.position.value->x = -3.f;
	tempRndr.position.value->y = -2.f;
	tempRndr.pivot.value->x = 0.5f;
	tempRndr.pivot.value->y = 0.5f;
	tempRndr.scale.value->x = 0.0085f;
	tempRndr.scale.value->y = 0.0065f;
	cvPush(&allRenderables, (void *)&tempRndr, sizeof(tempRndr));

	/* Scenes */
	scene tempScene;
	tempScene.renderableNum = 4;
	tempScene.renderables = malloc(tempScene.renderableNum * sizeof(renderable *));
	tempScene.renderables[0] = (renderable *)cvGet(&allRenderables, 0);
	tempScene.renderables[1] = (renderable *)cvGet(&allRenderables, 2);
	tempScene.renderables[2] = (renderable *)cvGet(&allRenderables, 4);
	tempScene.renderables[3] = (renderable *)cvGet(&allRenderables, 5);
	cvPush(&allScenes, (void *)&tempScene, sizeof(tempScene));
	tempScene.renderableNum = 2;
	tempScene.renderables = malloc(tempScene.renderableNum * sizeof(renderable *));
	tempScene.renderables[0] = (renderable *)cvGet(&allRenderables, 1);
	tempScene.renderables[1] = (renderable *)cvGet(&allRenderables, 3);
	cvPush(&allScenes, (void *)&tempScene, sizeof(tempScene));

	/* Cameras */
	camera tempCam;
	camInit(&tempCam, stateNum);
	vec3Set(tempCam.position.value, 0.f, 2.f, 7.f);
	tempCam.targetScene = (scene *)cvGet(&allScenes, 0);
	cvPush(&allCameras, (void *)&tempCam, sizeof(tempCam));
	//
	camInit(&tempCam, stateNum);
	tempCam.flags |= CAM_PROJECTION_ORTHO;
	tempCam.targetScene = (scene *)cvGet(&allScenes, 1);
	cvPush(&allCameras, (void *)&tempCam, sizeof(tempCam));

	/** Remove the special deletion code below the main loop as well. Also remove stateNum from skliLoad(). **/
	skliLoad(&((renderable *)cvGet(&allRenderables, 0))->skli, stateNum, prgPath, "Resources\\Skeletons\\CubeTestSkeleton.tds");
	skliLoad(&((renderable *)cvGet(&allRenderables, 1))->skli, stateNum, prgPath, "Resources\\Skeletons\\CubeTestSkeleton.tds");


	unsigned char prgRunning = 1;

	float globalTimeMod = 1.f;
	float framerate = 1000.f / 128.f;  // Desired renders per millisecond
	float tickrate = 1000.f / 64.f;  // Desired updates per millisecond
	uint32_t nextUpdate = 0.f;
	uint32_t nextRender = 0.f;

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

		const unsigned char windowChanged = gfxUpdateWindow(&gfxPrg);


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


		const uint32_t startTime = SDL_GetTicks();
		while(startTime >= nextUpdate){

			/* Animate */
			/** Could be merged with the update function but animating should be done in here. **/
			for(i = 0; i < allRenderables.size; ++i){
				rndrResetInterpolation((renderable *)cvGet(&allRenderables, i), stateNum);
				rndrAnimateTexture((renderable *)cvGet(&allRenderables, i), stateNum, tickrate * globalTimeMod);
				rndrAnimateSkeleton((renderable *)cvGet(&allRenderables, i), stateNum, tickrate * globalTimeMod);
			}


			/* Reset interpolation */
			for(i = 0; i < allCameras.size; ++i){
				camResetInterpolation((camera *)cvGet(&allCameras, i), stateNum);
			}


			/* Handle inputs */
			if(UP){
				globalTimeMod = 1.f;
				((renderable *)cvGet(&allRenderables, 0))->rotation.x += -0.1f * tickrate;
				((camera *)cvGet(&allCameras, 0))->position.value->z += -0.005f * tickrate;
				*((renderable *)cvGet(&allRenderables, 4))->targetPosition.value = *((camera *)cvGet(&allCameras, 0))->position.value;
			}
			if(DOWN){
				globalTimeMod = -1.f;
				((renderable *)cvGet(&allRenderables, 0))->rotation.x += 0.1f * tickrate;
				((camera *)cvGet(&allCameras, 0))->position.value->z += 0.005f * tickrate;
				*((renderable *)cvGet(&allRenderables, 4))->targetPosition.value = *((camera *)cvGet(&allCameras, 0))->position.value;
			}
			if(LEFT){
				((renderable *)cvGet(&allRenderables, 0))->rotation.y += -0.1f * tickrate;
				((renderable *)cvGet(&allRenderables, 3))->rotation.z += -0.1f * tickrate;
				((camera *)cvGet(&allCameras, 0))->position.value->x += -0.005f * tickrate;
				*((renderable *)cvGet(&allRenderables, 4))->targetPosition.value = *((camera *)cvGet(&allCameras, 0))->position.value;
			}
			if(RIGHT){
				((renderable *)cvGet(&allRenderables, 0))->rotation.y += 0.1f * tickrate;
				((renderable *)cvGet(&allRenderables, 3))->rotation.z += 0.1f * tickrate;
				((camera *)cvGet(&allCameras, 0))->position.value->x += 0.005f * tickrate;
				*((renderable *)cvGet(&allRenderables, 4))->targetPosition.value = *((camera *)cvGet(&allCameras, 0))->position.value;
			}


			/* Next frame */
			nextUpdate += tickrate;

		}


		/* Render the scene */
		const uint32_t endTime = SDL_GetTicks();
		if(endTime >= nextRender){

			/* Progress between current and next frame. */
			float interpT = (SDL_GetTicks() - (nextUpdate - tickrate)) / tickrate;
			if(interpT < 0.f){
				interpT = 0.f;
			}else if(interpT > 1.f){
				interpT = 1.f;
			}

			/* Update cameras */
			for(i = 0; i < allCameras.size; ++i){
				camUpdateViewMatrix((camera *)cvGet(&allCameras, i), 0, interpT);
				if(windowChanged){
					// If the window size changed, update the camera projection matrices as well
					((camera *)cvGet(&allCameras, i))->flags |= CAM_UPDATE_PROJECTION;
				}
				camUpdateProjectionMatrix((camera *)cvGet(&allCameras, i), gfxPrg.aspectRatioX, gfxPrg.aspectRatioY, 0, interpT);
			}

			/* Render */
			/** Remove later **/
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// Render the scene
			renderScene(&allCameras, 0, interpT, &gfxPrg);
			// Update the window
			SDL_GL_SwapWindow(gfxPrg.window);

			/* Next frame */
			nextRender = endTime + framerate;

		}

    }









	/** Special deletion code **/
	sklDelete(((renderable *)cvGet(&allRenderables, 0))->skli.skl);
	sklDelete(((renderable *)cvGet(&allRenderables, 1))->skli.skl);
	sklaDelete(((renderable *)cvGet(&allRenderables, 0))->skli.animations[0].anim);
	sklaDelete(((renderable *)cvGet(&allRenderables, 1))->skli.animations[0].anim);
	cleanup(&allTextures, &allTexWrappers, &allModels, &allCameras, &allSkeletons, &allSklAnimations, &allRenderables, &allScenes, &gfxPrg);

	return 0;

}

void cleanup(cVector *allTextures,  cVector *allTexWrappers,   cVector *allModels,      cVector *allCameras,
             cVector *allSkeletons, cVector *allSklAnimations, cVector *allRenderables, cVector *allScenes,
             gfxProgram *gfxPrg){

	gfxDestroyProgram(gfxPrg);

	size_t i;
	for(i = 0; i < allTextures->size; ++i){
		tDelete((texture *)cvGet(allTextures, i));
	}
	cvClear(allTextures);

	for(i = 0; i < allTexWrappers->size; ++i){
		twDelete((textureWrapper *)cvGet(allTexWrappers, i));
	}
	cvClear(allTexWrappers);

	for(i = 0; i < allModels->size; ++i){
		mdlDelete((model *)cvGet(allModels, i));
	}
	cvClear(allModels);

	cvClear(allCameras);

	for(i = 0; i < allSkeletons->size; ++i){
		sklDelete((skeleton *)cvGet(allSkeletons, i));
	}
	cvClear(allSkeletons);

	for(i = 0; i < allSklAnimations->size; ++i){
		sklaDelete((sklAnim *)cvGet(allSklAnimations, i));
	}
	cvClear(allSklAnimations);

	for(i = 0; i < allScenes->size; ++i){
		scnDelete((scene *)cvGet(allScenes, i));
	}
	cvClear(allScenes);

	for(i = 0; i < allRenderables->size; ++i){
		rndrDelete((renderable *)cvGet(allRenderables, i));
	}
	cvClear(allRenderables);

}
