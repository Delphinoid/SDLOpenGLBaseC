#ifndef SPACETRANSFORM_H
#define SPACETRANSFORM_H

#include "vec3.h"
#include "quat.h"

/** Maybe create some shared interpolation functions between spaceTransform and renderTransform? **/
typedef struct spaceTransform {

	vec3 position;     // Position of the object
	quat orientation;  // Quaternion representing rotation
	vec3 changeRot;    // Change in orientation, in eulers
	vec3 relPivot;     // The point the object is rotated around (relative to its position)

} spaceTransform;

void stInit(spaceTransform *st);

#endif
