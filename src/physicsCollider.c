#include "physicsCollider.h"
#include "memoryManager.h"
#include "colliderMesh.h"
#include "mat3.h"
#include "inline.h"

#define PHYSICS_COLLIDER_DEFAULT_VERTEX_MASS 1

void physColliderGenerateMassMesh(void *const local, float *const restrict mass, float *const restrict inverseMass, vec3 *const restrict centroid, const float **const vertexMassArray){

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

void physColliderGenerateMassComposite(void *const local, float *const restrict mass, float *const restrict inverseMass, vec3 *const restrict centroid, const float **const vertexMassArray){

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
#define physColliderGenerateMassCapsule   NULL
#define physColliderGenerateMassSphere    NULL
#define physColliderGenerateMassAABB      NULL
#define physColliderGenerateMassPoint     NULL

void (* const physColliderGenerateMassJumpTable[COLLIDER_TYPE_NUM])(
	void *const local,
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
__FORCE_INLINE__ void physColliderGenerateMass(collider *const local, float *const restrict mass, float *const restrict inverseMass, vec3 *const restrict centroid, const float **const vertexMassArray){

	/*
	** Calculates the collider's center of mass
	** and default AABB. Returns the total mass.
	*/

	physColliderGenerateMassJumpTable[local->type](&local->data, mass, inverseMass, centroid, vertexMassArray);

}


void physColliderGenerateMomentMesh(void *const local, mat3 *const restrict inertiaTensor, vec3 *const restrict centroid, const float **const vertexMassArray){

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

void physColliderGenerateMomentComposite(void *const local, mat3 *const restrict inertiaTensor, vec3 *const restrict centroid, const float **const vertexMassArray){

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
#define physColliderGenerateMomentCapsule   NULL
#define physColliderGenerateMomentSphere    NULL
#define physColliderGenerateMomentAABB      NULL
#define physColliderGenerateMomentPoint     NULL

void (* const physColliderGenerateMomentJumpTable[COLLIDER_TYPE_NUM])(
	void *const local,
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
__FORCE_INLINE__ void physColliderGenerateMoment(collider *const local, mat3 *const restrict inertiaTensor, vec3 *const restrict centroid, const float **const vertexMassArray){

	/*
	** Calculates the collider's moment of inertia tensor.
	*/

	physColliderGenerateMomentJumpTable[local->type](&local->data, inertiaTensor, centroid, vertexMassArray);

}

void physColliderDelete(collider *const restrict hull){
	if(hull->type == COLLIDER_TYPE_MESH){
		// Meshes reuse the local collider's
		// arrays, except for vertices and
		// normals which need to be modified.
		cMesh *const cHull = (cMesh *)&hull->data;
		memFree(cHull->vertices);
		memFree(cHull->normals);
	}
}
