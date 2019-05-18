#include "modulePhysics.h"
#include "memoryManager.h"
#include "mat3.h"
#include "inline.h"

#define PHYSICS_COLLIDER_DEFAULT_VERTEX_MASS 1

void physColliderInit(physCollider *const restrict c, const colliderType_t type, void *const restrict body){
	cInit(&c->c, type);
	c->density = 0.f;
	c->friction = 1.f;
	c->restitution = 1.f;
	c->layers = ~((physCollisionMask_t)0);
	c->node = NULL;
	c->contactCache = NULL;
	c->separationCache = NULL;
	c->body = body;
	c->base = NULL;
}

void physColliderInstantiate(physCollider *const restrict instance, physCollider *const restrict local, void *const restrict body){
	cInstantiate(&instance->c, &local->c);
	instance->density = local->density;
	instance->friction = local->friction;
	instance->restitution = local->restitution;
	instance->layers = local->layers;
	instance->node = NULL;
	instance->contactCache = NULL;
	instance->separationCache = NULL;
	instance->body = body;
	instance->base = &local->c;
}

return_t physColliderPermitCollision(const physCollider *const c1, const physCollider *const c2){
	// Prioritize contacts where the first
	// collider has the larger address and
	// make sure the collision masks overlap.
	return c1 > c2 && (c1->layers & c2->layers) > 0;
}


void physColliderGenerateMassMesh(void *const restrict local, float *const restrict mass, float *const restrict inverseMass, vec3 *const restrict centroid, const float **const vertexMassArray){

	cMesh *const cLocal = local;

	float tempMass = 0.f;
	float tempInverseMass = 0.f;
	vec3 tempCentroid = {.x = 0.f, .y = 0.f, .z = 0.f};

	if(cLocal->vertexNum > 0){

		const float *m = *vertexMassArray;
		const vec3 *v = cLocal->vertices;
		const vec3 *const vLast = &v[cLocal->vertexNum];

		float vertexMass;

		// Recursively calculate the center of mass.
		for(; v < vLast; ++v, ++m){

			if(*vertexMassArray != NULL){
				vertexMass = *m;
			}else{
				vertexMass = PHYSICS_COLLIDER_DEFAULT_VERTEX_MASS;
			}
			tempCentroid.x += v->x * vertexMass;
			tempCentroid.y += v->y * vertexMass;
			tempCentroid.z += v->z * vertexMass;
			tempMass += vertexMass;

		}

		// Calculate the collider's final center of mass.
		if(tempMass != 0.f){
			tempInverseMass = 1.f / tempMass;
			tempCentroid.x *= tempInverseMass;
			tempCentroid.y *= tempInverseMass;
			tempCentroid.z *= tempInverseMass;
		}

	}

	cLocal->centroid = tempCentroid;

	*mass = tempMass;
	*inverseMass = tempInverseMass;
	*centroid = tempCentroid;

}

void physColliderGenerateMassComposite(void *const restrict local, float *const restrict mass, float *const restrict inverseMass, vec3 *const restrict centroid, const float **const vertexMassArray){

	cComposite *const cLocal = local;

	const float **m = vertexMassArray;
	collider *c = cLocal->colliders;
	const collider *const cLast = &c[cLocal->colliderNum];

	float tempMass = 0.f;
	vec3 tempCentroid = {.x = 0.f, .y = 0.f, .z = 0.f};

	// Generate the mass properites of each collider, as
	// well as the total, weighted centroid of the body.
	for(; c < cLast; ++c, ++m){

		float colliderMass;
		float colliderInverseMass;
		vec3 colliderCentroid;

		physColliderGenerateMass(c, &colliderMass, &colliderInverseMass, &colliderCentroid, m);

		tempCentroid.x += colliderCentroid.x * colliderMass;
		tempCentroid.y += colliderCentroid.y * colliderMass;
		tempCentroid.z += colliderCentroid.z * colliderMass;
		tempMass += colliderMass;

	}

	*mass = tempMass;
	if(tempMass != 0.f){
		const float tempInverseMass = 1.f / tempMass;
		tempCentroid.x *= tempInverseMass;
		tempCentroid.y *= tempInverseMass;
		tempCentroid.z *= tempInverseMass;
		*inverseMass = tempInverseMass;
	}else{
		*inverseMass = 0.f;
	}
	*centroid = tempCentroid;

}

/** The lines below should eventually be removed. **/
#define physColliderGenerateMassCapsule NULL
#define physColliderGenerateMassSphere  NULL
#define physColliderGenerateMassAABB    NULL
#define physColliderGenerateMassPoint   NULL

void (* const physColliderGenerateMassJumpTable[COLLIDER_TYPE_NUM])(
	void *const restrict local,
	float *const restrict mass,
	float *const restrict inverseMass,
	vec3 *const restrict centroid,
	const float **const vertexMassArray
) = {
	physColliderGenerateMassMesh,
	physColliderGenerateMassCapsule,
	physColliderGenerateMassSphere,
	physColliderGenerateMassAABB,
	physColliderGenerateMassPoint,
	physColliderGenerateMassComposite
};
__FORCE_INLINE__ void physColliderGenerateMass(collider *const restrict local, float *const restrict mass, float *const restrict inverseMass, vec3 *const restrict centroid, const float **const vertexMassArray){

	/*
	** Calculates the collider's center of mass
	** and default AABB. Returns the total mass.
	*/

	physColliderGenerateMassJumpTable[local->type](&local->data, mass, inverseMass, centroid, vertexMassArray);

}


void physColliderGenerateMomentMesh(void *const restrict local, mat3 *const restrict inertiaTensor, vec3 *const restrict centroid, const float **const vertexMassArray){

	cMesh *const cLocal = local;

	const float *m = *vertexMassArray;
	const vec3 *v = cLocal->vertices;
	const vec3 *const vLast = &v[cLocal->vertexNum];

	float tempInertiaTensor[6] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};

	for(; v < vLast; ++v, ++m){
		const float x = v->x - centroid->x;  /** Is this correct? **/
		const float y = v->y - centroid->y;
		const float z = v->z - centroid->z;
		const float sqrX = x*x;
		const float sqrY = y*y;
		const float sqrZ = z*z;
		const float vertexMass = *vertexMassArray == NULL ? PHYSICS_COLLIDER_DEFAULT_VERTEX_MASS : *m;
		// xx
		tempInertiaTensor[0] += (sqrY + sqrZ) * vertexMass;
		// yy
		tempInertiaTensor[1] += (sqrX + sqrZ) * vertexMass;
		// zz
		tempInertiaTensor[2] += (sqrX + sqrY) * vertexMass;
		// xy yx
		tempInertiaTensor[3] -= x * y * vertexMass;
		// xz zx
		tempInertiaTensor[4] -= x * z * vertexMass;
		// yz zy
		tempInertiaTensor[5] -= y * z * vertexMass;
	}

	inertiaTensor->m[0][0] = tempInertiaTensor[0];
	inertiaTensor->m[1][1] = tempInertiaTensor[1];
	inertiaTensor->m[2][2] = tempInertiaTensor[2];
	inertiaTensor->m[0][1] = tempInertiaTensor[3];
	inertiaTensor->m[0][2] = tempInertiaTensor[4];
	inertiaTensor->m[1][2] = tempInertiaTensor[5];
	// No point calculating the same numbers twice.
	inertiaTensor->m[1][0] = tempInertiaTensor[3];
	inertiaTensor->m[2][0] = tempInertiaTensor[4];
	inertiaTensor->m[2][1] = tempInertiaTensor[5];

}

void physColliderGenerateMomentComposite(void *const restrict local, mat3 *const restrict inertiaTensor, vec3 *const restrict centroid, const float **const vertexMassArray){

	cComposite *const cLocal = local;

	const float **m = vertexMassArray;
	collider *c = cLocal->colliders;
	const collider *const cLast = &c[cLocal->colliderNum];

	float tempInertiaTensor[6] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};

	// Calculate the combined moment of inertia for the
	// collider as the sum of its collider's moments.
	for(; c < cLast; ++c, ++m){

		mat3 colliderInertiaTensor;
		physColliderGenerateMoment(c, &colliderInertiaTensor, centroid, m);

		tempInertiaTensor[0] += colliderInertiaTensor.m[0][0];
		tempInertiaTensor[1] += colliderInertiaTensor.m[1][1];
		tempInertiaTensor[2] += colliderInertiaTensor.m[2][2];
		tempInertiaTensor[3] += colliderInertiaTensor.m[0][1];
		tempInertiaTensor[4] += colliderInertiaTensor.m[0][2];
		tempInertiaTensor[5] += colliderInertiaTensor.m[1][2];

	}

	inertiaTensor->m[0][0] = tempInertiaTensor[0];
	inertiaTensor->m[1][1] = tempInertiaTensor[1];
	inertiaTensor->m[2][2] = tempInertiaTensor[2];
	inertiaTensor->m[0][1] = tempInertiaTensor[3];
	inertiaTensor->m[0][2] = tempInertiaTensor[4];
	inertiaTensor->m[1][2] = tempInertiaTensor[5];
	// No point calculating the same numbers twice.
	inertiaTensor->m[1][0] = tempInertiaTensor[3];
	inertiaTensor->m[2][0] = tempInertiaTensor[4];
	inertiaTensor->m[2][1] = tempInertiaTensor[5];

}

/** The lines below should eventually be removed. **/
#define physColliderGenerateMomentCapsule NULL
#define physColliderGenerateMomentSphere  NULL
#define physColliderGenerateMomentAABB    NULL
#define physColliderGenerateMomentPoint   NULL

void (* const physColliderGenerateMomentJumpTable[COLLIDER_TYPE_NUM])(
	void *const restrict local,
	mat3 *const restrict inertiaTensor,
	vec3 *const restrict centroid,
	const float **const vertexMassArray
) = {
	physColliderGenerateMomentMesh,
	physColliderGenerateMomentCapsule,
	physColliderGenerateMomentSphere,
	physColliderGenerateMomentAABB,
	physColliderGenerateMomentPoint,
	physColliderGenerateMomentComposite
};
__FORCE_INLINE__ void physColliderGenerateMoment(collider *const restrict local, mat3 *const restrict inertiaTensor, vec3 *const restrict centroid, const float **const vertexMassArray){

	/*
	** Calculates the collider's moment of inertia tensor.
	*/

	physColliderGenerateMomentJumpTable[local->type](&local->data, inertiaTensor, centroid, vertexMassArray);

}


cAABB cTransformMesh(void *const instance, const vec3 instanceCentroid, const void *const local, const vec3 localCentroid, const vec3 position, const quat orientation, const vec3 scale);
cAABB cTransformCapsule(void *const instance, const vec3 instanceCentroid, const void *const local, const vec3 localCentroid, const vec3 position, const quat orientation, const vec3 scale);
cAABB cTransformSphere(void *const instance, const vec3 instanceCentroid, const void *const local, const vec3 localCentroid, const vec3 position, const quat orientation, const vec3 scale);
cAABB cTransformAABB(void *const instance, const vec3 instanceCentroid, const void *const local, const vec3 localCentroid, const vec3 position, const quat orientation, const vec3 scale);
cAABB cTransformPoint(void *const instance, const vec3 instanceCentroid, const void *const local, const vec3 localCentroid, const vec3 position, const quat orientation, const vec3 scale);

return_t physColliderTransformMesh(physCollider *const restrict c, physIsland *const restrict island){
	const physRigidBody *const body = c->body;
	if(flagsAreSet(body->flags, PHYSICS_BODY_TRANSFORMED | PHYSICS_BODY_COLLISION_MODIFIED)){
		c->aabb = cTransformMesh(&c->c.data, body->centroidGlobal, &c->base->data, body->centroidLocal, body->configuration.position, body->configuration.orientation, body->configuration.scale);
		return physIslandUpdateCollider(island, c);
	}
	return 1;
}

/** The lines below should eventually be removed. **/
#define physColliderTransformCapsule   NULL
#define physColliderTransformSphere    NULL
#define physColliderTransformAABB      NULL
#define physColliderTransformPoint     NULL
#define physColliderTransformComposite NULL

return_t (* const physColliderTransformJumpTable[COLLIDER_TYPE_NUM])(
	physCollider *const restrict c,
	physIsland *const restrict island
) = {
	physColliderTransformMesh,
	physColliderTransformCapsule,
	physColliderTransformSphere,
	physColliderTransformAABB,
	physColliderTransformPoint,
	physColliderTransformComposite
};
__FORCE_INLINE__ return_t physColliderTransform(physCollider *const restrict c, physIsland *const restrict island){

	/*
	** Transforms a physics collider, updating its AABB in the AABB tree.
	*/

	return physColliderTransformJumpTable[c->c.type](c, island);

}


physContactPair *physColliderFindContact(const physCollider *const c1, const physCollider *const c2, physContactPair **const previous, physContactPair **const next){

	/*
	** Find a pair from a previous successful narrowphase collision check.
	**
	** Pairs are cached in increasing order of colliderB address, so once we find
	** an address greater than the supplied address we can perform an early exit.
	**
	** We also need to return the pair directly before it in the SLink
	** so we can perform an insertion or removal later on if we need to.
	*/

	physContactPair *p = NULL;
	physContactPair *i = c1->contactCache;

	while(i != NULL && c2 >= i->colliderB){
		// Check if the incident collider is the same.
		if(c2 == i->colliderB){
			*previous = p;
			*next = i;
			return i;
		}
		p = i;
		i = (physContactPair *)memQLinkNextA(i);
	}

	*previous = p;
	*next = i;
	return NULL;

}
physSeparationPair *physColliderFindSeparation(const physCollider *const c1, const physCollider *const c2, physSeparationPair **const previous, physSeparationPair **const next){

	/*
	** Find a pair from a previous failed narrowphase collision check.
	**
	** Pairs are cached in increasing order of colliderB address, so once we find
	** an address greater than the supplied address we can perform an early exit.
	**
	** We also need to return the pair directly before it in the SLink
	** so we can perform an insertion or removal later on if we need to.
	*/

	physSeparationPair *p = NULL;
	physSeparationPair *i = c1->separationCache;

	while(i != NULL && c2 >= i->colliderB){
		// Check if the incident collider is the same.
		if(c2 == i->colliderB){
			*previous = p;
			*next = i;
			return i;
		}
		p = i;
		i = (physSeparationPair *)memQLinkNextA(i);
	}

	*previous = p;
	*next = i;
	return NULL;

}

#if !defined PHYSICS_GAUSS_SEIDEL_SOLVER || defined PHYSICS_FORCE_VELOCITY_BAUMGARTE
void physColliderUpdateContacts(physCollider *const c, const float dt){
#else
void physColliderUpdateContacts(physCollider *const c){
#endif

	/*
	** Removes any contact that has been inactive for too long.
	*/

	physContactPair *i = c->contactCache;

	while(i != NULL && i->colliderA == c){
		physContactPair *const next = (physContactPair *)memQLinkNextA(i);
		if(i->inactive > 0){
			if(i->inactive > PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS){
				// Remove the contact.
				modulePhysicsContactPairFree(i);
			}else{
				physContactReset(&i->data);
			}
		}else{
			// Update the contact.
			#if !defined PHYSICS_GAUSS_SEIDEL_SOLVER || defined PHYSICS_FORCE_VELOCITY_BAUMGARTE
			physContactUpdate(&i->data, i->colliderA, i->colliderB, dt);
			#else
			physContactUpdate(&i->data, i->colliderA, i->colliderB);
			#endif
			++i->inactive;
		}
		i = next;
	}

}

void physColliderUpdateSeparations(physCollider *const c){

	/*
	** Removes any separation that has been inactive for too long.
	*/

	physSeparationPair *i = c->separationCache;

	while(i != NULL && i->colliderA == c){
		physSeparationPair *const next = (physSeparationPair *)memQLinkNextA(i);
		if(i->inactive > PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS){
			// Remove the separation.
			modulePhysicsSeparationPairFree(i);
		}else{
			++i->inactive;
		}
		i = next;
	}

}

void physColliderDelete(physCollider *const restrict c){
	if(c->base != NULL && flagsAreUnset(c->c.flags, COLLIDER_INSTANCE)){
		// If the collider is not part of physRigidBodyBase, free its local collider.
		cDelete(c->base);
		flagsSet(c->c.flags, COLLIDER_INSTANCE);
	}
	cDelete(&c->c);
	while(c->contactCache != NULL){
		modulePhysicsContactPairFree(c->contactCache);
	}
	while(c->separationCache != NULL){
		modulePhysicsSeparationPairFree(c->separationCache);
	}
}