#include "modulePhysics.h"
#include "inline.h"
#include <string.h>
#include <math.h>

#ifdef PHYSICS_CONTACT_FRICTION_CONSTRAINT
	#define physContactHalfwayA(c) c->frictionConstraint.rA
	#define physContactHalfwayB(c) c->frictionConstraint.rB
	#define physContactNormal(c)   c->frictionConstraint.normal
	#define physContactTangent1(c) c->frictionConstraint.tangent1
	#define physContactTangent2(c) c->frictionConstraint.tangent2
	#define physContactFriction(c) c->frictionConstraint.friction
#else
	#define physContactHalfwayA(c) c->rA
	#define physContactHalfwayB(c) c->rB
	#define physContactNormal(c)   c->normal
	#define physContactTangent1(c) c->tangent1
	#define physContactTangent2(c) c->tangent2
	#define physContactFriction(c) c->friction
#endif

static __FORCE_INLINE__ float physContactCalculateRestitution(const float r1, const float r2){
	return r1 >= r2 ? r1 : r2;
}

static __FORCE_INLINE__ float physContactCalculateFriction(const float f1, const float f2){
	#ifdef PHYSICS_FRICTION_GEOMETRIC_AVERAGE
	return sqrtf(f1 * f2);
	#else
	const float w1 = 1.414213562373095f * (1.f - f1) + 1.f;
	const float w2 = 1.414213562373095f * (1.f - f2) + 1.f;
	return (f1*w1 + f2*w2)/(w1 + w2);
	#endif
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

	const float pReciprocal = 1.f/manifold->contactNum;

	// Combined contact normal.
	vec3 normal, halfway;
	vec3ZeroP(&normal);
	vec3ZeroP(&halfway);

	// Initialize each contact.
	for(; cPoint < cPointLast; ++cPoint, ++pPoint){

		// Get the halfway points.
		const vec3 pHalfway = vec3VMultS(vec3VAddV(cPoint->pointA, cPoint->pointB), 0.5f);
		pPoint->rA = vec3VSubV(pHalfway, bodyA->centroidGlobal);
		pPoint->rB = vec3VSubV(pHalfway, bodyB->centroidGlobal);
		halfway = vec3VAddV(halfway, pHalfway);

		// Get the relative contact points.
		#ifdef PHYSICS_GAUSS_SEIDEL_SOLVER
		pPoint->pointA = quatRotateVec3FastApproximate(quatConjugateFast(bodyA->configuration.orientation), vec3VSubV(cPoint->pointA, bodyA->centroidGlobal));
		pPoint->pointB = quatRotateVec3FastApproximate(quatConjugateFast(bodyB->configuration.orientation), vec3VSubV(cPoint->pointB, bodyB->centroidGlobal));
		#else
		// Get the penetration depth.
		pPoint->separation = cPoint->separation;
		#endif

		// Get the contact key.
		pPoint->key = cPoint->key;

		// Initialize the accumulator.
		pPoint->normalImpulseAccumulator = 0.f;
		#ifndef PHYSICS_CONTACT_FRICTION_CONSTRAINT
		pPoint->tangentImpulseAccumulator1 = 0.f;
		pPoint->tangentImpulseAccumulator2 = 0.f;
		#endif

		// Add up the combined contact normal.
		normal = vec3VAddV(normal, cPoint->normal);

	}

	#ifdef PHYSICS_CONTACT_FRICTION_CONSTRAINT
	// Initialize the friction accumulators.
	vec2ZeroP(&contact->frictionConstraint.tangentImpulseAccumulator);
	contact->frictionConstraint.angularImpulseAccumulator = 0.f;
	#endif

	// Get the average halfway point, used for friction.
	halfway = vec3VMultS(halfway, pReciprocal);
	physContactHalfwayA(contact) = vec3VSubV(halfway, bodyA->centroidGlobal);
	physContactHalfwayB(contact) = vec3VSubV(halfway, bodyB->centroidGlobal);

	// Normalize the new total normal and generate tangents.
	normal = vec3NormalizeFastAccurate(normal);
	physContactNormal(contact) = normal;
	physContactTangent1(contact) = vec3Perpendicular(normal);
	physContactTangent2(contact) = vec3Cross(normal, physContactTangent1(contact));
	#ifdef PHYSICS_GAUSS_SEIDEL_SOLVER
	contact->normalA = quatRotateVec3FastApproximate(quatConjugateFast(bodyA->configuration.orientation), normal);
	#endif

	// Calculate the combined friction and restitution scalars.
	physContactFriction(contact) = physContactCalculateFriction   (colliderA->friction,    colliderB->friction);
	contact->restitution         = physContactCalculateRestitution(colliderA->restitution, colliderB->restitution);

	contact->contactNum = manifold->contactNum;

}

static __FORCE_INLINE__ void physContactPointWarmStart(physContactPoint *const restrict point, const physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	/*
	** Warm-start the persistent contact point.
	*/

	#ifdef PHYSICS_CONTACT_FRICTION_CONSTRAINT
	const vec3 impulse = vec3VMultS(physContactNormal(contact), point->normalImpulseAccumulator);
	#else
	const vec3 impulse = vec3VAddV(
		vec3VAddV(
			vec3VMultS(physContactNormal(contact), point->normalImpulseAccumulator),
			vec3VMultS(physContactTangent1(contact), point->tangentImpulseAccumulator1)
		),
		vec3VMultS(physContactTangent2(contact), point->tangentImpulseAccumulator2)
	);
	#endif

	physRigidBodyApplyVelocityImpulseInverse(bodyA, point->rA, impulse);
	physRigidBodyApplyVelocityImpulse(bodyB, point->rB, impulse);

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

	const float pReciprocal = 1.f/manifold->contactNum;

	// Combined contact normal.
	vec3 normal, halfway;
	vec3ZeroP(&normal);
	vec3ZeroP(&halfway);

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

					#ifndef PHYSICS_CONTACT_FRICTION_CONSTRAINT
					tempAccumulator = pcPoint->tangentImpulseAccumulator1;
					pcPoint->tangentImpulseAccumulator1 = pPoint->tangentImpulseAccumulator1;
					pPoint->tangentImpulseAccumulator1 = tempAccumulator;

					tempAccumulator = pcPoint->tangentImpulseAccumulator2;
					pcPoint->tangentImpulseAccumulator2 = pPoint->tangentImpulseAccumulator2;
					pPoint->tangentImpulseAccumulator2 = tempAccumulator;
					#endif

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
	physContactNormal(contact) = normal;
	physContactTangent1(contact) = vec3Perpendicular(normal);
	physContactTangent2(contact) = vec3Cross(normal, physContactTangent1(contact));
	#ifdef PHYSICS_GAUSS_SEIDEL_SOLVER
	contact->normalA = quatRotateVec3FastApproximate(quatConjugateFast(bodyA->configuration.orientation), normal);
	#endif

	// Calculate the combined friction and restitution scalars.
	physContactFriction(contact) = physContactCalculateFriction   (colliderA->friction,    colliderB->friction);
	contact->restitution         = physContactCalculateRestitution(colliderA->restitution, colliderB->restitution);

	// Initialize the accumulators for non-persisting contacts.
	cPoint = &manifold->contacts[0];
	pcPoint = &contact->contacts[0];
	flag = &persistent[0];
	for(; cPoint < cPointLast; ++cPoint, ++pcPoint, ++flag){

		// Get the halfway points.
		const vec3 pHalfway = vec3VMultS(vec3VAddV(cPoint->pointA, cPoint->pointB), 0.5f);
		pcPoint->rA = vec3VSubV(pHalfway, bodyA->centroidGlobal);
		pcPoint->rB = vec3VSubV(pHalfway, bodyB->centroidGlobal);
		halfway = vec3VAddV(halfway, pHalfway);

		// Get the relative contact points.
		#ifdef PHYSICS_GAUSS_SEIDEL_SOLVER
		pcPoint->pointA = quatRotateVec3FastApproximate(quatConjugateFast(bodyA->configuration.orientation), vec3VSubV(cPoint->pointA, bodyA->centroidGlobal));
		pcPoint->pointB = quatRotateVec3FastApproximate(quatConjugateFast(bodyB->configuration.orientation), vec3VSubV(cPoint->pointB, bodyB->centroidGlobal));
		#else
		// Get the penetration depth.
		pcPoint->separation = cPoint->separation;
		#endif

		if(*flag == 0){
			// Initialize the accumulator for non-persistent contacts.
			pcPoint->normalImpulseAccumulator = 0.f;
			#ifndef PHYSICS_CONTACT_FRICTION_CONSTRAINT
			pcPoint->tangentImpulseAccumulator1 = 0.f;
			pcPoint->tangentImpulseAccumulator2 = 0.f;
			#endif
			pcPoint->key = cPoint->key;
		}else{
			// Warm-start persistent contact points.
			physContactPointWarmStart(pcPoint, contact, bodyA, bodyB);
		}

	}

	// Get the average halfway point, used for friction.
	halfway = vec3VMultS(halfway, pReciprocal);
	physContactHalfwayA(contact) = vec3VSubV(halfway, bodyA->centroidGlobal);
	physContactHalfwayB(contact) = vec3VSubV(halfway, bodyB->centroidGlobal);

	// Warm-start the contact.
	#ifdef PHYSICS_CONTACT_FRICTION_CONSTRAINT
	physJointFrictionWarmStart(&contact->frictionConstraint, bodyA, bodyB);
	#endif

	contact->contactNum = manifold->contactNum;

}

static __FORCE_INLINE__ float physContactInverseEffectiveMass(const vec3 normalA, const vec3 pointA, const mat3 inverseInertiaTensorA, const vec3 normalB, const vec3 pointB, const mat3 inverseInertiaTensorB, const float inverseMassTotal){

	/*
	** Calculates the inverse effective mass for the contact.
	** This is used as the denominator for the impulse magnitude
	** (constraint Lagrange multiplier):
	**
	** 1 / ((JM^-1)J^T)
	**
	** Where J is the Jacobian row vector, J^T is its transpose
	** and M^-1 is the inverse mass matrix:
	**
	** J = [-n, -(rA x n), n, (rA x n)]
	**
	**       [    -n   ]
	**       [-(rA x n)]
	** J^T = [     n   ]
	**       [ (rA x n)]
	**
	**        [mA^-1  0    0    0  ]
	**        [  0  IA^-1  0    0  ]
	** M^-1 = [  0    0  mB^-1  0  ]
	**        [  0    0    0  IB^-1]
	**
	** J is derived from JV = C', where C is the contact
	** constraint equation and V is the velocity column vector:
	**
	** C = (pB - pA) . n >= 0
	** C' = dC/dt = ((vB + wB x rB) - (vA + wA x rA)) . n >= 0
	**
	**     [vA]
	**     [wA]
	** V = [vB]
	**     [wB]
	**
	** Thus, our initial expression above expands to the following:
	**
	** 1 / (mA + mB + ((rA X n) . (IA(rA x n))) + ((rB x n) . (IB(rB x n))))
	*/

	const vec3 angularDeltaA = vec3Cross(pointA, normalA);
	const vec3 angularDeltaB = vec3Cross(pointB, normalB);
	const float effectiveMass =
		inverseMassTotal +
		vec3Dot(angularDeltaA, mat3MMultVBra(inverseInertiaTensorA, angularDeltaA)) +
		vec3Dot(angularDeltaB, mat3MMultVBra(inverseInertiaTensorB, angularDeltaB));
	return (effectiveMass > 0.f ? 1.f/effectiveMass : 0.f);

}

static __FORCE_INLINE__ void physContactPointGenerateInverseEffectiveMass(physContactPoint *const restrict point, const physContact *const restrict contact, const physRigidBody *const restrict bodyA, const physRigidBody *const restrict bodyB, const float inverseMassTotal){

	/*
	** Calculate the inverse effective mass for the normal constraint.
	*/

	point->normalInverseEffectiveMass = physContactInverseEffectiveMass(
		physContactNormal(contact), point->rA, bodyA->inverseInertiaTensorGlobal,
		physContactNormal(contact), point->rB, bodyB->inverseInertiaTensorGlobal,
		inverseMassTotal
	);
	#ifndef PHYSICS_CONTACT_FRICTION_CONSTRAINT
	point->tangentInverseEffectiveMass1 = physContactInverseEffectiveMass(
		point->rA, physContactTangent1(contact), bodyA->inverseInertiaTensorGlobal,
		point->rB, physContactTangent1(contact), bodyB->inverseInertiaTensorGlobal,
		inverseMassTotal
	);
	point->tangentInverseEffectiveMass2 = physContactInverseEffectiveMass(
		point->rA, physContactTangent2(contact), bodyA->inverseInertiaTensorGlobal,
		point->rB, physContactTangent2(contact), bodyB->inverseInertiaTensorGlobal,
		inverseMassTotal
	);
	#endif

}

#ifndef PHYSICS_GAUSS_SEIDEL_SOLVER
static __FORCE_INLINE__ void physContactPointGenerateBias(physContactPoint *const restrict point, const physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, physCollider *const restrict colliderA, physCollider *const restrict colliderB, const float dt){
#else
static __FORCE_INLINE__ void physContactPointGenerateBias(physContactPoint *const restrict point, const physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, physCollider *const restrict colliderA, physCollider *const restrict colliderB){
#endif

	/*
	** Generate the constraint bias. Used in the
	** constraint's Lagrange multiplier, i.e.:
	**
	** -(JV + b)/((JM^-1)J^T)
	*/

	float temp;

	#ifndef PHYSICS_GAUSS_SEIDEL_SOLVER

	// Calculate potential slop.
	temp = point->separation + PHYSICS_LINEAR_SLOP;

	// Calculate the bias term.
	point->bias = -PHYSICS_BAUMGARTE_TERM * dt * (temp < 0.f ? temp : 0.f);

	#else
	point->bias = 0.f;
	#endif

	// Apply restitution bias if JV is within a particular threshold.
	// JV = (((wB x rB) + vB) - ((wA x rA) + vA)) . n
	temp = vec3Dot(
		vec3VSubV(
			vec3VSubV(
				vec3VAddV(
					vec3Cross(bodyB->angularVelocity, point->rB),
					bodyB->linearVelocity
				),
				bodyA->linearVelocity
			),
			vec3Cross(bodyA->angularVelocity, point->rA)
		),
		physContactNormal(contact)
	);
	if(temp < -PHYSICS_RESTITUTION_THRESHOLD){
		point->bias += contact->restitution * temp;
	}

}

#ifndef PHYSICS_GAUSS_SEIDEL_SOLVER
__FORCE_INLINE__ void physContactUpdate(physContact *const restrict contact, physCollider *const restrict colliderA, physCollider *const restrict colliderB, const float dt){
#else
__FORCE_INLINE__ void physContactUpdate(physContact *const restrict contact, physCollider *const restrict colliderA, physCollider *const restrict colliderB){
#endif

	/*
	** Builds a physContact from a cContact.
	*/

	physRigidBody *const bodyA = colliderA->body;
	physRigidBody *const bodyB = colliderB->body;

	physContactPoint *pPoint = &contact->contacts[0];
	const physContactPoint *const pPointLast = &pPoint[contact->contactNum];

	const float inverseMassTotal = bodyA->inverseMass + bodyB->inverseMass;

	// Calculate extra data for each contact point.
	for(; pPoint < pPointLast; ++pPoint){

		// Generate impulses, including the two
		// tangent impulses for friction simulation.
		physContactPointGenerateInverseEffectiveMass(pPoint, contact, bodyA, bodyB, inverseMassTotal);

		// Generate bias term.
		#ifndef PHYSICS_GAUSS_SEIDEL_SOLVER
		physContactPointGenerateBias(pPoint, contact, bodyA, bodyB, colliderA, colliderB, dt);
		#else
		physContactPointGenerateBias(pPoint, contact, bodyA, bodyB, colliderA, colliderB);
		#endif

	}

	#ifdef PHYSICS_CONTACT_FRICTION_CONSTRAINT
	physJointFrictionGenerateInverseEffectiveMass(&contact->frictionConstraint, bodyA, bodyB, inverseMassTotal);
	#endif

}

__FORCE_INLINE__ void physContactReset(physContact *const restrict contact){

	physContactPoint *pPoint = &contact->contacts[0];
	const physContactPoint *const pPointLast = &pPoint[contact->contactNum];

	// Clear accumulators and keys.
	for(; pPoint < pPointLast; ++pPoint){
		pPoint->normalImpulseAccumulator = 0.f;
		#ifndef PHYSICS_CONTACT_FRICTION_CONSTRAINT
		pPoint->tangentImpulseAccumulator1 = 0.f;
		pPoint->tangentImpulseAccumulator2 = 0.f;
		#endif
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

#ifndef PHYSICS_CONTACT_FRICTION_CONSTRAINT
static __FORCE_INLINE__ void physContactPointSolveVelocityTangents(physContactPoint *const restrict point, physContact *const restrict contact, physRigidBody *const bodyA, physRigidBody *const bodyB){

	/*
	** Solves the tangent impulses.
	*/

	float tangentImpulseAccumulatorNew, lambda;
	vec3 v, impulse;

	const float lambdaClamp = contact->friction * point->normalImpulseAccumulator;

	// Calculate the contact velocity.
	// ((wB x rB) + vB) - ((wA x rA) + vA)
	v = vec3VSubV(
		vec3VSubV(
			vec3VAddV(
				vec3Cross(bodyB->angularVelocity, point->rB),
				bodyB->linearVelocity
			),
			bodyA->linearVelocity
		),
		vec3Cross(bodyA->angularVelocity, point->rA)
	);

	/*
	** Tangent 1.
	*/

	// Calculate the frictional impulse magnitude.
	lambda = -vec3Dot(v, physContactTangent1(contact)) * point->tangentInverseEffectiveMass1;

	// Clamp the frictional impulse magnitude.
	tangentImpulseAccumulatorNew = point->tangentImpulseAccumulator1 + lambda;
	if(tangentImpulseAccumulatorNew < -lambdaClamp){
		tangentImpulseAccumulatorNew = -lambdaClamp;
	}else if(tangentImpulseAccumulatorNew > lambdaClamp){
		tangentImpulseAccumulatorNew = lambdaClamp;
	}
	lambda = tangentImpulseAccumulatorNew - point->tangentImpulseAccumulator1;
	point->tangentImpulseAccumulator1 = tangentImpulseAccumulatorNew;

	// Calculate the frictional impulse.
	impulse = vec3VMultS(physContactTangent1(contact), lambda);

	/*
	** Tangent 2.
	*/

	// Calculate the frictional impulse magnitude.
	lambda = -vec3Dot(v, physContactTangent2(contact)) * point->tangentInverseEffectiveMass2;

	// Clamp the frictional impulse magnitude.
	tangentImpulseAccumulatorNew = point->tangentImpulseAccumulator2 + lambda;
	if(tangentImpulseAccumulatorNew < -lambdaClamp){
		tangentImpulseAccumulatorNew = -lambdaClamp;
	}else if(tangentImpulseAccumulatorNew > lambdaClamp){
		tangentImpulseAccumulatorNew = lambdaClamp;
	}
	lambda = tangentImpulseAccumulatorNew - point->tangentImpulseAccumulator2;
	point->tangentImpulseAccumulator2 = tangentImpulseAccumulatorNew;

	// Calculate the frictional impulse.
	impulse = vec3VAddV(impulse, vec3VMultS(physContactTangent2(contact), lambda));

	/*
	** Apply both of the frictional impulses.
	*/
	physRigidBodyApplyVelocityImpulseInverse(bodyA, point->rA, impulse);
	physRigidBodyApplyVelocityImpulse(bodyB, point->rB, impulse);

}
#endif

static __FORCE_INLINE__ void physContactPointSolveVelocity(physContactPoint *const restrict point, physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	/*
	** Solves the normal impulse.
	*/

	float normalImpulseAccumulatorNew, lambda;
	vec3 impulse;

	// Evaluate the constraint expression, JV.
	// C = (pB - pA) . n
	// C' = dC/dt = (((wB x rB) + vB) - ((wA x rA) + vA)) . n
	// JV = C'
	lambda = vec3Dot(
		vec3VSubV(
			vec3VSubV(
				vec3VAddV(
					vec3Cross(bodyB->angularVelocity, point->rB),
					bodyB->linearVelocity
				),
				vec3Cross(bodyA->angularVelocity, point->rA)
			),
			bodyA->linearVelocity
		),
		physContactNormal(contact)
	);

	// Calculate the normal impulse magnitude,
	// i.e. the constraint's Lagrange multiplier.
	// -(JV + b)/((JM^-1)J^T)
	lambda = -point->normalInverseEffectiveMass * (lambda - point->bias);

	// Clamp the normal impulse magnitude.
	// The constraint equation states that impulse magnitude >= 0.
	normalImpulseAccumulatorNew = point->normalImpulseAccumulator + lambda;
	normalImpulseAccumulatorNew = normalImpulseAccumulatorNew > 0.f ? normalImpulseAccumulatorNew : 0.f;
	lambda = normalImpulseAccumulatorNew - point->normalImpulseAccumulator;
	point->normalImpulseAccumulator = normalImpulseAccumulatorNew;

	// Calculate the normal impulse.
	impulse = vec3VMultS(physContactNormal(contact), lambda);

	// Apply the normal impulse.
	physRigidBodyApplyVelocityImpulseInverse(bodyA, point->rA, impulse);
	physRigidBodyApplyVelocityImpulse(bodyB, point->rB, impulse);

}

void physContactSolveVelocityConstraints(physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	/*
	** Solves a contact's velocity constraints.
	*/

	physContactPoint *point = &contact->contacts[0];
	const physContactPoint *const pLast = &point[contact->contactNum];

	#ifdef PHYSICS_CONTACT_FRICTION_CONSTRAINT
	float normalImpulseTotal = 0.f;
	#endif

	// Solve each contact point.
	for(; point < pLast; ++point){
		// Solve frictional impulses.
		#ifndef PHYSICS_CONTACT_FRICTION_CONSTRAINT
		physContactPointSolveVelocityTangents(point, contact, bodyA, bodyB);
		#endif
		// Solve normal impulses.
		physContactPointSolveVelocity(point, contact, bodyA, bodyB);
		#ifdef PHYSICS_CONTACT_FRICTION_CONSTRAINT
		normalImpulseTotal += point->normalImpulseAccumulator;
		#endif
	}

	// Solve friction constraint impulses.
	#ifdef PHYSICS_CONTACT_FRICTION_CONSTRAINT
	physJointFrictionSolveVelocity(&contact->frictionConstraint, bodyA, bodyB, normalImpulseTotal);
	#endif

}

#ifdef PHYSICS_GAUSS_SEIDEL_SOLVER
static __FORCE_INLINE__ float physContactPointSolveConfigurationNormal(physContactPoint *const restrict point, physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	/*
	** Solves the normal constraint.
	*/

	float constraint, inverseEffectiveMass;
	float lambda;
	vec3 impulse;

	// Transform the contact points into global space.
	const vec3 pointGlobalA = vec3VAddV(quatRotateVec3FastApproximate(bodyA->configuration.orientation, point->pointA), bodyA->centroidGlobal);
	const vec3 pointGlobalB = vec3VAddV(quatRotateVec3FastApproximate(bodyB->configuration.orientation, point->pointB), bodyB->centroidGlobal);

	// Calculate the transformed normal and a point
	// halfway between both transformed contact points.
	const vec3 normal = quatRotateVec3FastApproximate(bodyA->configuration.orientation, contact->normalA);
	const vec3 halfway = pointGlobalB;//vec3VMultS(vec3VAddV(pointGlobalA, pointGlobalB), 0.5f);

	// Calculate the separation.
	const float separation = vec3Dot(vec3VSubV(pointGlobalB, pointGlobalA), normal) - PHYSICS_SEPARATION_BIAS_TOTAL;

	// Apply a slop to the configuration constraint.
	constraint = PHYSICS_BAUMGARTE_TERM * (separation + PHYSICS_LINEAR_SLOP);

	// Make sure the magnitude is less than 0.
	if(constraint < 0.f){

		// Calculate the new, transformed contact point offsets.
		const vec3 pointLocalA = vec3VSubV(halfway, bodyA->centroidGlobal);
		const vec3 pointLocalB = vec3VSubV(halfway, bodyB->centroidGlobal);

		// Calculate the new effective mass.
		inverseEffectiveMass = physContactInverseEffectiveMass(
			normal, pointLocalA, bodyA->inverseInertiaTensorGlobal,
			normal, pointLocalB, bodyB->inverseInertiaTensorGlobal,
			bodyA->inverseMass + bodyB->inverseMass
		);

		// Make sure the effective mass is greater than 0.
		if(inverseEffectiveMass > 0.f){

			// Clamp the constraint to prevent large corrections.
			if(constraint < -PHYSICS_LINEAR_CORRECTION){
				constraint = -PHYSICS_LINEAR_CORRECTION;
			}

			// Calculate the normal impulse magnitude,
			// i.e. the constraint's Lagrange multiplier.
			lambda = -constraint * inverseEffectiveMass;

			// Calculate the normal impulse.
			impulse = vec3VMultS(normal, lambda);

			// Apply the normal impulse.
			physRigidBodyApplyConfigurationImpulseInverse(bodyA, pointLocalA, impulse);
			physRigidBodyApplyConfigurationImpulse(bodyB, pointLocalB, impulse);

		}

	}

	return separation;

}

float physContactSolveConfigurationConstraints(physContact *const restrict contact, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, float error){

	/*
	** Solves a contact's position and orientation constraints.
	*/

	physContactPoint *point = &contact->contacts[0];
	const physContactPoint *const pLast = &point[contact->contactNum];

	// Solve each contact point.
	for(; point < pLast; ++point){
		const float separation = physContactPointSolveConfigurationNormal(point, contact, bodyA, bodyB);
		error = (error > separation ? separation : error);
	}

	return error;

}
#endif
