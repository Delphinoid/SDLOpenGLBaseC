#ifndef HITBOXCOLLISION_H
#define HITBOXCOLLISION_H

#include "hitbox.h"

#define COLLISION_DISTANCE_THRESHOLD 0.0001f

typedef struct {
	vec3 contactNormal;
	float penetrationDepth;
	vec3 contactPointA;
	vec3 contactPointB;
	vec3 contactTangentA;
	vec3 contactTangentB;
} hbCollisionData;

signed char hbCollision(const hitbox* c1, const vec3 *c1c, const hitbox *c2, const vec3 *c2c, hbCollisionData *cd);

#endif
