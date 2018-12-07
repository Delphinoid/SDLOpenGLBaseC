#include "physicsCollision.h"
#include "mat3.h"
#include "inline.h"

#define PHYSICS_COLLIDER_DEFAULT_VERTEX_MASS 1

static float physColliderGenerateMassMesh(physCollider *collider, float *vertexMassArray){

	cMesh *cHull = (cMesh *)&collider->c.hull;

	float totalMass = 0.f;

	if(cHull->vertexNum > 0){

		float *m = vertexMassArray;
		vec3 *v = cHull->vertices;
		vec3 *vLast = &v[cHull->vertexNum];
		float inverseTotalMass;
		float vertexMass;

		// Initialize the AABB to the first vertex.
		collider->aabb.left = cHull->vertices[0].x;
		collider->aabb.right = cHull->vertices[0].x;
		collider->aabb.top = cHull->vertices[0].y;
		collider->aabb.bottom = cHull->vertices[0].y;
		collider->aabb.front = cHull->vertices[0].z;
		collider->aabb.back = cHull->vertices[0].z;

		collider->centroid.x = 0.f;
		collider->centroid.y = 0.f;
		collider->centroid.z = 0.f;

		// Recursively calculate the center of mass and the AABB.
		for(; v < vLast; ++v, ++m){

			if(vertexMassArray != NULL){
				vertexMass = *m;
			}else{
				vertexMass = PHYSICS_COLLIDER_DEFAULT_VERTEX_MASS;
			}
			collider->centroid.x += v->x * vertexMass;
			collider->centroid.y += v->y * vertexMass;
			collider->centroid.z += v->z * vertexMass;
			totalMass += vertexMass;

			// Update aabb.left and aabb.right.
			if(v->x <= collider->aabb.left){
				collider->aabb.left = v->x;
			}else if(v->x > collider->aabb.right){
				collider->aabb.right = v->x;
			}
			// Update aabb.top and aabb.bottom.
			if(v->y >= collider->aabb.top){
				collider->aabb.top = v->y;
			}else if(v->y < collider->aabb.bottom){
				collider->aabb.bottom = v->y;
			}
			// Update aabb.front and aabb.back.
			if(v->z >= collider->aabb.front){
				collider->aabb.front = v->z;
			}else if(v->z < collider->aabb.back){
				collider->aabb.back = v->z;
			}

		}

		// Calculate the mesh's final center of mass.
		if(totalMass > 0.f){
			inverseTotalMass = 1.f / totalMass;
			collider->centroid.x *= inverseTotalMass;
			collider->centroid.y *= inverseTotalMass;
			collider->centroid.z *= inverseTotalMass;
		}

	}

	return totalMass;

}
static void physColliderGenerateMomentMesh(const physCollider *collider, const vec3 *centroid, const float *vertexMassArray, float *inertiaTensor){

	const cMesh *cHull = (const cMesh *)&collider->c.hull;

	const float *m = vertexMassArray;
	vec3 *v = cHull->vertices;
	vec3 *vLast = &v[cHull->vertexNum];

	inertiaTensor[0] = 0.f; inertiaTensor[1] = 0.f; inertiaTensor[2] = 0.f;
	inertiaTensor[3] = 0.f; inertiaTensor[4] = 0.f; inertiaTensor[5] = 0.f;

	for(; v < vLast; ++v, ++m){

		const float x = v->x - centroid->x;  /** Is this correct? **/
		const float y = v->y - centroid->y;
		const float z = v->z - centroid->z;
		const float sqrX = x*x;
		const float sqrY = y*y;
		const float sqrZ = z*z;
		float vertexMass;
		if(vertexMassArray != NULL){
			vertexMass = *m;
		}else{
			vertexMass = PHYSICS_COLLIDER_DEFAULT_VERTEX_MASS;
		}
		// xx
		inertiaTensor[0] += (sqrY + sqrZ) * vertexMass;
		// yy
		inertiaTensor[1] += (sqrX + sqrZ) * vertexMass;
		// zz
		inertiaTensor[2] += (sqrX + sqrY) * vertexMass;
		// xy yx
		inertiaTensor[3] -= x * y * vertexMass;
		// xz zx
		inertiaTensor[4] -= x * z * vertexMass;
		// yz zy
		inertiaTensor[5] -= y * z * vertexMass;

	}

}
static void physColliderUpdateMesh(physCollider *collider, const physCollider *local, const bone *configuration){

	cMesh *cGlobal = (cMesh *)&collider->c.hull;
	const cMesh *cLocal = (const cMesh *)&local->c.hull;

	vec3 *vLocal = cLocal->vertices;
	vec3 *vGlobal = cGlobal->vertices;
	vec3 *vLast = &vGlobal[cGlobal->vertexNum];

	// Update the collider's global centroid.
	collider->centroid.x = local->centroid.x + configuration->position.x;
	collider->centroid.y = local->centroid.y + configuration->position.y;
	collider->centroid.z = local->centroid.z + configuration->position.z;


	/* First iteration. */
	// Transform the vertex.
	// Subtract the local centroid from the vertex.
	vec3SubVFromVR(vLocal, &local->centroid, vGlobal);
	// Rotate the new vertex around (0, 0, 0).
	quatRotateVec3Fast(&configuration->orientation, vGlobal);
	// Scale the vertex.
	vec3MultVByV(vGlobal, &configuration->scale);
	// Translate it by the global centroid.
	vec3AddVToV(vGlobal, &collider->centroid);

	// Initialize them to the first vertex.
	collider->aabb.left = vGlobal->x;
	collider->aabb.right = vGlobal->x;
	collider->aabb.top = vGlobal->y;
	collider->aabb.bottom = vGlobal->y;
	collider->aabb.front = vGlobal->z;
	collider->aabb.back = vGlobal->z;


	/* Other iterations. */
	// Update each vertex.
	for(++vLocal, ++vGlobal; vGlobal < vLast; ++vLocal, ++vGlobal){

		// Transform the vertex.
		// Subtract the local centroid from the vertex.
		vec3SubVFromVR(vLocal, &local->centroid, vGlobal);
		// Rotate the new vertex around (0, 0, 0).
		quatRotateVec3Fast(&configuration->orientation, vGlobal);
		// Scale the vertex.
		vec3MultVByV(vGlobal, &configuration->scale);
		// Translate it by the global centroid.
		vec3AddVToV(vGlobal, &collider->centroid);

		// Update mesh minima and maxima.
		// Update aabb.left and aabb.right.
		if(vGlobal->x <= collider->aabb.left){
			collider->aabb.left = vGlobal->x;
		}else if(vGlobal->x > collider->aabb.right){
			collider->aabb.right = vGlobal->x;
		}
		// Update aabb.top and aabb.bottom.
		if(vGlobal->y >= collider->aabb.top){
			collider->aabb.top = vGlobal->y;
		}else if(vGlobal->y < collider->aabb.bottom){
			collider->aabb.bottom = vGlobal->y;
		}
		// Update aabb.front and aabb.back.
		if(vGlobal->z >= collider->aabb.front){
			collider->aabb.front = vGlobal->z;
		}else if(vGlobal->z < collider->aabb.back){
			collider->aabb.back = vGlobal->z;
		}

	}


	vLocal = cLocal->normals;
	vGlobal = cGlobal->normals;
	vLast = &vGlobal[cGlobal->faceNum];

	// Update each normal.
	for(; vGlobal < vLast; ++vLocal, ++vGlobal){
		quatRotateVec3FastR(&configuration->orientation, vLocal, vGlobal);
	}

}

static float physColliderGenerateMassCapsule(physCollider *collider, float *vertexMassArray){

	//

}
static void physColliderGenerateMomentCapsule(const physCollider *collider, const vec3 *centroid, const float *vertexMassArray, float *inertiaTensor){

	//

}
static void physColliderUpdateCapsule(physCollider *collider, const physCollider *local, const bone *configuration){

	//

}

static float physColliderGenerateMassSphere(physCollider *collider, float *vertexMassArray){

	//

}
static void physColliderGenerateMomentSphere(const physCollider *collider, const vec3 *centroid, const float *vertexMassArray, float *inertiaTensor){

	//

}
static void physColliderUpdateSphere(physCollider *collider, const physCollider *local, const bone *configuration){

	//

}

static float physColliderGenerateMassAABB(physCollider *collider, float *vertexMassArray){

	//

}
static void physColliderGenerateMomentAABB(const physCollider *collider, const vec3 *centroid, const float *vertexMassArray, float *inertiaTensor){

	//

}
static void physColliderUpdateAABB(physCollider *collider, const physCollider *local, const bone *configuration){

	//

}

static float (* const physColliderGenerateMassJumpTable[4])(physCollider *, float *) = {
	physColliderGenerateMassMesh,
	physColliderGenerateMassCapsule,
	physColliderGenerateMassSphere,
	physColliderGenerateMassAABB
};
__FORCE_INLINE__ float physColliderGenerateMass(physCollider *collider, float *vertexMassArray){

	/*
	** Calculates the collider's center of mass
	** and default AABB. Returns the total mass.
	*/

	return physColliderGenerateMassJumpTable[collider->c.type](collider, vertexMassArray);

}

static void (* const physColliderGenerateMomentJumpTable[4])(const physCollider *, const vec3 *, const float *, float *) = {
	physColliderGenerateMomentMesh,
	physColliderGenerateMomentCapsule,
	physColliderGenerateMomentSphere,
	physColliderGenerateMomentAABB
};
__FORCE_INLINE__ void physColliderGenerateMoment(const physCollider *collider, const vec3 *centroid, const float *vertexMassArray, float *inertiaTensor){

	/*
	** Calculates the collider's moment of inertia tensor.
	*/

	physColliderGenerateMomentJumpTable[collider->c.type](collider, centroid, vertexMassArray, inertiaTensor);

}


static void (* const physColliderUpdateJumpTable[4])(physCollider *, const physCollider *, const bone *) = {
	physColliderUpdateMesh,
	physColliderUpdateCapsule,
	physColliderUpdateSphere,
	physColliderUpdateAABB
};
__FORCE_INLINE__ void physColliderUpdate(physCollider *collider, const physCollider *local, const bone *configuration){

	/*
	** Updates the collider for collision detection.
	** Finds the new global AABB and updates the
	** collider's global position.
	*/

	physColliderUpdateJumpTable[collider->c.type](collider, local, configuration);

}

void physColliderDelete(physCollider *collider){
	cDelete(&collider->c);
}
