#ifndef COLLISION_H
#define COLLISION_H

#include "collider.h"
#include "vec3.h"
#include "return.h"

#define COLLISION_MAX_CONTACT_POINTS 4
#define COLLISION_CONTACT_TANGENTS 2

#define COLLISION_SEPARATION_TYPE_NULL   0
#define COLLISION_SEPARATION_TYPE_FACE_1 1  // hbCollisionInfo contains a face from the first
                                            // collider and a vertex from the second collider.
#define COLLISION_SEPARATION_TYPE_FACE_2 2  // hbCollisionInfo contains a face from the second
                                            // collider and a vertex from the first collider.
#define COLLISION_SEPARATION_TYPE_EDGE   3  // hbCollisionInfo contains two edges.

typedef uint_least8_t collisionType_t;
typedef uint_least8_t axisIndex_t;
typedef uint_least8_t contactIndex_t;

typedef struct {
	collisionType_t type;
	const void *feature;
} cCollisionInfo;

typedef struct {
	vec3 positionA;  // Contact point on collider A's surface.
	vec3 positionB;  // Contact point on collider B's surface.
	float depthSquared;
} cCollisionContact;

typedef struct {
	vec3 normal;
	contactIndex_t contactNum;
	cCollisionContact contacts[COLLISION_MAX_CONTACT_POINTS];
	vec3 tangents[COLLISION_CONTACT_TANGENTS];
} cCollisionContactManifold;

return_t cCollision(const collider *const restrict c1, const vec3 *const restrict c1c, const collider *const restrict c2, const vec3 *const restrict c2c, cCollisionInfo *const restrict info, cCollisionContactManifold *const restrict cm);
void cCollisionContactManifoldInit(cCollisionContactManifold *const restrict cm);
void cCollisionGenerateContactTangents(const vec3 *const restrict normal, vec3 *const restrict tangentA, vec3 *const restrict tangentB);

#endif
