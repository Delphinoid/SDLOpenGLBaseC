#include "collider.h"
#include "memoryManager.h"
#include "inline.h"
#include <string.h>

void cInit(collider *const restrict c, const colliderType_t type){
	c->type = type;
	c->flags = 0;
}


return_t cInstantiateMesh(void *const instance, const void *const local){
	return cMeshInstantiate((cMesh *)instance, (cMesh *)local);
}

return_t cInstantiateComposite(void *const instance, const void *const local){
	return cCompositeInstantiate((cComposite *)instance, (cComposite *)local);
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
	instance->flags = local->flags | COLLIDER_INSTANCE;
	return cInstantiateJumpTable[local->type](&instance->data, &local->data);

}


cAABB cTransformMesh(void *const instance, const vec3 instanceCentroid, const void *const local, const vec3 localCentroid, const vec3 position, const quat orientation, const vec3 scale){

	cMesh *const cInstance = instance;
	const cMesh *const cLocal = local;

	const vec3 *vLocal = cLocal->vertices;
	vec3 *vGlobal = cInstance->vertices;
	const vec3 *vLast = &vGlobal[cInstance->vertexNum];

	cAABB tempAABB = {.min.x = 0.f, .min.y = 0.f, .min.z = 0.f, .max.x = 0.f, .max.y = 0.f, .max.z = 0.f};

	// Update each collider and find the total bounding box.
	if(vGlobal < vLast){

		// Extrapolate the collider's centroid from its position.
		cMeshCentroidFromPosition(cInstance, cLocal, position, orientation, scale);

		/*
		** First iteration.
		*/
		// Transform the vertex.
		// Subtract the local centroid from the vertex.
		//vec3SubVFromVR(vLocal, localCentroid, vGlobal);
		// Rotate the new vertex around (0, 0, 0).
		//quatRotateVec3Fast(orientation, vGlobal);
		// Scale the vertex.
		//vec3MultVByV(vGlobal, scale);
		// Translate it by the global centroid.
		//vec3AddVToV(vGlobal, instanceCentroid);
		*vGlobal = vec3VAddV(vec3VMultV(quatRotateVec3Fast(orientation, vec3VSubV(*vLocal, localCentroid)), scale), instanceCentroid);

		// Initialize the AABB to the first vertex.
		tempAABB.min = *vGlobal;
		tempAABB.max = *vGlobal;

		/*
		** Remaining iterations.
		*/
		// Update each vertex.
		for(++vLocal, ++vGlobal; vGlobal < vLast; ++vLocal, ++vGlobal){

			// Transform the vertex.
			// Subtract the local centroid from the vertex.
			//vec3SubVFromVR(vLocal, localCentroid, vGlobal);
			// Rotate the new vertex around (0, 0, 0).
			//quatRotateVec3Fast(orientation, vGlobal);
			// Scale the vertex.
			//vec3MultVByV(vGlobal, scale);
			// Translate it by the global centroid.
			//vec3AddVToV(vGlobal, instanceCentroid);
			*vGlobal = vec3VAddV(vec3VMultV(quatRotateVec3Fast(orientation, vec3VSubV(*vLocal, localCentroid)), scale), instanceCentroid);

			// Update collider minima and maxima.
			// Update aabb.left and aabb.right.
			if(vGlobal->x <= tempAABB.min.x){
				tempAABB.min.x = vGlobal->x;
			}else if(vGlobal->x > tempAABB.max.x){
				tempAABB.max.x = vGlobal->x;
			}
			// Update aabb.bottom and aabb.top.
			if(vGlobal->y <= tempAABB.min.y){
				tempAABB.min.y = vGlobal->y;
			}else if(vGlobal->y > tempAABB.max.y){
				tempAABB.max.y = vGlobal->y;
			}
			// Update aabb.back and aabb.front.
			if(vGlobal->z <= tempAABB.min.z){
				tempAABB.min.z = vGlobal->z;
			}else if(vGlobal->z > tempAABB.max.z){
				tempAABB.max.z = vGlobal->z;
			}

		}

	}

	vLocal = cLocal->normals;
	vGlobal = cInstance->normals;
	vLast = &vGlobal[cInstance->faceNum];

	// Update each normal.
	for(; vGlobal < vLast; ++vLocal, ++vGlobal){
		*vGlobal = quatRotateVec3Fast(orientation, *vLocal);
	}

	return tempAABB;

}

cAABB cTransformComposite(void *const instance, const vec3 instanceCentroid, const void *const local, const vec3 localCentroid, const vec3 position, const quat orientation, const vec3 scale){

	cComposite *const cInstance = instance;
	const cComposite *const cLocal = local;

	collider *c1 = cInstance->colliders;
	collider *c2 = cLocal->colliders;
	const collider *const cLast = &c1[cInstance->colliderNum];

	/*if(aabb == NULL){
		for(; c1 < cLast; ++c1, ++c2){
			cTransform(c1, instanceCentroid, c2, localCentroid, NULL, configuration);
		}
	}else{
		cAABB *bounds = aabb;
		for(; c1 < cLast; ++c1, ++c2, ++bounds){
			cTransform(c1, instanceCentroid, c2, localCentroid, bounds, configuration);
		}
	}*/

	cAABB tempAABB = {.min.x = 0.f, .min.y = 0.f, .min.z = 0.f, .max.x = 0.f, .max.y = 0.f, .max.z = 0.f};

	// Update each collider and find the total bounding box.
	if(c1 < cLast){

		/*
		** First iteration.
		*/
		tempAABB = cTransform(c1, instanceCentroid, c2, localCentroid, position, orientation, scale);

		/*
		** Remaining iterations.
		*/
		for(++c1, ++c2; c1 < cLast; ++c1, ++c2){

			const cAABB colliderAABB = cTransform(c1, instanceCentroid, c2, localCentroid, position, orientation, scale);

			// Update collider minima and maxima.
			// Update aabb.left and aabb.right.
			if(colliderAABB.min.x <= tempAABB.min.x){
				tempAABB.min.x = colliderAABB.min.x;
			}else if(colliderAABB.max.x > tempAABB.max.x){
				tempAABB.max.x = colliderAABB.max.x;
			}
			// Update aabb.bottom and aabb.top.
			if(colliderAABB.min.y <= tempAABB.min.y){
				tempAABB.min.y = colliderAABB.min.y;
			}else if(colliderAABB.max.y > tempAABB.max.y){
				tempAABB.max.y = colliderAABB.max.y;
			}
			// Update aabb.back and aabb.front.
			if(colliderAABB.min.z <= tempAABB.min.z){
				tempAABB.min.z = colliderAABB.min.z;
			}else if(colliderAABB.max.z > tempAABB.max.z){
				tempAABB.max.z = colliderAABB.max.z;
			}

		}

	}

	return tempAABB;

}

/** The lines below should eventually be removed. **/
#define cTransformCapsule NULL
#define cTransformSphere  NULL
#define cTransformAABB    NULL
#define cTransformPoint   NULL

cAABB (* const cTransformJumpTable[COLLIDER_TYPE_NUM])(
	void *const instance,
	const vec3 instanceCentroid,
	const void *const local,
	const vec3 localCentroid,
	const vec3 position,
	const quat orientation,
	const vec3 scale
) = {
	cTransformMesh,
	cTransformCapsule,
	cTransformSphere,
	cTransformAABB,
	cTransformPoint,
	cTransformComposite
};
__FORCE_INLINE__ cAABB cTransform(collider *const instance, const vec3 instanceCentroid, const collider *const local, const vec3 localCentroid, const vec3 position, const quat orientation, const vec3 scale){

	/*
	** Updates the collider by transforming it.
	*/

	return cTransformJumpTable[instance->type](&instance->data, instanceCentroid, &local->data, localCentroid, position, orientation, scale);

}


void cDelete(collider *const restrict c){
	/*
	** Handle deletions for base and instance colliders.
	*/
	if(flagsAreSet(c->flags, COLLIDER_INSTANCE)){
		if(c->type == COLLIDER_TYPE_MESH){
			cMeshDelete((cMesh *)&c->data);
		}else if(c->type == COLLIDER_TYPE_COMPOSITE){
			cCompositeDelete((cComposite *)&c->data);
		}
	}else{
		if(c->type == COLLIDER_TYPE_MESH){
			cMeshDeleteBase((cMesh *)&c->data);
		}else if(c->type == COLLIDER_TYPE_COMPOSITE){
			cCompositeDeleteBase((cComposite *)&c->data);
		}
	}
}