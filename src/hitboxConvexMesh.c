#include "hitboxConvexMesh.h"
#include "hitboxCollision.h"
#include "engineMath.h"
#include <string.h>
#include <math.h>

#define COLLISION_MAX_ITERATIONS 64
#define COLLISION_MAX_FACE_NUM_EPA 64
#define COLLISION_MAX_EDGE_NUM_EPA 32

signed char hbMeshWavefrontObjLoad(hbMesh *hbm, const char *filePath);

void hbMeshInit(hbMesh *hbm){
	hbm->vertexNum = 0;
	hbm->vertices = NULL;
	hbm->indexNum = 0;
	hbm->indices = NULL;
}

static size_t hbMeshCollisionGetFarthestVertexIndex(const hbMesh *c, const vec3 *axis){
	/*
	** Finds the vertex in c that is farthest in
	** the direction of axis by projecting each
	** vertex onto axis.
	*/
	size_t r = 0;
	float max = vec3Dot(&c->vertices[0], axis);
	for(size_t i = 1; i < c->vertexNum; ++i){
		float s = vec3Dot(&c->vertices[i], axis);
		if(s > max){
			max = s;
			r = i;
		}
	}
	return r;
}

static void hbMeshCollisionMinkowskiSupport(const hbMesh *c1, const hbMesh *c2, const vec3 *axis, hbMeshSupportVertex *r){
	/*
	** Returns a point in Minkowski space on the edge of
	** the polygons' "Minkowski difference".
	*/
	// For the first polygon, find the vertex that is
	// farthest in the direction of axis. Use a
	// negative axis for the second polygon; this will
	// give us the two closest vertices along a given
	// axis.
	vec3 axisNegative;
	axisNegative.x = -axis->x;
	axisNegative.y = -axis->y;
	axisNegative.z = -axis->z;
	r->i1 = hbMeshCollisionGetFarthestVertexIndex(c1, axis);
	r->i2 = hbMeshCollisionGetFarthestVertexIndex(c2, &axisNegative);
	// Get the "Minkowski Difference" of the two vertices. r = v1 - v2
	vec3SubVFromVR(&c1->vertices[r->i1], &c2->vertices[r->i2], &r->v);
}

static inline void hbMeshCollisionGJKTriangle(unsigned char *simplexVertices, hbMeshSupportVertex simplex[4], vec3 *axis){

	const vec3 AO = {.x = -simplex[0].v.x,
	                 .y = -simplex[0].v.y,
	                 .z = -simplex[0].v.z};
	const vec3 AB = {.x = simplex[1].v.x - simplex[0].v.x,
	                 .y = simplex[1].v.y - simplex[0].v.y,
	                 .z = simplex[1].v.z - simplex[0].v.z};
	const vec3 AC = {.x = simplex[2].v.x - simplex[0].v.x,
	                 .y = simplex[2].v.y - simplex[0].v.y,
	                 .z = simplex[2].v.z - simplex[0].v.z};
	vec3 ABC;
	vec3Cross(&AB, &AC, &ABC);
	vec3 tempCross;

	vec3Cross(&AB, &ABC, &tempCross);
	if(vec3Dot(&tempCross, &AO) > 0.f){

		// AB x ABC does not cross the origin, so we need a
		// new simplex fragment. Change the direction and
		// remove vertex C by replacing it with vertex A.
		vec3Cross(&AB, &AO, &tempCross);
		vec3Cross(&tempCross, &AB, axis);
		simplex[2] = simplex[0];

	}else{

		vec3Cross(&ABC, &AC, &tempCross);
		if(vec3Dot(&tempCross, &AO) > 0.f){

			// ABC x AC does not cross the origin, so we need a
			// new simplex fragment. Change the direction and
			// remove vertex C by replacing it with vertex A.
			vec3Cross(&AC, &AO, &tempCross);
			vec3Cross(&tempCross, &AC, axis);
			simplex[1] = simplex[0];

		}else{

			// Checks have passed, a tetrahedron can be generated.
			if(vec3Dot(&ABC, &AO) > 0.f){
				// Create the base of the tetrahedron and set
				// dir to the direction from the base to the
				// origin.
				simplex[3] = simplex[2];
				simplex[2] = simplex[1];
				simplex[1] = simplex[0];
				*axis = ABC;
			}else{
				// The tetrahedron will be upside down, compensate.
				simplex[3] = simplex[1];
				simplex[1] = simplex[0];
				axis->x = -ABC.x;
				axis->y = -ABC.y;
				axis->z = -ABC.z;
			}

			*simplexVertices = 4;

		}

	}

}

static inline signed char hbMeshCollisionGJKTetrahedron(unsigned char *simplexVertices, hbMeshSupportVertex *simplex, vec3 *axis){

	// Check if the normal of ABC is crossing the origin.
	const vec3 AO = {.x = -simplex[0].v.x,
	                 .y = -simplex[0].v.y,
	                 .z = -simplex[0].v.z};
	const vec3 AB = {.x = simplex[1].v.x - simplex[0].v.x,
	                 .y = simplex[1].v.y - simplex[0].v.y,
	                 .z = simplex[1].v.z - simplex[0].v.z};
	const vec3 AC = {.x = simplex[2].v.x - simplex[0].v.x,
	                 .y = simplex[2].v.y - simplex[0].v.y,
	                 .z = simplex[2].v.z - simplex[0].v.z};
	vec3 tempCross;
	vec3Cross(&AB, &AC, &tempCross);
	if(vec3Dot(&tempCross, &AO) > 0.f){
		simplex[3] = simplex[2];
		simplex[2] = simplex[1];
		simplex[1] = simplex[0];
		*axis = tempCross;
		return 0;
	}

	// Check if the normal of ACD is crossing the origin.
	const vec3 AD = {.x = simplex[3].v.x - simplex[0].v.x,
	                 .y = simplex[3].v.y - simplex[0].v.y,
	                 .z = simplex[3].v.z - simplex[0].v.z};
	vec3Cross(&AC, &AD, &tempCross);
	if(vec3Dot(&tempCross, &AO) > 0.f){
		simplex[1] = simplex[0];
		*axis = tempCross;
		return 0;
	}

	// Check if the normal of ADB is crossing the origin.
	vec3Cross(&AD, &AB, &tempCross);
	if(vec3Dot(&tempCross, &AO) > 0.f){
		simplex[2] = simplex[3];
		simplex[3] = simplex[1];
		simplex[1] = simplex[0];
		*axis = tempCross;
		return 0;
	}

	return 1;

}

signed char hbMeshCollisionGJK(const hbMesh *c1, const vec3 *c1c, const hbMesh *c2, const vec3 *c2c, hbMeshSupportVertex *simplex){

	/*
	** Implementation of the Gilbert-Johnson-Keerthi distance algorithm,
	** which is only compatible with convex polytopes.
	*/

	size_t i;

	// Set the current number of vertices of our simplex. We will be
	// creating a line segment with vertices B and C before the main
	// loop, so we can set this to 2 preemptively.
	unsigned char simplexVertices = 2;

	// The first direction to check in is the direction of body2 from body1.
	vec3 axis;
	vec3SubVFromVR(c2c, c1c, &axis);

	// Create an initial vertex for the simplex.
	hbMeshCollisionMinkowskiSupport(c1, c2, &axis, &simplex[2]);

	// Create another vertex to form a line segment. B should be
	// a vertex in the opposite direction of C from the origin.
	axis.x = -simplex[2].v.x;
	axis.y = -simplex[2].v.y;
	axis.z = -simplex[2].v.z;
	hbMeshCollisionMinkowskiSupport(c1, c2, &axis, &simplex[1]);

	// If the origin was not crossed, the Minkowski space is
	// entirely on one side of the origin, so the two
	// polygons cannot be overlapping.
	if(vec3Dot(&simplex[1].v, &axis) < 0.f){
		return 0;
	}else{
		const vec3 BO = {.x = -simplex[1].v.x,
		                 .y = -simplex[1].v.y,
		                 .z = -simplex[1].v.z};
		const vec3 BC = {.x = simplex[2].v.x - simplex[1].v.x,
		                 .y = simplex[2].v.y - simplex[1].v.y,
		                 .z = simplex[2].v.z - simplex[1].v.z};
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

	for(i = 0; i < COLLISION_MAX_ITERATIONS; ++i){

		// Add a new vertex to our simplex.
		hbMeshCollisionMinkowskiSupport(c1, c2, &axis, &simplex[0]);
		if(vec3Dot(&simplex[0].v, &axis) <= 0.f){
			// If the new vertex has not crossed the origin on the given axis,
			// the origin cannot lie within the "Minkowski difference" of the
			// polygons. Therefore, the two polygons are not colliding.
			return 0;
		}

		// Check if the origin is enclosed in our simplex.
		if(++simplexVertices == 3){
			// We only have a triangle right now, find which
			// direction to search for our next vertex in.
			hbMeshCollisionGJKTriangle(&simplexVertices, simplex, &axis);
		}else if(hbMeshCollisionGJKTetrahedron(&simplexVertices, simplex, &axis)){
			// The origin lies within our simplex, so we know there was
			// a collision.
			return 1;
		}

	}

	return 0;

}

signed char hbMeshCollisionMPR(const hbMesh *c1, const vec3 *c1c, const hbMesh *c2, const vec3 *c2c, hbMeshSupportVertex *simplex){

	/*
	** Implementation of the Minkowski Portal Refinement algorithm,
	** which is only compatible with convex polytopes. Somewhat
	** similar to the Gilbert-Johnson-Keerthi distance algorithm.
	**
	** http://xenocollide.snethen.com/mpr2d.html
	*/

	size_t i;
	vec3 originRay, axis;
	vec3 vecSubVec1, vecSubVec2;
	hbMeshSupportVertex tempVertex;

	/*
	** Phase 1: Portal Discovery.
	** A portal is a plane between three support points that is
	** crossed by the origin ray, a vector from a point deep
	** within both bodies to the origin.
	*/
	// Obtain a point that lies inside both meshes by subtracting
	// c2's centroid from c1's.
	vec3SubVFromVR(c2c, c1c, &simplex[0].v);

	// The origin ray is the vector from this point to the origin.
	originRay.x = -simplex[0].v.x;
	originRay.y = -simplex[0].v.y;
	originRay.z = -simplex[0].v.z;

	// Find a new vertex in the direction of the origin ray.
	hbMeshCollisionMinkowskiSupport(c1, c2, &originRay, &simplex[1]);
	// If the origin was not crossed, the Minkowski space is
	// entirely on one side of the origin, so the two
	// polygons cannot be overlapping.
	if(vec3Dot(&simplex[1].v, &originRay) <= 0.f){
		return 0;
	}

	// Find a new vertex by searching in the direction of the
	// normal of the plane containing v0, v1 and the origin.
	vec3Cross(&simplex[1].v, &simplex[0].v, &axis);
	hbMeshCollisionMinkowskiSupport(c1, c2, &axis, &simplex[2]);
	// If the origin was not crossed, the Minkowski space is
	// entirely on one side of the origin, so the two
	// polygons cannot be overlapping.
	if(vec3Dot(&simplex[2].v, &axis) <= 0.f){
		return 0;
	}

	// Find a new vertex by searching in the direction of the
	// normal of the portal plane containing v0, v1 and v2.
	while(1){

		vec3SubVFromVR(&simplex[1].v, &originRay, &vecSubVec1);
		vec3SubVFromVR(&simplex[2].v, &originRay, &vecSubVec2);
		vec3Cross(&vecSubVec1, &vecSubVec2, &axis);

		// Find a new vertex in the direction of axis.
		hbMeshCollisionMinkowskiSupport(c1, c2, &axis, &simplex[3]);
		// If the origin was not crossed, the Minkowski space is
		// entirely on one side of the origin, so the two
		// polygons cannot be overlapping.
		if(vec3Dot(&simplex[3].v, &axis) <= 0.f){
			return 0;
		}

		// If the origin is on the other side of the portal plane
		// containing v0, v1 and v2, search in the opposite direction.
		if(vec3Dot(&axis, &originRay) < 0.f){
			const hbMeshSupportVertex temp = simplex[1];
			simplex[1] = simplex[2];
			simplex[2] = temp;
		}else{
			// If the origin is on the other side of the portal plane
			// containing v0, v2 and v3, search in the opposite direction.
			vec3Cross(&simplex[3].v, &simplex[2].v, &axis);
			if(vec3Dot(&axis, &simplex[0].v) < 0.f){
				simplex[1] = simplex[3];
			}else{
				// If the origin is on the other side of the portal plane
				// containing v0, v3 and v1, search in the opposite direction.
				vec3Cross(&simplex[1].v, &simplex[3].v, &axis);
				if(vec3Dot(&axis, &simplex[0].v) < 0.f){
					simplex[2] = simplex[3];
				}else{
					break;
				}
			}
		}

	}

	/*
	** Phase 2: Portal Refinement.
	*/
	for(i = 0; i < COLLISION_MAX_ITERATIONS; ++i){

		// If the origin is on the same side of the portal plane
		// as the interior point v0, we have found our simplex.
		vec3SubVFromVR(&simplex[2].v, &simplex[1].v, &vecSubVec1);
		vec3SubVFromVR(&simplex[3].v, &simplex[1].v, &vecSubVec2);
		vec3Cross(&vecSubVec1, &vecSubVec2, &axis);
		if(vec3Dot(&axis, &simplex[1].v) >= 0.f){
			return 1;
		}

		// Find a vertex in the direction of the portal plane's normal.
		hbMeshCollisionMinkowskiSupport(c1, c2, &axis, &tempVertex);
		vec3NormalizeFast(&axis);

		// If the origin is on the other side of the new support plane,
		// it is outside of the Minkowski difference.
		// Additionally, check the distance from the portal plane to the
		// new support plane to prevent infinite loops.
		vec3SubVFromVR(&tempVertex.v, &simplex[3].v, &vecSubVec1);
		if(vec3Dot(&tempVertex.v, &axis) <= 0.f || vec3Dot(&vecSubVec1, &axis) <= COLLISION_DISTANCE_THRESHOLD){
			return 0;
		}

		vec3Cross(&tempVertex.v, &simplex[1].v, &axis);
		if(vec3Dot(&axis, &simplex[0].v) < 0.f){
			vec3Cross(&tempVertex.v, &simplex[2].v, &axis);
			if(vec3Dot(&axis, &simplex[0].v) < 0.f){
				// The new portal is between vertices 2, 3 and 4 (tempVertex).
				simplex[1] = tempVertex;
			}else{
				// The new portal is between vertices 1, 2 and 4 (tempVertex).
				simplex[3] = tempVertex;
			}
		}else{
			vec3Cross(&tempVertex.v, &simplex[3].v, &axis);
			if(vec3Dot(&axis, &simplex[0].v) < 0.f){
				// The new portal is between vertices 2, 3 and 4 (tempVertex).
				simplex[2] = tempVertex;
			}else{
				// The new portal is between vertices 1, 2 and 4 (tempVertex).
				simplex[1] = tempVertex;
			}
		}

	}

	return 0;

}

typedef struct {
	// Three vertices and a normal.
	hbMeshSupportVertex vertex[3];
	vec3 normal;
} hbMeshEPAFaceHelper;

static inline void hbMeshEPAFaceInit(hbMeshEPAFaceHelper *face, const hbMeshSupportVertex *v0, const hbMeshSupportVertex *v1, const hbMeshSupportVertex *v2){
	// Create a new face.
	face->vertex[0] = *v0;
	face->vertex[1] = *v1;
	face->vertex[2] = *v2;
	faceNormal(&v0->v, &v1->v, &v2->v, &face->normal);
	vec3NormalizeFast(&face->normal);
}

typedef struct {
	// An edge that will be removed.
	hbMeshSupportVertex vertex[2];
} hbMeshEPAEdgeHelper;

static inline void hbMeshEPAEdgeInit(hbMeshEPAEdgeHelper *edge, const hbMeshSupportVertex *v0, const hbMeshSupportVertex *v1){
	// Create a new face.
	edge->vertex[0] = *v0;
	edge->vertex[1] = *v1;
}

void hbMeshCollisionEPA(const hbMesh *c1, const hbMesh *c2, hbMeshSupportVertex *simplex, hbCollisionData *cd){

	/*
	** Implementation of the expanding polytope algorithm. Extrapolates
	** additional collision information from two convex polygons and the
	** simplex generated by the Gilbert-Johnson-Keerthi algorithm.
	*/

	size_t i, j, k, l;
	size_t closestFace;
	float distance;
	hbMeshSupportVertex point;

	// Create the face and edge arrays.
	size_t faceNum = 4;
	hbMeshEPAFaceHelper faces[COLLISION_MAX_FACE_NUM_EPA];
	size_t edgeNum = 0;
	hbMeshEPAEdgeHelper edges[COLLISION_MAX_EDGE_NUM_EPA];

	// Ensure the vertex winding order of the simplex is CCW.
	vec3 temp1, temp2, temp3;
	vec3SubVFromVR(&simplex[1].v, &simplex[3].v, &temp1);
	vec3SubVFromVR(&simplex[2].v, &simplex[3].v, &temp2);
	vec3Cross(&temp1, &temp2, &temp3);
	vec3SubVFromVR(&simplex[0].v, &simplex[3].v, &temp1);
	if(vec3Dot(&temp1, &temp3) < 0.f){
		// If it's not, swap two vertices.
		hbMeshSupportVertex swap = simplex[0];
		simplex[0] = simplex[1];
		simplex[1] = swap;
	}

	// Generate a starting tetrahedron from the given vertices.
	hbMeshEPAFaceInit(&faces[0], &simplex[0], &simplex[1], &simplex[2]);  // Face 1 - ABC
	hbMeshEPAFaceInit(&faces[1], &simplex[0], &simplex[2], &simplex[3]);  // Face 2 - ACD
	hbMeshEPAFaceInit(&faces[2], &simplex[0], &simplex[3], &simplex[1]);  // Face 3 - ADB
	hbMeshEPAFaceInit(&faces[3], &simplex[1], &simplex[3], &simplex[2]);  // Face 4 - BDC

	/*
	** Find the edge on the Minkowski difference closest to the origin.
	*/
	for(i = 0; i < COLLISION_MAX_ITERATIONS; ++i){

		/*
		** Find the closest face to the origin.
		*/
		closestFace = 0;
		distance = vec3Dot(&faces[0].vertex[0].v, &faces[0].normal);
		for(j = 1; j < faceNum; ++j){

			// Get the minimum distance between the current face and the origin.
			// This is the dot product between the face's normal and one of its vertices.
			const float tempDistance = vec3Dot(&faces[j].vertex[0].v, &faces[j].normal);

			if(tempDistance < distance){
				// New closest face found.
				closestFace = j;
				distance = tempDistance;
			}

		}

		/*
		** Search in the direction of the closest face to find
		** a point on the edge of the Minkowski difference.
		*/
		hbMeshCollisionMinkowskiSupport(c1, c2, &faces[closestFace].normal, &point);
		if(vec3Dot(&point.v, &faces[closestFace].normal) - distance < COLLISION_DISTANCE_THRESHOLD){
			// The new point is not much further from the origin, break from the loop.
			break;
		}

		/*
		** For each face, if it is facing the new point, remove it and
		** create some new faces to join the new point to the simplex.
		*/
		for(j = 0; j < faceNum; ++j){

			// Check if the face's normal is pointing in the direction of the new point.
			// If it is, the face can be "seen" by the new point.
			vec3 dif;
			vec3SubVFromVR(&point.v, &faces[j].vertex[0].v, &dif);
			if(vec3Dot(&faces[j].normal, &dif) > 0.f){

				// Add each of the face's edges to the edge array,
				// ready to be removed in the next section.
				for(k = 0; k < 3; ++k){

					signed char add = 1;
					hbMeshEPAEdgeHelper newEdge;
					hbMeshEPAEdgeInit(&newEdge, &faces[j].vertex[k], &faces[j].vertex[(k+1)%3]);

					// Check if the new edge has already been added to the edge array.
					// An edge should never appear more than twice, so if it has already
					// been added both instances may be removed.
					for(l = 0; l < edgeNum; ++l){

						// Because a consistent CCW winding order is used, any additional
						// occurrence of the edge will have reversed vertex indices.
						if(memcmp(&newEdge.vertex[0], &edges[l].vertex[1], sizeof(vec3)) == 0 &&
						   memcmp(&newEdge.vertex[1], &edges[l].vertex[0], sizeof(vec3)) == 0){

							// Replace this edge with the last edge in the edge array
							// in order to remove it.
							--edgeNum;
							edges[l] = edges[edgeNum];
							add = 0;
							break;

						}
					}

					// If the new edge did not already exist, add it to the edge array.
					if(add){
						if(edgeNum >= COLLISION_MAX_EDGE_NUM_EPA){
							break;
						}
						edges[edgeNum] = newEdge;
						++edgeNum;
					}
				}

				// Replace this face with the last face in the face array
				// in order to remove it.
				--faceNum;
				faces[j] = faces[faceNum];
				--j;

			}

		}

		// Patch the holes in the polytope using the new point.
		for(j = 0; j < edgeNum; ++j){

			if(faceNum >= COLLISION_MAX_FACE_NUM_EPA){
				break;
			}

			hbMeshEPAFaceInit(&faces[faceNum], &edges[j].vertex[0], &edges[j].vertex[1], &point);

			// Ensure the vertex winding order of the simplex is CCW.
			if(vec3Dot(&faces[faceNum].vertex[0].v, &faces[faceNum].normal) < 0.f){
				// If it's not, swap two vertices.
				hbMeshSupportVertex swap = faces[faceNum].vertex[0];
				faces[faceNum].vertex[0] = faces[faceNum].vertex[1];
				faces[faceNum].vertex[1] = swap;
				// Invert the face's normal as well.
				faces[faceNum].normal.x = -faces[faceNum].normal.x;
				faces[faceNum].normal.y = -faces[faceNum].normal.y;
				faces[faceNum].normal.z = -faces[faceNum].normal.z;
			}

			++faceNum;

		}
		edgeNum = 0;

	}

	/*
	** Now we can extrapolate various collision information
	** using face, normal and distance.
	*/
	// Store the contact normal and penetration depth.
	cd->contactNormal = faces[closestFace].normal;
	cd->penetrationDepth = distance;

	// Project the origin onto the closest face.
	vec3 contact = cd->contactNormal;
	vec3MultVByS(&contact, distance);

	// Calculate the Barycentric coordinates of the projected origin.
	barycentric(&faces[closestFace].vertex[0].v, &faces[closestFace].vertex[1].v, &faces[closestFace].vertex[2].v, &contact, &contact);

	// The contact point on c1 is the linear combination of the original
	// vertices in c1 used to generate the support vertices for the
	// closest face, using the barycentric coordinates stored in contact.
	vec3CombineLinear(&c1->vertices[faces[closestFace].vertex[0].i1],
	                  &c1->vertices[faces[closestFace].vertex[1].i1],
	                  &c1->vertices[faces[closestFace].vertex[2].i1],
	                  contact.x, contact.y, contact.z, &cd->contactPointA);

	// The contact point on c2 is the linear combination of the original
	// vertices in c2 used to generate the support vertices for the
	// closest face, using the barycentric coordinates stored in contact.
	vec3CombineLinear(&c2->vertices[faces[closestFace].vertex[0].i2],
	                  &c2->vertices[faces[closestFace].vertex[1].i2],
	                  &c2->vertices[faces[closestFace].vertex[2].i2],
	                  contact.x, contact.y, contact.z, &cd->contactPointB);

	// Generate the contact tangents, perpendicular to each other and the contact normal.
	if(fabsf(cd->contactNormal.x) >= 0.57735f){
		vec3Set(&cd->contactTangentA, cd->contactNormal.y, -cd->contactNormal.x, 0.f);
	}else{
		vec3Set(&cd->contactTangentA, 0.f, cd->contactNormal.z, -cd->contactNormal.y);
	}
	vec3NormalizeFast(&cd->contactTangentA);
	vec3Cross(&cd->contactNormal, &cd->contactTangentA, &cd->contactTangentB);

}

void hbMeshDelete(hbMesh *hbm){
	if(hbm->vertices != NULL){
		free(hbm->vertices);
	}
	if(hbm->indices != NULL){
		free(hbm->indices);
	}
}
