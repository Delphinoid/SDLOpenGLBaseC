#include "hitboxConvexMesh.h"
#include "engineMath.h"
#include <math.h>
#include <string.h>

#define COLLISION_DISTANCE_THRESHOLD 0.0001f

signed char hbMeshWavefrontObjLoad(hbMesh *hbm, const char *filePath);

void hbMeshInit(hbMesh *hbm){
	hbm->vertexNum = 0;
	hbm->vertices = NULL;
	hbm->indexNum = 0;
	hbm->indices = NULL;
}

signed char hbMeshLoad(hbMesh *hbm, const char *prgPath, const char *filePath){

	hbMeshInit(hbm);

	size_t pathLen = strlen(prgPath);
	size_t fileLen = strlen(filePath);
	char *fullPath = malloc((pathLen+fileLen+1)*sizeof(char));
	if(fullPath == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	memcpy(fullPath, prgPath, pathLen);
	memcpy(fullPath+pathLen, filePath, fileLen);
	fullPath[pathLen+fileLen] = '\0';

	/** Replace and move the loading function here. **/
	if(!hbMeshWavefrontObjLoad(hbm, fullPath)){
		free(fullPath);
		return 0;
	}
	free(fullPath);

	return 1;

}

static size_t hbMeshCollisionGetFarthestVertexIndex(const hbMesh *c, const vec3 *axis){
	/*
	** Finds the vertex in mesh that is farthest in
	** the direction of axis by projecting each
	** vertex onto the axis.
	*/
	/** Generate a world state for each vertex? Not here actually, that makes no sense. **/
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

static void hbMeshCollisionMinkowskiSupport(const hbMesh *c1, const hbMesh *c2, const vec3 *axis, hbSupportVertex *r){
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

static inline void hbMeshCollisionGJKTriangle(unsigned char *simplexVertices, hbSupportVertex simplex[4], vec3 *axis){

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

static inline signed char hbMeshCollisionGJKTetrahedron(unsigned char *simplexVertices, hbSupportVertex *simplex, vec3 *axis){

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

signed char hbMeshCollisionGJK(const hbMesh *c1, const vec3 *c1c, const hbMesh *c2, const vec3 *c2c, hbSupportVertex *simplex){

	/*
	** Implementation of the Gilbert-Johnson-Keerthi distance algorithm,
	** which is only compatible with convex polytopes.
	*/

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
	if(vec3Dot(&simplex[1].v, &axis) <= 0.f){
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

	while(1){

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

}

signed char hbMeshCollisionMPR(const hbMesh *c1, const vec3 *c1c, const hbMesh *c2, const vec3 *c2c, hbSupportVertex *simplex){

	/*
	** Implementation of the Minkowski Portal Refinement algorithm,
	** which is only compatible with convex polytopes. Somewhat
	** similar to the Gilbert-Johnson-Keerthi distance algorithm.
	**
	** http://xenocollide.snethen.com/mpr2d.html
	*/

	vec3 originRay, axis;
	vec3 vecSubVec1, vecSubVec2;
	hbSupportVertex tempVertex;

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
			const hbSupportVertex temp = simplex[1];
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
	while(1){

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

}

signed char hbMeshCollisionEPA(const hbMesh *c1, const hbMesh *c2, const hbSupportVertex *simplex, hbMeshCollisionData *cd){

	/*
	** Implementation of the expanding polytope algorithm. Extrapolates
	** additional collision information from two convex polygons and the
	** simplex generated by the Gilbert-Johnson-Keerthi algorithm.
	*/

	// Create dynamic arrays for the polytope.
	size_t vertexNum = 4;
	size_t vertexCapacity = 8;
	hbSupportVertex *vertices = malloc(vertexCapacity*sizeof(hbSupportVertex));
	if(vertices == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	size_t indexNum = 12;
	size_t indexCapacity = 48;
	size_t *indices = malloc(indexCapacity*sizeof(size_t));
	if(indices == NULL){
		/** Memory allocation failure. **/
		free(vertices);
		return 0;
	}

	// Ensure the vertex winding order of the simplex is CCW.
	vec3 temp1, temp2, temp3;
	vec3SubVFromVR(&simplex[1].v, &simplex[3].v, &temp1);
	vec3SubVFromVR(&simplex[2].v, &simplex[3].v, &temp2);
	vec3Cross(&temp1, &temp2, &temp3);
	vec3SubVFromVR(&simplex[0].v, &simplex[3].v, &temp1);
	if(vec3Dot(&temp1, &temp3) > 0.f){
		// If it's not, swap two vertices.
		vertices[0] = simplex[1];
		vertices[1] = simplex[0];
	}else{
		vertices[0] = simplex[0];
		vertices[1] = simplex[1];
	}

	// Start with the given tetrahedron.
	vertices[2] = simplex[2];
	vertices[3] = simplex[3];
	indices[0] = 0; indices[1]  = 1; indices[2]  = 2;  // Face 1
	indices[3] = 0; indices[4]  = 1; indices[5]  = 3;  // Face 2
	indices[6] = 0; indices[7]  = 2; indices[8]  = 3;  // Face 3
	indices[9] = 1; indices[10] = 2; indices[11] = 3;  // Face 4

	size_t face = 0;
	vec3 normal;
	float distance = 0.f;

	while(1){

		float newDistance = 1.f/0.f;  // INFINITY

		/*
		** Find the closest face to the origin.
		*/
		size_t i;
		for(i = 0; i < indexNum; i+=3){
			// Get the minimum distance between the current face and the origin.
			// This is the dot product between the face's normal and one of its vertices.
			vec3 tempNormal;
			float tempDistance;
			faceNormal(&vertices[indices[i]].v, &vertices[indices[i+1]].v, &vertices[indices[i+2]].v, &tempNormal);
			vec3NormalizeFast(&tempNormal);
			tempDistance = vec3Dot(&tempNormal, &vertices[indices[i]].v);
			if(tempDistance < newDistance){
				face = i;
				normal = tempNormal;
				newDistance = tempDistance;
            }
		}

		/*
		** If the closest face is not much closer to the origin than the last face OR it is on
		** the edge of the Minkowski difference (it is part of the closest face), break from
		** the loop, otherwise remove each face that can be "seen" by this point and add new
		** ones to patch the gap in the polytope.
		*/
		if(newDistance - distance < COLLISION_DISTANCE_THRESHOLD){
			break;
		}else{

			distance = newDistance;

			// Find the closest vertex of the Minkowski difference in the direction of normal.
			hbSupportVertex point;
			hbMeshCollisionMinkowskiSupport(c1, c2, &normal, &point);

			// If the new point is part of the closest face, break from the loop.
			if(memcmp(&point.v, &vertices[indices[face]].v,   sizeof(vec3)) == 0 ||
			   memcmp(&point.v, &vertices[indices[face+1]].v, sizeof(vec3)) == 0 ||
			   memcmp(&point.v, &vertices[indices[face+2]].v, sizeof(vec3)) == 0){
				break;
			}

			// Resize the vertex array if necessary and add the new point.
			if(vertexNum >= vertexCapacity){
				vertexCapacity *= 2;
				hbSupportVertex *tempBuffer = realloc(vertices, vertexCapacity*sizeof(hbSupportVertex));
				if(tempBuffer == NULL){
					/** Memory allocation failure. **/
					free(indices);
					free(vertices);
					return 0;
				}
				vertices = tempBuffer;
			}
			vertices[vertexNum] = point;

			/*
			** For each face, remove those that can be "seen" by the new point
			** and patch the hole they leave using the new point.
			*/
			size_t oldIndexNum = indexNum;
			for(i = 0; i < oldIndexNum; i+=3){
				// Don't risk trying to work on a face that we've just added.
				if(indices[i+2] != vertexNum){

					// Check if the face's normal is pointing in the direction of the new point.
					// If it is, the face can be "seen" by the new point.
					vec3 dif;
					vec3SubVFromVR(&point.v, &vertices[indices[i]].v, &dif);
					if(vec3Dot(&normal, &dif) > 0){

						/*
						** Remove the current face and construct three new faces using each edge and the new point.
						** For point p and a CCW-winding face with vertices a, b and c, we construct the following faces:
						** a b p
						** b c p
						** c a p
						*/

						size_t currentFace[3] = {indices[i], indices[i+1], indices[i+2]};
						unsigned char remove = 0;
						signed char firstFace = 1;
						signed char secondFace = 1;
						signed char thirdFace = 1;

						// Loop forwards through each face, overwriting any faces that have the same indices as one
						// of three faces we're constructing, as well as indices[face].
						size_t j;
						for(j = 0; j < indexNum; j+=3){
							if(j == i){
								// Remove indices[i].
								oldIndexNum -= 3;
								remove += 3;
								continue;
							}
							if(indices[j+2] == vertexNum){
								// If the last vertex is p, we know there may be a face shared.
								// Check if the other two vertices are the same.
								// a
								if(indices[j] == indices[i]){
									// a b p
									if(indices[j+1] == indices[i+1]){
										firstFace = 0;
										remove += 3;
										continue;
									// a c p
									}else if(indices[j+1] == indices[i+2]){
										thirdFace = 0;
										remove += 3;
										continue;
									}
								// b
								}else if(indices[j] == indices[i+1]){
									// b a p
									if(indices[j+1] == indices[i]){
										firstFace = 0;
										remove += 3;
										continue;
									// b c p
									}else if(indices[j+1] == indices[i+2]){
										secondFace = 0;
										remove += 3;
										continue;
									}

								// c
								}else if(indices[j] == indices[i+2]){
									// c a p
									if(indices[j+1] == indices[i]){
										thirdFace = 0;
										remove += 3;
										continue;
									// c b p
									}else if(indices[j+1] == indices[i+1]){
										secondFace = 0;
										remove += 3;
										continue;
									}
								}
							}
							// If the current face is not to be overwritten, move it
							// back by 'remove' elements.
							memcpy(&indices[j-remove], &indices[j], 3*sizeof(size_t));
						}
						// Decrease indexNum by the number of elements we removed.
						indexNum -= remove;
						i -= remove;

						// Only index[i] was removed, so we potentially need to resize the indices array.
						if(remove == 3){
							// Resize the index array if necessary.
							if(indexNum+9 > indexCapacity){
								indexCapacity *= 2;
								size_t *tempBuffer = realloc(indices, indexCapacity*sizeof(size_t));
								if(tempBuffer == NULL){
									/** Memory allocation failure. **/
									free(indices);
									free(vertices);
									return 0;
								}
								indices = tempBuffer;
							}
						}

						// Add each new face.
						if(firstFace){
							// a b p
							indices[indexNum] = currentFace[0];
							++indexNum;
							indices[indexNum] = currentFace[1];
							++indexNum;
							indices[indexNum] = vertexNum;
							++indexNum;
						}
						if(secondFace){
							// b c p
							indices[indexNum] = currentFace[1];
							++indexNum;
							indices[indexNum] = currentFace[2];
							++indexNum;
							indices[indexNum] = vertexNum;
							++indexNum;
						}
						if(thirdFace){
							// c a p
							indices[indexNum] = currentFace[2];
							++indexNum;
							indices[indexNum] = currentFace[0];
							++indexNum;
							indices[indexNum] = vertexNum;
							++indexNum;
						}

					}

				}

				// Finally increase vertexNum, since we didn't do it before.
				++vertexNum;
			}
		}

	}


	/*
	** Now we can extrapolate various collision information
	** using face, normal and distance.
	*/

	// Store the contact normal and penetration depth.
	cd->contactNormal = normal;
	cd->penetrationDepth = distance;

	// Project the origin onto the closest face.
	vec3 contact = normal;
	vec3MultVByS(&contact, distance);
	// Calculate the Barycentric coordinates of the projected origin.
	barycentric(&vertices[indices[face]].v, &vertices[indices[face+1]].v, &vertices[indices[face+2]].v, &contact, &contact);
	// Generate the contact point on c1.
	cd->contactPointA.x = c1->vertices[vertices[indices[face]].i1].x   * contact.z +
	                      c1->vertices[vertices[indices[face+1]].i1].x * contact.x +
	                      c1->vertices[vertices[indices[face+2]].i1].x * contact.y;
	cd->contactPointA.y = c1->vertices[vertices[indices[face]].i1].y   * contact.z +
	                      c1->vertices[vertices[indices[face+1]].i1].y * contact.x +
	                      c1->vertices[vertices[indices[face+2]].i1].y * contact.y;
	cd->contactPointA.z = c1->vertices[vertices[indices[face]].i1].z   * contact.z +
	                      c1->vertices[vertices[indices[face+1]].i1].z * contact.x +
	                      c1->vertices[vertices[indices[face+2]].i1].z * contact.y;
	// Generate the contact point on c2.
	cd->contactPointB.x = c2->vertices[vertices[indices[face]].i1].x   * contact.z +
	                      c2->vertices[vertices[indices[face+1]].i1].x * contact.x +
	                      c2->vertices[vertices[indices[face+2]].i1].x * contact.y;
	cd->contactPointB.y = c2->vertices[vertices[indices[face]].i1].y   * contact.z +
	                      c2->vertices[vertices[indices[face+1]].i1].y * contact.x +
	                      c2->vertices[vertices[indices[face+2]].i1].y * contact.y;
	cd->contactPointB.z = c2->vertices[vertices[indices[face]].i1].z   * contact.z +
	                      c2->vertices[vertices[indices[face+1]].i1].z * contact.x +
	                      c2->vertices[vertices[indices[face+2]].i1].z * contact.y;

	// Generate the contact tangents, perpendicular to each other and the contact normal.
	if(fabsf(normal.x) >= 0.57735f){
		vec3Set(&cd->contactTangentA, normal.y, -normal.x, 0.f);
	}else{
		vec3Set(&cd->contactTangentA, 0.f, normal.z, -normal.y);
	}
	vec3NormalizeFast(&cd->contactTangentA);
	vec3Cross(&normal, &cd->contactTangentA, &cd->contactTangentB);

	// Free and return.
	free(indices);
	free(vertices);
	return 1;

}

void hbMeshDelete(hbMesh *hbm){
	if(hbm->vertices != NULL){
		free(hbm->vertices);
	}
	if(hbm->indices != NULL){
		free(hbm->indices);
	}
}
