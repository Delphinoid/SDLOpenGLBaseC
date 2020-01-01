#include "graphicsManager.h"
#include "graphicsManagerSettings.h"
#include "sprite.h"
#include "inline.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <string.h>
#include <stdio.h>

/** Tidy loading code, put variable declarations in blocks. **/

__FORCE_INLINE__ static return_t gfxMngrInitSDL(graphicsManager *const restrict gfxMngr){

	// Initialize SDL.
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

__FORCE_INLINE__ static return_t gfxMngrInitOGL(graphicsManager *const restrict gfxMngr){

	GLenum glError;
	GLenum glewError;

	// Initialize GLEW.
	glewExperimental = GL_TRUE;

	glewError = glewInit();
	if(glewError != GLEW_OK){
		printf("Error initializing GLEW: %s\n", glewGetErrorString(glewError));
		return 0;
	}
	// Flush the error buffer. glewInit() sets it to 1280 (invalid context) with GL versions 3.2 and up.
	glGetError();


	// Initialize OpenGL.
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error initializing OpenGL: %u\n", glError);
		return 0;
	}


	return 1;

}

__FORCE_INLINE__ static return_t gfxMngrLoadShaders(graphicsManager *const restrict gfxMngr, const char *const restrict prgPath){
	return_t r;
	if(
		(r = shdrPrgLoad(&gfxMngr->shdrPrgSpr.id, prgPath, "s_vertex_sprite.gls", "s_fragment.gls")) <= 0 ||
		(r = shdrPrgSprLink(&gfxMngr->shdrPrgSpr)) <= 0 ||
		(r = shdrPrgLoad(&gfxMngr->shdrPrgObj.id, prgPath, "s_vertex.gls", "s_fragment.gls")) <= 0 ||
		(r = shdrPrgObjLink(&gfxMngr->shdrPrgObj)) <= 0
	){
		return r;
	}
	return 1;
}

return_t gfxMngrInit(graphicsManager *const restrict gfxMngr, const char *const restrict prgPath){

	return_t r;

	gfxMngr->windowAspectRatioX = GFX_DEFAULT_WINDOW_ASPECT_RATIO_X;
	gfxMngr->windowAspectRatioY = GFX_DEFAULT_WINDOW_ASPECT_RATIO_Y;
	gfxMngr->windowWidth = GFX_DEFAULT_WINDOW_WIDTH;
	gfxMngr->windowHeight = GFX_DEFAULT_WINDOW_HEIGHT;
	gfxMngr->windowStretchToFit = 0;
	gfxMngr->windowModified = 2;

	r = gfxMngrInitSDL(gfxMngr);
	if(r <= 0){
		return r;
	}
	r = gfxMngrInitOGL(gfxMngr);
	if(r <= 0){
		return r;
	}
	shdrDataInit(&gfxMngr->shdrData);
	return gfxMngrLoadShaders(gfxMngr, prgPath);

}

unsigned int gfxMngrWindowChanged(graphicsManager *const restrict gfxMngr){
	int windowWidth, windowHeight;
	SDL_GetWindowSize(gfxMngr->window, &windowWidth, &windowHeight);
	if((int)gfxMngr->windowWidth != windowWidth || (int)gfxMngr->windowHeight != windowHeight){
		gfxMngr->windowWidth = windowWidth;
		gfxMngr->windowHeight = windowHeight;
		gfxMngr->windowModified = 2;
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
	if(gfxMngrWindowChanged(gfxMngr) == 2){
		gfxMngrUpdateViewport(gfxMngr);
		gfxMngr->windowModified = 1;
	}else if(gfxMngr->windowModified > 0){
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
	gfxMngr->windowModified = 2;
}

void gfxMngrSetViewportAspectRatio(graphicsManager *const restrict gfxMngr, const float x, const float y){
	gfxMngr->windowAspectRatioX = x;
	gfxMngr->windowAspectRatioY = y;
	gfxMngr->windowModified = 2;
}

void gfxMngrSetViewportAspectX(graphicsManager *const restrict gfxMngr, const float x){
	gfxMngr->windowAspectRatioX = x;
	gfxMngr->windowModified = 2;
}

void gfxMngrSetViewportAspectY(graphicsManager *const restrict gfxMngr, const float y){
	gfxMngr->windowAspectRatioY = y;
	gfxMngr->windowModified = 2;
}

void gfxMngrSetViewportSize(graphicsManager *const restrict gfxMngr, const unsigned int width, const unsigned int height){
	gfxMngr->viewport.width = width;
	gfxMngr->viewport.height = height;
	gfxMngr->windowModified = 2;
}

void gfxMngrSetViewportWidth(graphicsManager *const restrict gfxMngr, const unsigned int width){
	gfxMngr->viewport.width = width;
	gfxMngr->windowModified = 2;
}

void gfxMngrSetViewportHeight(graphicsManager *const restrict gfxMngr, const unsigned int height){
	gfxMngr->viewport.height = height;
	gfxMngr->windowModified = 2;
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
	if(textureID != gfxMngr->shdrData.lastTexID){
		gfxMngr->shdrData.lastTexID = textureID;
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

	shdrPrgDelete(&gfxMngr->shdrPrgObj);
	shdrPrgDelete(&gfxMngr->shdrPrgSpr);

}
