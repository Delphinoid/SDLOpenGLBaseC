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

static __FORCE_INLINE__ void physContactPointWarmStart(physContactPoint *const restrict point, const physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	/*
	** Warm-start the contact.
	*/

	vec3 impulse;
	vec3 accumulator;

	vec3MultVBySR(&contact->normal, point->normalImpulseAccumulator, &impulse);
	vec3MultVBySR(&contact->tangentA, point->tangentImpulseAccumulatorA, &accumulator);
	vec3AddVToV(&impulse, &accumulator);
	vec3MultVBySR(&contact->tangentB, point->tangentImpulseAccumulatorB, &accumulator);
	vec3AddVToV(&impulse, &accumulator);

	physRigidBodyApplyImpulseInverse(bodyA, &point->pointA, &impulse);
	physRigidBodyApplyImpulse(bodyB, &point->pointB, &impulse);

}

static __FORCE_INLINE__ void physContactInit(physContact *const restrict contact, const cContact *const restrict manifold, const physRigidBody *const restrict bodyA, const physRigidBody *const restrict bodyB, const physCollider *const restrict colliderA, const physCollider *const restrict colliderB){

	/*
	** Sets the incident and reference bodies
	** and reset the impulse accumulators.
	**
	** Also generates a global normal and tangents.
	*/

	physContactPoint *pPoint = &contact->contacts[0];
	const cContactPoint *cPoint = &manifold->contacts[0];
	const cContactPoint *const cPointLast = &cPoint[manifold->contactNum];

	// Combined contact normal.
	vec3 normal;
	vec3Zero(&normal);

	// Initialize each contact.
	for(; cPoint < cPointLast; ++cPoint, ++pPoint){

		// Add up the combined contact normal.
		vec3AddVToV(&normal, &cPoint->normal);

		// Get the relative contact points.
		vec3SubVFromVR(&cPoint->point, &bodyA->centroidGlobal, &pPoint->pointA);
		vec3SubVFromVR(&cPoint->point, &bodyB->centroidGlobal, &pPoint->pointB);
		pPoint->penetrationDepth = cPoint->penetrationDepth;
		pPoint->key = cPoint->key;

		// Initialize the accumulators.
		pPoint->normalImpulseAccumulator = 0.f;
		pPoint->tangentImpulseAccumulatorA = 0.f;
		pPoint->tangentImpulseAccumulatorB = 0.f;

	}


	// Normalize the new total normal and generate tangents.
	vec3NormalizeFastAccurate(&normal);
	physContactGenerateTangents(&normal, &contact->tangentA, &contact->tangentB);
	contact->normal = normal;

	// Calculate the combined friction and restitution scalars.
	contact->friction    = physContactCalculateFriction   (colliderA->friction,    colliderB->friction);
	contact->restitution = physContactCalculateRestitution(colliderA->restitution, colliderB->restitution);

	contact->contactNum = manifold->contactNum;

}

static __FORCE_INLINE__ void physContactPersist(physContact *const restrict contact, const cContact *const restrict manifold, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, const physCollider *const restrict colliderA, const physCollider *const restrict colliderB){

	/*
	** Copies the accumulators for persisting contacts
	** and resets non-persisting accumulators.
	**
	** Also generates a global normal and tangents.
	*/

	const cContactPoint *cPoint = &manifold->contacts[0];
	physContactPoint *pcPoint = &contact->contacts[0];
	const cContactPoint *const cPointLast = &cPoint[manifold->contactNum];
	const physContactPoint *const pPointLast = &pcPoint[contact->contactNum];

	// Keep track of the contacts we're updating.
	unsigned int persistent[COLLISION_MANIFOLD_MAX_CONTACT_POINTS];
	unsigned int *flag = &persistent[0];

	// Combined contact normal.
	vec3 normal;

	vec3Zero(&normal);
	memset(persistent, 0, sizeof(unsigned int)*COLLISION_MANIFOLD_MAX_CONTACT_POINTS);

	// Manage persistent contacts.
	for(; cPoint < cPointLast; ++cPoint, ++pcPoint, ++flag){

		physContactPoint *pPoint = &contact->contacts[0];
		for(; pPoint < pPointLast; ++pPoint){

			// We've found a contact in the old manifold with a
			// matching key. Swap the impulse accumulators and
			// keys so that the ones we're replacing aren't lost.
			if(memcmp(&pPoint->key, &cPoint->key, sizeof(cContactKey)) == 0){

				if(pPoint != pcPoint){

					const cContactKey tempKey = pcPoint->key;
					float tempAccumulator;

					pcPoint->key = pPoint->key;
					pPoint->key = tempKey;

					tempAccumulator = pcPoint->normalImpulseAccumulator;
					pcPoint->normalImpulseAccumulator = pPoint->normalImpulseAccumulator;
					pPoint->normalImpulseAccumulator = tempAccumulator;

					tempAccumulator = pcPoint->tangentImpulseAccumulatorA;
					pcPoint->tangentImpulseAccumulatorA = pPoint->tangentImpulseAccumulatorA;
					pPoint->tangentImpulseAccumulatorA = tempAccumulator;

					tempAccumulator = pcPoint->tangentImpulseAccumulatorB;
					pcPoint->tangentImpulseAccumulatorB = pPoint->tangentImpulseAccumulatorB;
					pPoint->tangentImpulseAccumulatorB = tempAccumulator;

				}

				*flag = 1;
				break;

			}

		}

		// Add up the combined contact normal.
		vec3AddVToV(&normal, &cPoint->normal);

	}

	// Initialize the accumulators for non-persisting contacts.
	cPoint = &manifold->contacts[0];
	pcPoint = &contact->contacts[0];
	flag = &persistent[0];
	for(; cPoint < cPointLast; ++cPoint, ++pcPoint, ++flag){

		// Get the relative contact points.
		vec3SubVFromVR(&cPoint->point, &bodyA->centroidGlobal, &pcPoint->pointA);
		vec3SubVFromVR(&cPoint->point, &bodyB->centroidGlobal, &pcPoint->pointB);
		pcPoint->penetrationDepth = cPoint->penetrationDepth;

		if(*flag == 0){
			// Initialize the accumulators for non-persistent contacts.
			pcPoint->key = cPoint->key;
			pcPoint->normalImpulseAccumulator = 0.f;
			pcPoint->tangentImpulseAccumulatorA = 0.f;
			pcPoint->tangentImpulseAccumulatorB = 0.f;
		}else{
			// Warm-start persistent contacts.
			physContactPointWarmStart(pcPoint, contact, bodyA, bodyB);
		}

	}

	// Normalize the new total normal and generate tangents.
	vec3NormalizeFastAccurate(&normal);
	physContactGenerateTangents(&normal, &contact->tangentA, &contact->tangentB);
	contact->normal = normal;

	// Calculate the combined friction and restitution scalars.
	contact->friction    = physContactCalculateFriction   (colliderA->friction,    colliderB->friction);
	contact->restitution = physContactCalculateRestitution(colliderA->restitution, colliderB->restitution);

	contact->contactNum = manifold->contactNum;

}

static __FORCE_INLINE__ void physContactPointGenerateImpulses(physContactPoint *const restrict point, const physContact *const restrict contact, const physRigidBody *const restrict bodyA, const physRigidBody *const restrict bodyB){

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
	vec3CrossR(&point->pointA, &contact->normal, &angularDeltaA);
	mat3MultMByVBraR(&bodyA->inverseInertiaTensorGlobal, &angularDeltaA, &angularDeltaLinearA);
	vec3CrossR(&point->pointB, &contact->normal, &angularDeltaB);
	mat3MultMByVBraR(&bodyB->inverseInertiaTensorGlobal, &angularDeltaB, &angularDeltaLinearB);
	point->normalImpulseDenominator = 1.f / (
		inverseMassTotal +
		vec3Dot(&angularDeltaA, &angularDeltaLinearA) +
		vec3Dot(&angularDeltaB, &angularDeltaLinearB)
	);

	// Calculate tangent A's impulse magnitude denominator.
	vec3CrossR(&contact->tangentA, &point->pointA, &angularDeltaA);
	mat3MultMByVBraR(&bodyA->inverseInertiaTensorGlobal, &angularDeltaA, &angularDeltaLinearA);
	vec3CrossR(&contact->tangentA, &point->pointB, &angularDeltaB);
	mat3MultMByVBraR(&bodyB->inverseInertiaTensorGlobal, &angularDeltaB, &angularDeltaLinearB);
	point->tangentImpulseDenominatorA = 1.f / (
		inverseMassTotal +
		vec3Dot(&angularDeltaA, &angularDeltaLinearA) +
		vec3Dot(&angularDeltaB, &angularDeltaLinearB)
	);

	// Calculate tangent B's impulse magnitude denominator.
	vec3CrossR(&contact->tangentB, &point->pointA, &angularDeltaA);
	mat3MultMByVBraR(&bodyA->inverseInertiaTensorGlobal, &angularDeltaA, &angularDeltaLinearA);
	vec3CrossR(&contact->tangentB, &point->pointB, &angularDeltaB);
	mat3MultMByVBraR(&bodyB->inverseInertiaTensorGlobal, &angularDeltaB, &angularDeltaLinearB);
	point->tangentImpulseDenominatorB = 1.f / (
		inverseMassTotal +
		vec3Dot(&angularDeltaA, &angularDeltaLinearA) +
		vec3Dot(&angularDeltaB, &angularDeltaLinearB)
	);

}

static __FORCE_INLINE__ void physContactPointGenerateBias(physContactPoint *const restrict point, const physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, physCollider *const restrict colliderA, physCollider *const restrict colliderB, const float dt){

	float temp;
	vec3 v1, v2;

	// Calculate potential slop.
	temp = point->penetrationDepth + PHYSICS_PENETRATION_SLOP;

	// Calculate the bias term.
	point->bias = -PHYSICS_BAUMGARTE_TERM * dt * (temp < 0.f ? temp : 0.f);

	// Apply restitution bias.
	vec3CrossR(&bodyB->angularVelocity, &point->pointB, &v1);
	vec3AddVToV(&v1, &bodyB->linearVelocity);
	vec3CrossR(&bodyA->angularVelocity, &point->pointA, &v2);
	vec3SubVFromV1(&v1, &bodyA->linearVelocity);
	vec3SubVFromV1(&v1, &v2);
	temp = vec3Dot(&v1, &contact->normal);
	if(temp < -PHYSICS_RESTITUTION_THRESHOLD){
		point->bias += -contact->restitution * temp;
	}

}

static __FORCE_INLINE__ void physContactPointGenerate(physContactPoint *const restrict point, const physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, physCollider *const restrict colliderA, physCollider *const restrict colliderB, const float dt){

	// Generate impulses, including the two
	// tangent impulses for friction simulation.
	physContactPointGenerateImpulses(point, contact, bodyA, bodyB);

	// Generate bias term.
	physContactPointGenerateBias(point, contact, bodyA, bodyB, colliderA, colliderB, dt);

}

__FORCE_INLINE__ void physContactUpdate(physContact *const restrict contact, physCollider *const restrict colliderA, physCollider *const restrict colliderB, const float dt){

	/*
	** Builds a physContact from a cContact.
	*/

	physRigidBody *const bodyA = colliderA->body;
	physRigidBody *const bodyB = colliderB->body;

	physContactPoint *pPoint = &contact->contacts[0];
	const physContactPoint *const pPointLast = &pPoint[contact->contactNum];

	// Calculate extra data for each contact point.
	for(; pPoint < pPointLast; ++pPoint){
		physContactPointGenerate(pPoint, contact, bodyA, bodyB, colliderA, colliderB, dt);
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

return_t physCollisionQuery(aabbNode *const restrict n1, aabbNode *const restrict n2){

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
					//return 1;
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
					physContactPersist(&((physContactPair *)pair)->data, &manifold, c1->body, c2->body, c1, c2);
				}else{
					// Allocate a new contact.
					pair = modulePhysicsContactPairAllocate();
					if(pair == NULL){
						/** Memory allocation failure. **/
						return -1;
					}
					// Initialize the pair and its contact.
					physContactPairInit(pair, c1, c2, previous, next);
					physContactInit(&((physContactPair *)pair)->data, &manifold, c1->body, c2->body, c1, c2);
				}
				// Update the contact.
				// Moved to physColliderUpdateContacts().
				// physContactUpdate(&((physContactPair *)pair)->data, c1->body, c2->body, c1, c2, dt);

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

static __FORCE_INLINE__ void physContactPointSolveTangents(physContactPoint *const restrict point, physContact *const restrict contact, physRigidBody *const bodyA, physRigidBody *const bodyB){

	/*
	** Solves the friction impulses.
	*/

	float lambda, lambdaClamp;
	float temp1, temp2;
	vec3 v1, v2;
	vec3 impulse;

	// Calculate the contact velocity.
	vec3CrossR(&bodyB->angularVelocity, &point->pointB, &v1);
	vec3AddVToV(&v1, &bodyB->linearVelocity);
	vec3CrossR(&bodyA->angularVelocity, &point->pointA, &v2);
	vec3SubVFromV1(&v1, &bodyA->linearVelocity);
	vec3SubVFromV1(&v1, &v2);


	/*
	** Tangent A.
	*/

	// Calculate the frictional impulse.
	lambda = -vec3Dot(&v1, &contact->tangentA) * point->tangentImpulseDenominatorA;
	lambdaClamp = contact->friction * point->tangentImpulseDenominatorA;

	// Clamp the frictional impulse.
	temp1 = point->tangentImpulseAccumulatorA;
	temp2 = temp1 + lambda;
	if(temp2 < -lambdaClamp){
		point->tangentImpulseAccumulatorA = -lambdaClamp;
	}else if(temp2 > lambdaClamp){
		point->tangentImpulseAccumulatorA = lambdaClamp;
	}else{
		point->tangentImpulseAccumulatorA = temp2;
	}
	lambda = point->tangentImpulseAccumulatorA - temp1;

	// Apply the frictional impulse.
	vec3MultVBySR(&contact->tangentA, lambda, &impulse);

	physRigidBodyApplyImpulseInverse(bodyA, &point->pointA, &impulse);
	physRigidBodyApplyImpulse(bodyB, &point->pointB, &impulse);


	/*
	** Tangent B.
	*/

	// Calculate the frictional impulse.
	lambda = -vec3Dot(&v1, &contact->tangentB) * point->tangentImpulseDenominatorB;
	lambdaClamp = contact->friction * point->tangentImpulseDenominatorB;

	// Clamp the frictional impulse.
	temp1 = point->tangentImpulseAccumulatorB;
	temp2 = temp1 + lambda;
	if(temp2 < -lambdaClamp){
		point->tangentImpulseAccumulatorB = -lambdaClamp;
	}else if(temp2 > lambdaClamp){
		point->tangentImpulseAccumulatorB = lambdaClamp;
	}else{
		point->tangentImpulseAccumulatorB = temp2;
	}
	lambda = point->tangentImpulseAccumulatorB - temp1;

	// Apply the frictional impulse.
	vec3MultVBySR(&contact->tangentB, lambda, &impulse);

	physRigidBodyApplyImpulseInverse(bodyA, &point->pointA, &impulse);
	physRigidBodyApplyImpulse(bodyB, &point->pointB, &impulse);

}

static __FORCE_INLINE__ void physContactPointSolveNormal(physContactPoint *const restrict point, physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	/*
	** Solves the normal impulse.
	*/

	float lambda;
	float temp1, temp2;
	vec3 impulse, v2;

	// Calculate the contact velocity.
	vec3CrossR(&bodyB->angularVelocity, &point->pointB, &impulse);
	vec3AddVToV(&impulse, &bodyB->linearVelocity);
	vec3CrossR(&bodyA->angularVelocity, &point->pointA, &v2);
	vec3SubVFromV1(&impulse, &bodyA->linearVelocity);
	vec3SubVFromV1(&impulse, &v2);
	temp1 = vec3Dot(&impulse, &contact->normal);

	// Calculate the impulse magnitude.
	lambda = point->normalImpulseDenominator * (-temp1 + point->bias);

	// Clamp the normal impulse.
	temp1 = point->normalImpulseAccumulator;
	temp2 = temp1 + lambda;
	point->normalImpulseAccumulator = temp2 > 0.f ? temp2 : 0.f;
	lambda = point->normalImpulseAccumulator - temp1;

	// Apply the normal impulse.
	vec3MultVBySR(&contact->normal, lambda, &impulse);

	physRigidBodyApplyImpulseInverse(bodyA, &point->pointA, &impulse);
	physRigidBodyApplyImpulse(bodyB, &point->pointB, &impulse);

}

void physContactSolve(physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	/*
	** Solves a contact.
	*/

	physContactPoint *point = &contact->contacts[0];
	const physContactPoint *const pLast = &point[contact->contactNum];

	// Solve each contact point.
	for(; point < pLast; ++point){
		physContactPointSolveNormal(point, contact, bodyA, bodyB);
		physContactPointSolveTangents(point, contact, bodyA, bodyB);
	}

}