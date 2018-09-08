#ifndef GFXPROGRAM_H
#define GFXPROGRAM_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "vertex.h"
#include "skeleton.h"
#include "memoryShared.h"

#define DEFAULT_GL_VERSION_MAJOR 3
#define DEFAULT_GL_VERSION_MINOR 3
#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 450
#define DEFAULT_ASPECT_RATIO_X 16
#define DEFAULT_ASPECT_RATIO_Y 9
#define DEFAULT_FREQUENCY 22050
#define DEFAULT_CHANNELS 2
#define DEFAULT_CHUNKSIZE 2048

#define SPR_MAX_BATCH_SIZE (6*64)  /** Move this! **/
#define GFX_MAX_TEX_SAMPLER_NUM 1

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
	GLuint boneArrayID[SKL_MAX_BONE_NUM];
	/*GLuint bonePositionArrayID[SKL_MAX_BONE_NUM];
	GLuint boneOrientationArrayID[SKL_MAX_BONE_NUM];
	GLuint boneScaleArrayID[SKL_MAX_BONE_NUM];*/
	GLuint alphaID;

	// Uniform buffers.
	vertex sprVertexBatchBuffer[SPR_MAX_BATCH_SIZE];  // An array of vertices used for batch rendering sprites.
	vec3 sklBindAccumulator[SKL_MAX_BONE_NUM];        // Accumulates bind states for bones before rendering.
	mat4 sklTransformState[SKL_MAX_BONE_NUM];         // Stores the renderable's transform state before rendering.

	// Texture samplers.
	GLuint textureSamplerArrayID[GFX_MAX_TEX_SAMPLER_NUM];

	// Previously bound texture ID for more efficient binding.
	GLuint lastTexID;

	// VAO and VBO for rendering sprites.
	GLuint spriteVaoID;
	GLuint spriteVboID;

	// Identity matrix.
	mat4 identityMatrix;

	// Window sizes (should be stored elsewhere).
	int windowWidth;
	int windowHeight;
	byte_t aspectRatioX;
	byte_t aspectRatioY;
	int lastWindowWidth;
	int lastWindowHeight;
	signed char stretchToFit;
	signed char windowChanged;

} graphicsManager;

signed char gfxMngrInit(graphicsManager *gfxMngr, const char *prgPath);
signed char gfxMngrUpdateWindow(graphicsManager *gfxMngr);
void gfxMngrDestroyProgram(graphicsManager *gfxMngr);

#endif
