#ifndef COLLISION_H
#define COLLISION_H

#include "collider.h"

#define COLLISION_MAX_CONTACT_POINTS 8
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
	axisIndex_t axisID;  // The ID of a face or an edge.
} cCollisionInfo;

typedef struct {
	float depthSquared;
	vec3 position;
} cCollisionContact;

typedef struct {
	vec3 normal;
	contactIndex_t contactNum;
	cCollisionContact contacts[COLLISION_MAX_CONTACT_POINTS];
	vec3 tangents[COLLISION_CONTACT_TANGENTS];
} cCollisionContactManifold;

return_t cCollision(const collider *c1, const vec3 *c1c, const collider *c2, const vec3 *c2c, cCollisionInfo *info, cCollisionContactManifold *cm);
void cCollisionContactManifoldInit(cCollisionContactManifold *cm);
void cCollisionGenerateContactTangents(const vec3 *normal, vec3 *tangentA, vec3 *tangentB);

#endif
