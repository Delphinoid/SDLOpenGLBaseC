#include "gfxProgram.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

size_t ltostr(long n, char **s);

static unsigned char gfxInitSDL(gfxProgram *gfxPrg);
static unsigned char gfxInitOGL(gfxProgram *gfxPrg);
static unsigned char gfxLoadShaders(gfxProgram *gfxPrg, char *prgPath);
static unsigned char gfxCreateBuffers(gfxProgram *gfxPrg);

unsigned char gfxInitProgram(gfxProgram *gfxPrg, char *prgPath){

	gfxPrg->windowWidth = DEFAULT_WIDTH;
	gfxPrg->windowHeight = DEFAULT_HEIGHT;
	gfxPrg->biggestDimension = gfxPrg->windowWidth > gfxPrg->windowHeight ? gfxPrg->windowWidth : gfxPrg->windowHeight;
	gfxPrg->lastWindowWidth = 0;
	gfxPrg->lastWindowHeight = 0;

	return (gfxInitSDL(gfxPrg) && gfxInitOGL(gfxPrg) && gfxLoadShaders(gfxPrg, prgPath) && gfxCreateBuffers(gfxPrg));

}

static unsigned char gfxInitSDL(gfxProgram *gfxPrg){

	/* Initialize SDL */
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
		printf("Error initializing SDL library: %s\n", SDL_GetError());
		return 0;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, DEFAULT_GL_VERSION_MAJOR);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, DEFAULT_GL_VERSION_MINOR);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Create the window and context
	gfxPrg->window = SDL_CreateWindow("Luna", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DEFAULT_WIDTH, DEFAULT_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if(!gfxPrg->window || !(gfxPrg->context = SDL_GL_CreateContext(gfxPrg->window))){
		printf("Error initializing SDL library: %s\n", SDL_GetError());
		return 0;
	}

	// Disable VSync
	SDL_GL_SetSwapInterval(0);

	// Initialize SDL extension libraries SDL_image and SDL_mixer
	if(!IMG_Init(IMG_INIT_PNG || IMG_INIT_JPG)){
		printf("Error initializing SDL extension library SDL_image: %s\n", IMG_GetError());
		return 0;
	}
	if(Mix_OpenAudio(DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, DEFAULT_CHANNELS, DEFAULT_CHUNKSIZE) < 0){
		printf("Error initializing SDL extension library SDL_mixer: %s\n", Mix_GetError());
		return 0;
	}

	return 1;

}

static unsigned char gfxInitOGL(gfxProgram *gfxPrg){

	/* Initialize GLEW */
	glewExperimental = GL_TRUE;

	GLenum glewError = glewInit();
	if(glewError != GLEW_OK){
		printf("Error initializing GLEW: %s\n", glewGetErrorString(glewError));
		return 0;
	}
	glGetError();  // Flush the error buffer. glewInit() sets it to 1280 (invalid context) with GL versions 3.2 and up


	/* Initialize OpenGL */
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLenum glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error initializing OpenGL: %u\n", glError);
		return 0;
	}


	return 1;

}

static unsigned char gfxLoadShaders(gfxProgram *gfxPrg, char *prgPath){

	/* Vertex shader */
	char *vertexShaderExtra = "Resources\\Shaders\\vertexShader.vsh";
	size_t pathLen = strlen(prgPath);
	size_t vsExtraLen = strlen(vertexShaderExtra);
	char *vertexShaderPath = malloc((pathLen+vsExtraLen+1)*sizeof(char));
	if(vertexShaderPath == NULL){
		printf("Error loading vertex shader:\nMemory allocation failure.\n");
		return 0;
	}
	memcpy(vertexShaderPath, prgPath, pathLen);
	memcpy(vertexShaderPath+pathLen, vertexShaderExtra, vsExtraLen);
	vertexShaderPath[pathLen+vsExtraLen] = '\0';

	/* Load vertex shader */
	FILE *vertexShaderFile = fopen(vertexShaderPath, "rb");
	free(vertexShaderPath);
	fseek(vertexShaderFile, 0, SEEK_END);
	long size = ftell(vertexShaderFile);
	rewind(vertexShaderFile);
	char *vertexShaderCode = malloc((size+1)*sizeof(char));
	if(vertexShaderCode == NULL){
		printf("Error loading vertex shader:\nMemory allocation failure.\n");
		return 0;
	}
	fread(vertexShaderCode, sizeof(char), size, vertexShaderFile);
	vertexShaderCode[size] = '\0';
	fclose(vertexShaderFile);

	/* Compile vertex shader */
	gfxPrg->vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	const char *vertexShaderCodePointer = vertexShaderCode;
	glShaderSource(gfxPrg->vertexShaderID, 1, &vertexShaderCodePointer, NULL);
	free(vertexShaderCode);
	glCompileShader(gfxPrg->vertexShaderID);

	/* Validate vertex shader */
	GLint compileStatus = GL_FALSE;
 	int infoLogLength;
	glGetShaderiv(gfxPrg->vertexShaderID, GL_COMPILE_STATUS, &compileStatus);
 	glGetShaderiv(gfxPrg->vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
 	if(infoLogLength > 1){
		char vertexShaderError[infoLogLength+1];
 		glGetShaderInfoLog(gfxPrg->vertexShaderID, infoLogLength, NULL, &vertexShaderError[0]);
 		printf("Error validating vertex shader: %s", &vertexShaderError[0]);
 		return 0;
 	}


	/* Fragment shader */
	char *fragmentShaderExtra = "Resources\\Shaders\\fragmentShader.fsh";
	size_t fsExtraLen = strlen(fragmentShaderExtra);
	char *fragmentShaderPath = malloc((pathLen+fsExtraLen+1)*sizeof(char));
	if(fragmentShaderPath == NULL){
		printf("Error loading vertex shader:\nMemory allocation failure.\n");
		return 0;
	}
	memcpy(fragmentShaderPath, prgPath, pathLen);
	memcpy(fragmentShaderPath+pathLen, fragmentShaderExtra, fsExtraLen);
	fragmentShaderPath[pathLen+fsExtraLen] = '\0';

	/* Load fragment shader */
	FILE *fragmentShaderFile = fopen(fragmentShaderPath, "rb");
	free(fragmentShaderPath);
	fseek(fragmentShaderFile, 0, SEEK_END);
	size = ftell(fragmentShaderFile);
	rewind(fragmentShaderFile);
	char *fragmentShaderCode = malloc((size+1)*sizeof(char));
	if(fragmentShaderCode == NULL){
		printf("Error loading vertex shader:\nMemory allocation failure.\n");
		return 0;
	}
	fread(fragmentShaderCode, sizeof(char), size, fragmentShaderFile);
	fragmentShaderCode[size] = '\0';
	fclose(fragmentShaderFile);

	/* Compile fragment shader */
	gfxPrg->fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	const char *fragmentShaderCodePointer = fragmentShaderCode;
	glShaderSource(gfxPrg->fragmentShaderID, 1, &fragmentShaderCodePointer, NULL);
	free(fragmentShaderCode);
	glCompileShader(gfxPrg->fragmentShaderID);

	/* Validate fragment shader */
	glGetShaderiv(gfxPrg->fragmentShaderID, GL_COMPILE_STATUS, &compileStatus);
 	glGetShaderiv(gfxPrg->fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
 	if(infoLogLength > 1){
		char fragmentShaderError[infoLogLength+1];
 		glGetShaderInfoLog(gfxPrg->fragmentShaderID, infoLogLength, NULL, &fragmentShaderError[0]);
 		printf("Error validating fragment shader: %s", &fragmentShaderError[0]);
 		return 0;
 	}


	/* Link the program */
	gfxPrg->shaderProgramID = glCreateProgram();
 	glAttachShader(gfxPrg->shaderProgramID, gfxPrg->vertexShaderID);
 	glAttachShader(gfxPrg->shaderProgramID, gfxPrg->fragmentShaderID);
 	glLinkProgram(gfxPrg->shaderProgramID);

 	glDetachShader(gfxPrg->shaderProgramID, gfxPrg->vertexShaderID);
 	glDetachShader(gfxPrg->shaderProgramID, gfxPrg->fragmentShaderID);
 	glDeleteShader(gfxPrg->vertexShaderID);
 	glDeleteShader(gfxPrg->fragmentShaderID);

	/* Use the program */
	glUseProgram(gfxPrg->shaderProgramID);


	/* Link the uniform variables */
	gfxPrg->mvpMatrixID       = glGetUniformLocation(gfxPrg->shaderProgramID, "mvpMatrix");
	gfxPrg->textureFragmentID = glGetUniformLocation(gfxPrg->shaderProgramID, "textureFragment");
	gfxPrg->alphaID           = glGetUniformLocation(gfxPrg->shaderProgramID, "alpha");

	/* Create references to each bone  */
	size_t d;
	for(d = 0; d < MAX_BONE_NUM; d++){

		char *num;
		size_t numLen = ltostr(d, &num);
		char uniformString[21+numLen+2];

		memcpy(&uniformString, "bonePositionArray[", 18);
		memcpy(&uniformString[18], num, numLen);
		uniformString[18+numLen] = ']';
		uniformString[18+numLen+1] = '\0';
		gfxPrg->bonePositionArrayID[d]    = glGetUniformLocation(gfxPrg->shaderProgramID, uniformString);

		memcpy(&uniformString, "boneOrientationArray[", 21);
		memcpy(&uniformString[21], num, numLen);
		uniformString[21+numLen] = ']';
		uniformString[21+numLen+1] = '\0';
		gfxPrg->boneOrientationArrayID[d] = glGetUniformLocation(gfxPrg->shaderProgramID, uniformString);

		free(num);

	}

	/* Create references to each texture sampler */
	for(d = 0; d < MAX_TEX_SAMPLER_NUM; d++){

		char *num;
		size_t numLen = ltostr(d, &num);
		char uniformString[15+numLen+2];

		memcpy(&uniformString, "textureSampler[", 15);
		memcpy(&uniformString+15, num, numLen);
		uniformString[15+numLen] = ']';
		uniformString[15+numLen+1] = '\0';
		gfxPrg->textureSamplerArrayID[d] = glGetUniformLocation(gfxPrg->shaderProgramID, uniformString);
		glUniform1i(gfxPrg->textureSamplerArrayID[d], 0);

		free(num);

	}


	GLenum glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error loading shaders:\n%u\n", glError);
		free(vertexShaderPath);   free(vertexShaderCode);
 		free(fragmentShaderPath); free(fragmentShaderCode);
		return 0;
	}

	free(vertexShaderPath);   free(vertexShaderCode);
	free(fragmentShaderPath); free(fragmentShaderCode);
 	return 1;

}

static unsigned char gfxCreateBuffers(gfxProgram *gfxPrg){

	/* Set lastTexID to 0 since we haven't rendered anything yet */
	gfxPrg->lastTexID = 0;

	/* VAO and VBO for rendering sprites */
	// Create and bind the sprite VAO
	/**glGenVertexArrays(1, &gfxPrg->spriteVaoID);
	glBindVertexArray(gfxPrg->spriteVaoID);
	// Create and bind the sprite VBO
	glGenBuffers(1, &gfxPrg->spriteVboID);
	glBindBuffer(GL_ARRAY_BUFFER, gfxPrg->spriteVboID);
	// Position offset
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, pos));
	glEnableVertexAttribArray(0);
	// UV offset
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, u));
	glEnableVertexAttribArray(1);
	// Normals offset
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, nx));
	glEnableVertexAttribArray(2);
	// We don't want anything else to modify the VAO
	glBindVertexArray(0);**/

	GLenum glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error creating buffers:\n%u\n", glError);
		return 0;
	}
	return 1;

}

void gfxDestroyProgram(gfxProgram *gfxPrg){

	IMG_Quit();
	Mix_Quit();

	SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	SDL_GL_DeleteContext(gfxPrg->context);
	SDL_DestroyWindow(gfxPrg->window);
	SDL_Quit();

	glDeleteProgram(gfxPrg->shaderProgramID);

	if(gfxPrg->spriteVaoID != 0){
		glDeleteVertexArrays(1, &gfxPrg->spriteVaoID);
	}
	if(gfxPrg->spriteVboID != 0){
		glDeleteBuffers(1, &gfxPrg->spriteVboID);
	}

}
