#ifndef OBJECT_H
#define OBJECT_H

#include "graphicsRenderGroup.h"
#include "renderable.h"
#include "skeleton.h"
#include "physicsRigidBody.h"
#include "physicsSolver.h"
#include "hitbox.h"
#include "state.h"

/**                   **/
/** Remove rndrConfig **/
/**                   **/

/**#define OBJ_MAX_RENDERABLE_NUM 256**/

typedef uint_least8_t renderableIndex_t;

typedef struct {

	/** Include previous state num. **/

	char *name;

	skeleton *skl;  // The object's recommended animation skeleton.

	animIndex_t animationNum;       // Number of animations.
	animIndex_t animationCapacity;  // The maximum number of animations that the object can play at once.
	sklAnim **animations;           // Array of pointers to animations associated with the object.

	physRigidBody *skeletonBodies;                 // Rigid bodies for each bone.
	///flags_t *skeletonBodyFlags;                    // Default flags for each rigid body.
	///physConstraintIndex_t *skeletonConstraintNum;  // Number of default constraints for each bone.
	physConstraint **skeletonConstraints;          // Default constraints for each bone.
	hbArray *skeletonHitboxes;                     // Hitboxes for each bone.

	renderableIndex_t renderableNum;  // Default number of attached renderables.
	renderable *renderables;          // Default renderable array.

	stateIndex_t stateNum;  // Maximum number of previous states.

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

	object *base;

	sklInstance skeletonData;  // An array of skeletal animation instances.

	bone *configuration;  // Array of custom bone transformations.

	stateIndex_t stateNum;     // Number of previous states.
	objiState state;
	objiState **oldestStatePrevious;  // The oldest state's previous pointer.

	physRBInstance *skeletonPhysics;  // Array of physics bodies, one for each bone in skl.
	hbArray *skeletonHitboxes;        // Array of hitbox arrays, one for each bone in skl.

	/** Remove renderableNum. **/
	renderableIndex_t renderableNum;  // Current number of attached renderables.
	rndrInstance *renderables;        // Renderable instance array.

} objInstance;

void objInit(object *obj);
/** I don't like the cVectors being passed in here at all. **/
return_t objLoad(object *obj, const char *prgPath, const char *filePath,
                 cVector *allTextures, cVector *allTexWrappers, cVector *allSkeletons, cVector *allModels, cVector *allSklAnimations);
void objDelete(object *obj);

return_t objiInit(objInstance *obj);
return_t objiStateCopy(void *o, void *c);
void objiResetInterpolation(void *obj);
void objiDelete(objInstance *obj);

return_t objiInstantiate(objInstance *obji, object *base);

return_t objiStatePreallocate(objInstance *obji);

return_t objiNewRenderable(objInstance *obji, model *mdl, textureWrapper *tw);
return_t objiNewRenderableFromBase(objInstance *obji, const renderable *rndr);
return_t objiNewRenderableFromInstance(objInstance *obji, const rndrInstance *rndr);
return_t objiDeleteRenderable(objInstance *obji, const renderableIndex_t id);
return_t objiInitSkeleton(objInstance *obji, skeleton *skl);
//return_t objInitPhysics(object *obj);

void objiBoneSetPhysicsFlags(objInstance *obji, const boneIndex_t boneID, const flags_t flags);

sklAnim *objiGetAnimation(objInstance *obji, const animIndex_t id);
sklAnim *objiFindAnimation(const objInstance *obji, const char *name);
void objiSetAnimationType(objInstance *obji, const animIndex_t slot, const flags_t additive);
return_t objiChangeAnimation(objInstance *obji, const animIndex_t slot, sklAnim *anim, const frameIndex_t frame, const float blendTime);
void objiClearAnimation(objInstance *obji, const animIndex_t slot);

void objiApplyLinearForce(objInstance *obji, const boneIndex_t boneID, const vec3 *F);
void objiApplyAngularForceGlobal(objInstance *obji, const boneIndex_t boneID, const vec3 *F, const vec3 *r);
void objiApplyForceGlobal(objInstance *obji, const boneIndex_t boneID, const vec3 *F, const vec3 *r);

/*void objiApplyForceAtGlobalPoint(objInstance *obji, const size_t boneID, const vec3 *F, const vec3 *r);
void objiAddLinearVelocity(objInstance *obji, const size_t boneID, const float x, const float y, const float z);
void objiApplyLinearImpulse(objInstance *obji, const size_t boneID, const float x, const float y, const float z);
void objiAddAngularVelocity(objInstance *obji, const size_t boneID, const float angle, const float x, const float y, const float z);*/

return_t objiUpdate(objInstance *obji, physicsSolver *solver, const float elapsedTime, const float dt);

gfxRenderGroup_t objiRenderGroup(const objInstance *obji, const float interpT);
void objiGenerateSprite(const objInstance *obji, const renderableIndex_t rndr, const float interpT, const float *texFrag, vertex *vertices);

#endif
