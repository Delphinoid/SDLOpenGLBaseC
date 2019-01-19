#include "modulePhysics.h"
#include "inline.h"
#include <string.h>
#include <math.h>

#define PHYSICS_PAIR_INACTIVE (physCollider *)0x00
#define PHYSICS_PAIR_INVALID  (physCollider *)0x01

static __FORCE_INLINE__ float physContactCalculateRestitution(const float r1, const float r2){
	return r1 >= r2 ? r1 : r2;
}

static __FORCE_INLINE__ float physContactCalculateFriction(const float f1, const float f2){
	return sqrtf(f1 * f2);
}

static __FORCE_INLINE__ void physContactInit(physContact *const restrict contact){

	/*
	** Set the incident and reference bodies
	** and reset the impulse accumulators.
	*/

	physContactPoint *pPoint = &contact->contacts[0];
	const physContactPoint *const pPointLast = &pPoint[COLLISION_MANIFOLD_MAX_CONTACT_POINTS];

	// Initialize the tangent accumulators.
	contact->tangentImpulseAccumulatorA = 0.f;
	contact->tangentImpulseAccumulatorB = 0.f;

	// Initialize the normal accumulators for each contact point.
	for(; pPoint < pPointLast; ++pPoint){
		pPoint->normalImpulseAccumulator = 0.f;
	}

	// Doing a memset appears to be faster than
	// looping through the manifold and reseting
	// only the impulse accumulators. However,
	// it does not scale as well with the various
	// optimization levels.
	// memset(&contact->contacts[0], 0, sizeof(physContactPoint)*COLLISION_MANIFOLD_MAX_CONTACT_POINTS);

}

static __FORCE_INLINE__ void physContactGenerateTangents(const vec3 *const restrict normal, vec3 *const restrict tangentA, vec3 *const restrict tangentB){
	// Generate the contact tangents, perpendicular to each other and the contact normal.
	// Used for frictional calculations.
	if(fabsf(normal->x) >= sqrtf(1.f / 3.f)){  // 0x3F13CD3A
		vec3Set(tangentA, normal->y, -normal->x, 0.f);
	}else{
		vec3Set(tangentA, 0.f, normal->z, -normal->y);
	}
	vec3NormalizeFastAccurate(tangentA);
	vec3CrossR(normal, tangentA, tangentB);
}

static __FORCE_INLINE__ void physContactPointGenerateImpulses(physContactPoint *const restrict point, const physRigidBody *const restrict bodyA, const physRigidBody *const restrict bodyB){

	/*
	** Calculates the impulse magnitude denominators for
	** the contact normal and both contact tangents.
	**
	** 1 / (mA + mB + ((rA X n) . (IA(rA x n))) + ((rB x n) . (IB(rB x n))))
	*/

	const float inverseMassTotal = bodyA->inverseMass + bodyB->inverseMass;
	vec3 angularDeltaA, angularDeltaB;
	vec3 angularDeltaLinearA, angularDeltaLinearB;

	// Calculate the normal impulse magnitude denominator.
	vec3CrossR(&point->pointA, &point->normal, &angularDeltaA);
	mat3MultMByVBraR(&bodyA->inverseInertiaTensorGlobal, &angularDeltaA, &angularDeltaLinearA);
	vec3CrossR(&point->pointB, &point->normal, &angularDeltaB);
	mat3MultMByVBraR(&bodyB->inverseInertiaTensorGlobal, &angularDeltaB, &angularDeltaLinearB);
	point->normalImpulse = 1.f / (
		inverseMassTotal +
		vec3Dot(&angularDeltaA, &angularDeltaLinearA) +
		vec3Dot(&angularDeltaB, &angularDeltaLinearB)
	);

	// Calculate tangent A's impulse magnitude denominator.
	vec3CrossR(&point->pointA, &point->tangentA, &angularDeltaA);
	mat3MultMByVBraR(&bodyA->inverseInertiaTensorGlobal, &angularDeltaA, &angularDeltaLinearA);
	vec3CrossR(&point->pointB, &point->tangentA, &angularDeltaB);
	mat3MultMByVBraR(&bodyB->inverseInertiaTensorGlobal, &angularDeltaB, &angularDeltaLinearB);
	point->tangentImpulseA = 1.f / (
		inverseMassTotal +
		vec3Dot(&angularDeltaA, &angularDeltaLinearA) +
		vec3Dot(&angularDeltaB, &angularDeltaLinearB)
	);

	// Calculate tangent B's impulse magnitude denominator.
	vec3CrossR(&point->pointA, &point->tangentB, &angularDeltaA);
	mat3MultMByVBraR(&bodyA->inverseInertiaTensorGlobal, &angularDeltaA, &angularDeltaLinearA);
	vec3CrossR(&point->pointB, &point->tangentB, &angularDeltaB);
	mat3MultMByVBraR(&bodyB->inverseInertiaTensorGlobal, &angularDeltaB, &angularDeltaLinearB);
	point->tangentImpulseB = 1.f / (
		inverseMassTotal +
		vec3Dot(&angularDeltaA, &angularDeltaLinearA) +
		vec3Dot(&angularDeltaB, &angularDeltaLinearB)
	);

}

static __FORCE_INLINE__ void physContactPointWarmStart(physContactPoint *const restrict point, const physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	/*
	** Warm-start the contact.
	*/

	vec3 impulse;
	vec3 accumulator;

	vec3MultVBySR(&point->normal, point->normalImpulseAccumulator, &impulse);
	vec3MultVBySR(&point->tangentA, contact->tangentImpulseAccumulatorA, &accumulator);
	vec3AddVToV(&impulse, &accumulator);
	vec3MultVBySR(&point->tangentB, contact->tangentImpulseAccumulatorB, &accumulator);
	vec3AddVToV(&impulse, &accumulator);

	// Update body A's linear velocity.
	vec3MultVBySR(&impulse, bodyA->inverseMass, &accumulator);
	vec3SubVFromV1(&bodyA->linearVelocity, &accumulator);
	// Update body A's angular velocity.
	vec3CrossR(&point->pointA, &impulse, &accumulator);
	mat3MultMByVBra(&bodyA->inverseInertiaTensorGlobal, &accumulator);
	vec3SubVFromV1(&bodyA->angularVelocity, &accumulator);

	// Update body B's linear velocity.
	vec3MultVBySR(&impulse, bodyB->inverseMass, &accumulator);
	vec3AddVToV(&bodyB->linearVelocity, &accumulator);
	// Update body B's angular velocity.
	vec3CrossR(&point->pointB, &impulse, &accumulator);
	mat3MultMByVBra(&bodyB->inverseInertiaTensorGlobal, &accumulator);
	vec3AddVToV(&bodyB->angularVelocity, &accumulator);

}

static __FORCE_INLINE__ void physContactPointGenerateBias(physContactPoint *const restrict point, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, physCollider *const restrict colliderA, physCollider *const restrict colliderB, const float dt){

	float temp;
	vec3 v1, v2;

	// Calculate potential slop.
	temp = point->penetrationDepth + PHYSICS_PENETRATION_SLOP;

	// Calculate the bias term.
	point->bias = -PHYSICS_BAUMGARTE_TERM * dt * (temp < 0.f ? temp : 0.f);

	// Apply restitution bias.
	vec3CrossR(&bodyB->angularVelocity, &point->pointB, &v1);
	vec3AddVToV(&v1, &bodyB->linearVelocity);
	vec3CrossR(&bodyA->angularVelocity, &point->pointB, &v2);
	vec3SubVFromV2(&bodyA->linearVelocity, &v2);
	vec3SubVFromV1(&v1, &v2);
	temp = vec3Dot(&v1, &point->normal);

	if(temp < -1.f){
		point->bias += -physContactCalculateRestitution(colliderA->restitution, colliderB->restitution) * temp;
	}

}

static __FORCE_INLINE__ void physContactPointGenerate(physContactPoint *const restrict point, const cContactPoint *const restrict base, const physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, physCollider *const restrict colliderA, physCollider *const restrict colliderB, const float dt){

	// Get the relative contact points.
	vec3SubVFromVR(&base->pointA, &bodyA->centroidGlobal, &point->pointA);
	vec3SubVFromVR(&base->pointB, &bodyB->centroidGlobal, &point->pointB);
	point->normal = base->normal;
	point->penetrationDepth = base->penetrationDepth;

	// Calculate the contact tangents.
	physContactGenerateTangents(&point->normal, &point->tangentA, &point->tangentB);

	// Generate impulses, including the two
	// tangent impulses for friction simulation.
	physContactPointGenerateImpulses(point, bodyA, bodyB);

	// Warm-start the contact.
	physContactPointWarmStart(point, contact, bodyA, bodyB);

	// Generate bias term.
	physContactPointGenerateBias(point, bodyA, bodyB, colliderA, colliderB, dt);

}

static __FORCE_INLINE__ void physContactUpdate(physContact *const restrict contact, const cContact *const restrict manifold, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, physCollider *const restrict colliderA, physCollider *const restrict colliderB, const float dt){

	/*
	** Builds a physContact from a cContact.
	*/

	const cContactPoint *cPoint = &manifold->contacts[0];
	physContactPoint *pPoint = &contact->contacts[0];
	const physContactPoint *const pPointLast = &pPoint[manifold->contactNum];

	contact->contactNum = manifold->contactNum;

	// Calculate extra data for each contact point.
	for(; pPoint < pPointLast; ++pPoint, ++cPoint){
		physContactPointGenerate(pPoint, cPoint, contact, bodyA, bodyB, colliderA, colliderB, dt);
	}

}

static __FORCE_INLINE__ void physContactPersist(physContact *const restrict contact, const cContact *const restrict manifold){

	const cContactPoint *cPoint = &manifold->contacts[0];
	physContactPoint *pcPoint = &contact->contacts[0];
	const cContactPoint *const cPointLast = &cPoint[manifold->contactNum];
	const physContactPoint *const pPointLast = &pcPoint[contact->contactNum];

	// Manage persistent contacts.
	for(; cPoint < cPointLast; ++cPoint, ++pcPoint){
		physContactPoint *pPoint = &contact->contacts[0];
		for(; pPoint < pPointLast; ++pPoint){
			// We've found a contact in the old manifold with a
			// matching key. Swap the impulse accumulators and
			// keys so that the ones we're replacing aren't lost.
			if(memcmp(&pPoint->key, &cPoint->key, sizeof(cContactKey)) == 0){
				if(pPoint != pcPoint){
					const float tempAccumulator = pcPoint->normalImpulseAccumulator;
					const cContactKey tempKey = pcPoint->key;
					pcPoint->normalImpulseAccumulator = pPoint->normalImpulseAccumulator;
					pPoint->normalImpulseAccumulator = tempAccumulator;
					pcPoint->key = pPoint->key;
					pPoint->key = tempKey;
				}
				break;
			}
		}
	}

}


__FORCE_INLINE__ void physContactPairDeactivate(void *const restrict pair){
	((physContactPair *)pair)->colliderA = PHYSICS_PAIR_INACTIVE;
}
__FORCE_INLINE__ void physSeparationPairDeactivate(void *const restrict pair){
	((physSeparationPair *)pair)->colliderA = PHYSICS_PAIR_INACTIVE;
}
__FORCE_INLINE__ void physContactPairInvalidate(void *const restrict pair){
	((physContactPair *)pair)->colliderA = PHYSICS_PAIR_INVALID;
}
__FORCE_INLINE__ void physSeparationPairInvalidate(void *const restrict pair){
	((physSeparationPair *)pair)->colliderA = PHYSICS_PAIR_INVALID;
}

__FORCE_INLINE__ return_t physContactPairIsActive(physContactPair *const restrict pair){
	return pair->colliderA > PHYSICS_PAIR_INVALID;
}
__FORCE_INLINE__ return_t physSeparationPairIsActive(physSeparationPair *const restrict pair){
	return pair->colliderA > PHYSICS_PAIR_INVALID;
}
__FORCE_INLINE__ return_t physContactPairIsInactive(physContactPair *const restrict pair){
	return pair->colliderA <= PHYSICS_PAIR_INVALID;
}
__FORCE_INLINE__ return_t physSeparationPairIsInactive(physSeparationPair *const restrict pair){
	return pair->colliderA <= PHYSICS_PAIR_INVALID;
}
__FORCE_INLINE__ return_t physContactPairIsInvalid(physContactPair *const restrict pair){
	return pair->colliderA == PHYSICS_PAIR_INVALID;
}
__FORCE_INLINE__ return_t physSeparationPairIsInvalid(physSeparationPair *const restrict pair){
	return pair->colliderA == PHYSICS_PAIR_INVALID;
}

__FORCE_INLINE__ void physContactPairRefresh(physContactPair *const restrict pair){
	pair->inactive = 0;
}
__FORCE_INLINE__ void physSeparationPairRefresh(physSeparationPair *const restrict pair){
	pair->inactive = 0;
}

void physContactPairInit(physContactPair *const pair, physCollider *const c1, physCollider *const c2, physContactPair *previous, physContactPair *next){

	/*
	** Initializes a pair using an insertion point.
	*/

	if(previous != NULL){
		// Insert between the previous pair and its next pair.
		previous->nextA = pair;
	}else{
		// Insert directly before the first pair.
		c1->contactCache = pair;
	}
	if(next != NULL){
		if(next->colliderA == c1){
			next->prevA = pair;
		}else{
			next->prevB = pair;
		}
	}
	pair->prevA = previous;
	pair->nextA = next;

	// Find the previous and next nodes for the second collider.
	previous = NULL;
	next = c2->contactCache;
	while(next != NULL && next->colliderA == c2){
		previous = next;
		next = next->nextA;
	}

	if(previous != NULL){
		// Insert between the previous pair and its next pair.
		previous->nextA = pair;
	}else{
		// Insert directly before the first pair.
		c2->contactCache = pair;
	}
	if(next != NULL){
		if(next->colliderA == c2){
			next->prevA = pair;
		}else{
			next->prevB = pair;
		}
	}
	pair->prevB = previous;
	pair->nextB = next;

	// Set the pair's miscellaneous variables.
	pair->colliderA = c1;
	pair->colliderB = c2;
	pair->inactive = 0;

}
void physSeparationPairInit(physSeparationPair *const pair, physCollider *const c1, physCollider *const c2, physSeparationPair *previous, physSeparationPair *next){

	/*
	** Initializes a pair using an insertion point.
	*/

	if(previous != NULL){
		// Insert between the previous pair and its next pair.
		previous->nextA = pair;
	}else{
		// Insert directly before the first pair.
		c1->separationCache = pair;
	}
	if(next != NULL){
		if(next->colliderA == c1){
			next->prevA = pair;
		}else{
			next->prevB = pair;
		}
	}
	pair->prevA = previous;
	pair->nextA = next;

	// Find the previous and next nodes for the second collider.
	previous = NULL;
	next = c2->separationCache;
	while(next != NULL && next->colliderA == c2){
		previous = next;
		next = next->nextA;
	}

	if(previous != NULL){
		// Insert between the previous pair and its next pair.
		previous->nextA = pair;
	}else{
		// Insert directly before the first pair.
		c2->separationCache = pair;
	}
	if(next != NULL){
		if(next->colliderA == c2){
			next->prevA = pair;
		}else{
			next->prevB = pair;
		}
	}
	pair->prevB = previous;
	pair->nextB = next;

	// Set the pair's miscellaneous variables.
	pair->colliderA = c1;
	pair->colliderB = c2;
	pair->inactive = 0;

}
void physContactPairDelete(physContactPair *const pair){

	/*
	** Removes a pair from its linked lists.
	*/

	physContactPair *temp;

	// Remove references from the previous pairs.
	temp = pair->prevA;
	if(temp != NULL){
		if(temp->colliderA == pair->colliderA){
			temp->nextA = pair->nextA;
		}else{
			temp->nextB = pair->nextA;
		}
	}else{
		pair->colliderA->contactCache = pair->nextA;
	}
	temp = pair->prevB;
	if(temp != NULL){
		if(temp->colliderB == pair->colliderB){
			temp->nextB = pair->nextB;
		}else{
			temp->nextA = pair->nextB;
		}
	}else{
		pair->colliderB->contactCache = pair->nextB;
	}

	// Remove references from the next pairs.
	temp = pair->nextA;
	if(temp != NULL){
		if(temp->colliderA == pair->colliderA){
			temp->prevA = pair->prevA;
		}else{
			temp->prevB = pair->prevA;
		}
	}
	temp = pair->nextB;
	if(temp != NULL){
		if(temp->colliderB == pair->colliderB){
			temp->prevB = pair->prevB;
		}else{
			temp->prevA = pair->prevB;
		}
	}

	physContactPairDeactivate(pair);

}
void physSeparationPairDelete(physSeparationPair *const pair){

	/*
	** Removes a pair from its linked lists.
	*/

	physSeparationPair *temp;

	// Remove references from the previous pairs.
	temp = pair->prevA;
	if(temp != NULL){
		if(temp->colliderA == pair->colliderA){
			temp->nextA = pair->nextA;
		}else{
			temp->nextB = pair->nextA;
		}
	}else{
		pair->colliderA->separationCache = pair->nextA;
	}
	temp = pair->prevB;
	if(temp != NULL){
		if(temp->colliderB == pair->colliderB){
			temp->nextB = pair->nextB;
		}else{
			temp->nextA = pair->nextB;
		}
	}else{
		pair->colliderB->separationCache = pair->nextB;
	}

	// Remove references from the next pairs.
	temp = pair->nextA;
	if(temp != NULL){
		if(temp->colliderA == pair->colliderA){
			temp->prevA = pair->prevA;
		}else{
			temp->prevB = pair->prevA;
		}
	}
	temp = pair->nextB;
	if(temp != NULL){
		if(temp->colliderB == pair->colliderB){
			temp->prevB = pair->prevB;
		}else{
			temp->prevA = pair->prevB;
		}
	}

	physSeparationPairDeactivate(pair);

}

return_t physCollisionQuery(aabbNode *const restrict n1, aabbNode *const restrict n2, const float dt){

	/*
	** Manages the broadphase and narrowphase
	** between a potential contact pair.
	*/

	physCollider *const c1 = (physCollider *)n1->data.leaf.value;
	physCollider *const c2 = (physCollider *)n2->data.leaf.value;

	// Prioritize contacts where the first
	// collider has the larger address.
	// Also make sure that they don't share
	// the same owner.
	if(c1 > c2 && c1->body != c2->body){

		// Broadphase collision check.
		// Checks the colliders' collision masks and their tightly-fitting AABBs.
		if((c1->layers & c2->layers) > 0 && cAABBCollision(&c1->aabb, &c2->aabb)){

			cContact manifold;
			physSeparation separation;
			physSeparation *separationPointer;

			// Find a previous separation for the pair, if one exists.
			void *previous;
			void *next;
			void *pair = physColliderFindSeparation(c1, c2, (physSeparationPair **)&previous, (physSeparationPair **)&next);

			// If a separation does exist, check it.
			if(pair != NULL){
				separationPointer = &((physSeparationPair *)pair)->data;
				if(cCheckSeparation(&c1->c, &c2->c, separationPointer)){
					// The separation still exists, refresh it and exit.
					physSeparationPairRefresh((physSeparationPair *)pair);
					return 1;
				}
			}else{
				// Create a new separation.
				separationPointer = &separation;
			}

			// Narrowphase collision check.
			if(cCheckCollision(&c1->c, &c2->c, separationPointer, &manifold)){

				// The two colliders are in contact.
				// If a contact pair already exists, refresh
				// it, otherwise allocate a new pair.
				pair = physColliderFindContact(c1, c2, (physContactPair **)&previous, (physContactPair **)&next);
				if(pair != NULL){
					// The contact already exists, refresh it.
					physContactPairRefresh((physContactPair *)pair);
					physContactPersist(&((physContactPair *)pair)->data, &manifold);
				}else{
					// Allocate a new contact.
					pair = modulePhysicsContactPairAllocate();
					if(pair == NULL){
						/** Memory allocation failure. **/
						return -1;
					}
					// Initialize the pair and its contact.
					physContactPairInit(pair, c1, c2, previous, next);
					physContactInit(&((physContactPair *)pair)->data);
				}
				// Update the contact.
				physContactUpdate(&((physContactPair *)pair)->data, &manifold, c1->body, c2->body, c1, c2, dt);

			}else{

				// The two colliders are not in contact.
				// If a separation pair already exists, refresh
				// it, otherwise allocate a new pair.
				if(pair != NULL){
					// The separation already exists, refresh it.
					physSeparationPairRefresh((physSeparationPair *)pair);
				}else{
					// Allocate a new separation.
					pair = modulePhysicsSeparationPairAllocate();
					if(pair == NULL){
						/** Memory allocation failure. **/
						return -1;
					}
					// Initialize the pair and its separation.
					physSeparationPairInit(pair, c1, c2, previous, next);
					((physSeparationPair *)pair)->data = separation;
				}

			}

		}

	}

	return 1;

}

void physContactSolve(physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	/*
	** Solves a contact.
	*/



}