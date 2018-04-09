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
	rndrConfig tempRndrConfig;  // The object's global space configuration.

	skeleton *skl;              // The object's animation skeleton.
	sklInstance animationData;  // An array of skeletal animations.

	bone *configuration;     // Array of custom bone transformations.
	/** Split these into three arrays for position, orientation and scale? **/
	bone *skeletonState[2];  // The global skeleton states from the previous and last updates.

	signed char physicsSimulate;  // Whether or not to simulate physics on the model's skeleton.
	prbInstance *physicsState;    // An array of physics bodies, one for each bone in mdl.skl.
	hitbox **hitboxState;

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

void objUpdate(object *obj, const camera *cam, const float elapsedTime);
signed char objRenderMethod(object *obj, const float interpT);
void objGenerateSprite(const object *obj, const size_t rndr, const float interpT, const float *texFrag, vertex *vertices);

#endif
