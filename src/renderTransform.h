#ifndef RENDERTRANSFORM_H
#define RENDERTRANSFORM_H

#include <stdlib.h>
#include "vec3.h"

/** Maybe create some shared interpolation functions between spaceTransform and renderTransform? **/
typedef struct {
	/** Not happy with target and changeRot being in here **/
	vec3 target;          // Relative target position, used for target billboards
	vec3 scale;           // Scale of the object (z = 1)
	float alpha;          // A value to multiply against the alpha values of the pixels
} renderTransform;

void rtInit(renderTransform *rt);

#endif
