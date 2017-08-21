#ifndef GFXPROGRAM_H
#define GFXPROGRAM_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "mat4.h"

#define DEFAULT_GL_VERSION_MAJOR 3
#define DEFAULT_GL_VERSION_MINOR 3
#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 450
#define DEFAULT_ASPECT_RATIO_X 16
#define DEFAULT_ASPECT_RATIO_Y 9
#define DEFAULT_FREQUENCY 22050
#define DEFAULT_CHANNELS 2
#define DEFAULT_CHUNKSIZE 2048

#define MAX_BONE_NUM 128
#define MAX_TEX_SAMPLER_NUM 1

typedef struct {

	// OpenGL / SDL window and context
	SDL_Window *window;
	SDL_GLContext context;

	// Shaders
	GLuint vertexShaderID;
	GLuint fragmentShaderID;
	GLuint shaderProgramID;

	// Per-instance uniforms
	GLuint mvpMatrixID;
	GLuint textureFragmentID;
	GLuint boneArrayID[MAX_BONE_NUM];
	GLuint alphaID;

	// Texture samplers
	GLuint textureSamplerArrayID[MAX_TEX_SAMPLER_NUM];

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
	unsigned char stretchToFit;

} gfxProgram;

unsigned char gfxInitProgram(gfxProgram *gfxPrg, const char *prgPath);
unsigned char gfxUpdateWindow(gfxProgram *gfxPrg);
void gfxDestroyProgram(gfxProgram *gfxPrg);

#endif
