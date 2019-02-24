#include "graphicsManager.h"
#include "memoryManager.h"
#include "helpersFileIO.h"
#include "helpersMisc.h"
#include "inline.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <string.h>
#include <stdio.h>

/** Tidy loading code, put variable declarations in blocks. **/

static return_t gfxMngrInitSDL(graphicsManager *const restrict gfxMngr){

	/*
	** Initialize SDL.
	*/
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
		printf("Error initializing SDL library: %s\n", SDL_GetError());
		return 0;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GFX_DEFAULT_GL_VERSION_MAJOR);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GFX_DEFAULT_GL_VERSION_MINOR);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Create the window and context.
	gfxMngr->window = SDL_CreateWindow("Luna", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GFX_DEFAULT_WINDOW_WIDTH, GFX_DEFAULT_WINDOW_HEIGHT, GFX_DEFAULT_WINDOW_FLAGS);
	if(!gfxMngr->window || !(gfxMngr->context = SDL_GL_CreateContext(gfxMngr->window))){
		printf("Error initializing SDL library: %s\n", SDL_GetError());
		return 0;
	}

	// Disable VSync.
	SDL_GL_SetSwapInterval(0);

	// Initialize SDL extension libraries SDL_image and SDL_mixer.
	if(!IMG_Init(IMG_INIT_PNG || IMG_INIT_JPG)){
		printf("Error initializing SDL extension library SDL_image: %s\n", IMG_GetError());
		return 0;
	}
	if(Mix_OpenAudio(GFX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, GFX_DEFAULT_CHANNELS, GFX_DEFAULT_CHUNKSIZE) < 0){
		printf("Error initializing SDL extension library SDL_mixer: %s\n", Mix_GetError());
		return 0;
	}

	return 1;

}

static return_t gfxMngrInitOGL(graphicsManager *const restrict gfxMngr){

	/*
	** Initialize GLEW.
	*/
	glewExperimental = GL_TRUE;

	GLenum glewError = glewInit();
	if(glewError != GLEW_OK){
		printf("Error initializing GLEW: %s\n", glewGetErrorString(glewError));
		return 0;
	}
	// Flush the error buffer. glewInit() sets it to 1280 (invalid context) with GL versions 3.2 and up.
	glGetError();


	/*
	** Initialize OpenGL.
	*/
	glClearColor(0.f, 0.f, 0.f, 0.f);
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

static return_t gfxMngrLoadShaders(graphicsManager *const restrict gfxMngr, const char *const restrict prgPath){

	/*
	** Vertex shader.
	*/
	const char *const restrict vertexShaderExtra = "Resources"FILE_PATH_DELIMITER_STRING"Shaders"FILE_PATH_DELIMITER_STRING"s_vertex.gls";
	const size_t pathLen = strlen(prgPath);
	const size_t vsExtraLen = strlen(vertexShaderExtra);
	char *const restrict vertexShaderPath = memAllocate((pathLen+vsExtraLen+1)*sizeof(char));
	if(vertexShaderPath == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	memcpy(vertexShaderPath, prgPath, pathLen);
	memcpy(vertexShaderPath+pathLen, vertexShaderExtra, vsExtraLen);
	vertexShaderPath[pathLen+vsExtraLen] = '\0';

	// Load vertex shader.
	FILE *const restrict vertexShaderFile = fopen(vertexShaderPath, "rb");
	fseek(vertexShaderFile, 0, SEEK_END);
	long size = ftell(vertexShaderFile);
	rewind(vertexShaderFile);
	char *const vertexShaderCode = memAllocate((size+1)*sizeof(char));
	if(vertexShaderCode == NULL){
		/** Memory allocation failure. **/
		memFree(vertexShaderPath);
		fclose(vertexShaderFile);
		return -1;
	}
	if(fread(vertexShaderCode, sizeof(char), size, vertexShaderFile) != size){
		memFree(vertexShaderPath);
		fclose(vertexShaderFile);
		return -1;
	}
	vertexShaderCode[size] = '\0';
	fclose(vertexShaderFile);

	// Compile vertex shader.
	gfxMngr->vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	const char *const vertexShaderCodePointer = vertexShaderCode;
	glShaderSource(gfxMngr->vertexShaderID, 1, &vertexShaderCodePointer, NULL);
	glCompileShader(gfxMngr->vertexShaderID);
	memFree(vertexShaderPath);
	memFree(vertexShaderCode);

	// Validate vertex shader.
	GLint compileStatus = GL_FALSE;
 	int infoLogLength;
	glGetShaderiv(gfxMngr->vertexShaderID, GL_COMPILE_STATUS, &compileStatus);
 	glGetShaderiv(gfxMngr->vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
 	if(infoLogLength > 1){
		char *const restrict vertexShaderError = memAllocate((infoLogLength+1)*sizeof(char));
		if(vertexShaderError == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
 		glGetShaderInfoLog(gfxMngr->vertexShaderID, infoLogLength, NULL, &vertexShaderError[0]);
 		printf("Error validating vertex shader: %s", &vertexShaderError[0]);
 		memFree(vertexShaderError);
 		return 0;
 	}


	/*
	** Fragment shader.
	*/
	const char *const restrict fragmentShaderExtra = "Resources"FILE_PATH_DELIMITER_STRING"Shaders"FILE_PATH_DELIMITER_STRING"s_fragment.gls";
	const size_t fsExtraLen = strlen(fragmentShaderExtra);
	char *const restrict fragmentShaderPath = memAllocate((pathLen+fsExtraLen+1)*sizeof(char));
	if(fragmentShaderPath == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	memcpy(fragmentShaderPath, prgPath, pathLen);
	memcpy(fragmentShaderPath+pathLen, fragmentShaderExtra, fsExtraLen);
	fragmentShaderPath[pathLen+fsExtraLen] = '\0';

	// Load fragment shader.
	FILE *const restrict fragmentShaderFile = fopen(fragmentShaderPath, "rb");
	fseek(fragmentShaderFile, 0, SEEK_END);
	size = ftell(fragmentShaderFile);
	rewind(fragmentShaderFile);
	char *const fragmentShaderCode = memAllocate((size+1)*sizeof(char));
	if(fragmentShaderCode == NULL){
		/** Memory allocation failure. **/
		memFree(fragmentShaderPath);
		fclose(fragmentShaderFile);
		return -1;
	}
	if(fread(fragmentShaderCode, sizeof(char), size, fragmentShaderFile) != size){
		memFree(fragmentShaderPath);
		fclose(fragmentShaderFile);
		return -1;
	}
	fragmentShaderCode[size] = '\0';
	fclose(fragmentShaderFile);

	// Compile fragment shader.
	gfxMngr->fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	const char *const fragmentShaderCodePointer = fragmentShaderCode;
	glShaderSource(gfxMngr->fragmentShaderID, 1, &fragmentShaderCodePointer, NULL);
	glCompileShader(gfxMngr->fragmentShaderID);
	memFree(fragmentShaderPath);
	memFree(fragmentShaderCode);

	// Validate fragment shader.
	glGetShaderiv(gfxMngr->fragmentShaderID, GL_COMPILE_STATUS, &compileStatus);
 	glGetShaderiv(gfxMngr->fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
 	if(infoLogLength > 1){
		char *const restrict fragmentShaderError = memAllocate((infoLogLength+1)*sizeof(char));
		if(fragmentShaderError == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
 		glGetShaderInfoLog(gfxMngr->fragmentShaderID, infoLogLength, NULL, &fragmentShaderError[0]);
 		printf("Error validating fragment shader: %s", &fragmentShaderError[0]);
 		memFree(fragmentShaderError);
 		return 0;
 	}


	/*
	** Link the program.
	*/
	gfxMngr->shaderProgramID = glCreateProgram();
 	glAttachShader(gfxMngr->shaderProgramID, gfxMngr->vertexShaderID);
 	glAttachShader(gfxMngr->shaderProgramID, gfxMngr->fragmentShaderID);
 	glLinkProgram(gfxMngr->shaderProgramID);

 	glDetachShader(gfxMngr->shaderProgramID, gfxMngr->vertexShaderID);
 	glDetachShader(gfxMngr->shaderProgramID, gfxMngr->fragmentShaderID);
 	glDeleteShader(gfxMngr->vertexShaderID);
 	glDeleteShader(gfxMngr->fragmentShaderID);

	// Use the program.
	glUseProgram(gfxMngr->shaderProgramID);


	/*
	** Link the uniform variables.
	*/
	gfxMngr->vpMatrixID        = glGetUniformLocation(gfxMngr->shaderProgramID, "vpMatrix");
	gfxMngr->textureFragmentID = glGetUniformLocation(gfxMngr->shaderProgramID, "textureFragment");
	gfxMngr->alphaID           = glGetUniformLocation(gfxMngr->shaderProgramID, "alpha");
	gfxMngr->mipID             = glGetUniformLocation(gfxMngr->shaderProgramID, "mip");

	// Create references to each bone.
	boneIndex_t i;
	for(i = 0; i < SKELETON_MAX_BONE_NUM; ++i){

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

	// Create references to each texture sampler.
	for(i = 0; i < GFX_TEXTURE_SAMPLER_NUM; ++i){

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

static return_t gfxMngrCreateBuffers(graphicsManager *const restrict gfxMngr){

	// Set lastTexID to 0 since we haven't rendered anything yet.
	gfxMngr->lastTexID = 0;

	/*
	** VAO and VBO for rendering sprites.
	*/
	// Create and bind the sprite VAO.
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

return_t gfxMngrInit(graphicsManager *const restrict gfxMngr, const char *const restrict prgPath){

	return_t r;

	mat4Identity(&gfxMngr->identityMatrix);
	gfxMngr->windowAspectRatioX = GFX_DEFAULT_WINDOW_ASPECT_RATIO_X;
	gfxMngr->windowAspectRatioY = GFX_DEFAULT_WINDOW_ASPECT_RATIO_Y;
	gfxMngr->windowWidth = GFX_DEFAULT_WINDOW_WIDTH;
	gfxMngr->windowHeight = GFX_DEFAULT_WINDOW_HEIGHT;
	gfxMngr->windowStretchToFit = 0;
	gfxMngr->windowModified = 1;
	gfxMngr->biasMIP = GFX_DEFAULT_BIAS_MIP;
	gfxMngr->biasLOD = GFX_DEFAULT_BIAS_LOD;

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
	glUniform1f(gfxMngr->mipID, GFX_DEFAULT_BIAS_MIP);
	return gfxMngrCreateBuffers(gfxMngr);

}

unsigned int gfxMngrWindowChanged(graphicsManager *const restrict gfxMngr){
	int windowWidth, windowHeight;
	SDL_GetWindowSize(gfxMngr->window, &windowWidth, &windowHeight);
	if((int)gfxMngr->windowWidth != windowWidth || (int)gfxMngr->windowHeight != windowHeight){
		gfxMngr->windowWidth = windowWidth;
		gfxMngr->windowHeight = windowHeight;
		gfxMngr->windowModified = 1;
	}
	return gfxMngr->windowModified;
}

void gfxMngrUpdateViewport(graphicsManager *const restrict gfxMngr){
	if(gfxMngr->windowStretchToFit){
		gfxMngr->viewport.x = 0;
		gfxMngr->viewport.y = 0;
		gfxMngr->viewport.width = gfxMngr->windowWidth;
		gfxMngr->viewport.height = gfxMngr->windowHeight;
	}else{
		float tempWidth  = gfxMngr->windowWidth  / gfxMngr->windowAspectRatioX;
		float tempHeight = gfxMngr->windowHeight / gfxMngr->windowAspectRatioY;
		if(tempWidth > tempHeight){
			gfxMngr->viewport.width  = (unsigned int)tempHeight * gfxMngr->windowAspectRatioX;
			gfxMngr->viewport.height = gfxMngr->windowHeight;
		}else if(tempWidth < tempHeight){
			gfxMngr->viewport.width  = gfxMngr->windowWidth;
			gfxMngr->viewport.height = (unsigned int)tempWidth * gfxMngr->windowAspectRatioY;
		}else{
			gfxMngr->viewport.width  = gfxMngr->windowWidth;
			gfxMngr->viewport.height = gfxMngr->windowHeight;
		}
		gfxMngr->viewport.x = (gfxMngr->windowWidth - gfxMngr->viewport.width)  >> 1;
		gfxMngr->viewport.y = (gfxMngr->windowHeight - gfxMngr->viewport.height) >> 1;
	}
	gfxViewReset(&gfxMngr->viewLast);
}

void gfxMngrUpdateWindow(graphicsManager *const restrict gfxMngr){
	if(gfxMngrWindowChanged(gfxMngr)){
		gfxMngrUpdateViewport(gfxMngr);
		gfxMngr->windowModified = 0;
	}
}

int gfxMngrSetWindowMode(graphicsManager *const restrict gfxMngr, const Uint32 mode){
	int r;
	if(mode == SDL_WINDOW_FULLSCREEN){
		SDL_DisplayMode displayMode;
		SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(gfxMngr->window), &displayMode);
		r = SDL_SetWindowFullscreen(gfxMngr->window, SDL_WINDOW_FULLSCREEN);
		SDL_SetWindowSize(gfxMngr->window, displayMode.w, displayMode.h);
	}else if(mode == 0){
		r = SDL_SetWindowFullscreen(gfxMngr->window, 0);
		SDL_SetWindowSize(gfxMngr->window, GFX_DEFAULT_WINDOW_WIDTH, GFX_DEFAULT_WINDOW_HEIGHT);
	}else{
		return 0;
	}
	return r;
}

void gfxMngrSetWindowFill(graphicsManager *const restrict gfxMngr, const unsigned int fill){
	gfxMngr->windowStretchToFit = fill;
	gfxMngr->windowModified = 1;
}

void gfxMngrSetViewportAspectRatio(graphicsManager *const restrict gfxMngr, const float x, const float y){
	gfxMngr->windowAspectRatioX = x;
	gfxMngr->windowAspectRatioY = y;
	gfxMngr->windowModified = 1;
}

void gfxMngrSetViewportAspectX(graphicsManager *const restrict gfxMngr, const float x){
	gfxMngr->windowAspectRatioX = x;
	gfxMngr->windowModified = 1;
}

void gfxMngrSetViewportAspectY(graphicsManager *const restrict gfxMngr, const float y){
	gfxMngr->windowAspectRatioY = y;
	gfxMngr->windowModified = 1;
}

void gfxMngrSetViewportSize(graphicsManager *const restrict gfxMngr, const unsigned int width, const unsigned int height){
	gfxMngr->viewport.width = width;
	gfxMngr->viewport.height = height;
	gfxMngr->windowModified = 1;
}

void gfxMngrSetViewportWidth(graphicsManager *const restrict gfxMngr, const unsigned int width){
	gfxMngr->viewport.width = width;
	gfxMngr->windowModified = 1;
}

void gfxMngrSetViewportHeight(graphicsManager *const restrict gfxMngr, const unsigned int height){
	gfxMngr->viewport.height = height;
	gfxMngr->windowModified = 1;
}

__FORCE_INLINE__ void gfxMngrSwitchView(graphicsManager *const restrict gfxMngr, const gfxView *const restrict view){
    if(memcmp(view, &gfxMngr->viewLast, sizeof(gfxView))){
		glViewport(
			gfxMngr->viewport.x + (int)(view->x * gfxMngr->viewport.width),
			gfxMngr->viewport.y + (int)(view->y * gfxMngr->viewport.height),
			(unsigned int)view->width * gfxMngr->viewport.width,
			(unsigned int)view->height * gfxMngr->viewport.height
		);
		gfxMngr->viewLast = *view;
    }
}

__FORCE_INLINE__ void gfxMngrBindTexture(graphicsManager *const restrict gfxMngr, const GLenum texture, const GLuint textureID){
	glActiveTexture(texture);
	if(textureID != gfxMngr->lastTexID){
		gfxMngr->lastTexID = textureID;
		glBindTexture(GL_TEXTURE_2D, textureID);
	}
}

void gfxMngrDestroyProgram(graphicsManager *const restrict gfxMngr){

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
