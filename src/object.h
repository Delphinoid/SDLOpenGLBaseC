#ifndef OBJECT_H
#define OBJECT_H

#include "graphicsRenderGroup.h"
#include "renderable.h"
#include "skeleton.h"
#include "physicsRigidBody.h"
#include "physicsSolver.h"
#include "collider.h"
#include "state.h"

/** Fix skeletons then physics structs. **/

/**                   **/
/** Remove rndrConfig **/
/**                   **/

/**#define OBJECT_MAX_RENDERABLE_NUM 256**/

typedef uint_least8_t renderableIndex_t;

typedef struct {

	/** Include previous state num. **/

	const skeleton *skl;  // The object's recommended animation skeleton.

	animIndex_t animationMax;  // The maximum number of animations that the object can play at once.

	animIndex_t animationNum;  // Number of animations.
	sklAnim **animations;      // Array of pointers to animations associated with the object.

	physRigidBody *skeletonBodies;     // Rigid bodies for each bone.
	colliderArray *skeletonColliders;  // Collider arrays for each bone.

	renderable *renderables;  // Default renderable array.

	stateIndex_t stateMax;  // Maximum number of previous states.

	char *name;

} object;

typedef struct objiState objiState;
typedef struct objiState {

	bone *skeleton;       // Skeleton state.
	objiState *previous;  // Previous state.

} objiState;

/** Store pointer to previous bones / hitboxes. **/
typedef struct {

	/** Access base object animations array? **/
	/** Store model matrix here, update only when changed? **/
	/** Pivot variable? **/

	bone *configuration;  // Array of custom bone transformations.

	sklInstance skeletonData;  // An array of skeletal animation instances.

	stateIndex_t stateMax;  // Maximum number of previous states.

	stateIndex_t stateNum;  // Number of previous states.
	objiState state;
	objiState **oldestStatePrevious;  // The oldest state's previous pointer.

	physRBInstance *skeletonBodies;    // Rigid body instances for each bone.
	colliderArray *skeletonColliders;  // Collider arrays for each bone.

	rndrInstance *renderables;  // Renderable instance array.

	const object *base;

} objInstance;

void objInit(object *const restrict obj);
return_t objLoad(object *const restrict obj, const char *const restrict prgPath, const char *const restrict filePath);
void objDelete(object *const restrict obj);

return_t objiInit(objInstance *const restrict obj);
void objiDelete(objInstance *const restrict obj);

return_t objiInstantiate(objInstance *const restrict obji, const object *const restrict base);

return_t objiStatePreallocate(objInstance *const restrict obji);

return_t objiNewRenderable(objInstance *const restrict obji, model *const mdl, textureWrapper *const tw);
return_t objiNewRenderableFromBase(objInstance *const restrict obji, const renderable *const rndr);
return_t objiNewRenderableFromInstance(objInstance *const restrict obji, const rndrInstance *const rndr);
return_t objiInitSkeleton(objInstance *const restrict obji, const skeleton *const skl);
///return_t objInitPhysics(object *obj);

///void objiBoneSetPhysicsFlags(objInstance *obji, const boneIndex_t boneID, const flags_t flags);

sklAnim *objiGetAnimation(const objInstance *const restrict obji, const animIndex_t id);
sklAnim *objiFindAnimation(const objInstance *const restrict obji, const char *const restrict name);
/**void objiSetAnimationType(objInstance *obji, const animIndex_t slot, const flags_t additive);
return_t objiChangeAnimation(objInstance *obji, const animIndex_t slot, sklAnim *anim, const frameIndex_t frame, const float blendTime);
void objiClearAnimation(objInstance *obji, const animIndex_t slot);

void objiApplyLinearForce(objInstance *obji, const boneIndex_t boneID, const vec3 *F);
void objiApplyAngularForceGlobal(objInstance *obji, const boneIndex_t boneID, const vec3 *F, const vec3 *r);
void objiApplyForceGlobal(objInstance *obji, const boneIndex_t boneID, const vec3 *F, const vec3 *r);

void objiApplyForceAtGlobalPoint(objInstance *obji, const size_t boneID, const vec3 *F, const vec3 *r);
void objiAddLinearVelocity(objInstance *obji, const size_t boneID, const float x, const float y, const float z);
void objiApplyLinearImpulse(objInstance *obji, const size_t boneID, const float x, const float y, const float z);
void objiAddAngularVelocity(objInstance *obji, const size_t boneID, const float angle, const float x, const float y, const float z);**/

return_t objiUpdate(objInstance *const restrict obji, physicsSolver *const restrict solver, const float elapsedTime, const float dt);

gfxRenderGroup_t objiRenderGroup(const objInstance *const restrict obji, const float interpT);
void objiGenerateSprite(const objInstance *const restrict obji, const rndrInstance *const restrict rndr, const float interpT, const float *const restrict texFrag, vertex *const restrict vertices);

#endif
