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

	skeleton *skl;  // The object's recommended animation skeleton.

	size_t animationNum;       // Number of animations.
	sklAnim **animations;      // Array of pointers to animations associated with the object.
	size_t animationCapacity;  // The maximum number of animations that the object can play at once.

	physRigidBody *skeletonBodies;         // Rigid bodies for each bone.
	unsigned char *skeletonBodyFlags;      // Default flags for each rigid body.
	size_t *skeletonConstraintNum;         // Number of default constraints for each bone.
	physConstraint **skeletonConstraints;  // Default constraints for each bone.
	hbArray *skeletonHitboxes;             // Hitboxes for each bone.

	size_t renderableNum;     // Default number of attached renderables.
	renderable *renderables;  // Default renderable array.

} object;

typedef struct {

	object *base;
	/** Access base object animations array? **/

	/** Store model matrix here, update only when changed? **/
	/** Remove rndrConfig. **/
	rndrConfig tempRndrConfig;  // The object's global space configuration.

	skeleton *skl;              // The object's animation skeleton.
	sklInstance animationData;  // An array of skeletal animation instances.

	bone *configuration;     // Array of custom bone transformations.
	/** Split these into three arrays for position, orientation and scale? **/
	bone *skeletonState[2];  // The global skeleton states from the current and previous updates.

	physRBInstance *skeletonPhysics;  // Array of physics bodies, one for each bone in skl.
	hbArray *skeletonHitboxes;  // Array of hitbox arrays, one for each bone in skl.

	size_t renderableNum;       // Current number of attached renderables.
	rndrInstance *renderables;  // Renderable instance array.

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
signed char objiNewRenderableFromBase(objInstance *obji, renderable *rndr);
signed char objiNewRenderableFromInstance(objInstance *obji, rndrInstance *rndr);
signed char objiDeleteRenderable(objInstance *obji, size_t id);
signed char objiInitSkeleton(objInstance *obji, skeleton *skl);
//signed char objInitPhysics(object *obj);

void objiBoneSetPhysicsFlags(objInstance *obji, size_t boneID, unsigned char flags);

sklAnim *objiGetAnimation(objInstance *obji, const size_t id);
sklAnim *objiFindAnimation(objInstance *obji, const char *name);
void objiSetAnimationType(objInstance *obji, const size_t slot, const signed char additive);
signed char objiChangeAnimation(objInstance *obji, const size_t slot, sklAnim *anim, const size_t frame, const float blendTime);
void objiClearAnimation(objInstance *obji, const size_t slot);

void objiApplyLinearForce(objInstance *obji, const size_t boneID, const vec3 *F);
void objiApplyAngularForceGlobal(objInstance *obji, const size_t boneID, const vec3 *F, const vec3 *r);
void objiApplyForceGlobal(objInstance *obji, const size_t boneID, const vec3 *F, const vec3 *r);

/*void objiApplyForceAtGlobalPoint(objInstance *obji, const size_t boneID, const vec3 *F, const vec3 *r);
void objiAddLinearVelocity(objInstance *obji, const size_t boneID, const float x, const float y, const float z);
void objiApplyLinearImpulse(objInstance *obji, const size_t boneID, const float x, const float y, const float z);
void objiAddAngularVelocity(objInstance *obji, const size_t boneID, const float angle, const float x, const float y, const float z);*/

void objiUpdate(objInstance *obj, const camera *cam, const float elapsedTime, const float dt);
signed char objiRenderMethod(objInstance *obji, const float interpT);
void objiGenerateSprite(const objInstance *obji, const size_t rndr, const float interpT, const float *texFrag, vertex *vertices);

#endif