#include "renderable.h"
#include <stdio.h>
#include "fps.h"

#define radianRatio 0.017453292  // = PI / 180, used for converting degrees to radians

void renderScene(cVector *allRenderables, gfxProgram *gfxPrg, camera *cam);
void cleanup(cVector *allTextures, cVector *allTexWrappers, cVector *allModels, cVector *allRenderables, gfxProgram *gfxPrg);

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
	cVector allRenderables; cvInit(&allRenderables, 1);  // Holds renderables

	texture tempTex;
	tLoad(&tempTex, prgPath, "Resources\\Images\\Guy\\Guy.png");
	cvPush(&allTextures, (void *)&tempTex, sizeof(tempTex));
	tLoad(&tempTex, prgPath, "Resources\\Images\\Misc\\Kobold.png");
	cvPush(&allTextures, (void *)&tempTex, sizeof(tempTex));
	tLoad(&tempTex, prgPath, "Resources\\Images\\Misc\\Avatar.png");
	cvPush(&allTextures, (void *)&tempTex, sizeof(tempTex));

	textureWrapper tempTexWrap;
	twLoad(&tempTexWrap, prgPath, "Resources\\Textures\\Static\\AvatarStatic.tdt", &allTextures);
	cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));
	twLoad(&tempTexWrap, prgPath, "Resources\\Textures\\Animated\\GuySpr.tdt", &allTextures);
	cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));
	twLoad(&tempTexWrap, prgPath, "Resources\\Textures\\Animated\\GuyMulti.tdt", &allTextures);
	cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));
	twLoad(&tempTexWrap, prgPath, "Resources\\Textures\\Static\\KoboldStatic.tdt", &allTextures);
	cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));

	model tempMdl;
	mdlCreateSprite(&tempMdl, "sprite");
	cvPush(&allModels, (void *)&tempMdl, sizeof(tempMdl));
	mdlLoadWavefrontObj(&tempMdl, prgPath, "Resources\\Models\\CubeTest.obj");
	cvPush(&allModels, (void *)&tempMdl, sizeof(tempMdl));

	/* Models */
	renderable tempRndr;
	rndrInit(&tempRndr);
	tempRndr.mdl = (model *)cvGet(&allModels, 1);
	tempRndr.tex.texWrap = (textureWrapper *)cvGet(&allTexWrappers, 1);
	cvPush(&allRenderables, (void *)&tempRndr, sizeof(tempRndr));
	tempRndr.tex.texWrap = (textureWrapper *)cvGet(&allTexWrappers, 2);
	tempRndr.sTrans.position.x = 0.25f;
	tempRndr.sTrans.position.y = 0.5f;
	vec3SetS(&tempRndr.rTrans.scale, 0.1f);
	rndrRotateX(&tempRndr, 45.f);
	rndrRotateY(&tempRndr, 45.f);
	tempRndr.rTrans.alpha = 0.5f;
	rndrHudElement(&tempRndr, 1);
	tempRndr.hudScaleMode = 1;
	cvPush(&allRenderables, (void *)&tempRndr, sizeof(tempRndr));

	/* Sprites */
	rndrInit(&tempRndr);
	tempRndr.sprite = 1;
	tempRndr.mdl = (model *)cvGet(&allModels, 0);
	tempRndr.tex.texWrap = (textureWrapper *)cvGet(&allTexWrappers, 0);
	tempRndr.width = 2.f;
	tempRndr.height = 2.f;
	tempRndr.sTrans.relPivot.x = tempRndr.width / 2.f;
	tempRndr.sTrans.relPivot.y = tempRndr.height / 2.f;
	tempRndr.rTrans.scale.x = 2.f;
	tempRndr.rTrans.scale.y = 2.f;
	cvPush(&allRenderables, (void *)&tempRndr, sizeof(tempRndr));
	tempRndr.width = 1.f;
	tempRndr.height = 1.f;
	tempRndr.sTrans.relPivot.x = 0.f;
	tempRndr.sTrans.relPivot.y = 0.f;
	tempRndr.rTrans.scale.x = 0.25f;
	tempRndr.rTrans.scale.y = 0.25f;
	rndrHudElement(&tempRndr, 1);
	tempRndr.hudScaleMode = 1;
	cvPush(&allRenderables, (void *)&tempRndr, sizeof(tempRndr));
	tempRndr.sTrans.position.x = 4.f;
	tempRndr.sTrans.position.y = 0.f;
	tempRndr.sTrans.position.z = -3.f;
	tempRndr.width = 2.f;
	tempRndr.height = 2.f;
	tempRndr.sTrans.relPivot.x = tempRndr.width / 2.f;
	tempRndr.sTrans.relPivot.y = tempRndr.height / 2.f;
	tempRndr.rTrans.scale.x = 2.f;
	tempRndr.rTrans.scale.y = 2.f;
	tempRndr.billboardY = 1;
	rndrHudElement(&tempRndr, 0);
	cvPush(&allRenderables, (void *)&tempRndr, sizeof(tempRndr));
	tempRndr.tex.texWrap = (textureWrapper *)cvGet(&allTexWrappers, 3);
	tempRndr.sTrans.position.x = -3.f;
	tempRndr.sTrans.position.y = -2.f;
	tempRndr.billboardY = 0;
	cvPush(&allRenderables, (void *)&tempRndr, sizeof(tempRndr));


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
			rndrRotateX((renderable *)cvGet(&allRenderables, 0), -2.f * fpsHandler.fpsMod);
			//sprRotateX((sprite *)cvGet(&allSprites, 0), -2.f * fpsHandler.fpsMod);
			camMoveZ(&cam, -0.1f * fpsHandler.fpsMod);
			((renderable *)cvGet(&allRenderables, 4))->rTrans.target = cam.position;
		}
		if(DOWN){
			rndrRotateX((renderable *)cvGet(&allRenderables, 0), 2.f * fpsHandler.fpsMod);
			//sprRotateX((sprite *)cvGet(&allSprites, 0), 2.f * fpsHandler.fpsMod);
			camMoveZ(&cam, 0.1f * fpsHandler.fpsMod);
			((renderable *)cvGet(&allRenderables, 4))->rTrans.target = cam.position;
		}
		if(LEFT){
			rndrRotateY((renderable *)cvGet(&allRenderables, 0), -2.f * fpsHandler.fpsMod);
			rndrRotateZ((renderable *)cvGet(&allRenderables, 3), -2.f * fpsHandler.fpsMod);
			camMoveX(&cam, -0.1f * fpsHandler.fpsMod);
			((renderable *)cvGet(&allRenderables, 4))->rTrans.target = cam.position;
		}
		if(RIGHT){
			rndrRotateY((renderable *)cvGet(&allRenderables, 0), 2.f * fpsHandler.fpsMod);
			rndrRotateZ((renderable *)cvGet(&allRenderables, 3), 2.f * fpsHandler.fpsMod);
			camMoveX(&cam, 0.1f * fpsHandler.fpsMod);
			((renderable *)cvGet(&allRenderables, 4))->rTrans.target = cam.position;
		}

		// Get mouse position relative to its position in the last call
		SDL_GetRelativeMouseState(&mouseRelX, &mouseRelY);


		/* Animate */
		// Animate the renderables
		unsigned int d;
		for(d = 0; d < allRenderables.size; d++){
			rndrAnimateTex((renderable *)cvGet(&allRenderables, d), 1.f);
		}


		/* Render the scene */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		camCreateViewMatrix(&cam, &gfxPrg.viewMatrix);

		// Render the scene
		renderScene(&allRenderables, &gfxPrg, &cam);

		// Update the window
		SDL_GL_SwapWindow(gfxPrg.window);


		/* Pause the program to maintain a constant FPS */
		fpsDelay(&fpsHandler);

	}


	cleanup(&allTextures, &allTexWrappers, &allModels, &allRenderables, &gfxPrg);
	return 0;

}

void cleanup(cVector *allTextures, cVector *allTexWrappers, cVector *allModels, cVector *allRenderables, gfxProgram *gfxPrg){
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
	for(d = 0; d < allRenderables->size; d++){
		rndrDelete((renderable *)cvGet(allRenderables, d));
	}
	/*for(d = 0; d < allSprites->size; d++){
		sprDelete((sprite *)cvGet(allSprites, d));
	}*/
}
