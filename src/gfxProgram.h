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
#define DEFAULT_FREQUENCY 22050
#define DEFAULT_CHANNELS 2
#define DEFAULT_CHUNKSIZE 2048

typedef struct gfxProgram {

	// OpenGL / SDL window and context
	SDL_Window *window;
	SDL_GLContext context;

	// Shaders
	GLuint vertexShaderID;
	GLuint fragmentShaderID;

	// Main uniform variables for the shaders
	GLuint shaderProgramID;
	GLuint mvpMatrixID;
	GLuint textureFragmentID;

	// Texture samplers
	GLuint textureSampler0;

	// Extra uniform variables for advanced rendering
	GLuint alphaID;

	// Various matrices for rendering
	mat4 identityMatrix;
	mat4 projectionMatrixFrustum;
	mat4 projectionMatrixOrtho;
	mat4 viewMatrix;

	// Previously bound texture ID for more efficient binding
	GLuint lastTexID;

	// VAO and VBO for rendering sprites
	GLuint spriteVaoID;
	GLuint spriteVboID;

	// Window sizes (should be stored elsewhere)
	int windowWidth;
	int windowHeight;
	int biggestDimension;
	int lastWindowWidth;
	int lastWindowHeight;

} gfxProgram;

unsigned char gfxInitProgram(gfxProgram *gfxPrg, char *prgPath);
unsigned char gfxInitSDL(gfxProgram *gfxPrg);
unsigned char gfxInitOGL(gfxProgram *gfxPrg);
unsigned char gfxLoadShaders(gfxProgram *gfxPrg, char *prgPath);
unsigned char gfxCreateBuffers(gfxProgram *gfxPrg);
void gfxDestroyProgram(gfxProgram *gfxPrg);

#endif
