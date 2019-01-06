#include "collider.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

void cInit(collider *const restrict c, const colliderType_t type){
	c->type = type;
}


return_t cInstantiateMesh(void *const instance, const void *const local){

	cMesh *const cInstance = instance;
	const cMesh *const cLocal = local;

	cVertexIndex_t vertexArraySize;
	cFaceIndex_t normalArraySize;

	vertexArraySize = cLocal->vertexNum * sizeof(vec3);
	cInstance->vertices = memAllocate(vertexArraySize);
	if(cInstance->vertices == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	normalArraySize = cLocal->faceNum * sizeof(vec3);
	cInstance->normals = memAllocate(normalArraySize);
	if(cInstance->normals == NULL){
		/** Memory allocation failure. **/
		memFree(cInstance->vertices);
		return -1;
	}

	cInstance->vertexNum = cLocal->vertexNum;
	cInstance->edgeMax   = cLocal->edgeMax;
	cInstance->faceNum   = cLocal->faceNum;
	cInstance->edgeNum   = cLocal->edgeNum;

	memcpy(cInstance->vertices, cLocal->vertices, vertexArraySize);
	memcpy(cInstance->normals,  cLocal->normals,  normalArraySize);

	// Re-use the faces and edges arrays. Vertices and
	// normals, however, are modified each update when the
	// collider's configuration changes.
	cInstance->faces = cLocal->faces;
	cInstance->edges = cLocal->edges;

	return 1;

}

return_t cInstantiateComposite(void *const instance, const void *const local){

	cComposite *const cInstance = instance;
	const cComposite *const cLocal = local;

	const size_t bufferSize = sizeof(collider)*cLocal->colliderNum;
	collider *tempBuffer = memAllocate(bufferSize);
	if(tempBuffer != NULL){

		collider *c1 = tempBuffer;
		const collider *c2 = cLocal->colliders;
		const collider *const cLast = (const collider *)((const byte_t *)&tempBuffer + bufferSize);

		// Loop through each collider, instantiating them.
		for(; c1 < cLast; ++c1, ++c2){
			if(cInstantiate(c1, c2) < 0){
				/** Memory allocation failure. **/
				while(c1 > tempBuffer){
					--c1;
					cDelete(c1);
				}
				return -1;
			}
		}

		cInstance->colliders = tempBuffer;
		cInstance->colliderNum = cLocal->colliderNum;
		return 1;

	}

	/** Memory allocation failure. **/
	return -1;

}

/** The lines below should eventually be removed. **/
#define cInstantiateCapsule NULL
#define cInstantiateSphere  NULL
#define cInstantiateAABB    NULL
#define cInstantiatePoint   NULL

return_t (* const cInstantiateJumpTable[COLLIDER_TYPE_NUM])(
	void *const instance,
	const void *const local
) = {
	cInstantiateMesh,
	cInstantiateCapsule,
	cInstantiateSphere,
	cInstantiateAABB,
	cInstantiatePoint,
	cInstantiateComposite
};
__FORCE_INLINE__ return_t cInstantiate(collider *const instance, const collider *const local){

	/*
	** Instantiates a collider by performing a copy.
	*/

	instance->type = local->type;
	return cInstantiateJumpTable[local->type](&instance->data, &local->data);

}


void cTransformMesh(void *const instance, const vec3 *const instanceCentroid, const void *const local, const vec3 *const localCentroid, cAABB *const restrict aabb, const bone *const configuration){

	cMesh *const cInstance = instance;
	const cMesh *const cLocal = local;

	const vec3 *vLocal = cLocal->vertices;
	vec3 *vGlobal = cInstance->vertices;
	const vec3 *vLast = &vGlobal[cInstance->vertexNum];

	cAABB tempAABB = {.left = 0.f, .right = 0.f, .top = 0.f, .bottom = 0.f, .front = 0.f, .back = 0.f};

	// Update each collider and find the total bounding box.
	if(vGlobal < vLast){

		// Extrapolate the collider's centroid from its position.
		cInstance->centroid = cLocal->centroid;
		quatRotateVec3Fast(&configuration->orientation, &cInstance->centroid);
		vec3AddVToV(&cInstance->centroid, &configuration->position);

		/*
		** First iteration.
		*/
		// Transform the vertex.
		// Subtract the local centroid from the vertex.
		vec3SubVFromVR(vLocal, localCentroid, vGlobal);
		// Rotate the new vertex around (0, 0, 0).
		quatRotateVec3Fast(&configuration->orientation, vGlobal);
		// Scale the vertex.
		vec3MultVByV(vGlobal, &configuration->scale);
		// Translate it by the global centroid.
		vec3AddVToV(vGlobal, instanceCentroid);

		// Initialize the AABB to the first vertex.
		tempAABB.left = vGlobal->x;
		tempAABB.right = vGlobal->x;
		tempAABB.top = vGlobal->y;
		tempAABB.bottom = vGlobal->y;
		tempAABB.front = vGlobal->z;
		tempAABB.back = vGlobal->z;

		/*
		** Remaining iterations.
		*/
		// Update each vertex.
		for(++vLocal, ++vGlobal; vGlobal < vLast; ++vLocal, ++vGlobal){

			// Transform the vertex.
			// Subtract the local centroid from the vertex.
			vec3SubVFromVR(vLocal, localCentroid, vGlobal);
			// Rotate the new vertex around (0, 0, 0).
			quatRotateVec3Fast(&configuration->orientation, vGlobal);
			// Scale the vertex.
			vec3MultVByV(vGlobal, &configuration->scale);
			// Translate it by the global centroid.
			vec3AddVToV(vGlobal, instanceCentroid);

			// Update collider minima and maxima.
			// Update aabb.left and aabb.right.
			if(vGlobal->x <= tempAABB.left){
				tempAABB.left = vGlobal->x;
			}else if(vGlobal->x > tempAABB.right){
				tempAABB.right = vGlobal->x;
			}
			// Update aabb.top and aabb.bottom.
			if(vGlobal->y >= tempAABB.top){
				tempAABB.top = vGlobal->y;
			}else if(vGlobal->y < tempAABB.bottom){
				tempAABB.bottom = vGlobal->y;
			}
			// Update aabb.front and aabb.back.
			if(vGlobal->z >= tempAABB.front){
				tempAABB.front = vGlobal->z;
			}else if(vGlobal->z < tempAABB.back){
				tempAABB.back = vGlobal->z;
			}

		}

	}

	vLocal = cLocal->normals;
	vGlobal = cInstance->normals;
	vLast = &vGlobal[cInstance->faceNum];

	// Update each normal.
	for(; vGlobal < vLast; ++vLocal, ++vGlobal){
		quatRotateVec3FastR(&configuration->orientation, vLocal, vGlobal);
	}

	if(aabb != NULL){
		*aabb = tempAABB;
	}

}

void cTransformComposite(void *const instance, const vec3 *const instanceCentroid, const void *const local, const vec3 *const localCentroid, cAABB *const restrict aabb, const bone *const configuration){

	cComposite *const cInstance = instance;
	const cComposite *const cLocal = local;

	collider *c1 = cInstance->colliders;
	collider *c2 = cLocal->colliders;
	const collider *const cLast = &c1[cInstance->colliderNum];

	cAABB tempAABB = {.left = 0.f, .right = 0.f, .top = 0.f, .bottom = 0.f, .front = 0.f, .back = 0.f};

	// Update each collider and find the total bounding box.
	if(c1 < cLast){

		/*
		** First iteration.
		*/
		cTransform(c1, instanceCentroid, c2, localCentroid, &tempAABB, configuration);

		/*
		** Remaining iterations.
		*/
		for(++c1, ++c2; c1 < cLast; ++c1, ++c2){

			cAABB colliderAABB;
			cTransform(c1, instanceCentroid, c2, localCentroid, &colliderAABB, configuration);

			// Update collider minima and maxima.
			// Update aabb.left and aabb.right.
			if(colliderAABB.left <= tempAABB.left){
				tempAABB.left = colliderAABB.left;
			}else if(colliderAABB.right > tempAABB.right){
				tempAABB.right = colliderAABB.right;
			}
			// Update aabb.top and aabb.bottom.
			if(colliderAABB.top >= tempAABB.top){
				tempAABB.top = colliderAABB.top;
			}else if(colliderAABB.bottom < tempAABB.bottom){
				tempAABB.bottom = colliderAABB.bottom;
			}
			// Update aabb.front and aabb.back.
			if(colliderAABB.front >= tempAABB.front){
				tempAABB.front = colliderAABB.front;
			}else if(colliderAABB.back < tempAABB.back){
				tempAABB.back = colliderAABB.back;
			}

		}

	}

	if(aabb != NULL){
		*aabb = tempAABB;
	}

}

/** The lines below should eventually be removed. **/
#define cTransformCapsule NULL
#define cTransformSphere  NULL
#define cTransformAABB    NULL
#define cTransformPoint   NULL

void (* const cTransformJumpTable[COLLIDER_TYPE_NUM])(
	void *const instance,
	const vec3 *const instanceCentroid,
	const void *const local,
	const vec3 *const localCentroid,
	cAABB *const restrict aabb,
	const bone *const configuration
) = {
	cTransformMesh,
	cTransformCapsule,
	cTransformSphere,
	cTransformAABB,
	cTransformPoint,
	cTransformComposite
};
__FORCE_INLINE__ void cTransform(collider *const instance, const vec3 *const instanceCentroid, const collider *const local, const vec3 *const localCentroid, cAABB *const restrict aabb, const bone *const configuration){

	/*
	** Updates the collider by transforming it.
	*/

	cTransformJumpTable[instance->type](&instance->data, instanceCentroid, &local->data, localCentroid, aabb, configuration);

}


void cDelete(collider *const restrict c){
	if(c->type == COLLIDER_TYPE_MESH){
		cMeshDelete((cMesh *)&c->data);
	}
}