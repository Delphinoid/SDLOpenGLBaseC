#ifndef GRAPHICSPROGRAM_H
#define GRAPHICSPROGRAM_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "graphicsViewport.h"
#include "shader.h"
#include "memoryShared.h"

/** Should default models, textures and perhaps skeletons be here? **/
/** Actually, maybe work on eliminating this entire structure. **/
/** Shaders and the window / context should be separated. **/

typedef struct graphicsManager {

	// OpenGL / SDL window and context.
	SDL_Window *window;
	SDL_GLContext context;

	// Shader programs.
	shaderProgramObject shdrPrgObj;
	shaderProgramSprite shdrPrgSpr;
	shaderData shdrData;

	// Master viewport and last view rendered.
	gfxViewport viewport;
	gfxView viewLast;

	// Window variables (should be stored elsewhere or in a struct).
	float windowAspectRatioX;
	float windowAspectRatioY;
	unsigned int windowWidth;
	unsigned int windowHeight;
	unsigned int windowStretchToFit;
	unsigned int windowModified;

} graphicsManager;

return_t gfxMngrInit(graphicsManager *const restrict gfxMngr, const char *const restrict prgPath);
unsigned int gfxMngrWindowChanged(graphicsManager *const restrict gfxMngr);
void gfxMngrUpdateWindow(graphicsManager *const restrict gfxMngr);
int gfxMngrSetWindowMode(graphicsManager *const restrict gfxMngr, const Uint32 mode);
void gfxMngrSetWindowFill(graphicsManager *const restrict gfxMngr, const unsigned int fill);
void gfxMngrSetViewportAspectRatio(graphicsManager *const restrict gfxMngr, const float x, const float y);
void gfxMngrSetViewportAspectX(graphicsManager *const restrict gfxMngr, const float x);
void gfxMngrSetViewportAspectY(graphicsManager *const restrict gfxMngr, const float y);
void gfxMngrSetViewportSize(graphicsManager *const restrict gfxMngr, const unsigned int width, const unsigned int height);
void gfxMngrSetViewportWidth(graphicsManager *const restrict gfxMngr, const unsigned int width);
void gfxMngrSetViewportHeight(graphicsManager *const restrict gfxMngr, const unsigned int height);
void gfxMngrSwitchView(graphicsManager *const restrict gfxMngr, const gfxView *const restrict view);
void gfxMngrBindTexture(graphicsManager *const restrict gfxMngr, const GLenum texture, const GLuint textureID);
void gfxMngrDestroyProgram(graphicsManager *const restrict gfxMngr);

#endif
