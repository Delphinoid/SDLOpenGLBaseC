#ifndef OBJECT_H
#define OBJECT_H

#include "interpState.h"
#include "renderable.h"
#include "skeleton.h"
#include "physicsRigidBody.h"
#include "hitbox.h"

/**                   **/
/** Remove rndrConfig **/
/**                   **/

typedef struct {

	char *name;
	/** Store model matrix here, update only when changed? **/
	rndrConfig configuration;  // The object's global space configuration.

	skeleton *skl;              // The object's animation skeleton.
	sklInstance animationData;  // An array of skeletal animations.

	bone *customState;       // Array of custom bone transformations.
	bone *skeletonState[2];  // The local skeleton states from the previous and last updates.

	/** Make part of renderables? **/
	signed char physicsSimulate;  // Whether or not to simulate physics on the object's skeleton.
	physRigidBody *physicsState;  // An array of physics bodies, one for each bone in skl.

	hitbox **hitboxData;  // An array of hitbox arrays, one for each bone in skl.

	size_t renderableNum;
	renderable *renderables;

} object;

signed char objInit(void *obj);
signed char objNew(void *obj);
signed char objStateCopy(void *o, void *c);
void objResetInterpolation(void *obj);
void objDelete(void *obj);

signed char objInitSkeleton(object *obj, skeleton *skl);
signed char objNewRenderable(object *obj);
signed char objDeleteRenderable(object *obj, size_t id);

void objUpdate(object *obj, const float elapsedTime);
signed char objRenderMethod(object *obj, const float interpT);

#endif
