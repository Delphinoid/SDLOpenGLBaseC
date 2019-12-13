#ifndef GRAPHICSPROGRAM_H
#define GRAPHICSPROGRAM_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "graphicsManagerSettings.h"
#include "graphicsViewport.h"
#include "skeletonShared.h"
#include "bone.h"
#include "vertex.h"
#include "memoryShared.h"

typedef struct graphicsManager {

	// OpenGL / SDL window and context.
	SDL_Window *window;
	SDL_GLContext context;

	// Shaders.
	GLuint vertexShaderID;
	GLuint fragmentShaderID;
	GLuint shaderProgramID;

	// Per-instance uniforms.
	GLuint vpMatrixID;
	GLuint textureFragmentID[GFX_TEXTURE_SAMPLER_NUM];
	GLuint boneArrayID[SKELETON_MAX_BONE_NUM];
	GLuint alphaID;
	GLuint mipID;

	// Uniform buffers.
	vertex sprVertexBatchBuffer[SPRITE_MAX_BATCH_SIZE];  // An array of vertices used for batch rendering sprites.
	bone sklBindAccumulator[SKELETON_MAX_BONE_NUM];      // Accumulates bind states for bones before rendering.
	mat4 sklTransformState[SKELETON_MAX_BONE_NUM];       // Stores the renderable's transform state before rendering.

	// Texture samplers.
	GLuint textureSamplerArrayID[GFX_TEXTURE_SAMPLER_NUM];

	// Previously bound texture ID for more efficient binding.
	GLuint lastTexID;

	// Identity matrix.
	mat4 identityMatrix;

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

	// Current MIP and LODs biases.
	float  biasMIP;
	size_t biasLOD;

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
