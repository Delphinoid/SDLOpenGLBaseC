#include "physicsCollision.h"
#include "mat3.h"
#include "inline.h"

#define PHYS_COLLIDER_DEFAULT_VERTEX_MASS 1

static float physColliderGenerateMassMesh(physCollider *collider, float *vertexMassArray){

	hbMesh *cHull = (hbMesh *)&collider->hb.hull;

	float totalMass = 0.f;

	if(cHull->vertexNum > 0){

		size_t i;
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
		for(i = 0; i < cHull->vertexNum; ++i){

			if(vertexMassArray != NULL){
				vertexMass = vertexMassArray[i];
			}else{
				vertexMass = PHYS_COLLIDER_DEFAULT_VERTEX_MASS;
			}
			collider->centroid.x += cHull->vertices[i].x * vertexMass;
			collider->centroid.y += cHull->vertices[i].y * vertexMass;
			collider->centroid.z += cHull->vertices[i].z * vertexMass;
			totalMass += vertexMass;

			// Update aabb.left and aabb.right.
			if(cHull->vertices[i].x <= collider->aabb.left){
				collider->aabb.left = cHull->vertices[i].x;
			}else if(cHull->vertices[i].x > collider->aabb.right){
				collider->aabb.right = cHull->vertices[i].x;
			}
			// Update aabb.top and aabb.bottom.
			if(cHull->vertices[i].y >= collider->aabb.top){
				collider->aabb.top = cHull->vertices[i].y;
			}else if(cHull->vertices[i].y < collider->aabb.bottom){
				collider->aabb.bottom = cHull->vertices[i].y;
			}
			// Update aabb.front and aabb.back.
			if(cHull->vertices[i].z >= collider->aabb.front){
				collider->aabb.front = cHull->vertices[i].z;
			}else if(cHull->vertices[i].z < collider->aabb.back){
				collider->aabb.back = cHull->vertices[i].z;
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

	const hbMesh *cHull = (const hbMesh *)&collider->hb.hull;

	size_t i;
	inertiaTensor[0] = 0.f; inertiaTensor[1] = 0.f; inertiaTensor[2] = 0.f;
	inertiaTensor[3] = 0.f; inertiaTensor[4] = 0.f; inertiaTensor[5] = 0.f;

	for(i = 0; i < cHull->vertexNum; ++i){

		const float x = cHull->vertices[i].x - centroid->x;  /** Is this correct? **/
		const float y = cHull->vertices[i].y - centroid->y;
		const float z = cHull->vertices[i].z - centroid->z;
		const float sqrX = x*x;
		const float sqrY = y*y;
		const float sqrZ = z*z;
		float vertexMass;
		if(vertexMassArray != NULL){
			vertexMass = vertexMassArray[i];
		}else{
			vertexMass = PHYS_COLLIDER_DEFAULT_VERTEX_MASS;
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

	hbMesh *cGlobal = (hbMesh *)&collider->hb.hull;
	const hbMesh *cLocal = (const hbMesh *)&local->hb.hull;

	size_t i;

	// Update the collider's global centroid.
	collider->centroid.x = local->centroid.x + configuration->position.x;
	collider->centroid.y = local->centroid.y + configuration->position.y;
	collider->centroid.z = local->centroid.z + configuration->position.z;

	// Update each vertex.
	for(i = 0; i < cGlobal->vertexNum; ++i){

		// Transform the vertex.
		// Subtract the local centroid from the vertex.
		vec3SubVFromVR(&cLocal->vertices[i], &local->centroid, &cGlobal->vertices[i]);
		// Rotate the new vertex around (0, 0, 0).
		quatRotateVec3Fast(&configuration->orientation, &cGlobal->vertices[i]);
		// Scale the vertex.
		vec3MultVByV(&cGlobal->vertices[i], &configuration->scale);
		// Translate it by the global centroid.
		vec3AddVToV(&cGlobal->vertices[i], &collider->centroid);

		// Update mesh minima and maxima.
		if(i == 0){
			// Initialize them to the first vertex.
			collider->aabb.left = cGlobal->vertices[i].x;
			collider->aabb.right = cGlobal->vertices[i].x;
			collider->aabb.top = cGlobal->vertices[i].y;
			collider->aabb.bottom = cGlobal->vertices[i].y;
			collider->aabb.front = cGlobal->vertices[i].z;
			collider->aabb.back = cGlobal->vertices[i].z;
		}else{
			// Update aabb.left and aabb.right.
			if(cGlobal->vertices[i].x <= collider->aabb.left){
				collider->aabb.left = cGlobal->vertices[i].x;
			}else if(cGlobal->vertices[i].x > collider->aabb.right){
				collider->aabb.right = cGlobal->vertices[i].x;
			}
			// Update aabb.top and aabb.bottom.
			if(cGlobal->vertices[i].y >= collider->aabb.top){
				collider->aabb.top = cGlobal->vertices[i].y;
			}else if(cGlobal->vertices[i].y < collider->aabb.bottom){
				collider->aabb.bottom = cGlobal->vertices[i].y;
			}
			// Update aabb.front and aabb.back.
			if(cGlobal->vertices[i].z >= collider->aabb.front){
				collider->aabb.front = cGlobal->vertices[i].z;
			}else if(cGlobal->vertices[i].z < collider->aabb.back){
				collider->aabb.back = cGlobal->vertices[i].z;
			}
		}

	}

	// Update each normal.
	for(i = 0; i < cGlobal->faceNum; ++i){
		quatRotateVec3FastR(&configuration->orientation, &cLocal->normals[i], &cGlobal->normals[i]);
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

	return physColliderGenerateMassJumpTable[collider->hb.type](collider, vertexMassArray);

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

	physColliderGenerateMomentJumpTable[collider->hb.type](collider, centroid, vertexMassArray, inertiaTensor);

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

	physColliderUpdateJumpTable[collider->hb.type](collider, local, configuration);

}

void physColliderDelete(physCollider *collider){
	hbDelete(&collider->hb);
}
