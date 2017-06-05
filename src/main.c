#include "gfxProgram.h"
#include <stdio.h>
#include "camera.h"
#include "model.h"
#include "sprite.h"
#include "fps.h"

#define radianRatio 0.017453292  // = PI / 180, used for converting degrees to radians

void renderScene(gfxProgram *gfxPrg, camera *cam, cVector *allModels, cVector *allSprites);
void cleanup(gfxProgram *gfxPrg, cVector *allTextures, cVector *allTexWrappers, cVector *allModels, cVector *allSprites);

int main(int argc, char *argv[]){

	char *prgPath = (char*)argv[0];
	prgPath[strrchr(prgPath, '\\') - prgPath + 1] = '\0';  // Removes program name (everything after the last backslash) from the path
	/** prgPath can be replaced with ".\\", but it may present some problems when running directly from Code::Blocks. **/

	gfxProgram gfxPrg;
	if(!gfxInitProgram(&gfxPrg, prgPath)){
		return 1;
	}

	/** Configs should be loaded here **/


	/** Temp **/
	mat4Identity(&gfxPrg.identityMatrix);
	mat4Perspective(&gfxPrg.projectionMatrixFrustum, 45.f * radianRatio, (float)DEFAULT_WIDTH / (float)DEFAULT_HEIGHT, 1.f, 1000.f);
	/*mat4Ortho(&gfxPrg.projectionMatrixOrtho, 0.f, 1.f, 1.f, 0.f, 1.f, -1.f);
	mat4Scale(&gfxPrg.projectionMatrixOrtho, 1.f / ((float)gfxPrg.windowWidth  / (float)gfxPrg.biggestDimension),
	                                         1.f / ((float)gfxPrg.windowHeight / (float)gfxPrg.biggestDimension),
	                                         1.f);*/


	/** Most of the code below this line will be removed eventually **/
	cVector allTextures; cvInit(&allTextures, 1);        // Holds textures
	cVector allTexWrappers; cvInit(&allTexWrappers, 1);  // Holds textureWrappers
	cVector allModels; cvInit(&allModels, 1);            // Holds models
	cVector allSprites; cvInit(&allSprites, 1);          // Holds sprites

	texture tempTex = {.name = NULL};
	tLoad(&tempTex, prgPath, "Resources\\Images\\Guy\\Guy.png");
	cvPush(&allTextures, (void *)&tempTex, sizeof(tempTex));
	tLoad(&tempTex, prgPath, "Resources\\Images\\Misc\\Kobold.png");
	cvPush(&allTextures, (void *)&tempTex, sizeof(tempTex));
	tLoad(&tempTex, prgPath, "Resources\\Images\\Misc\\Avatar.png");
	cvPush(&allTextures, (void *)&tempTex, sizeof(tempTex));

	textureWrapper tempTexWrap = {.name = NULL};
	twLoad(&tempTexWrap, prgPath, "Resources\\Textures\\Static\\AvatarStatic.tdt", &allTextures);
	cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));
	twLoad(&tempTexWrap, prgPath, "Resources\\Textures\\Animated\\GuySpr.tdt", &allTextures);
	cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));
	twLoad(&tempTexWrap, prgPath, "Resources\\Textures\\Animated\\GuyMulti.tdt", &allTextures);
	cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));
	twLoad(&tempTexWrap, prgPath, "Resources\\Textures\\Static\\KoboldStatic.tdt", &allTextures);
	cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));

	model tempMdl;
	mdlLoad(&tempMdl, prgPath, "Resources\\Models\\CubeTest1.obj", &allTexWrappers);
	cvPush(&allModels, (void *)&tempMdl, sizeof(tempMdl));
	mdlLoad(&tempMdl, prgPath, "Resources\\Models\\CubeTest2.obj", &allTexWrappers);
	tempMdl.position.x = 0.25f;
	tempMdl.position.y = 0.5f;
	tempMdl.scale = vec3NewS(0.1f);
	mdlRotateX(&tempMdl, 45.f);
	mdlRotateY(&tempMdl, 45.f);
	tempMdl.alpha = 0.5f;
	mdlHudElement(&tempMdl, 1);
	tempMdl.hudScaleMode = 1;
	cvPush(&allModels, (void *)&tempMdl, sizeof(tempMdl));

	sprite tempSpr;
	sprLoad(&tempSpr, prgPath, ".", &allTexWrappers);
	tempSpr.width = 2.f;
	tempSpr.height = 2.f;
	tempSpr.relPivot.x = tempSpr.width / 2;
	tempSpr.relPivot.y = tempSpr.height / 2;
	tempSpr.scale = vec3NewS(2.f);
	cvPush(&allSprites, (void *)&tempSpr, sizeof(tempSpr));
	tempSpr.width = 1.f;
	tempSpr.height = 1.f;
	tempSpr.relPivot.x = 0.f;
	tempSpr.relPivot.y = 0.f;
	tempSpr.scale = vec3NewS(0.25f);
	sprHudElement(&tempSpr, 1);
	tempSpr.hudScaleMode = 1;
	cvPush(&allSprites, (void *)&tempSpr, sizeof(tempSpr));
	tempSpr.position.x = 4.f;
	tempSpr.position.y = 0.f;
	tempSpr.position.z = -3.f;
	tempSpr.width = 2.f;
	tempSpr.height = 2.f;
	tempSpr.relPivot.x = tempSpr.width / 2;
	tempSpr.relPivot.y = tempSpr.height / 2;
	tempSpr.billboardY = 1;
	tempSpr.scale = vec3NewS(2.f);
	sprHudElement(&tempSpr, 0);
	cvPush(&allSprites, (void *)&tempSpr, sizeof(tempSpr));
	tempSpr.texture = (textureWrapper *)cvGet(&allTexWrappers, 3);
	tempSpr.position.x = -3.f;
	tempSpr.position.y = -2.f;
	tempSpr.billboardY = 0;
	cvPush(&allSprites, (void *)&tempSpr, sizeof(tempSpr));


	unsigned char prgRunning = 1;
	camera cam; camInit(&cam);
	SDL_Event prgEventHandler;
	fps fpsHandler; fpsStart(&fpsHandler, 61, 121);

	unsigned char UP    = 0;
	unsigned char DOWN  = 0;
	unsigned char LEFT  = 0;
	unsigned char RIGHT = 0;

	unsigned char lockMouse = 0;
	int mouseRelX;
	int mouseRelY;

	while(prgRunning){

		/* Update FPS */
		fpsUpdate(&fpsHandler);


		/* If the window size has changed, resize the OpenGL viewport */
		SDL_GetWindowSize(gfxPrg.window, &gfxPrg.windowWidth, &gfxPrg.windowHeight);
		if(gfxPrg.windowWidth != gfxPrg.lastWindowWidth || gfxPrg.windowHeight != gfxPrg.lastWindowHeight){
			gfxPrg.biggestDimension = gfxPrg.windowWidth > gfxPrg.windowHeight ? gfxPrg.windowWidth : gfxPrg.windowHeight;
			mat4Ortho(&gfxPrg.projectionMatrixOrtho, 0.f, 1.f, 1.f, 0.f, 1.f, -1.f);
			mat4Scale(&gfxPrg.projectionMatrixOrtho, 1.f / ((float)gfxPrg.windowWidth  / (float)gfxPrg.biggestDimension),
			                                         1.f / ((float)gfxPrg.windowHeight / (float)gfxPrg.biggestDimension),
			                                         1.f);
			glViewport(0, 0, gfxPrg.windowWidth, gfxPrg.windowHeight);
			gfxPrg.lastWindowWidth = gfxPrg.windowWidth;
			gfxPrg.lastWindowHeight = gfxPrg.windowHeight;
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
			mdlRotateX((model *)cvGet(&allModels, 0), -2.f * fpsHandler.fpsMod);
			//sprRotateX((sprite *)cvGet(&allSprites, 0), -2.f * fpsHandler.fpsMod);
			camMoveZ(&cam, -0.1f * fpsHandler.fpsMod);
			((sprite *)cvGet(&allSprites, 2))->target = cam.position;
		}
		if(DOWN){
			mdlRotateX((model *)cvGet(&allModels, 0), 2.f * fpsHandler.fpsMod);
			//sprRotateX((sprite *)cvGet(&allSprites, 0), 2.f * fpsHandler.fpsMod);
			camMoveZ(&cam, 0.1f * fpsHandler.fpsMod);
			((sprite *)cvGet(&allSprites, 2))->target = cam.position;
		}
		if(LEFT){
			mdlRotateY((model *)cvGet(&allModels, 0), -2.f * fpsHandler.fpsMod);
			sprRotateZ((sprite *)cvGet(&allSprites, 1), -2.f * fpsHandler.fpsMod);
			camMoveX(&cam, -0.1f * fpsHandler.fpsMod);
			((sprite *)cvGet(&allSprites, 2))->target = cam.position;
		}
		if(RIGHT){
			mdlRotateY((model *)cvGet(&allModels, 0), 2.f * fpsHandler.fpsMod);
			sprRotateZ((sprite *)cvGet(&allSprites, 1), 2.f * fpsHandler.fpsMod);
			camMoveX(&cam, 0.1f * fpsHandler.fpsMod);
			((sprite *)cvGet(&allSprites, 2))->target = cam.position;
		}

		// Get mouse position relative to its position in the last call
		SDL_GetRelativeMouseState(&mouseRelX, &mouseRelY);


		/* Animate */
		// Animate the models
		unsigned int d;
		for(d = 0; d < allModels.size; d++){
			mdlAnimateTex((model *)cvGet(&allModels, d));
		}
		// Animate the sprites
		for(d = 0; d < allSprites.size; d++){
			sprAnimateTex((sprite *)cvGet(&allSprites, d));
		}


		/* Render the scene */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		camCreateViewMatrix(&cam, &gfxPrg.viewMatrix);

		// Render the scene
		renderScene(&gfxPrg, &cam, &allModels, &allSprites);

		// Update the window
		SDL_GL_SwapWindow(gfxPrg.window);


		/* Pause the program to maintain a constant FPS */
		fpsDelay(&fpsHandler);

	}


	cleanup(&gfxPrg, &allTextures, &allTexWrappers, &allModels, &allSprites);
	return 0;

}

void cleanup(gfxProgram *gfxPrg, cVector *allTextures, cVector *allTexWrappers, cVector *allModels, cVector *allSprites){
	gfxDestroyProgram(gfxPrg);
	unsigned int d;
	for(d = 0; d < allTextures->size; d++){
		tDelete((texture *)cvGet(allTextures, d));
	}
	for(d = 0; d < allTexWrappers->size; d++){
		twDelete((textureWrapper *)cvGet(allTexWrappers, d));
	}
	for(d = 0; d < allModels->size; d++){
		mdlDelete((model *)cvGet(allModels, d));
	}
	/*for(d = 0; d < allSprites->size; d++){
		sprDelete((sprite *)cvGet(allSprites, d));
	}*/
}
