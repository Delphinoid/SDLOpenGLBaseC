#include "physCollider.h"

float physColliderGenerateMassProperties(physCollider *collider, float *vertexMassArray){

	/*
	** Calculates the collider's center of mass
	** and default AABB. Returns the total mass.
	*/

	float totalMass = 0.f;

	if(collider->hull.vertexNum > 0){

		size_t i;
		vec3 *v0;
		vec3 *v1;
		float temp;
		float doubleVolume = 0.f;

		// Initialize the AABB to the first vertex.
		collider->aabb.left = collider->hull.vertices[0].x;
		collider->aabb.right = collider->hull.vertices[0].x;
		collider->aabb.top = collider->hull.vertices[0].y;
		collider->aabb.bottom = collider->hull.vertices[0].y;
		collider->aabb.front = collider->hull.vertices[0].z;
		collider->aabb.back = collider->hull.vertices[0].z;

		// Recursively calculate the center of mass and the AABB.
		for(i = 0; i < collider->hull.vertexNum; ++i){

			v0 = &collider->hull.vertices[i-1];
			v1 = &collider->hull.vertices[i];
			temp = v0->x * v1->y - v0->y * v1->x * vertexMassArray[i];
			collider->centroid.x += (v0->x + v1->x) * temp;
			collider->centroid.y += (v0->y + v1->y) * temp;
			collider->centroid.z += (v0->z + v1->z) * temp;
			doubleVolume += temp;
			totalMass += vertexMassArray[i];

			// Update aabb.left and aabb.right.
			if(collider->hull.vertices[i].x <= collider->aabb.left){
				collider->aabb.left = collider->hull.vertices[i].x;
			}else if(collider->hull.vertices[i].x > collider->aabb.right){
				collider->aabb.right = collider->hull.vertices[i].x;
			}
			// Update aabb.top and aabb.bottom.
			if(collider->hull.vertices[i].y >= collider->aabb.top){
				collider->aabb.top = collider->hull.vertices[i].y;
			}else if(collider->hull.vertices[i].y < collider->aabb.bottom){
				collider->aabb.bottom = collider->hull.vertices[i].y;
			}
			// Update aabb.front and aabb.back.
			if(collider->hull.vertices[i].z >= collider->aabb.front){
				collider->aabb.front = collider->hull.vertices[i].z;
			}else if(collider->hull.vertices[i].z < collider->aabb.back){
				collider->aabb.back = collider->hull.vertices[i].z;
			}

		}

		// Final iteration with the last and first vertices.
		v0 = &collider->hull.vertices[collider->hull.vertexNum-1];
		v1 = &collider->hull.vertices[0];
		temp = v0->x * v1->y - v0->y * v1->x * vertexMassArray[i];
		collider->centroid.x += (v0->x + v1->x) * temp;
		collider->centroid.y += (v0->y + v1->y) * temp;
		collider->centroid.z += (v0->z + v1->z) * temp;
		doubleVolume += temp;

		// Calculate the mesh's final center of mass.
		temp = 1.f / (3.f * doubleVolume * totalMass);
		collider->centroid.x *= temp;
		collider->centroid.y *= temp;
		collider->centroid.z *= temp;

	}

	return totalMass;

}

void physColliderDelete(physCollider *collider){
	if(collider->hull.vertices != NULL){
		free(collider->hull.vertices);
	}
	if(collider->hull.indices != NULL){
		free(collider->hull.indices);
	}
}
