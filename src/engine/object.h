#ifndef OBJECT_H
#define OBJECT_H

#include "objectState.h"
#include "skeleton.h"
#include "physicsShared.h"
#include "graphicsRenderGroup.h"
#include "state.h"

/** Fix skeletons then physics structs. **/

/**                   **/
/** Remove rndrConfig **/
/**                   **/

/**#define OBJECT_MAX_RENDERABLE_NUM 256**/

///typedef uint_least8_t renderableIndex_t;

typedef struct model model;
typedef struct textureWrapper textureWrapper;
typedef struct renderableBase renderableBase;
typedef struct renderable renderable;
typedef struct physRigidBodyBase physRigidBodyBase;
typedef struct physRigidBody physRigidBody;
typedef struct collider collider;
typedef struct vertex vertex;
typedef struct camera camera;
typedef struct graphicsManager graphicsManager;

/** TODO: Make the instance in all base / instance pairs less reliant on the base if possible. **/
typedef struct objectBase {

	/** Include previous state num. **/

	const skeleton *skl;  // The object's recommended animation skeleton.

	animIndex_t animationAllocate;  // The number of animations to allocate on instantiation.

	animIndex_t animationNum;  // Number of animations.
	sklAnim **animations;      // Array of pointers to animations associated with the object.

	boneIndex_t *skeletonBodyIDs;       // Which bone each rigid body corresponds to.
	physRigidBodyBase *skeletonBodies;  // Rigid bodies for each bone.
	collider *skeletonColliders;        // Collider arrays for each bone.

	renderableBase *renderables;  // Default renderable array.

	boneIndex_t skeletonBodyNum;      // Number of rigid bodies attached to the skeleton.
	boneIndex_t skeletonColliderNum;  // Number of colliders attached to the skeleton.
	stateIndex_t stateMax;  // Maximum number of previous states.

	char *name;

} objectBase;

/** Store pointer to previous bones / hitboxes. **/
typedef struct object {

	/** Access base object animations array? **/
	/** Store model matrix here, update only when changed? **/
	/** Pivot variable? **/

	transform *configuration;  // Array of custom bone transformations.

	sklInstance skeletonData;  // An array of skeletal animation instances.

	/** Remove these? **/
	physicsBodyIndex_t skeletonBodyNum;  // Number of rigid bodies attached to the skeleton.
	boneIndex_t skeletonColliderNum;     // Number of colliders attached to the skeleton.
	stateIndex_t stateMax;  // Maximum number of previous states.

	stateIndex_t stateNum;  // Number of previous states.
	objectState state;
	objectState **oldestStatePrevious;  // The oldest state's previous pointer.

	physicsBodyIndex_t *skeletonBodyIDs;  // Which bone each rigid body corresponds to.
	physRigidBody *skeletonBodies;        // Rigid body instances for each bone.
	collider *skeletonColliders;          // Collider arrays for each bone.

	renderable *renderables;  // Renderable instance array.

	const objectBase *base;

} object;

void objBaseInit(objectBase *const __RESTRICT__ base);
return_t objBaseLoad(objectBase *const __RESTRICT__ base, const char *const __RESTRICT__ filePath, const size_t filePathLength);
void objBaseDelete(objectBase *const __RESTRICT__ base);

return_t objInit(object *const __RESTRICT__ obj);
void objDelete(object *const __RESTRICT__ obj);

return_t objInstantiate(object *const __RESTRICT__ obj, const objectBase *const __RESTRICT__ base);

return_t objStatePreallocate(object *const __RESTRICT__ obj);

return_t objNewRenderable(object *const __RESTRICT__ obj, model *const mdl, textureWrapper *const tw);
return_t objNewRenderableFromBase(object *const __RESTRICT__ obj, const renderableBase *const rndr);
return_t objNewRenderableFromInstance(object *const __RESTRICT__ obj, const renderable *const rndr);
///return_t objInitSkeleton(object *const __RESTRICT__ obj, const skeleton *const skl);

physRigidBody *objBoneGetPhysicsBody(const object *const __RESTRICT__ obj, const boneIndex_t boneID);

void objPhysicsPrepare(object *const __RESTRICT__ obj);
void objPhysicsBodySimulate(object *const __RESTRICT__ obj, const boneIndex_t boneID);
void objPhysicsBodySuspend(object *const __RESTRICT__ obj, const boneIndex_t boneID);
///void objBoneSetPhysicsFlags(object *obj, const boneIndex_t boneID, const flags_t flags);

sklAnim *objGetAnimation(const object *const __RESTRICT__ obj, const animIndex_t id);
sklAnim *objFindAnimation(const object *const __RESTRICT__ obj, const char *const __RESTRICT__ name);
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

return_t objTick(object *const __RESTRICT__ obj, const float dt_ms);

void objGenerateSprite(const object *const __RESTRICT__ obj, const renderable *const __RESTRICT__ rndr, const float interpT, const float *const __RESTRICT__ texFrag, vertex *const __RESTRICT__ vertices);

gfxRenderGroup_t objRenderGroup(const object *const __RESTRICT__ obj, const float interpT);
void objRender(const object *const __RESTRICT__ obj, graphicsManager *const __RESTRICT__ gfxMngr, const camera *const __RESTRICT__ cam, const float distance, const float interpT);

#endif
