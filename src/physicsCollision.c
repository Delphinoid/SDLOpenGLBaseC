#include "modulePhysics.h"
#include "inline.h"
#include <string.h>
#include <math.h>

static __FORCE_INLINE__ float physContactCalculateRestitution(const float r1, const float r2){
	return r1 >= r2 ? r1 : r2;
}

static __FORCE_INLINE__ float physContactCalculateFriction(const float f1, const float f2){
	return sqrtf(f1 * f2);
}

static __FORCE_INLINE__ void physContactGenerateTangents(const vec3 normal, vec3 *const restrict tangentA, vec3 *const restrict tangentB){
	// Generate the contact tangents, perpendicular to each other and the contact normal.
	// Used for frictional calculations.
	if(fabsf(normal.x) >= sqrtf(1.f / 3.f)){  // 0x3F13CD3A
		*tangentA = vec3New(normal.y, -normal.x, 0.f);
	}else{
		*tangentA = vec3New(0.f, normal.z, -normal.y);
	}
	*tangentA = vec3NormalizeFastAccurate(*tangentA);
	*tangentB = vec3Cross(normal, *tangentA);
}

static __FORCE_INLINE__ void physContactPointWarmStart(physContactPoint *const restrict point, const physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	/*
	** Warm-start the contact.
	*/

	const vec3 impulse = vec3VAddV(
		vec3VAddV(
			vec3VMultS(contact->normal, point->normalImpulseAccumulator),
			vec3VMultS(contact->tangentA, point->tangentImpulseAccumulatorA)
		),
		vec3VMultS(contact->tangentB, point->tangentImpulseAccumulatorB)
	);

	physRigidBodyApplyImpulseInverse(bodyA, point->pointA, impulse);
	physRigidBodyApplyImpulse(bodyB, point->pointB, impulse);

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
		normal = vec3VAddV(normal, cPoint->normal);

		// Get the relative contact points.
		pPoint->pointA = vec3VSubV(cPoint->point, bodyA->centroidGlobal);
		pPoint->pointB = vec3VSubV(cPoint->point, bodyB->centroidGlobal);
		pPoint->penetrationDepth = cPoint->penetrationDepth;
		pPoint->key = cPoint->key;

		// Initialize the accumulators.
		pPoint->normalImpulseAccumulator = 0.f;
		pPoint->tangentImpulseAccumulatorA = 0.f;
		pPoint->tangentImpulseAccumulatorB = 0.f;

	}


	// Normalize the new total normal and generate tangents.
	normal = vec3NormalizeFastAccurate(normal);
	physContactGenerateTangents(normal, &contact->tangentA, &contact->tangentB);
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
		normal = vec3VAddV(normal, cPoint->normal);

	}

	// Normalize the new total normal and generate tangents.
	normal = vec3NormalizeFastAccurate(normal);
	physContactGenerateTangents(normal, &contact->tangentA, &contact->tangentB);
	contact->normal = normal;

	// Calculate the combined friction and restitution scalars.
	contact->friction    = physContactCalculateFriction   (colliderA->friction,    colliderB->friction);
	contact->restitution = physContactCalculateRestitution(colliderA->restitution, colliderB->restitution);

	// Initialize the accumulators for non-persisting contacts.
	cPoint = &manifold->contacts[0];
	pcPoint = &contact->contacts[0];
	flag = &persistent[0];
	for(; cPoint < cPointLast; ++cPoint, ++pcPoint, ++flag){

		// Get the relative contact points.
		pcPoint->pointA = vec3VSubV(cPoint->point, bodyA->centroidGlobal);
		pcPoint->pointB = vec3VSubV(cPoint->point, bodyB->centroidGlobal);
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
	angularDeltaA = vec3Cross(point->pointA, contact->normal);
	angularDeltaLinearA = mat3MMultVBra(bodyA->inverseInertiaTensorGlobal, angularDeltaA);
	angularDeltaB = vec3Cross(point->pointB, contact->normal);
	angularDeltaLinearB = mat3MMultVBra(bodyB->inverseInertiaTensorGlobal, angularDeltaB);
	point->normalImpulseDenominator = 1.f / (
		inverseMassTotal +
		vec3Dot(angularDeltaA, angularDeltaLinearA) +
		vec3Dot(angularDeltaB, angularDeltaLinearB)
	);

	// Calculate tangent A's impulse magnitude denominator.
	angularDeltaA = vec3Cross(contact->tangentA, point->pointA);
	angularDeltaLinearA = mat3MMultVBra(bodyA->inverseInertiaTensorGlobal, angularDeltaA);
	angularDeltaB = vec3Cross(contact->tangentA, point->pointB);
	angularDeltaLinearB = mat3MMultVBra(bodyB->inverseInertiaTensorGlobal, angularDeltaB);
	point->tangentImpulseDenominatorA = 1.f / (
		inverseMassTotal +
		vec3Dot(angularDeltaA, angularDeltaLinearA) +
		vec3Dot(angularDeltaB, angularDeltaLinearB)
	);

	// Calculate tangent B's impulse magnitude denominator.
	angularDeltaA = vec3Cross(contact->tangentB, point->pointA);
	angularDeltaLinearA = mat3MMultVBra(bodyA->inverseInertiaTensorGlobal, angularDeltaA);
	angularDeltaB = vec3Cross(contact->tangentB, point->pointB);
	angularDeltaLinearB = mat3MMultVBra(bodyB->inverseInertiaTensorGlobal, angularDeltaB);
	point->tangentImpulseDenominatorB = 1.f / (
		inverseMassTotal +
		vec3Dot(angularDeltaA, angularDeltaLinearA) +
		vec3Dot(angularDeltaB, angularDeltaLinearB)
	);

}

static __FORCE_INLINE__ void physContactPointGenerateBias(physContactPoint *const restrict point, const physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, physCollider *const restrict colliderA, physCollider *const restrict colliderB, const float dt){

	float temp;

	// Calculate potential slop.
	temp = point->penetrationDepth + PHYSICS_PENETRATION_SLOP;

	// Calculate the bias term.
	point->bias = -PHYSICS_BAUMGARTE_TERM * dt * (temp < 0.f ? temp : 0.f);

	// Apply restitution bias.
	temp = vec3Dot(
		vec3VSubV(
			vec3VSubV(
				vec3VAddV(
					vec3Cross(bodyB->angularVelocity, point->pointB),
					bodyB->linearVelocity
				),
				bodyA->linearVelocity
			),
			vec3Cross(bodyA->angularVelocity, point->pointA)
		),
		contact->normal
	);
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

__FORCE_INLINE__ void physContactReset(physContact *const restrict contact){

	physContactPoint *pPoint = &contact->contacts[0];
	const physContactPoint *const pPointLast = &pPoint[contact->contactNum];

	// Clear accumulators and keys.
	for(; pPoint < pPointLast; ++pPoint){
		pPoint->normalImpulseAccumulator = 0.f;
		pPoint->tangentImpulseAccumulatorA = 0.f;
		pPoint->tangentImpulseAccumulatorB = 0.f;
		pPoint->key.inEdgeR  = (cEdgeIndex_t)-1;
		pPoint->key.outEdgeR = (cEdgeIndex_t)-1;
		pPoint->key.inEdgeI  = (cEdgeIndex_t)-1;
		pPoint->key.outEdgeI = (cEdgeIndex_t)-1;
	}

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
		memQLinkNextA(previous) = (byte_t *)pair;
	}else{
		// Insert directly before the first pair.
		c1->contactCache = pair;
	}
	if(next != NULL){
		if(next->colliderA == c1){
			memQLinkPrevA(next) = (byte_t *)pair;
		}else{
			memQLinkPrevB(next) = (byte_t *)pair;
		}
	}
	memQLinkPrevA(pair) = (byte_t *)previous;
	memQLinkNextA(pair) = (byte_t *)next;

	// Find the previous and next nodes for the second collider.
	previous = NULL;
	next = c2->contactCache;
	while(next != NULL && next->colliderA == c2){
		previous = next;
		next = (physContactPair *)memQLinkNextA(next);
	}

	if(previous != NULL){
		// Insert between the previous pair and its next pair.
		memQLinkNextA(previous) = (byte_t *)pair;
	}else{
		// Insert directly before the first pair.
		c2->contactCache = pair;
	}
	if(next != NULL){
		memQLinkPrevB(next) = (byte_t *)pair;
	}
	memQLinkPrevB(pair) = (byte_t *)previous;
	memQLinkNextB(pair) = (byte_t *)next;

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
		memQLinkNextA(previous) = (byte_t *)pair;
	}else{
		// Insert directly before the first pair.
		c1->separationCache = pair;
	}
	if(next != NULL){
		if(next->colliderA == c1){
			memQLinkPrevA(next) = (byte_t *)pair;
		}else{
			memQLinkPrevB(next) = (byte_t *)pair;
		}
	}
	memQLinkPrevA(pair) = (byte_t *)previous;
	memQLinkNextA(pair) = (byte_t *)next;

	// Find the previous and next nodes for the second collider.
	previous = NULL;
	next = c2->separationCache;
	while(next != NULL && next->colliderA == c2){
		previous = next;
		next = (physSeparationPair *)memQLinkNextA(next);
	}

	if(previous != NULL){
		// Insert between the previous pair and its next pair.
		memQLinkNextA(previous) = (byte_t *)pair;
	}else{
		// Insert directly before the first pair.
		c2->separationCache = pair;
	}
	if(next != NULL){
		memQLinkPrevB(next) = (byte_t *)pair;
	}
	memQLinkPrevB(pair) = (byte_t *)previous;
	memQLinkNextB(pair) = (byte_t *)next;

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
	temp = (physContactPair *)memQLinkPrevA(pair);
	if(temp != NULL){
		memQLinkNextA(temp) = memQLinkNextA(pair);
	}else{
		pair->colliderA->contactCache = (physContactPair *)memQLinkNextA(pair);
	}
	temp = (physContactPair *)memQLinkPrevB(pair);
	if(temp != NULL){
		if(temp->colliderA == pair->colliderB){
			memQLinkNextA(temp) = memQLinkNextB(pair);
		}else{
			memQLinkNextB(temp) = memQLinkNextB(pair);
		}
	}else{
		pair->colliderB->contactCache = (physContactPair *)memQLinkNextB(pair);
	}

	// Remove references from the next pairs.
	temp = (physContactPair *)memQLinkNextA(pair);
	if(temp != NULL){
		if(temp->colliderA == pair->colliderA){
			memQLinkPrevA(temp) = memQLinkPrevA(pair);
		}else{
			memQLinkPrevB(temp) = memQLinkPrevA(pair);
		}
	}
	temp = (physContactPair *)memQLinkNextB(pair);
	if(temp != NULL){
		memQLinkPrevB(temp) = memQLinkPrevB(pair);
	}

}
void physSeparationPairDelete(physSeparationPair *const pair){

	/*
	** Removes a pair from its linked lists.
	*/

	physSeparationPair *temp;

	// Remove references from the previous pairs.
	temp = (physSeparationPair *)memQLinkPrevA(pair);
	if(temp != NULL){
		memQLinkNextA(temp) = memQLinkNextA(pair);
	}else{
		pair->colliderA->separationCache = (physSeparationPair *)memQLinkNextA(pair);
	}
	temp = (physSeparationPair *)memQLinkPrevB(pair);
	if(temp != NULL){
		if(temp->colliderA == pair->colliderB){
			memQLinkNextA(temp) = memQLinkNextB(pair);
		}else{
			memQLinkNextB(temp) = memQLinkNextB(pair);
		}
	}else{
		pair->colliderB->separationCache = (physSeparationPair *)memQLinkNextB(pair);
	}

	// Remove references from the next pairs.
	temp = (physSeparationPair *)memQLinkNextA(pair);
	if(temp != NULL){
		if(temp->colliderA == pair->colliderA){
			memQLinkPrevA(temp) = memQLinkPrevA(pair);
		}else{
			memQLinkPrevB(temp) = memQLinkPrevA(pair);
		}
	}
	temp = (physSeparationPair *)memQLinkNextB(pair);
	if(temp != NULL){
		memQLinkPrevB(temp) = memQLinkPrevB(pair);
	}

}

return_t physCollisionQuery(aabbNode *const n1, aabbNode *const n2){

	/*
	** Manages the broadphase and narrowphase
	** between a potential contact pair.
	*/

	physCollider *const c1 = (physCollider *)n1->data.leaf.value;
	physCollider *const c2 = (physCollider *)n2->data.leaf.value;

	// Make sure the colliders and their owning bodies permit collisions.
	if(physColliderPermitCollision(c1, c2) && physRigidBodyPermitCollision((physRigidBody *)c1->body, (physRigidBody *)c2->body)){

		// Broadphase collision check.
		if(cAABBCollision(&c1->aabb, &c2->aabb)){

			cContact manifold;
			physSeparation separation;
			physSeparation *separationPointer;

			// Find a previous separation for the pair, if one exists.
			void *previous, *next;
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
	vec3 v, impulse;

	// Calculate the contact velocity.
	v = vec3VSubV(
		vec3VSubV(
			vec3VAddV(
				vec3Cross(bodyB->angularVelocity, point->pointB),
				bodyB->linearVelocity
			),
			bodyA->linearVelocity
		),
		vec3Cross(bodyA->angularVelocity, point->pointA)
	);


	/*
	** Tangent A.
	*/

	// Calculate the frictional impulse.
	lambda = -vec3Dot(v, contact->tangentA) * point->tangentImpulseDenominatorA;
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
	impulse = vec3VMultS(contact->tangentA, lambda);

	physRigidBodyApplyImpulseInverse(bodyA, point->pointA, impulse);
	physRigidBodyApplyImpulse(bodyB, point->pointB, impulse);


	/*
	** Tangent B.
	*/

	// Calculate the frictional impulse.
	lambda = -vec3Dot(v, contact->tangentB) * point->tangentImpulseDenominatorB;
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
	impulse = vec3VMultS(contact->tangentB, lambda);

	physRigidBodyApplyImpulseInverse(bodyA, point->pointA, impulse);
	physRigidBodyApplyImpulse(bodyB, point->pointB, impulse);

}

static __FORCE_INLINE__ void physContactPointSolveNormal(physContactPoint *const restrict point, physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	/*
	** Solves the normal impulse.
	*/

	float lambda;
	float temp1, temp2;
	vec3 impulse;

	// Calculate the contact velocity.
	temp1 = vec3Dot(
		vec3VSubV(
			vec3VSubV(
				vec3VAddV(
					vec3Cross(bodyB->angularVelocity, point->pointB),
					bodyB->linearVelocity
				),
				bodyA->linearVelocity
			),
			vec3Cross(bodyA->angularVelocity, point->pointA)
		),
		contact->normal
	);

	// Calculate the impulse magnitude.
	lambda = point->normalImpulseDenominator * (-temp1 + point->bias);

	// Clamp the normal impulse.
	temp1 = point->normalImpulseAccumulator;
	temp2 = temp1 + lambda;
	point->normalImpulseAccumulator = temp2 > 0.f ? temp2 : 0.f;
	lambda = point->normalImpulseAccumulator - temp1;

	// Apply the normal impulse.
	impulse = vec3VMultS(contact->normal, lambda);

	physRigidBodyApplyImpulseInverse(bodyA, point->pointA, impulse);
	physRigidBodyApplyImpulse(bodyB, point->pointB, impulse);

}

void physContactSolve(physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	/*
	** Solves a contact.
	*/

	physContactPoint *point = &contact->contacts[0];
	const physContactPoint *const pLast = &point[contact->contactNum];

	// Solve each contact point.
	for(; point < pLast; ++point){
		physContactPointSolveTangents(point, contact, bodyA, bodyB);
		physContactPointSolveNormal(point, contact, bodyA, bodyB);
	}

}
