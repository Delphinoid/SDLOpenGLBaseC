#include "physics.h"

#define GJK_MAX_ITERATIONS 64

static inline void gjkSupport(const physicsBody *body1, const physicsBody *body2, const vec3 *axis, vec3 *r){

	/*
	** Returns a point in Minkowski space on the edge of
	** the polygons' "Minkowski difference".
	*/

	// For the first polygon, find the vertex that is
	// farthest in the direction of axis. Use a
	// negative axis for the second polygon; this will
	// give us the two closest vertices along a given
	// axis.
	const size_t index1 = meshGetFarthestVertex(&body1->hull, axis);
	vec3 axisNegative;
	axisNegative.x = -axis->x;
	axisNegative.y = -axis->y;
	axisNegative.z = -axis->z;
	const size_t index2 = meshGetFarthestVertex(&body2->hull, &axisNegative);

	// Get the "Minkowski Difference" of the two vertices.
	r->x = body1->hull.vertices[index1].position.x - body2->hull.vertices[index2].position.x;
	r->y = body1->hull.vertices[index1].position.y - body2->hull.vertices[index2].position.y;
	r->z = body1->hull.vertices[index1].position.z - body2->hull.vertices[index2].position.z;

}

static inline void gjkTriangle(unsigned char *simplexVertices, vec3 *a, vec3 *b, vec3 *c, vec3 *d, vec3 *axis){

	const vec3 AO = {.x = -a->x,
	                 .y = -a->y,
	                 .z = -a->z};
	const vec3 AB = {.x = b->x - a->x,
	                 .y = b->y - a->y,
	                 .z = b->z - a->z};
	const vec3 AC = {.x = c->x - a->x,
	                 .y = c->y - a->y,
	                 .z = c->z - a->z};
	vec3 ABC;
	vec3Cross(&AB, &AC, &ABC);
	vec3 tempCross;

	vec3Cross(&AB, &ABC, &tempCross);
	if(vec3Dot(&tempCross, &AO) > 0){

		// AB x ABC does not cross the origin, so we need a
		// new simplex fragment. Change the direction and
		// remove vertex C by replacing it with vertex A.
		vec3Cross(&AB, &AO, &tempCross);
		vec3Cross(&tempCross, &AB, axis);
		*c = *a;

	}else{

		vec3Cross(&ABC, &AC, &tempCross);
		if(vec3Dot(&tempCross, &AO) > 0){

			// ABC x AC does not cross the origin, so we need a
			// new simplex fragment. Change the direction and
			// remove vertex C by replacing it with vertex A.
			vec3Cross(&AC, &AO, &tempCross);
			vec3Cross(&tempCross, &AC, axis);
			*b = *a;

		}else{

			// Checks have passed, a tetrahedron can be generated.
			if(vec3Dot(&ABC, &AO) > 0){
				// Create the base of the tetrahedron and set
				// dir to the direction from the base to the
				// origin.
				*d = *c;
				*c = *b;
				*b = *a;
				*axis = ABC;
			}else{
				// The tetrahedron will be upside down, compensate.
				*d = *b;
				*b = *a;
				axis->x = -ABC.x;
				axis->y = -ABC.y;
				axis->z = -ABC.z;
			}

			*simplexVertices = 4;

		}

	}

}

static inline signed char gjkTetrahedron(unsigned char *simplexVertices, vec3 *a, vec3 *b, vec3 *c, vec3 *d, vec3 *axis){

	// Check if the normal of ABC is crossing the origin.
	const vec3 AO = {.x = -a->x,
	                 .y = -a->y,
	                 .z = -a->z};
	const vec3 AB = {.x = b->x - a->x,
	                 .y = b->y - a->y,
	                 .z = b->z - a->z};
	const vec3 AC = {.x = c->x - a->x,
	                 .y = c->y - a->y,
	                 .z = c->z - a->z};
	vec3 tempCross;
	vec3Cross(&AB, &AC, &tempCross);
	if(vec3Dot(&tempCross, &AO) > 0){
		*d = *c;
		*c = *b;
		*b = *a;
		*axis = tempCross;
		return 0;
	}

	// Check if the normal of ACD is crossing the origin.
	const vec3 AD = {.x = d->x - a->x,
	                 .y = d->y - a->y,
	                 .z = d->z - a->z};
	vec3Cross(&AC, &AD, &tempCross);
	if(vec3Dot(&tempCross, &AO) > 0){
		*b = *a;
		*axis = tempCross;
		return 0;
	}

	// Check if the normal of ADB is crossing the origin.
	vec3Cross(&AD, &AB, &tempCross);
	if(vec3Dot(&tempCross, &AO) > 0){
		*c = *d;
		*d = *b;
		*b = *a;
		*axis = tempCross;
		return 0;
	}

	return 1;

}

static inline void epaMTV(const physicsBody *body1, const physicsBody *body2, const vec3 *a, const vec3 *b, const vec3 *c, const vec3 *d, vec3 *mtv){
	//
}

signed char gjkCollision(const physicsBody *body1, const physicsBody *body2, vec3 *mtv){

	/*
	** Implementation of the Gilbert-Johnson-Keerthi distance algorithm,
	** which is only compatible with convex polytopes. Also uses the
	** expanding polytope algorithm to solve for the MTV (Minimum
	** Translation Vector) if a non-NULL mtv pointer is passed in.
	*/

	// Our simplex vertices, that will eventually form a tetrahedron
	// enclosing the origin if the two polygons are colliding.
	vec3 a, b, c, d;
	vec3SetS(&d, 0.f);  // Initialize d to prevent compiler warnings. Not totally necessary.

	// Set the current number of vertices of our simplex. We will be
	// creating a line segment with vertices B and C before the main
	// loop, so we can set this to 2 preemptively.
	unsigned char simplexVertices = 2;

	// The first direction to check in is the direction of body2 from body1.
	vec3 axis;
	vec3SubVFromVR(&body2->position.value, &body1->position.value, &axis);

	// Create an initial vertex for the simplex.
	gjkSupport(body1, body2, &axis, &c);

	// Create another vertex to form a line segment. B should be
	// a vertex in the opposite direction of C from the origin.
	axis.x = -c.x;
	axis.y = -c.y;
	axis.z = -c.z;
	gjkSupport(body1, body2, &axis, &b);

	// If the origin was not crossed, the Minkowski space is
	// entirely on one side of the origin, so the two
	// polygons cannot be overlapping.
	if(vec3Dot(&b, &axis) < 0){
		return 0;
	}else{
		const vec3 BO = {.x = -b.x,
		                 .y = -b.y,
		                 .z = -b.z};
		const vec3 BC = {.x = c.x - b.x,
		                 .y = c.y - b.y,
		                 .z = c.z - b.z};
		vec3 tempCross;
		// Set the new search axis to the axis perpendicular
		// to the line segment BC, towards the origin.
		vec3Cross(&BC, &BO, &tempCross);
		vec3Cross(&tempCross, &BC, &axis);
		if(axis.x == 0.f && axis.y == 0.f && axis.z == 0.f){
			// The origin is on the line segment BC.
			vec3Set(&tempCross, 1.f, 0.f, 0.f);
			vec3Cross(&BC, &tempCross, &axis);
			if(axis.x == 0.f && axis.y == 0.f && axis.z == 0.f){
				vec3Set(&tempCross, 0.f, 0.f, -1.f);
				vec3Cross(&BC, &tempCross, &axis);
			}
		}
	}

	size_t i;
	for(i = 0; i < GJK_MAX_ITERATIONS; ++i){

		// Add a new vertex to our simplex.
		gjkSupport(body1, body2, &axis, &a);
		if(vec3Dot(&a, &axis) < 0){
			// If the new vertex has not crossed the origin on the given axis,
			// the origin cannot lie within the "Minkowski difference" of the
			// polygons. Therefore, the two polygons are not colliding.
			return 0;
		}

		// Check if the origin is enclosed in our simplex.
		if(++simplexVertices == 3){
			// We only have a triangle right now, find which
			// direction to search for our next vertex in.
			gjkTriangle(&simplexVertices, &a, &b, &c, &d, &axis);
		}else if(gjkTetrahedron(&simplexVertices, &a, &b, &c, &d, &axis)){
			// The origin lies within our simplex. If an mtv pointer
			// was specified, calculate the MTV, otherwise return.
			if(mtv != NULL){
				epaMTV(body1, body2, &a, &b, &c, &d, mtv);
			}
			return 1;
		}

	}

	return 0;

}
