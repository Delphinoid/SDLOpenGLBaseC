#include "camera.h"
#include <stdio.h>
#include "fps.h"

#define RADIAN_RATIO 0.017453292  // = PI / 180, used for converting degrees to radians

void renderScene(cVector *allCameras, gfxProgram *gfxPrg);
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
	rndrInit(&tempRndr);
	tempRndr.mdl = (model *)cvGet(&allModels, 1);
	tempRndr.twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 1);
	cvPush(&allRenderables, (void *)&tempRndr, sizeof(tempRndr));
	tempRndr.twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 2);
	tempRndr.sTrans.position.x = 0.5f;
	tempRndr.sTrans.position.y = 0.5f;
	vec3SetS(&tempRndr.rTrans.scale, 0.15f);
	rndrRotateX(&tempRndr, 45.f);
	rndrRotateY(&tempRndr, 45.f);
	tempRndr.rTrans.alpha = 0.5f;
	cvPush(&allRenderables, (void *)&tempRndr, sizeof(tempRndr));

	/* Sprite Renderables */
	rndrInit(&tempRndr);
	tempRndr.sprite = 1;
	tempRndr.mdl = (model *)cvGet(&allModels, 0);
	tempRndr.twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 0);
	tempRndr.sTrans.relPivot.x = 0.5f;
	tempRndr.sTrans.relPivot.y = 0.5f;
	tempRndr.rTrans.scale.x = 0.026f;
	tempRndr.rTrans.scale.y = 0.026f;
	cvPush(&allRenderables, (void *)&tempRndr, sizeof(tempRndr));
	tempRndr.sTrans.position.y = 1.f;
	tempRndr.sTrans.relPivot.x = 0.f;
	tempRndr.sTrans.relPivot.y = 1.f;
	tempRndr.rTrans.scale.x = 0.0026f;
	tempRndr.rTrans.scale.y = 0.0026f;
	cvPush(&allRenderables, (void *)&tempRndr, sizeof(tempRndr));
	tempRndr.sTrans.position.x = 4.f;
	tempRndr.sTrans.position.y = 0.f;
	tempRndr.sTrans.position.z = -3.f;
	tempRndr.sTrans.relPivot.x = 0.5f;
	tempRndr.sTrans.relPivot.y = 0.5f;
	tempRndr.rTrans.scale.x = 0.026f;
	tempRndr.rTrans.scale.y = 0.026f;
	tempRndr.flags |= RNDR_BILLBOARD_TARGET | RNDR_BILLBOARD_Y;
	cvPush(&allRenderables, (void *)&tempRndr, sizeof(tempRndr));
	tempRndr.twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 3);
	tempRndr.sTrans.position.x = -3.f;
	tempRndr.sTrans.position.y = -2.f;
	tempRndr.rTrans.scale.x = 0.0085f;
	tempRndr.rTrans.scale.y = 0.0065f;
	tempRndr.flags = 0;
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
	camera tempCam; camInit(&tempCam);
	vec3Set(&tempCam.position, 0.f, 2.f, 7.f);
	tempCam.targetScene = (scene *)cvGet(&allScenes, 0);
	cvPush(&allCameras, (void *)&tempCam, sizeof(tempCam));
	vec3Set(&tempCam.position, 0.f, 0.f, 0.f);
	tempCam.flags |= CAM_PROJECTION_ORTHO;
	tempCam.targetScene = (scene *)cvGet(&allScenes, 1);
	cvPush(&allCameras, (void *)&tempCam, sizeof(tempCam));

	/** Remove the special deletion code below the main loop as well **/
	skliLoad(&((renderable *)cvGet(&allRenderables, 0))->skli, prgPath, "Resources\\Skeletons\\CubeTestSkeleton.tds");
	skliLoad(&((renderable *)cvGet(&allRenderables, 1))->skli, prgPath, "Resources\\Skeletons\\CubeTestSkeleton.tds");


	unsigned char prgRunning = 1;
	SDL_Event prgEventHandler;
	fps fpsHandler; fpsStart(&fpsHandler, 61, 121);

	float globalTimeMod = 1.f;
	uint32_t startTick;
	uint32_t ticksElapsed = 0;

	unsigned char UP    = 0;
	unsigned char DOWN  = 0;
	unsigned char LEFT  = 0;
	unsigned char RIGHT = 0;

	unsigned char lockMouse = 0;
	int mouseRelX;
	int mouseRelY;

	size_t i;

	while(prgRunning){

		/* Set tick that the frame is starting on */
		startTick = SDL_GetTicks();

		/* Update FPS */
		fpsUpdate(&fpsHandler);


		/* If the window size has changed, resize the OpenGL viewport */
		if(gfxUpdateWindow(&gfxPrg)){
			// Cameras will also have to be adjusted
			for(i = 0; i < allCameras.size; ++i){
				((camera *)cvGet(&allCameras, i))->flags |= CAM_UPDATE_PROJECTION;
			}
		}


		/* Detect input */
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


		/* Handle inputs */
		if(UP){
			rndrRotateX((renderable *)cvGet(&allRenderables, 0), -2.f * fpsHandler.fpsMod);
			//sprRotateX((sprite *)cvGet(&allSprites, 0), -2.f * fpsHandler.fpsMod);
			camMoveZ((camera *)cvGet(&allCameras, 0), -0.1f * fpsHandler.fpsMod);
			((renderable *)cvGet(&allRenderables, 4))->rTrans.targetPosition = ((camera *)cvGet(&allCameras, 0))->position;
		}
		if(DOWN){
			rndrRotateX((renderable *)cvGet(&allRenderables, 0), 2.f * fpsHandler.fpsMod);
			//sprRotateX((sprite *)cvGet(&allSprites, 0), 2.f * fpsHandler.fpsMod);
			camMoveZ((camera *)cvGet(&allCameras, 0), 0.1f * fpsHandler.fpsMod);
			((renderable *)cvGet(&allRenderables, 4))->rTrans.targetPosition = ((camera *)cvGet(&allCameras, 0))->position;
		}
		if(LEFT){
			rndrRotateY((renderable *)cvGet(&allRenderables, 0), -2.f * fpsHandler.fpsMod);
			rndrRotateZ((renderable *)cvGet(&allRenderables, 3), -2.f * fpsHandler.fpsMod);
			camMoveX((camera *)cvGet(&allCameras, 0), -0.1f * fpsHandler.fpsMod);
			((renderable *)cvGet(&allRenderables, 4))->rTrans.targetPosition = ((camera *)cvGet(&allCameras, 0))->position;
		}
		if(RIGHT){
			rndrRotateY((renderable *)cvGet(&allRenderables, 0), 2.f * fpsHandler.fpsMod);
			rndrRotateZ((renderable *)cvGet(&allRenderables, 3), 2.f * fpsHandler.fpsMod);
			camMoveX((camera *)cvGet(&allCameras, 0), 0.1f * fpsHandler.fpsMod);
			((renderable *)cvGet(&allRenderables, 4))->rTrans.targetPosition = ((camera *)cvGet(&allCameras, 0))->position;
		}

		// Get mouse position relative to its position in the last call
		SDL_GetRelativeMouseState(&mouseRelX, &mouseRelY);


		/* Animate */
		// Animate the renderables
		for(i = 0; i < allRenderables.size; ++i){
			rndrAnimateTexture((renderable *)cvGet(&allRenderables, i), (float)ticksElapsed * globalTimeMod);
			rndrAnimateSkeleton((renderable *)cvGet(&allRenderables, i), (float)ticksElapsed * globalTimeMod);
		}


		/* Render the scene */
		// Update cameras
		for(i = 0; i < allCameras.size; ++i){
			camUpdateViewMatrix((camera *)cvGet(&allCameras, i));
			camUpdateProjectionMatrix((camera *)cvGet(&allCameras, i), gfxPrg.aspectRatioX, gfxPrg.aspectRatioY);
		}

		/** Remove later **/
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render the scene
		renderScene(&allCameras, &gfxPrg);

		// Update the window
		SDL_GL_SwapWindow(gfxPrg.window);


		/* Pause the program to maintain a constant FPS */
		fpsDelay(&fpsHandler);

		/* Update time elapsed in the last frame */
		ticksElapsed = SDL_GetTicks() - startTick;

	}





	float framerate = 1000.f / 128.f;  // Minimum time between renders
	float tickrate = 1000.f / 64.f;    // Milliseconds per frame to simulate
	uint32_t currentTime = SDL_GetTicks();

    while(prgRunning){

		const uint32_t startTime = SDL_GetTicks();
		const uint32_t elapsedTime = startTime - currentTime;
		currentTime = startTime;

		float timeSlice = elapsedTime;

		while(timeSlice >= tickrate){

			// Take input and update
			/** **/

			timeSlice -= tickrate;

		}

		// Position between last state and next state
		const float interp = 1.f - elapsedTime / tickrate;

		// Render
		/** **/

		// Sleep if necessary to maintain a capped framerate
		const float sleepTime = framerate - SDL_GetTicks() - startTime;
		if(sleepTime > 0.f){
			SDL_Delay((uint32_t)sleepTime);
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
