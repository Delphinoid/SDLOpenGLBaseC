#include "physicsCollider.h"
#include "colliderConvexMesh.h"
#include "mat3.h"
#include "inline.h"

#define PHYSICS_COLLIDER_DEFAULT_VERTEX_MASS 1

static float physColliderGenerateMassMesh(physCollider *const restrict collider, const float *const vertexMassArray){

	cMesh *const cHull = (cMesh *)&collider->c.data;

	float totalMass = 0.f;

	if(cHull->vertexNum > 0){

		const float *m = vertexMassArray;
		const vec3 *v = cHull->vertices;
		const vec3 *const vLast = &v[cHull->vertexNum];
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
static void physColliderGenerateMomentMesh(const physCollider *const restrict collider, const vec3 *const restrict centroid, const float *const vertexMassArray, float *const restrict inertiaTensor){

	const cMesh *const cHull = (const cMesh *)&collider->c.data;

	const float *m = vertexMassArray;
	const vec3 *v = cHull->vertices;
	const vec3 *const vLast = &v[cHull->vertexNum];

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
static void physColliderUpdateMesh(physCollider *const restrict collider, const physCollider *const restrict local, const bone *const restrict configuration){

	const cMesh *const cGlobal = (cMesh *)&collider->c.data;
	const cMesh *const cLocal = (const cMesh *)&local->c.data;

	const vec3 *vLocal = cLocal->vertices;
	vec3 *vGlobal = cGlobal->vertices;
	const vec3 *vLast = &vGlobal[cGlobal->vertexNum];

	// Update the collider's global centroid.
	collider->centroid.x = local->centroid.x + configuration->position.x;
	collider->centroid.y = local->centroid.y + configuration->position.y;
	collider->centroid.z = local->centroid.z + configuration->position.z;


	/*
	** First iteration.
	*/
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


	/*
	** Other iterations.
	*/
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

static float physColliderGenerateMassCapsule(physCollider *const restrict collider, const float *const vertexMassArray){

	//

}
static void physColliderGenerateMomentCapsule(const physCollider *const restrict collider, const vec3 *const restrict centroid, const float *const restrict vertexMassArray, float *const restrict inertiaTensor){

	//

}
static void physColliderUpdateCapsule(physCollider *const restrict collider, const physCollider *const restrict local, const bone *const restrict configuration){

	//

}

static float physColliderGenerateMassSphere(physCollider *const restrict collider, const float *const vertexMassArray){

	//

}
static void physColliderGenerateMomentSphere(const physCollider *const restrict collider, const vec3 *const restrict centroid, const float *const restrict vertexMassArray, float *const restrict inertiaTensor){

	//

}
static void physColliderUpdateSphere(physCollider *const restrict collider, const physCollider *const restrict local, const bone *const restrict configuration){

	//

}

static float physColliderGenerateMassAABB(physCollider *const restrict collider, const float *const vertexMassArray){

	//

}
static void physColliderGenerateMomentAABB(const physCollider *const restrict collider, const vec3 *const restrict centroid, const float *const restrict vertexMassArray, float *const restrict inertiaTensor){

	//

}
static void physColliderUpdateAABB(physCollider *const restrict collider, const physCollider *const restrict local, const bone *const restrict configuration){

	//

}

static float physColliderGenerateMassPoint(physCollider *const restrict collider, const float *const vertexMassArray){

	//

}
static void physColliderGenerateMomentPoint(const physCollider *const restrict collider, const vec3 *const restrict centroid, const float *const restrict vertexMassArray, float *const restrict inertiaTensor){

	//

}
static void physColliderUpdatePoint(physCollider *const restrict collider, const physCollider *const restrict local, const bone *const restrict configuration){

	//

}

static float (* const physColliderGenerateMassJumpTable[5])(physCollider *const restrict, const float *const) = {
	physColliderGenerateMassMesh,
	physColliderGenerateMassCapsule,
	physColliderGenerateMassSphere,
	physColliderGenerateMassAABB,
	physColliderGenerateMassPoint
};
__FORCE_INLINE__ float physColliderGenerateMass(physCollider *const restrict collider, const float *const vertexMassArray){

	/*
	** Calculates the collider's center of mass
	** and default AABB. Returns the total mass.
	*/

	return physColliderGenerateMassJumpTable[collider->c.type](collider, vertexMassArray);

}

static void (* const physColliderGenerateMomentJumpTable[5])(const physCollider *const restrict, const vec3 *const restrict, const float *const restrict, float *const restrict) = {
	physColliderGenerateMomentMesh,
	physColliderGenerateMomentCapsule,
	physColliderGenerateMomentSphere,
	physColliderGenerateMomentAABB,
	physColliderGenerateMomentPoint
};
__FORCE_INLINE__ void physColliderGenerateMoment(const physCollider *const restrict collider, const vec3 *const restrict centroid, const float *const restrict vertexMassArray, float *const restrict inertiaTensor){

	/*
	** Calculates the collider's moment of inertia tensor.
	*/

	physColliderGenerateMomentJumpTable[collider->c.type](collider, centroid, vertexMassArray, inertiaTensor);

}


static void (* const physColliderUpdateJumpTable[5])(physCollider *const restrict, const physCollider *const restrict, const bone *const restrict) = {
	physColliderUpdateMesh,
	physColliderUpdateCapsule,
	physColliderUpdateSphere,
	physColliderUpdateAABB,
	physColliderUpdatePoint
};
__FORCE_INLINE__ void physColliderUpdate(physCollider *const restrict collider, const physCollider *const restrict local, const bone *const restrict configuration){

	/*
	** Updates the collider for collision detection.
	** Finds the new global AABB and updates the
	** collider's global position.
	*/

	physColliderUpdateJumpTable[collider->c.type](collider, local, configuration);

}

void physColliderDelete(physCollider *const restrict collider){
	cDelete(&collider->c);
}
