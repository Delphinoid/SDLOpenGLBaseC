#include "colliderAABB.h"
#include "inline.h"

__HINT_INLINE__ void cAABBExpand(cAABB *const restrict c, const float addend){
	/*
	** Expand the AABB by a specified amount.
	*/
	vec3SubSFromV(&c->min, addend);
	vec3AddSToV(&c->max, addend);
}
__HINT_INLINE__ void cAABBExpandVelocity(cAABB *const restrict c, const vec3 *const restrict velocity, const float factor){
	/*
	** Expand the AABB using a linear velocity.
	*/
	vec3 product;
	vec3MultVBySR(velocity, factor, &product);
	if(product.x >= 0.f){
		c->max.x += product.x;
	}else{
		c->min.x += product.x;
	}
	if(product.y >= 0.f){
		c->max.y += product.y;
	}else{
		c->min.y += product.y;
	}
	if(product.z >= 0.f){
		c->max.z += product.z;
	}else{
		c->min.z += product.z;
	}
}

__HINT_INLINE__ void cAABBCombine(cAABB *const restrict c1, cAABB *const restrict c2, cAABB *const restrict r){
	/*
	** Merge the two AABBs to create one that can contain them both.
	*/
	vec3Min(&c1->min, &c2->min, &r->min);
	vec3Max(&c1->max, &c2->max, &r->max);
}

__HINT_INLINE__ float cAABBVolume(const cAABB *const restrict c){
	vec3 v;
	// Because the bounding box is axis-aligned,
	// a simple subtraction will calculate the
	// length of its sides.
	vec3SubVFromVR(&c->min, &c->max, &v);
	return v.x * v.y * v.z;
}
__HINT_INLINE__ float cAABBSurfaceArea(const cAABB *const restrict c){
	vec3 v;
	// Because the bounding box is axis-aligned,
	// a simple subtraction will calculate the
	// length of its sides.
	vec3SubVFromVR(&c->min, &c->max, &v);
	return 2.f * (v.x * (v.y + v.z) + v.y * v.z);
}
__HINT_INLINE__ float cAABBSurfaceAreaHalf(const cAABB *const restrict c){
	vec3 v;
	// Because the bounding box is axis-aligned,
	// a simple subtraction will calculate the
	// length of its sides.
	vec3SubVFromVR(&c->min, &c->max, &v);
	return v.x * (v.y + v.z) + v.y * v.z;
}

__HINT_INLINE__ float cAABBVolumeCombined(const cAABB *const restrict c1, const cAABB *const restrict c2){
	vec3 v1, v2;
	vec3Min(&c1->min, &c2->min, &v1);
	vec3Max(&c1->max, &c2->max, &v2);
	vec3SubVFromV1(&v1, &v2);
	return v1.x * v1.y * v1.z;
}
__HINT_INLINE__ float cAABBSurfaceAreaCombined(const cAABB *const restrict c1, const cAABB *const restrict c2){
	vec3 v1, v2;
	vec3Min(&c1->min, &c2->min, &v1);
	vec3Max(&c1->max, &c2->max, &v2);
	vec3SubVFromV1(&v1, &v2);
	return 2.f * (v1.x * (v1.y + v1.z) + v1.y * v1.z);
}
__HINT_INLINE__ float cAABBSurfaceAreaHalfCombined(const cAABB *const restrict c1, const cAABB *const restrict c2){
	vec3 v1, v2;
	vec3Min(&c1->min, &c2->min, &v1);
	vec3Max(&c1->max, &c2->max, &v2);
	vec3SubVFromV1(&v1, &v2);
	return v1.x * (v1.y + v1.z) + v1.y * v1.z;
}

__HINT_INLINE__ return_t cAABBEncapsulates(const cAABB *const restrict container, const cAABB *const restrict containee){
	return container->min.x <= containee->min.x && container->min.y <= containee->min.y && container->min.z <= containee->min.z &&
	       container->max.x >= containee->max.x && container->max.y >= containee->max.y && container->max.z >= containee->max.z;
}
__HINT_INLINE__ return_t cAABBCollision(const cAABB *const restrict c1, const cAABB *const restrict c2){
	return c1->min.x <= c2->max.x && c1->max.x >= c2->min.x &&
	       c1->min.y <= c2->max.y && c1->max.y >= c2->min.y &&
	       c1->min.z <= c2->max.z && c1->max.z >= c2->min.z;
}