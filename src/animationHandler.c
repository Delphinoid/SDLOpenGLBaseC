#include "animationHandler.h"

void animInit(animationHandler *a){
	a->delayMod = 1.f;
	a->currentAnim = 0;
	a->currentFrame = 0;
	a->currentLoops = 0;
	a->lastUpdate = 0.f;
}

void animChange(animationHandler *a, size_t newAnim){
	a->currentAnim = newAnim;
	a->currentFrame = 0;
	a->currentLoops = 0;
	a->lastUpdate = 0.f;
}

unsigned char animFinished(animationHandler *a, cVector *frameDelays,
                           int desiredLoops, uint32_t currentTick, float globalDelayMod){
	// Return true if the animation has finished looping, is on the last frame
	// and the last frame has played for long enough
	return a->currentLoops >= desiredLoops ||
	       a->currentAnim >= frameDelays->size-1 ||
	       (currentTick-a->lastUpdate) <
	       *((float *)cvGet(frameDelays, a->currentFrame))*a->delayMod*globalDelayMod;
}

void animAdvance(animationHandler *a, cVector *frameDelays,
                 int desiredLoops, uint32_t currentTick, float globalDelayMod){

	// Make sure lastUpdate has been set
	if(a->lastUpdate == 0.f){
		a->lastUpdate = currentTick;
	}

	// Only animate the texture if the animation has more than one
	// frame and can still be animated
	if(a->delayMod != 0.f && globalDelayMod != 0.f && frameDelays->size > 1 &&
	   (desiredLoops < 0 || a->currentLoops >= desiredLoops ||
	    a->currentAnim >= frameDelays->size-1)){

		// Time passed since last update
		float deltaTime = (currentTick - a->lastUpdate);
		// Multiplier applied to the current frame's delay in order to slow down / speed up the animation
		float totalDelayMod = a->delayMod * globalDelayMod; /**1.f / (a->speed * speedMod);**/
		float currentFrameDelay = *((float *)cvGet(frameDelays, a->currentFrame))*totalDelayMod;

		/* While deltaTime exceeds the time that the current frame should last and the
		texture can still be animated, advance the animation */
		while(deltaTime >= currentFrameDelay &&
		      (desiredLoops < 0 || a->currentLoops < desiredLoops ||
		       a->currentAnim < frameDelays->size-1)){

			// Add the delay to frameProgress and advance the animation
			deltaTime -= currentFrameDelay;
			a->lastUpdate += currentFrameDelay;
			a->currentFrame++;

			// Reset the animation if frame exceeds the number of frames in the animation
			if(a->currentFrame == frameDelays->size){
				a->currentLoops++;
				// If the animation can loop, set it to the first frame
				if(desiredLoops >= a->currentLoops ||
				   desiredLoops < 0){
					a->currentFrame = 0;
				// Otherwise set it to the final frame
				}else{
					a->currentFrame = frameDelays->size-1;
				}
			}

			currentFrameDelay = *((float *)cvGet(frameDelays, a->currentFrame))*totalDelayMod;

		}

	}

}
