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

typedef uint_least8_t modelIndex_t;

typedef struct modelBase modelBase;
typedef struct model model;
typedef struct textureWrapper textureWrapper;
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

	sklAnim **animations;      // Array of pointers to animations associated with the object.
	animIndex_t animationNum;  // Number of animations.

	boneIndex_t *skeletonBodyIDs;       // Which bone each rigid body corresponds to.
	physRigidBodyBase *skeletonBodies;  // Rigid bodies for each bone.
	collider *skeletonColliders;        // Collider arrays for each bone.

	const modelBase **models;  // Default renderable array.
	modelIndex_t modelNum;

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

	objState state;
	objState **oldestStatePrevious;  // The oldest state's previous pointer.
	stateIndex_t stateNum;  // Number of previous states.

	physicsBodyIndex_t *skeletonBodyIDs;  // Which bone each rigid body corresponds to.
	physRigidBody *skeletonBodies;        // Rigid body instances for each bone.
	collider *skeletonColliders;          // Collider arrays for each bone.

	model *models;  // Renderable instance array.

	const objectBase *base;

} object;

void objBaseInit(objectBase *const __RESTRICT__ base);
return_t objBaseLoad(objectBase *const __RESTRICT__ base, const char *const __RESTRICT__ filePath, const size_t filePathLength);
void objBaseDelete(objectBase *const __RESTRICT__ base);

return_t objInit(object *const __RESTRICT__ obj);
void objDelete(object *const __RESTRICT__ obj);

return_t objInstantiate(object *const __RESTRICT__ obj, const objectBase *const __RESTRICT__ base);

return_t objStatePreallocate(object *const __RESTRICT__ obj);

return_t objNewModelFromBase(object *const __RESTRICT__ obj, const modelBase *const mdl);
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

void objGenerateSprite(const object *const __RESTRICT__ obj, const model *const __RESTRICT__ mdl, const float interpT, const float *const __RESTRICT__ texFrag, vertex *const __RESTRICT__ vertices);

gfxRenderGroup_t objRenderGroup(const object *const __RESTRICT__ obj, const float interpT);
void objRender(const object *const __RESTRICT__ obj, graphicsManager *const __RESTRICT__ gfxMngr, const camera *const __RESTRICT__ cam, const float distance, const float interpT);

#endif
