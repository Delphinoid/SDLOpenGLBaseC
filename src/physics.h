#ifndef PHYSICS_H
#define PHYSICS_H

#include "mesh.h"
#include "bone.h"
#include "interpState.h"

typedef struct {

	// Whether or not the body is affected by rigid body dynamics.
	signed char simulate;

	/* Physical constants. */
	mesh hull;      // The shape of the body.
	float area;     // The body's area.
	vec3 centroid;  // The body's center of mass.
	float inverseMass;                 // The reciprocal of the body's mass.
	float inverseInertiaTensor[3][3];  // The inverse of the inertia tensor used for angular momentum.
	float restitutionCoefficient;      // The ratio of energy kept after a collision.

	/* Physical properties. */
	/** Multiples of some of these (such as position and orientation) need to be stored, but I'm lazy for now. **/
	interpVec3 position;  /** Don't like using interpVec3 here. **/
	quat orientation;
	vec3 netForce;
	vec3 netTorque;
	vec3 angularVelocity;
	vec3 angularMomentum;

} physicsBody;

typedef struct {

	/* The positions of the bodies being connected in kinematicsChain.bodies. */
	size_t b0;
	size_t b1;

	/* Constraints. */
	vec3 *constraintPosition;
	quat *constraintOrientation;
	vec3 constraintOffsetMin;
	vec3 constraintOffsetMax;
	vec3 constraintRotationMin;
	vec3 constraintRotationMax;

} physicsJoint;

typedef struct {
	/*
	** A network of rigid bodies. The first acts as the root, which
	** uses an offset to the main position, rotation, etc.
	*/
	physicsBody *bodies;   // A vector of each rigid body in the chain.
	physicsJoint *joints;  // Depth-first vector of each joint.
	bone *skeletonState;  // Stores the last delta state of each animated bone
	                      // from the renderable's skeletal animation instance.
	                      // Used for applying physics to animated skeletons
	                      // by finding the velocity and angular velocity of
	                      // each component.
} kinematicsChain;

signed char gjkCollision(const physicsBody *obj1, const physicsBody *obj2, vec3 *mtv);

#endif
