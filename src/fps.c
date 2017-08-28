#include "fps.h"
#include <SDL2/SDL.h>

void fpsStart(fps *f, const float dFps, const float cFps){
	f->desiredFps = dFps;
	f->capFps = cFps;
	f->startTick = SDL_GetTicks();
	f->totalFrames = 0;
}

void fpsUpdate(fps *f){
	++f->totalFrames;
	f->frameTick = SDL_GetTicks();
	f->actualFps = (f->totalFrames / (double)(f->frameTick - f->startTick)) * 1000;  // Actual fps = frames per millisecond multiplied by 1,000
	f->fpsMod = f->desiredFps / f->actualFps;
}

void fpsDelay(fps *f){
	if(f->actualFps > f->capFps && f->capFps > 0){
		//while(SDL_GetTicks() < f->frameTick + (1000 / f->capFps)){}  // Pauses until current tick is not less than the first tick of the frame plus how long the frame should be displayed
		//while(SDL_GetTicks() < f->startTick + f->totalFrames * (1000 / f->capFps)){}  // Same as above, slightly more accurate but slightly slower
		SDL_Delay((1000.0 / f->capFps) - (f->frameTick - SDL_GetTicks()));  // Milliseconds per frame minus how long the frame took (SDL_Delay is best for delays over 10 milliseconds)
	}
}
