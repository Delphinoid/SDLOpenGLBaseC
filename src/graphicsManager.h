#ifndef GRAPHICSPROGRAM_H
#define GRAPHICSPROGRAM_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "graphicsViewport.h"
#include "vertex.h"
#include "skeleton.h"
#include "memoryShared.h"
#include "mat4.h"

#define GFX_DEFAULT_GL_VERSION_MAJOR 3
#define GFX_DEFAULT_GL_VERSION_MINOR 3
#define GFX_DEFAULT_ASPECT_RATIO_X 16.f
#define GFX_DEFAULT_ASPECT_RATIO_Y 9.f
#define GFX_DEFAULT_WIDTH 800
#define GFX_DEFAULT_HEIGHT 450
#define GFX_DEFAULT_FREQUENCY 22050
#define GFX_DEFAULT_CHANNELS 2
#define GFX_DEFAULT_CHUNKSIZE 2048

#define SPRITE_MAX_BATCH_SIZE (6*64)  /** Move this! **/
#define GFX_TEXTURE_SAMPLER_NUM 1

#define GFX_DEFAULT_BIAS_MIP 0.f
#define GFX_DEFAULT_BIAS_LOD 0

typedef struct {

	// OpenGL / SDL window and context.
	SDL_Window *window;
	SDL_GLContext context;

	// Shaders.
	GLuint vertexShaderID;
	GLuint fragmentShaderID;
	GLuint shaderProgramID;

	// Per-instance uniforms.
	GLuint vpMatrixID;
	GLuint textureFragmentID;
	GLuint boneArrayID[SKELETON_MAX_BONE_NUM];
	/*GLuint bonePositionArrayID[SKELETON_MAX_BONE_NUM];
	GLuint boneOrientationArrayID[SKELETON_MAX_BONE_NUM];
	GLuint boneScaleArrayID[SKELETON_MAX_BONE_NUM];*/
	GLuint alphaID;
	GLuint mipID;

	// Uniform buffers.
	vertex sprVertexBatchBuffer[SPRITE_MAX_BATCH_SIZE];  // An array of vertices used for batch rendering sprites.
	vec3 sklBindAccumulator[SKELETON_MAX_BONE_NUM];      // Accumulates bind states for bones before rendering.
	mat4 sklTransformState[SKELETON_MAX_BONE_NUM];       // Stores the renderable's transform state before rendering.

	// Texture samplers.
	GLuint textureSamplerArrayID[GFX_TEXTURE_SAMPLER_NUM];

	// Previously bound texture ID for more efficient binding.
	GLuint lastTexID;

	// VAO and VBO for rendering sprites.
	GLuint spriteVaoID;
	GLuint spriteVboID;

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
	unsigned int windowWidthLast;
	unsigned int windowHeightLast;
	unsigned int windowStretchToFit;
	unsigned int windowModified;

	// Current MIP and LODs biases.
	float  biasMIP;
	size_t biasLOD;

} graphicsManager;

return_t gfxMngrInit(graphicsManager *gfxMngr, const char *prgPath);
unsigned int gfxMngrUpdateWindow(graphicsManager *gfxMngr);
void gfxMngrSwitchView(graphicsManager *gfxMngr, const gfxView *view);
void gfxMngrBindTexture(graphicsManager *gfxMngr, const GLenum texture, const GLuint textureID);
void gfxMngrDestroyProgram(graphicsManager *gfxMngr);

#endif
