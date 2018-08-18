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

#define OBJ_MAX_RENDERABLE_NUM 256

typedef uint8_t renderableIndex_t;

typedef struct {

	char *name;

	skeleton *skl;  // The object's recommended animation skeleton.

	animIndex_t animationNum;       // Number of animations.
	animIndex_t animationCapacity;  // The maximum number of animations that the object can play at once.
	sklAnim **animations;           // Array of pointers to animations associated with the object.

	physRigidBody *skeletonBodies;             // Rigid bodies for each bone.
	flags_t *skeletonBodyFlags;                // Default flags for each rigid body.
	constraintIndex_t *skeletonConstraintNum;  // Number of default constraints for each bone.
	physConstraint **skeletonConstraints;      // Default constraints for each bone.
	hbArray *skeletonHitboxes;                 // Hitboxes for each bone.

	renderableIndex_t renderableNum;  // Default number of attached renderables.
	renderable *renderables;          // Default renderable array.

} object;

typedef struct {

	object *base;
	/** Access base object animations array? **/

	/** Store model matrix here, update only when changed? **/
	/** Remove rndrConfig. **/
	rndrConfig tempRndrConfig;  // The object's global space configuration.

	//skeleton *skl;              // The object's animation skeleton.
	sklInstance skeletonData;  // An array of skeletal animation instances.

	bone *configuration;  // Array of custom bone transformations.
	/** Split these into three arrays for position, orientation and scale? **/
	bone *skeletonState;  // The global skeleton states from the current and previous updates.
	                      // Stored as the current configuration for bone i followed by its
	                      // previous configuration.

	physRBInstance *skeletonPhysics;  // Array of physics bodies, one for each bone in skl.
	hbArray *skeletonHitboxes;  // Array of hitbox arrays, one for each bone in skl.

	renderableIndex_t renderableNum;  // Current number of attached renderables.
	rndrInstance *renderables;        // Renderable instance array.

} objInstance;

void objInit(object *obj);
/** I don't like the cVectors being passed in here at all. **/
signed char objLoad(object *obj, const char *prgPath, const char *filePath,
                    cVector *allTextures, cVector *allTexWrappers, cVector *allSkeletons, cVector *allModels, cVector *allSklAnimations);
void objDelete(object *obj);

signed char objiInit(void *obj);
signed char objiStateCopy(void *o, void *c);
void objiResetInterpolation(void *obj);
void objiDelete(void *obj);

signed char objiInstantiate(objInstance *obji, object *base);

signed char objiNewRenderable(objInstance *obji, model *mdl, textureWrapper *tw);
signed char objiNewRenderableFromBase(objInstance *obji, const renderable *rndr);
signed char objiNewRenderableFromInstance(objInstance *obji, const rndrInstance *rndr);
signed char objiDeleteRenderable(objInstance *obji, const renderableIndex_t id);
signed char objiInitSkeleton(objInstance *obji, skeleton *skl);
//signed char objInitPhysics(object *obj);

void objiBoneSetPhysicsFlags(objInstance *obji, const boneIndex_t boneID, const flags_t flags);

sklAnim *objiGetAnimation(objInstance *obji, const animIndex_t id);
sklAnim *objiFindAnimation(objInstance *obji, const char *name);
void objiSetAnimationType(objInstance *obji, const animIndex_t slot, const signed char additive);
signed char objiChangeAnimation(objInstance *obji, const animIndex_t slot, sklAnim *anim, const frameIndex_t frame, const float blendTime);
void objiClearAnimation(objInstance *obji, const animIndex_t slot);

void objiApplyLinearForce(objInstance *obji, const boneIndex_t boneID, const vec3 *F);
void objiApplyAngularForceGlobal(objInstance *obji, const boneIndex_t boneID, const vec3 *F, const vec3 *r);
void objiApplyForceGlobal(objInstance *obji, const boneIndex_t boneID, const vec3 *F, const vec3 *r);

/*void objiApplyForceAtGlobalPoint(objInstance *obji, const size_t boneID, const vec3 *F, const vec3 *r);
void objiAddLinearVelocity(objInstance *obji, const size_t boneID, const float x, const float y, const float z);
void objiApplyLinearImpulse(objInstance *obji, const size_t boneID, const float x, const float y, const float z);
void objiAddAngularVelocity(objInstance *obji, const size_t boneID, const float angle, const float x, const float y, const float z);*/

void objiUpdate(objInstance *obji, physicsSolver *solver, const float elapsedTime, const float dt);

signed char objiRenderMethod(objInstance *obji, const float interpT);
void objiGenerateSprite(const objInstance *obji, const renderableIndex_t rndr, const float interpT, const float *texFrag, vertex *vertices);

#endif
