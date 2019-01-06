#ifndef PHYSICSCOLLIDER_H
#define PHYSICSCOLLIDER_H

#include "physicsRigidBody.h"

/** Pass rigid bodies into jump tables and remove this file? **/

// Forward declarations for inlining.
extern void (* const physColliderGenerateMassJumpTable[COLLIDER_TYPE_NUM])(
	void *const local,
	float *const restrict mass,
	float *const restrict inverseMass,
	vec3 *const restrict centroid,
	const float **const restrict vertexMassArray
);
extern void (* const physColliderGenerateMomentJumpTable[COLLIDER_TYPE_NUM])(
	void *const local,
	mat3 *const restrict inertiaTensor,
	vec3 *const restrict centroid,
	const float **const restrict vertexMassArray
);

void physColliderGenerateMass(physRigidBodyLocal *const restrict local, const float **const vertexMassArray);
void physColliderGenerateMoment(physRigidBodyLocal *const restrict local, const float **const vertexMassArray);
void physColliderDelete(collider *const hull);

#endif
