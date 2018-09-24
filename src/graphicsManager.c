#include "graphicsManager.h"
#include "helpersMisc.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <string.h>
#include <stdio.h>

static return_t gfxMngrInitSDL(graphicsManager *gfxMngr);
static return_t gfxMngrInitOGL(graphicsManager *gfxMngr);
static return_t gfxMngrLoadShaders(graphicsManager *gfxMngr, const char *prgPath);
static return_t gfxMngrCreateBuffers(graphicsManager *gfxMngr);

return_t gfxMngrInit(graphicsManager *gfxMngr, const char *prgPath){

	return_t r;

	mat4Identity(&gfxMngr->identityMatrix);
	gfxMngr->windowWidth = DEFAULT_WIDTH;
	gfxMngr->windowHeight = DEFAULT_HEIGHT;
	gfxMngr->aspectRatioX = DEFAULT_ASPECT_RATIO_X;
	gfxMngr->aspectRatioY = DEFAULT_ASPECT_RATIO_Y;
	gfxMngr->lastWindowWidth = 0;
	gfxMngr->lastWindowHeight = 0;
	gfxMngr->stretchToFit = 0;
	gfxMngr->windowChanged = 1;

	r = gfxMngrInitSDL(gfxMngr);
	if(r <= 0){
		return r;
	}
	r = gfxMngrInitOGL(gfxMngr);
	if(r <= 0){
		return r;
	}
	r = gfxMngrLoadShaders(gfxMngr, prgPath);
	if(r <= 0){
		return r;
	}
	return gfxMngrCreateBuffers(gfxMngr);

}

static return_t gfxMngrInitSDL(graphicsManager *gfxMngr){

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
	gfxMngr->window = SDL_CreateWindow("Luna", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DEFAULT_WIDTH, DEFAULT_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if(!gfxMngr->window || !(gfxMngr->context = SDL_GL_CreateContext(gfxMngr->window))){
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

static return_t gfxMngrInitOGL(graphicsManager *gfxMngr){

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
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLenum glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error initializing OpenGL: %u\n", glError);
		return 0;
	}


	return 1;

}

static return_t gfxMngrLoadShaders(graphicsManager *gfxMngr, const char *prgPath){

	/* Vertex shader */
	const char *vertexShaderExtra = "Resources\\Shaders\\vertexShader.vsh";
	const size_t pathLen = strlen(prgPath);
	const size_t vsExtraLen = strlen(vertexShaderExtra);
	char *vertexShaderPath = malloc((pathLen+vsExtraLen+1)*sizeof(char));
	if(vertexShaderPath == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	memcpy(vertexShaderPath, prgPath, pathLen);
	memcpy(vertexShaderPath+pathLen, vertexShaderExtra, vsExtraLen);
	vertexShaderPath[pathLen+vsExtraLen] = '\0';

	/* Load vertex shader */
	FILE *vertexShaderFile = fopen(vertexShaderPath, "rb");
	fseek(vertexShaderFile, 0, SEEK_END);
	long size = ftell(vertexShaderFile);
	rewind(vertexShaderFile);
	char *vertexShaderCode = malloc((size+1)*sizeof(char));
	if(vertexShaderCode == NULL){
		/** Memory allocation failure. **/
		free(vertexShaderPath);
		return -1;
	}
	fread(vertexShaderCode, sizeof(char), size, vertexShaderFile);
	vertexShaderCode[size] = '\0';
	fclose(vertexShaderFile);

	/* Compile vertex shader */
	gfxMngr->vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	const char *vertexShaderCodePointer = vertexShaderCode;
	glShaderSource(gfxMngr->vertexShaderID, 1, &vertexShaderCodePointer, NULL);
	glCompileShader(gfxMngr->vertexShaderID);
	free(vertexShaderPath);
	free(vertexShaderCode);

	/* Validate vertex shader */
	GLint compileStatus = GL_FALSE;
 	int infoLogLength;
	glGetShaderiv(gfxMngr->vertexShaderID, GL_COMPILE_STATUS, &compileStatus);
 	glGetShaderiv(gfxMngr->vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
 	if(infoLogLength > 1){
		char *vertexShaderError = malloc((infoLogLength+1)*sizeof(char));
		if(vertexShaderError == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
 		glGetShaderInfoLog(gfxMngr->vertexShaderID, infoLogLength, NULL, &vertexShaderError[0]);
 		printf("Error validating vertex shader: %s", &vertexShaderError[0]);
 		free(vertexShaderError);
 		return 0;
 	}


	/* Fragment shader */
	const char *fragmentShaderExtra = "Resources\\Shaders\\fragmentShader.fsh";
	const size_t fsExtraLen = strlen(fragmentShaderExtra);
	char *fragmentShaderPath = malloc((pathLen+fsExtraLen+1)*sizeof(char));
	if(fragmentShaderPath == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	memcpy(fragmentShaderPath, prgPath, pathLen);
	memcpy(fragmentShaderPath+pathLen, fragmentShaderExtra, fsExtraLen);
	fragmentShaderPath[pathLen+fsExtraLen] = '\0';

	/* Load fragment shader */
	FILE *fragmentShaderFile = fopen(fragmentShaderPath, "rb");
	fseek(fragmentShaderFile, 0, SEEK_END);
	size = ftell(fragmentShaderFile);
	rewind(fragmentShaderFile);
	char *fragmentShaderCode = malloc((size+1)*sizeof(char));
	if(fragmentShaderCode == NULL){
		/** Memory allocation failure. **/
		free(fragmentShaderPath);
		return -1;
	}
	fread(fragmentShaderCode, sizeof(char), size, fragmentShaderFile);
	fragmentShaderCode[size] = '\0';
	fclose(fragmentShaderFile);

	/* Compile fragment shader */
	gfxMngr->fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	const char *fragmentShaderCodePointer = fragmentShaderCode;
	glShaderSource(gfxMngr->fragmentShaderID, 1, &fragmentShaderCodePointer, NULL);
	glCompileShader(gfxMngr->fragmentShaderID);
	free(fragmentShaderPath);
	free(fragmentShaderCode);

	/* Validate fragment shader */
	glGetShaderiv(gfxMngr->fragmentShaderID, GL_COMPILE_STATUS, &compileStatus);
 	glGetShaderiv(gfxMngr->fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
 	if(infoLogLength > 1){
		char *fragmentShaderError = malloc((infoLogLength+1)*sizeof(char));
		if(fragmentShaderError == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
 		glGetShaderInfoLog(gfxMngr->fragmentShaderID, infoLogLength, NULL, &fragmentShaderError[0]);
 		printf("Error validating fragment shader: %s", &fragmentShaderError[0]);
 		free(fragmentShaderError);
 		return 0;
 	}


	/* Link the program */
	gfxMngr->shaderProgramID = glCreateProgram();
 	glAttachShader(gfxMngr->shaderProgramID, gfxMngr->vertexShaderID);
 	glAttachShader(gfxMngr->shaderProgramID, gfxMngr->fragmentShaderID);
 	glLinkProgram(gfxMngr->shaderProgramID);

 	glDetachShader(gfxMngr->shaderProgramID, gfxMngr->vertexShaderID);
 	glDetachShader(gfxMngr->shaderProgramID, gfxMngr->fragmentShaderID);
 	glDeleteShader(gfxMngr->vertexShaderID);
 	glDeleteShader(gfxMngr->fragmentShaderID);

	/* Use the program */
	glUseProgram(gfxMngr->shaderProgramID);


	/* Link the uniform variables */
	gfxMngr->vpMatrixID        = glGetUniformLocation(gfxMngr->shaderProgramID, "vpMatrix");
	gfxMngr->textureFragmentID = glGetUniformLocation(gfxMngr->shaderProgramID, "textureFragment");
	gfxMngr->alphaID           = glGetUniformLocation(gfxMngr->shaderProgramID, "alpha");

	/* Create references to each bone  */
	boneIndex_t i;
	for(i = 0; i < SKL_MAX_BONE_NUM; ++i){

		char num[LTOSTR_MAX_LENGTH];
		const size_t numLen = ltostr(i, &num[0]);
		char uniformString[11+LTOSTR_MAX_LENGTH];  // LTOSTR_MAX_LENGTH includes NULL terminator.

		memcpy(&uniformString[0], "boneArray[", 10*sizeof(char));
		memcpy(&uniformString[10], num, numLen*sizeof(char));
		uniformString[10+numLen] = ']';
		uniformString[10+numLen+1] = '\0';
		gfxMngr->boneArrayID[i] = glGetUniformLocation(gfxMngr->shaderProgramID, uniformString);

		/*memcpy(&uniformString[10+numLen], "].position\0", 11*sizeof(char));
		gfxMngr->bonePositionArrayID[i] = glGetUniformLocation(gfxMngr->shaderProgramID, uniformString);

		memcpy(&uniformString[10+numLen], "].orientation\0", 14*sizeof(char));
		gfxMngr->boneOrientationArrayID[i] = glGetUniformLocation(gfxMngr->shaderProgramID, uniformString);

		memcpy(&uniformString[10+numLen], "].scale\0", 8*sizeof(char));
		gfxMngr->boneScaleArrayID[i] = glGetUniformLocation(gfxMngr->shaderProgramID, uniformString);*/

	}

	/* Create references to each texture sampler */
	for(i = 0; i < GFX_MAX_TEX_SAMPLER_NUM; ++i){

		char num[LTOSTR_MAX_LENGTH];
		const size_t numLen = ltostr(i, &num[0]);
		char uniformString[16+LTOSTR_MAX_LENGTH];  // LTOSTR_MAX_LENGTH includes NULL terminator.

		memcpy(&uniformString[0], "textureSampler[", 15);
		memcpy(&uniformString[15], num, numLen);
		uniformString[15+numLen] = ']';
		uniformString[15+numLen+1] = '\0';
		gfxMngr->textureSamplerArrayID[i] = glGetUniformLocation(gfxMngr->shaderProgramID, uniformString);
		glUniform1i(gfxMngr->textureSamplerArrayID[i], 0);

	}


	GLenum glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error loading shaders: %u\n", glError);
		return 0;
	}
 	return 1;

}

static return_t gfxMngrCreateBuffers(graphicsManager *gfxMngr){

	/* Set lastTexID to 0 since we haven't rendered anything yet */
	gfxMngr->lastTexID = 0;

	/* VAO and VBO for rendering sprites */
	// Create and bind the sprite VAO
	/**glGenVertexArrays(1, &gfxMngr->spriteVaoID);
	glBindVertexArray(gfxMngr->spriteVaoID);
	// Create and bind the sprite VBO
	glGenBuffers(1, &gfxMngr->spriteVboID);
	glBindBuffer(GL_ARRAY_BUFFER, gfxMngr->spriteVboID);
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
		printf("Error creating buffers: %u\n", glError);
		return 0;
	}
	return 1;

}

return_t gfxMngrUpdateWindow(graphicsManager *gfxMngr){
	SDL_GetWindowSize(gfxMngr->window, &gfxMngr->windowWidth, &gfxMngr->windowHeight);
	if(gfxMngr->windowWidth != gfxMngr->lastWindowWidth || gfxMngr->windowHeight != gfxMngr->lastWindowHeight){
		GLint screenX, screenY, screenWidth, screenHeight;
		if(gfxMngr->stretchToFit){
			screenX = 0;
			screenY = 0;
			screenWidth = gfxMngr->windowWidth;
			screenHeight = gfxMngr->windowHeight;
		}else{
			float tempWidth  = gfxMngr->windowWidth  / gfxMngr->aspectRatioX;
			float tempHeight = gfxMngr->windowHeight / gfxMngr->aspectRatioY;
			if(tempWidth > tempHeight){
				screenWidth  = tempHeight * gfxMngr->aspectRatioX;
				screenHeight = gfxMngr->windowHeight;
			}else if(tempWidth < tempHeight){
				screenWidth  = gfxMngr->windowWidth;
				screenHeight = tempWidth * gfxMngr->aspectRatioY;
			}else{
				screenWidth  = gfxMngr->windowWidth;
				screenHeight = gfxMngr->windowHeight;
			}
			screenX = (gfxMngr->windowWidth  - screenWidth)  >> 1;
			screenY = (gfxMngr->windowHeight - screenHeight) >> 1;
		}
		glViewport(screenX, screenY, screenWidth, screenHeight);
		gfxMngr->lastWindowWidth = gfxMngr->windowWidth;
		gfxMngr->lastWindowHeight = gfxMngr->windowHeight;
		gfxMngr->windowChanged = 1;
	}else{
		gfxMngr->windowChanged = 0;
	}
	return gfxMngr->windowChanged;
}

void gfxMngrDestroyProgram(graphicsManager *gfxMngr){

	IMG_Quit();
	Mix_Quit();

	SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	SDL_GL_DeleteContext(gfxMngr->context);
	SDL_DestroyWindow(gfxMngr->window);
	SDL_Quit();

	glDeleteProgram(gfxMngr->shaderProgramID);

	if(gfxMngr->spriteVaoID != 0){
		glDeleteVertexArrays(1, &gfxMngr->spriteVaoID);
	}
	if(gfxMngr->spriteVboID != 0){
		glDeleteBuffers(1, &gfxMngr->spriteVboID);
	}

}
