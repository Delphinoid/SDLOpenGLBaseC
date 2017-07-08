#ifndef SPACETRANSFORM_H
#define SPACETRANSFORM_H

#include "vec3.h"
#include "quat.h"

/** Maybe create some shared interpolation functions between spaceTransform and renderTransform? **/
typedef struct {
	vec3 position;     // Position of the object
	quat orientation;  // Quaternion representing orientation
	vec3 changeRot;    // Change in rotation, in eulers
	vec3 relPivot;     // The point the object is rotated around (relative to its position)
} spaceTransform;

void stInit(spaceTransform *st);

#endif
