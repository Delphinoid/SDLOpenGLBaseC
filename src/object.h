#ifndef OBJECT_H
#define OBJECT_H

#include "graphicsRenderGroup.h"
#include "physicsRigidBody.h"
#include "physicsIsland.h"
#include "renderable.h"
#include "skeleton.h"
#include "collider.h"
#include "state.h"

/** Fix skeletons then physics structs. **/

/**                   **/
/** Remove rndrConfig **/
/**                   **/

/**#define OBJECT_MAX_RENDERABLE_NUM 256**/

typedef uint_least8_t renderableIndex_t;

/** TODO: Make the instance in all base / instance pairs less reliant on the base if possible. **/
typedef struct {

	/** Include previous state num. **/

	const skeleton *skl;  // The object's recommended animation skeleton.

	animIndex_t animationMax;  // The maximum number of animations that the object can play at once.

	animIndex_t animationNum;  // Number of animations.
	sklAnim **animations;      // Array of pointers to animations associated with the object.

	physRigidBodyBase *skeletonBodies;  // Rigid bodies for each bone.
	collider *skeletonColliders;        // Collider arrays for each bone.

	renderable *renderables;  // Default renderable array.

	stateIndex_t stateMax;  // Maximum number of previous states.

	char *name;

} objectBase;

/** This should have its own allocator. **/
typedef struct objState objState;
typedef struct objState {

	bone *skeleton;      // Skeleton state.
	objState *previous;  // Previous state.

} objState;

/** Store pointer to previous bones / hitboxes. **/
typedef struct {

	/** Access base object animations array? **/
	/** Store model matrix here, update only when changed? **/
	/** Pivot variable? **/

	bone *configuration;  // Array of custom bone transformations.

	sklInstance skeletonData;  // An array of skeletal animation instances.

	stateIndex_t stateMax;  // Maximum number of previous states.

	stateIndex_t stateNum;  // Number of previous states.
	objState state;
	objState **oldestStatePrevious;  // The oldest state's previous pointer.

	physRigidBody *skeletonBodies;  // Rigid body instances for each bone.
	collider *skeletonColliders;    // Collider arrays for each bone.

	rndrInstance *renderables;  // Renderable instance array.

	const objectBase *base;

} object;

void objBaseInit(objectBase *const restrict base);
return_t objBaseLoad(objectBase *const restrict base, const char *const restrict prgPath, const char *const restrict filePath);
void objBaseDelete(objectBase *const restrict base);

return_t objInit(object *const restrict obj);
void objDelete(object *const restrict obj);

return_t objInstantiate(object *const restrict obj, const objectBase *const restrict base);

return_t objStatePreallocate(object *const restrict obj);

return_t objNewRenderable(object *const restrict obj, model *const mdl, textureWrapper *const tw);
return_t objNewRenderableFromBase(object *const restrict obj, const renderable *const rndr);
return_t objNewRenderableFromInstance(object *const restrict obj, const rndrInstance *const rndr);
return_t objInitSkeleton(object *const restrict obj, const skeleton *const skl);
///return_t objInitPhysics(object *obj);

physRigidBody *objBoneGetPhysicsBody(const object *const restrict obj, const boneIndex_t boneID);
///void objBoneSetPhysicsFlags(object *obj, const boneIndex_t boneID, const flags_t flags);

sklAnim *objGetAnimation(const object *const restrict obj, const animIndex_t id);
sklAnim *objFindAnimation(const object *const restrict obj, const char *const restrict name);
/**void objSetAnimationType(object *obj, const animIndex_t slot, const flags_t additive);
return_t objChangeAnimation(object *obj, const animIndex_t slot, sklAnim *anim, const frameIndex_t frame, const float blendTime);
void objClearAnimation(object *obj, const animIndex_t slot);

void objApplyLinearForce(object *obj, const boneIndex_t boneID, const vec3 *F);
void objApplyAngularForceGlobal(object *obj, const boneIndex_t boneID, const vec3 *F, const vec3 *r);
void objApplyForceGlobal(object *obj, const boneIndex_t boneID, const vec3 *F, const vec3 *r);

void objApplyForceAtGlobalPoint(object *obj, const size_t boneID, const vec3 *F, const vec3 *r);
void objAddLinearVelocity(object *obj, const size_t boneID, const float x, const float y, const float z);
void objApplyLinearImpulse(object *obj, const size_t boneID, const float x, const float y, const float z);
void objAddAngularVelocity(object *obj, const size_t boneID, const float angle, const float x, const float y, const float z);**/

return_t objUpdate(object *const restrict obj, physIsland *const restrict island, const float elapsedTime, const float dt);

gfxRenderGroup_t objRenderGroup(const object *const restrict obj, const float interpT);
void objGenerateSprite(const object *const restrict obj, const rndrInstance *const restrict rndr, const float interpT, const float *const restrict texFrag, vertex *const restrict vertices);

#endif
