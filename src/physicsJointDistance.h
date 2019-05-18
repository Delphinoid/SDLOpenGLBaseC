#ifndef PHYSICSJOINTDISTANCE_H
#define PHYSICSJOINTDISTANCE_H

#include "vec3.h"

/*
** Constrains two bodies so that they
** stay at a certain distance from each
** other. Can be rigid or spring-like.
*/

typedef struct physJoint physJoint;
typedef struct {

	// Application points in both bodies' local spaces.
	vec3 pointA;
	vec3 pointB;

	// Valid distance range.
	float distanceMin;
	float distanceMax;
	float tolerance;

	// Spring constants.
	float stiffness;
	float damping;

} physJointDistance;

void physJointSolveVelocityConstraintsDistance(const physJoint *const restrict joint);

#endif