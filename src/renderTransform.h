#ifndef RENDERTRANSFORM_H
#define RENDERTRANSFORM_H

#include "vec3.h"

/** Maybe create some shared interpolation functions between spaceTransform and renderTransform? **/
typedef struct renderTransform {

	/** Not happy with target and changeRot being in here **/
	vec3 target;          // Relative target position, used for target billboards
	vec3 scale;           // Scale of the object (z = 1)
	float alpha;          // A value to multiply against the alpha values of the pixels
	unsigned int currentAnim;   // Position of the current animation in the texture wrapper's animations vector
	unsigned int currentFrame;  // The current frame of the animation
	int timesLooped;
	float frameProgress;  // When this is greater than 1, it progresses the animation by frameProgress (rounded towards 0) frames
	                      // and subtracts frameProgress (rounded towards 0) from itself. The lower the FPS, the higher the value
	                      // that will be added to frameProgress and vice versa

} renderTransform;

void rtInit(renderTransform *rt);

#endif
