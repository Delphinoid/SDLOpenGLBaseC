#ifndef PHYSICS_H
#define PHYSICS_H

#include "mesh.h"
#include "interpState.h"

// Forward declaration for sklAnimBlend.
typedef struct physicsJoint physicsJoint;

typedef struct {

	/* Physical constants. */
	mesh hull;
	float area;
	vec3 centroid;
	float mass;  // The mass of the object, in kilograms.

	/* Physical properties required only for simulation. */
	vec3 rotation;  // Rotation of the object relative to its parent.
	vec3 velocity;  // Velocity of the object relative to its parent.

	/* Physical properties required for rendering and validation. */
	/** Remove the ResetInterp() functions. **/
	interpVec3 position;  // Position of the object relative to its parent.
	interpQuat orientation;
	interpVec3 targetPosition;     // Target position, used for target billboards.
	interpQuat targetOrientation;  // Target orientation, used for target billboards.
	interpVec3 scale;

	/* The joint that connects this body to the next. */
	physicsJoint *next;

} physicsBody;

typedef struct physicsJoint {

	/* The body that this joint connects to. */
	physicsBody *body;

	/* Constraints. */
	vec3 *constraintPosition;
	quat *constraintOrientation;
	vec3 constraintOffsetMin;
	vec3 constraintOffsetMax;
	vec3 constraintRotationMin;
	vec3 constraintRotationMax;

} physicsJoint;

typedef struct {
	physicsBody *bodies;  // Depth-first vector of each body.
} kinematicsChain;

signed char gjkCollision(const physicsBody *obj1, const physicsBody *obj2, vec3 *mtv);

#endif
