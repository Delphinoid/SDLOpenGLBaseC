#ifndef HITBOXCOLLISION_H
#define HITBOXCOLLISION_H

#include "hitbox.h"

#define COLLISION_MAX_CONTACT_POINTS 8
#define COLLISION_CONTACT_TANGENTS 2

#define COLLISION_SEPARATION_TYPE_NULL   0
#define COLLISION_SEPARATION_TYPE_FACE_1 1  // hbCollisionInfo contains a face from the first
                                            // collider and a vertex from the second collider.
#define COLLISION_SEPARATION_TYPE_FACE_2 2  // hbCollisionInfo contains a face from the second
                                            // collider and a vertex from the first collider.
#define COLLISION_SEPARATION_TYPE_EDGE   3  // hbCollisionInfo contains two edges.

typedef uint8_t axisIndex_t;
typedef uint8_t contactIndex_t;

typedef struct {
	unsigned char type;
	axisIndex_t axisID;  // The ID of a face or an edge.
} hbCollisionInfo;

typedef struct {
	float depthSquared;
	vec3 position;
} hbCollisionContact;

typedef struct {
	vec3 normal;
	contactIndex_t contactNum;
	hbCollisionContact contacts[COLLISION_MAX_CONTACT_POINTS];
	vec3 tangents[COLLISION_CONTACT_TANGENTS];
} hbCollisionContactManifold;

signed char hbCollision(const hitbox* c1, const vec3 *c1c, const hitbox *c2, const vec3 *c2c, hbCollisionInfo *info, hbCollisionContactManifold *cm);
void hbCollisionContactManifoldInit(hbCollisionContactManifold *cm);
void hbCollisionGenerateContactTangents(const vec3 *normal, vec3 *tangentA, vec3 *tangentB);

#endif
