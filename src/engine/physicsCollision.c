#include "physicsCollision.h"
#include "physicsCollider.h"
#include "physicsRigidBody.h"
#include "modulePhysics.h"
#include "physicsConstraint.h"
#include "aabbTree.h"
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

// ----------------------------------------------------------------------
//
// Contacts involve solving both a contact constraint and a
// friction constraint. For more information on friction
// constraints, please see physicsJointFriction.c.
//
// ----------------------------------------------------------------------
//
// Contact constraint equation:
//
// C : (pB - pA) . n >= 0.
//
// Differentiating so we can solve w.r.t. velocity:
//
// C' : dC/dt = (((wB X rB) + vB) - ((wA X rA) + vA)) . n >= 0,
//
// where n is the contact normal and the p terms are the
// contact points in global space.
//
// ----------------------------------------------------------------------
//
// Given the velocity vector
//
//     [vA]
//     [wA]
// V = [vB]
//     [wB]
//
// and the identity JV = C', we can solve for the Jacobian J:
//
// J = [-n, -(rA X n), n, (rB X n)].
//
// Finally, adding a potential bias term, we have
//
// C' : JV + b >= 0.
//
// ----------------------------------------------------------------------
//
// The effective mass for the constraint is given by (JM^-1)J^T,
// where M^-1 is the inverse mass matrix and J^T is the transposed
// Jacobian.
//
//        [mA^-1  0    0    0  ]
//        [  0  IA^-1  0    0  ]
// M^-1 = [  0    0  mB^-1  0  ]
//        [  0    0    0  IB^-1],
//
//       [    -n   ]
//       [-(rA X n)]
// J^T = [     n   ]
//       [ (rB X n)].
//
// Expanding results in
//
// (JM^-1)J^T = mA^-1 + mB^-1 + ((rA X n) . (IA^-1 * (rA X n))) + ((rB X n) . (IB^-1 * (rB X n))).
//
// ----------------------------------------------------------------------
//
// Semi-implicit Euler:
//
// V   = V_i + dt * M^-1 * F,
// V_f = V   + dt * M^-1 * P.
//
// Where V_i is the initial velocity vector, V_f is the final
// velocity vector, F is the external force on the body (e.g.
// gravity), P is the constraint force and dt is the timestep.
//
// Using P = J^T * lambda and lambda' = dt * lambda, we can
// solve for the impulse magnitude (constraint Lagrange
// multiplier) lambda':
//
// JV_f + b = 0
// J(V + dt * M^-1 * P) + b = 0
// JV + dt * (JM^-1)P + b = 0
// JV + dt * (JM^-1)J^T . lambda + b = 0
// dt * (JM^-1)J^T . lambda = -(JV + b)
// dt * lambda = -(JV + b)/((JM^-1)J^T)
// lambda' = -(JV + b)/((JM^-1)J^T).
//
// ----------------------------------------------------------------------

static __FORCE_INLINE__ float physContactCalculateRestitution(const float r1, const float r2){
	return r1 >= r2 ? r1 : r2;
}

static __FORCE_INLINE__ float physContactCalculateFriction(const float f1, const float f2){
	#ifdef PHYSICS_CONTACT_FRICTION_GEOMETRIC_AVERAGE
	return sqrtf(f1 * f2);
	#else
	const float w1 = 1.414213562373095f * (1.f - f1) + 1.f;
	const float w2 = 1.414213562373095f * (1.f - f2) + 1.f;
	return (f1*w1 + f2*w2)/(w1 + w2);
	#endif
}

__FORCE_INLINE__ void physContactInit(physContact *const __RESTRICT__ contact, const cContact *const __RESTRICT__ manifold, const physRigidBody *const __RESTRICT__ bodyA, const physRigidBody *const __RESTRICT__ bodyB, const physCollider *const __RESTRICT__ colliderA, const physCollider *const __RESTRICT__ colliderB){

	// Sets the incident and reference bodies
	// and reset the impulse accumulators.
	//
	// Also generates a global normal and tangents.

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
		#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
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
	#ifdef PHYSICS_CONTACT_FRICTION_DELAY
	vec3ZeroP(&physContactTangent1(contact));
	vec3ZeroP(&physContactTangent2(contact));
	#else
	vec3OrthonormalBasis(normal, &physContactTangent1(contact), &physContactTangent2(contact));
	#endif
	//physContactTangent1(contact) = vec3Orthogonal(normal);
	//physContactTangent2(contact) = vec3Cross(normal, physContactTangent1(contact));
	#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
	contact->normalA = quatRotateVec3FastApproximate(quatConjugateFast(bodyA->configuration.orientation), normal);
	#endif

	// Calculate the combined friction and restitution scalars.
	physContactFriction(contact) = physContactCalculateFriction   (colliderA->friction,    colliderB->friction);
	contact->restitution         = physContactCalculateRestitution(colliderA->restitution, colliderB->restitution);

	contact->contactNum = manifold->contactNum;

}

#ifdef PHYSICS_CONSTRAINT_WARM_START
static __FORCE_INLINE__ void physContactPointWarmStart(physContactPoint *const __RESTRICT__ point, const physContact *const __RESTRICT__ contact, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB){

	// Warm-start the persistent contact point.

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
#endif

__FORCE_INLINE__ void physContactPersist(physContact *const __RESTRICT__ contact, const cContact *const __RESTRICT__ manifold, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, const physCollider *const __RESTRICT__ colliderA, const physCollider *const __RESTRICT__ colliderB){

	// Copies the accumulators for persisting contacts
	// and resets non-persisting accumulators.
	//
	// Also generates a global normal and tangents.

	const cContactPoint *cPoint = manifold->contacts;
	physContactPoint *pcPoint = contact->contacts;
	const cContactPoint *const cPointLast = &cPoint[manifold->contactNum];
	const physContactPoint *const pPointLast = &pcPoint[contact->contactNum];

	// Keep track of the contacts we're updating.
	unsigned int persistent[COLLISION_MANIFOLD_MAX_CONTACT_POINTS];
	unsigned int *flag = persistent;

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
	vec3OrthonormalBasis(normal, &physContactTangent1(contact), &physContactTangent2(contact));
	///physContactTangent1(contact) = vec3Orthogonal(normal);
	///physContactTangent2(contact) = vec3Cross(normal, physContactTangent1(contact));
	#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
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
		#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
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
			#ifdef PHYSICS_CONSTRAINT_WARM_START
			physContactPointWarmStart(pcPoint, contact, bodyA, bodyB);
			#endif
		}

	}

	// Get the average halfway point, used for friction.
	halfway = vec3VMultS(halfway, pReciprocal);
	physContactHalfwayA(contact) = vec3VSubV(halfway, bodyA->centroidGlobal);
	physContactHalfwayB(contact) = vec3VSubV(halfway, bodyB->centroidGlobal);

	// Warm-start the contact.
	#if defined(PHYSICS_CONTACT_FRICTION_CONSTRAINT) && defined(PHYSICS_CONSTRAINT_WARM_START)
	physJointFrictionWarmStart(&contact->frictionConstraint, bodyA, bodyB);
	#endif

	contact->contactNum = manifold->contactNum;

}

static __FORCE_INLINE__ float physContactEffectiveMass(const vec3 normalA, const vec3 pointA, const mat3 inverseInertiaTensorA, const vec3 normalB, const vec3 pointB, const mat3 inverseInertiaTensorB, const float inverseMassTotal){

	// Effective mass:
	// (JM^-1)J^T = mA^-1 + mB^-1 + ((rA X n) . (IA^-1 * (rA X n))) + ((rB X n) . (IB^-1 * (rB X n)))
	const vec3 angularDeltaA = vec3Cross(pointA, normalA);
	const vec3 angularDeltaB = vec3Cross(pointB, normalB);
	return inverseMassTotal +
	       vec3Dot(angularDeltaA, mat3MMultV(inverseInertiaTensorA, angularDeltaA)) +
	       vec3Dot(angularDeltaB, mat3MMultV(inverseInertiaTensorB, angularDeltaB));

}

static __FORCE_INLINE__ void physContactPointGenerateInverseEffectiveMass(physContactPoint *const __RESTRICT__ point, const physContact *const __RESTRICT__ contact, const physRigidBody *const __RESTRICT__ bodyA, const physRigidBody *const __RESTRICT__ bodyB, const float inverseMassTotal){

	// Calculate the inverse effective mass for the normal constraint.

	float effectiveMass = physContactEffectiveMass(
		physContactNormal(contact), point->rA, bodyA->inverseInertiaTensorGlobal,
		physContactNormal(contact), point->rB, bodyB->inverseInertiaTensorGlobal,
		inverseMassTotal
	);
	point->normalInverseEffectiveMass = (effectiveMass > 0.f ? 1.f/effectiveMass : 0.f);

	#ifndef PHYSICS_CONTACT_FRICTION_CONSTRAINT
	effectiveMass = physContactEffectiveMass(
		physContactTangent1(contact), point->rA, bodyA->inverseInertiaTensorGlobal,
		physContactTangent1(contact), point->rB, bodyB->inverseInertiaTensorGlobal,
		inverseMassTotal
	);
	point->tangentInverseEffectiveMass1 = (effectiveMass > 0.f ? 1.f/effectiveMass : 0.f);

	effectiveMass = physContactEffectiveMass(
		physContactTangent2(contact), point->rA, bodyA->inverseInertiaTensorGlobal,
		physContactTangent2(contact), point->rB, bodyB->inverseInertiaTensorGlobal,
		inverseMassTotal
	);
	point->tangentInverseEffectiveMass2 = (effectiveMass > 0.f ? 1.f/effectiveMass : 0.f);
	#endif

}

#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
static __FORCE_INLINE__ void physContactPointGenerateBias(physContactPoint *const __RESTRICT__ point, const physContact *const __RESTRICT__ contact, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, const float frequency){
#else
static __FORCE_INLINE__ void physContactPointGenerateBias(physContactPoint *const __RESTRICT__ point, const physContact *const __RESTRICT__ contact, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB){
#endif

	// Generate the constraint bias. Used in the
	// constraint's Lagrange multiplier, i.e.:
	//
	// -(JV + b)/((JM^-1)J^T)

	float temp;

	#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL

	// Calculate potential slop.
	temp = point->separation + PHYSICS_LINEAR_SLOP;

	// Calculate the bias term.
	point->bias = PHYSICS_BAUMGARTE_TERM * frequency * (temp < 0.f ? temp : 0.f);

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

#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
__FORCE_INLINE__ void physContactPresolveConstraints(physContact *const __RESTRICT__ contact, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, const float frequency){
#else
__FORCE_INLINE__ void physContactPresolveConstraints(physContact *const __RESTRICT__ contact, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB){
#endif

	// Builds a physContact from a cContact.

	physContactPoint *pPoint = &contact->contacts[0];
	const physContactPoint *const pPointLast = &pPoint[contact->contactNum];

	const float inverseMassTotal = bodyA->inverseMass + bodyB->inverseMass;

	// Calculate extra data for each contact point.
	for(; pPoint < pPointLast; ++pPoint){

		// Generate impulses, including the two
		// tangent impulses for friction simulation.
		physContactPointGenerateInverseEffectiveMass(pPoint, contact, bodyA, bodyB, inverseMassTotal);

		// Generate bias term.
		#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
		physContactPointGenerateBias(pPoint, contact, bodyA, bodyB, frequency);
		#else
		physContactPointGenerateBias(pPoint, contact, bodyA, bodyB);
		#endif

	}

	#ifdef PHYSICS_CONTACT_FRICTION_CONSTRAINT
	physJointFrictionGenerateInverseEffectiveMass(&contact->frictionConstraint, bodyA, bodyB, inverseMassTotal);
	#endif

}

__FORCE_INLINE__ void physContactReset(physContact *const __RESTRICT__ contact){

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


__FORCE_INLINE__ void physContactPairRefresh(physContactPair *const __RESTRICT__ pair){
	pair->inactive = 0;
}
__FORCE_INLINE__ void physSeparationPairRefresh(physSeparationPair *const __RESTRICT__ pair){
	pair->inactive = 0;
}

void physContactPairInit(physContactPair *const pair, physCollider *const c1, physCollider *const c2, physContactPair *previous, physContactPair *next){

	// Initializes a pair using an insertion point.

	if(previous != NULL){
		// Insert between the previous pair and its next pair.
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		memQLinkNextA(previous) = (byte_t *)pair;
		#else
		previous->nextA = pair;
		#endif
	}else{
		// Insert directly before the first pair.
		c1->contactCache = pair;
	}
	if(next != NULL){
		if(next->colliderA == c1){
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			memQLinkPrevA(next) = (byte_t *)pair;
			#else
			next->prevA = pair;
			#endif
		}else{
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			memQLinkPrevB(next) = (byte_t *)pair;
			#else
			next->prevB = pair;
			#endif
		}
	}
	#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
	memQLinkPrevA(pair) = (byte_t *)previous;
	memQLinkNextA(pair) = (byte_t *)next;
	#else
	pair->prevA = previous;
	pair->nextA = next;
	#endif

	// Find the previous and next nodes for the second collider.
	previous = NULL;
	next = c2->contactCache;
	while(next != NULL && next->colliderA == c2){
		previous = next;
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		next = (physContactPair *)memQLinkNextA(next);
		#else
		next = next->nextA;
		#endif
	}

	if(previous != NULL){
		// Insert between the previous pair and its next pair.
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		memQLinkNextA(previous) = (byte_t *)pair;
		#else
		previous->nextA = pair;
		#endif
	}else{
		// Insert directly before the first pair.
		c2->contactCache = pair;
	}
	if(next != NULL){
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		memQLinkPrevB(next) = (byte_t *)pair;
		#else
		next->prevB = pair;
		#endif
	}
	#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
	memQLinkPrevB(pair) = (byte_t *)previous;
	memQLinkNextB(pair) = (byte_t *)next;
	#else
	pair->prevB = previous;
	pair->nextB = next;
	#endif

	// Set the pair's miscellaneous variables.
	pair->colliderA = c1;
	pair->colliderB = c2;
	pair->inactive = 0;

}
void physSeparationPairInit(physSeparationPair *const pair, physCollider *const c1, physCollider *const c2, physSeparationPair *previous, physSeparationPair *next){

	// Initializes a pair using an insertion point.

	if(previous != NULL){
		// Insert between the previous pair and its next pair.
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		memQLinkNextA(previous) = (byte_t *)pair;
		#else
		previous->nextA = pair;
		#endif
	}else{
		// Insert directly before the first pair.
		c1->separationCache = pair;
	}
	if(next != NULL){
		if(next->colliderA == c1){
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			memQLinkPrevA(next) = (byte_t *)pair;
			#else
			next->prevA = pair;
			#endif
		}else{
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			memQLinkPrevB(next) = (byte_t *)pair;
			#else
			next->prevB = pair;
			#endif
		}
	}
	#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
	memQLinkPrevA(pair) = (byte_t *)previous;
	memQLinkNextA(pair) = (byte_t *)next;
	#else
	pair->prevA = previous;
	pair->nextA = next;
	#endif

	// Find the previous and next nodes for the second collider.
	previous = NULL;
	next = c2->separationCache;
	while(next != NULL && next->colliderA == c2){
		previous = next;
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		next = (physSeparationPair *)memQLinkNextA(next);
		#else
		next = next->nextA;
		#endif
	}

	if(previous != NULL){
		// Insert between the previous pair and its next pair.
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		memQLinkNextA(previous) = (byte_t *)pair;
		#else
		previous->nextA = pair;
		#endif
	}else{
		// Insert directly before the first pair.
		c2->separationCache = pair;
	}
	if(next != NULL){
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		memQLinkPrevB(next) = (byte_t *)pair;
		#else
		next->prevB = pair;
		#endif
	}
	#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
	memQLinkPrevB(pair) = (byte_t *)previous;
	memQLinkNextB(pair) = (byte_t *)next;
	#else
	pair->prevB = previous;
	pair->nextB = next;
	#endif

	// Set the pair's miscellaneous variables.
	pair->colliderA = c1;
	pair->colliderB = c2;
	pair->inactive = 0;

}
void physContactPairDelete(physContactPair *const pair){

	// Removes a pair from its linked lists.

	physContactPair *temp;

	// Remove references from the previous pairs.
	#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
	temp = (physContactPair *)memQLinkPrevA(pair);
	#else
	temp = pair->prevA;
	#endif
	if(temp != NULL){
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		memQLinkNextA(temp) = memQLinkNextA(pair);
		#else
		temp->nextA = pair->nextA;
		#endif
	}else{
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		pair->colliderA->contactCache = (physContactPair *)memQLinkNextA(pair);
		#else
		pair->colliderA->contactCache = pair->nextA;
		#endif
	}
	#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
	temp = (physContactPair *)memQLinkPrevB(pair);
	#else
	temp = pair->prevB;
	#endif
	if(temp != NULL){
		if(temp->colliderA == pair->colliderB){
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			memQLinkNextA(temp) = memQLinkNextB(pair);
			#else
			temp->nextA = pair->nextB;
			#endif
		}else{
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			memQLinkNextB(temp) = memQLinkNextB(pair);
			#else
			temp->nextB = pair->nextB;
			#endif
		}
	}else{
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		pair->colliderB->contactCache = (physContactPair *)memQLinkNextB(pair);
		#else
		pair->colliderB->contactCache = pair->nextB;
		#endif
	}

	// Remove references from the next pairs.
	#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
	temp = (physContactPair *)memQLinkNextA(pair);
	#else
	temp = pair->nextA;
	#endif
	if(temp != NULL){
		if(temp->colliderA == pair->colliderA){
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			memQLinkPrevA(temp) = memQLinkPrevA(pair);
			#else
			temp->prevA = pair->prevA;
			#endif
		}else{
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			memQLinkPrevB(temp) = memQLinkPrevA(pair);
			#else
			temp->prevB = pair->prevA;
			#endif
		}
	}
	#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
	temp = (physContactPair *)memQLinkNextB(pair);
	#else
	temp = pair->nextB;
	#endif
	if(temp != NULL){
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		memQLinkPrevB(temp) = memQLinkPrevB(pair);
		#else
		temp->prevB = pair->prevB;
		#endif
	}

}
void physSeparationPairDelete(physSeparationPair *const pair){

	// Removes a pair from its linked lists.

	physSeparationPair *temp;

	// Remove references from the previous pairs.
	#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
	temp = (physSeparationPair *)memQLinkPrevA(pair);
	#else
	temp = pair->prevA;
	#endif
	if(temp != NULL){
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		memQLinkNextA(temp) = memQLinkNextA(pair);
		#else
		temp->nextA = pair->nextA;
		#endif
	}else{
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		pair->colliderA->separationCache = (physSeparationPair *)memQLinkNextA(pair);
		#else
		pair->colliderA->separationCache = pair->nextA;
		#endif
	}
	#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
	temp = (physSeparationPair *)memQLinkPrevB(pair);
	#else
	temp = pair->prevB;
	#endif
	if(temp != NULL){
		if(temp->colliderA == pair->colliderB){
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			memQLinkNextA(temp) = memQLinkNextB(pair);
			#else
			temp->nextA = pair->nextB;
			#endif
		}else{
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			memQLinkNextB(temp) = memQLinkNextB(pair);
			#else
			temp->nextB = pair->nextB;
			#endif
		}
	}else{
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		pair->colliderB->separationCache = (physSeparationPair *)memQLinkNextB(pair);
		#else
		pair->colliderB->separationCache = pair->nextB;
		#endif
	}

	// Remove references from the next pairs.
	#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
	temp = (physSeparationPair *)memQLinkNextA(pair);
	#else
	temp = pair->nextA;
	#endif
	if(temp != NULL){
		if(temp->colliderA == pair->colliderA){
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			memQLinkPrevA(temp) = memQLinkPrevA(pair);
			#else
			temp->prevA = pair->prevA;
			#endif
		}else{
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			memQLinkPrevB(temp) = memQLinkPrevA(pair);
			#else
			temp->prevB = pair->prevA;
			#endif
		}
	}
	#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
	temp = (physSeparationPair *)memQLinkNextB(pair);
	#else
	temp = pair->nextB;
	#endif
	if(temp != NULL){
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		memQLinkPrevB(temp) = memQLinkPrevB(pair);
		#else
		temp->prevB = pair->prevB;
		#endif
	}

}

#ifndef PHYSICS_CONTACT_FRICTION_CONSTRAINT
static __FORCE_INLINE__ void physContactPointSolveVelocityTangents(physContactPoint *const __RESTRICT__ point, physContact *const __RESTRICT__ contact, physRigidBody *const bodyA, physRigidBody *const bodyB){

	// Solves the tangent impulses.

	float tangentImpulseAccumulatorNew, lambda;
	vec3 v, impulse;

	const float lambdaClamp = contact->friction * point->normalImpulseAccumulator;

	// Calculate the contact velocity.
	// ((wB X rB) + vB) - ((wA X rA) + vA)
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

	// Tangent 1.

	// Calculate the frictional impulse magnitude.
	lambda = -vec3Dot(v, physContactTangent1(contact)) * point->tangentInverseEffectiveMass1;

	// Clamp the frictional impulse magnitude.
	tangentImpulseAccumulatorNew = point->tangentImpulseAccumulator1 + lambda;
	if(tangentImpulseAccumulatorNew <= -lambdaClamp){
		tangentImpulseAccumulatorNew = -lambdaClamp;
	}else if(tangentImpulseAccumulatorNew > lambdaClamp){
		tangentImpulseAccumulatorNew = lambdaClamp;
	}
	lambda = tangentImpulseAccumulatorNew - point->tangentImpulseAccumulator1;
	point->tangentImpulseAccumulator1 = tangentImpulseAccumulatorNew;

	// Calculate the frictional impulse.
	impulse = vec3VMultS(physContactTangent1(contact), lambda);

	// Tangent 2.

	// Calculate the frictional impulse magnitude.
	lambda = -vec3Dot(v, physContactTangent2(contact)) * point->tangentInverseEffectiveMass2;

	// Clamp the frictional impulse magnitude.
	tangentImpulseAccumulatorNew = point->tangentImpulseAccumulator2 + lambda;
	if(tangentImpulseAccumulatorNew <= -lambdaClamp){
		tangentImpulseAccumulatorNew = -lambdaClamp;
	}else if(tangentImpulseAccumulatorNew > lambdaClamp){
		tangentImpulseAccumulatorNew = lambdaClamp;
	}
	lambda = tangentImpulseAccumulatorNew - point->tangentImpulseAccumulator2;
	point->tangentImpulseAccumulator2 = tangentImpulseAccumulatorNew;

	// Calculate the frictional impulse.
	impulse = vec3VAddV(impulse, vec3VMultS(physContactTangent2(contact), lambda));

	// Apply both of the frictional impulses.
	physRigidBodyApplyVelocityImpulseInverse(bodyA, point->rA, impulse);
	physRigidBodyApplyVelocityImpulse(bodyB, point->rB, impulse);

}
#endif

static __FORCE_INLINE__ void physContactPointSolveVelocity(physContactPoint *const __RESTRICT__ point, physContact *const __RESTRICT__ contact, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB){

	// Solves the normal impulse.

	float normalImpulseAccumulatorNew, lambda;
	vec3 impulse;

	// Evaluate the constraint expression, JV.
	// C = (pB - pA) . n >= 0
	// C' = dC/dt = (((wB X rB) + vB) - ((wA X rA) + vA)) . n >= 0
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
	lambda = -point->normalInverseEffectiveMass * (lambda + point->bias);

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

void physContactSolveVelocityConstraints(physContact *const __RESTRICT__ contact, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB){

	// Solves a contact's velocity constraints.

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
	physJointFrictionSolveVelocityConstraints(&contact->frictionConstraint, bodyA, bodyB, normalImpulseTotal);
	#endif

}

#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
static __FORCE_INLINE__ float physContactPointSolveConfigurationNormal(physContactPoint *const __RESTRICT__ point, physContact *const __RESTRICT__ contact, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB){

	// Solves the normal constraint.

	// Transform the contact points into global space.
	const vec3 pointGlobalA = vec3VAddV(quatRotateVec3FastApproximate(bodyA->configuration.orientation, point->pointA), bodyA->centroidGlobal);
	const vec3 pointGlobalB = vec3VAddV(quatRotateVec3FastApproximate(bodyB->configuration.orientation, point->pointB), bodyB->centroidGlobal);

	// Calculate the transformed normal and a point
	// halfway between both transformed contact points.
	vec3 normal = quatRotateVec3FastApproximate(bodyA->configuration.orientation, contact->normalA);
	const vec3 halfway = pointGlobalB;//vec3VMultS(vec3VAddV(pointGlobalA, pointGlobalB), 0.5f);

	// Calculate the separation.
	const float separation = vec3Dot(vec3VSubV(pointGlobalB, pointGlobalA), normal) - PHYSICS_SEPARATION_BIAS_TOTAL;

	// Apply a slop to the configuration constraint.
	float constraint = PHYSICS_BAUMGARTE_TERM * (separation + PHYSICS_LINEAR_SLOP);

	// Make sure the magnitude is less than 0.
	if(constraint < 0.f){

		// Calculate the new, transformed contact point offsets.
		const vec3 pointLocalA = vec3VSubV(halfway, bodyA->centroidGlobal);
		const vec3 pointLocalB = vec3VSubV(halfway, bodyB->centroidGlobal);

		// Calculate the new effective mass.
		const float effectiveMass = physContactEffectiveMass(
			normal, pointLocalA, bodyA->inverseInertiaTensorGlobal,
			normal, pointLocalB, bodyB->inverseInertiaTensorGlobal,
			bodyA->inverseMass + bodyB->inverseMass
		);

		// Make sure the effective mass is greater than 0.
		if(effectiveMass > 0.f){

			// Clamp the constraint to prevent large corrections.
			if(constraint < -PHYSICS_MAXIMUM_LINEAR_CORRECTION){
				constraint = -PHYSICS_MAXIMUM_LINEAR_CORRECTION;
			}

			// Calculate the normal impulse.
			normal = vec3VMultS(normal, -constraint / effectiveMass);

			// Apply the normal impulse.
			physRigidBodyApplyConfigurationImpulseInverse(bodyA, pointLocalA, normal);
			physRigidBodyApplyConfigurationImpulse(bodyB, pointLocalB, normal);

		}

	}

	return separation;

}

float physContactSolveConfigurationConstraints(physContact *const __RESTRICT__ contact, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, float separation){

	// Solves a contact's position and orientation constraints.

	physContactPoint *point = &contact->contacts[0];
	const physContactPoint *const pLast = &point[contact->contactNum];

	// Solve each contact point.
	for(; point < pLast; ++point){
		const float newSeparation = physContactPointSolveConfigurationNormal(point, contact, bodyA, bodyB);
		separation = (separation > newSeparation ? newSeparation : separation);
	}

	return separation;

}
#endif
