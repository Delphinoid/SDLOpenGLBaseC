#ifndef GFXPROGRAM_H
#define GFXPROGRAM_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "model.h"

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

	// OpenGL / SDL window and context
	SDL_Window *window;
	SDL_GLContext context;

	// Shaders
	GLuint vertexShaderID;
	GLuint fragmentShaderID;
	GLuint shaderProgramID;

	// Per-instance uniforms
	GLuint vpMatrixID;
	GLuint textureFragmentID;
	GLuint bonePositionArrayID[SKL_MAX_BONE_NUM];
	GLuint boneOrientationArrayID[SKL_MAX_BONE_NUM];
	GLuint boneScaleArrayID[SKL_MAX_BONE_NUM];
	GLuint alphaID;

	// Uniform buffers
	vertex sprVertexBatchBuffer[SPR_MAX_BATCH_SIZE];  // An array of vertices used for batch rendering sprites.
	bone sklAnimationState[SKL_MAX_BONE_NUM];         // Stores the object's animation state.

	// Texture samplers
	GLuint textureSamplerArrayID[GFX_MAX_TEX_SAMPLER_NUM];

	// Previously bound texture ID for more efficient binding
	GLuint lastTexID;

	// VAO and VBO for rendering sprites
	GLuint spriteVaoID;
	GLuint spriteVboID;

	// Window sizes (should be stored elsewhere)
	int windowWidth;
	int windowHeight;
	unsigned char aspectRatioX;
	unsigned char aspectRatioY;
	int lastWindowWidth;
	int lastWindowHeight;
	signed char stretchToFit;
	signed char windowChanged;

} gfxProgram;

signed char gfxInitProgram(gfxProgram *gfxPrg, const char *prgPath);
signed char gfxUpdateWindow(gfxProgram *gfxPrg);
void gfxDestroyProgram(gfxProgram *gfxPrg);

#endif
