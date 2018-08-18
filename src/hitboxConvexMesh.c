#include "hitboxConvexMesh.h"
#include "hitboxCollision.h"
#include "engineMath.h"
#include <string.h>
#include <math.h>

#if defined(_MSC_VER) || defined(__MINGW32__)
	#include <malloc.h>
#else
	#include <alloca.h>
#endif

#define COLLISION_MAX_ITERATIONS 64
#define COLLISION_MAX_FACE_NUM_EPA 64
#define COLLISION_MAX_EDGE_NUM_EPA 32

#define COLLISION_PARALLEL_THRESHOLD 0.0001f
#define COLLISION_DISTANCE_THRESHOLD 0.0001f

void hbMeshInit(hbMesh *hbm){
	hbm->vertexNum = 0;
	hbm->vertices = NULL;
	hbm->faceNum = 0;
	hbm->normals = NULL;
	hbm->faces = NULL;
	hbm->edgeNum = 0;
	hbm->edges = NULL;
}

static inline hbVertexIndex_t hbMeshCollisionSupport(const hbMesh *c, const vec3 *axis){
	/*
	** Finds the vertex in c that is farthest in
	** the direction of axis by projecting each
	** vertex onto axis.
	*/
	hbVertexIndex_t i;
	hbVertexIndex_t r = 0;
	float max = vec3Dot(&c->vertices[0], axis);
	for(i = 1; i < c->vertexNum; ++i){
		float s = vec3Dot(&c->vertices[i], axis);
		if(s > max){
			r = i;
			max = s;
		}
	}
	return r;
}

typedef struct {
	float depth;
	// Face index.
	hbFaceIndex_t index;
} hbMeshSHFaceHelper;

typedef struct {
	float depth;
	// The first collider's edge's origin vertex index.
	hbEdgeIndex_t index1;
	// The second collider's edge's origin vertex index.
	hbEdgeIndex_t index2;
} hbMeshSHEdgeHelper;

typedef struct {
	// The first collider's deepest penetrating face.
	// Provides the minimum penetration vector.
	hbMeshSHFaceHelper face1;
	// The second collider's deepest penetrating face.
	// Provides the minimum penetration vector.
	hbMeshSHFaceHelper face2;
	// Two penetrating edges for edge-edge contact.
	hbMeshSHEdgeHelper edge;
} hbMeshPenetrationPlanes;

static inline void hbMeshSHFaceInit(hbMeshSHFaceHelper *e){
	e->depth = -INFINITY;
	e->index = (hbFaceIndex_t)-1;
}

static inline void hbMeshSHEdgeInit(hbMeshSHEdgeHelper *e){
	e->depth = -INFINITY;
	e->index1 = (hbEdgeIndex_t)-1;
	e->index2 = (hbEdgeIndex_t)-1;
}

static inline void hbMeshPenetrationPlanesInit(hbMeshPenetrationPlanes *planes){
	hbMeshSHFaceInit(&planes->face1);
	hbMeshSHFaceInit(&planes->face2);
	hbMeshSHEdgeInit(&planes->edge);
}

static signed char hbMeshClipPatchRegion(const hbMesh *reference, const hbFaceIndex_t referenceFace,
                                         const vec3 *incidentNormal, const vec3 *incidentVertex,
                                         hbMeshEdge *currentEdge, const hbMeshEdge *endEdge,
                                         hbCollisionContactManifold *cm){

	/*
	** Loops from currentEdge to endEdge, projecting the
	** ending vertex of each edge onto the incident plane
	** and adding it as a contact point. This function is
	** invoked if multiple edges in a row lie outside the
	** clipping region.
	*/

	vec3 *contact = &cm->contacts[cm->contactNum].position;
	hbMeshEdge *nextEdge;
	float depthSquared;

	// Loop through the edges inbetween the start and end edges, adding the end vertices.
	do {

		if(currentEdge->face == referenceFace){
			// The next edge is not a twin.
			nextEdge = &reference->edges[currentEdge->next];
		}else{
			// The next edge is a twin.
			nextEdge = &reference->edges[currentEdge->twinNext];
		}
		if(currentEdge->end == nextEdge->start || currentEdge->end == nextEdge->end){
			// The ending vertex is currentEdge->end.
			*contact = reference->vertices[currentEdge->end];
		}else{
			// The ending vertex is currentEdge->start.
			*contact = reference->vertices[currentEdge->start];
		}

		// Only add the vertex if it is behind the incident face.
		depthSquared = pointPlaneDistance(incidentNormal, incidentVertex, contact);
		if(depthSquared <= 0.f){
			// If the vertex is behind the incident face, project it onto it.
			pointPlaneProject(incidentNormal, incidentVertex, contact);
			cm->contacts[cm->contactNum].depthSquared = -depthSquared;
			++cm->contactNum;
			if(cm->contactNum == COLLISION_MAX_CONTACT_POINTS){
				return 1;
			}
			contact = &cm->contacts[cm->contactNum].position;
		}
		currentEdge = nextEdge;

	} while(currentEdge != endEdge);

	return 0;

}

static inline signed char hbMeshClipEdge(const vec3 *normal, const vec3 *vertex, vec3 *start, vec3 *end){
	/*
	** Clips an edge intersecting a plane. If there is
	** no intersection, the function returns 0.
	** If the starting vertex was clipped, it returns 1.
	** If the ending vertex was clipped, it returns 2.
	*/
	const float startDistance = pointPlaneDistance(normal, vertex, start);
	const float endDistance = pointPlaneDistance(normal, vertex, end);
	// Check if the starting vertex is behind the plane.
	if(startDistance <= 0.f){
		// If the starting vertex is behind the plane and
		// the ending vertex is in front of it, clip the
		// ending vertex. If both vertices are behind the
		// plane, there is no need to clip anything.
		if(endDistance > 0.f){
			vec3Lerp(start, end, startDistance / (startDistance - endDistance), end);
		}
		return 2;
	}else if(endDistance <= 0.f){
		// If the starting vertex is in front of the plane
		// and the ending vertex is behind it, clip the
		// starting vertex.
		vec3Lerp(start, end, startDistance / (startDistance - endDistance), start);
		return 1;
	}
	return 0;
}
static inline signed char hbMeshClipEdgeAlloc(const vec3 *normal, const vec3 *vertex, const vec3 *start, const vec3 *end, vec3 *clip){
	/*
	** Clips an edge intersecting a plane. If there is
	** no intersection, the function returns 0.
	** If the starting vertex was clipped, it returns 1.
	** If the ending vertex was clipped, it returns 2.
	*/
	const float startDistance = pointPlaneDistance(normal, vertex, start);
	const float endDistance = pointPlaneDistance(normal, vertex, end);
	// Check if the starting vertex is behind the plane.
	if(startDistance <= 0.f){
		// If the starting vertex is behind the plane and
		// the ending vertex is in front of it, clip the
		// ending vertex. If both vertices are behind the
		// plane, there is no need to clip anything.
		if(endDistance > 0.f){
			vec3Lerp(start, end, startDistance / (startDistance - endDistance), clip);
		}else{
			*clip = *end;
		}
		return 2;
	}else if(endDistance <= 0.f){
		// If the starting vertex is in front of the plane
		// and the ending vertex is behind it, clip the
		// starting vertex.
		vec3Lerp(start, end, startDistance / (startDistance - endDistance), clip);
		return 1;
	}
	return 0;
}

static inline void hbMeshClipEdgeContact(const hbMesh *reference, const hbMesh *incident,
                                         const hbEdgeIndex_t referenceEdge, const hbEdgeIndex_t incidentEdge,
                                         hbCollisionContactManifold *cm){

	printf("Not yet implemented.\n");

}

static void hbMeshClipFaceContact(const hbMesh *reference, const hbMesh *incident,
                                  const hbFaceIndex_t referenceFace, const hbFaceIndex_t incidentFace,
                                  hbCollisionContactManifold *cm){

	/*
	** Generates a contact manifold by clipping the edges of
	** the incident face against the faces adjacent to the
	** reference face.
	*/

	hbMeshEdge *incidentFaceFirstEdge = &incident->edges[incident->faces[incidentFace].edge];
	hbMeshEdge *incidentEdge = incidentFaceFirstEdge;
	hbMeshEdge *incidentEdgeNext;

	hbMeshEdge *referenceFaceFirstEdge = &reference->edges[reference->faces[referenceFace].edge];
	hbMeshEdge *referenceEdge;
	hbMeshEdge *referenceEdgeNext;

	hbMeshEdge *startRegion = NULL;
	hbMeshEdge *endRegionFinal = NULL;
	signed char swapEdgesFinal;

	const vec3 *referenceVertex = &reference->vertices[reference->edges[reference->faces[referenceFace].edge].start];
	const vec3 *incidentNormal = &incident->normals[incidentFace];
	const vec3 *incidentVertex = &incident->vertices[incident->edges[incident->faces[incidentFace].edge].start];

	// Calculate the contact normal.
	cm->normal.x = -reference->normals[referenceFace].x;
	cm->normal.y = -reference->normals[referenceFace].y;
	cm->normal.z = -reference->normals[referenceFace].z;

	// Generate the contact tangents.
	hbCollisionGenerateContactTangents(&cm->normal, &cm->tangents[0], &cm->tangents[1]);

	// Loop through every edge of the incident face.
	do {

		signed char startClipped = 0;
		float depthSquared;
		vec3 start;
		vec3 end;

		hbMeshEdge *startRegionOld = startRegion;
		hbMeshEdge *endRegion = NULL;

		// Get the next edge.
		if(incidentEdge->face == incidentFace){
			incidentEdgeNext = &incident->edges[incidentEdge->next];
		}else{
			incidentEdgeNext = &incident->edges[incidentEdge->twinNext];
		}
		// Swap the start and end vertices around to
		// stay consistent with previous edges.
		if(incidentEdge->end == incidentEdgeNext->start || incidentEdge->end == incidentEdgeNext->end){
			start = incident->vertices[incidentEdge->start];
			end = incident->vertices[incidentEdge->end];
		}else{
			start = incident->vertices[incidentEdge->end];
			end = incident->vertices[incidentEdge->start];
		}

		// Loop through every edge on the reference face
		// to find its adjacent faces. Clip the edge of the
		// incident mesh against each of the adjacent face's
		// normals.
		referenceEdge = referenceFaceFirstEdge;
		do {

			// Clip the vertex with the normal of the
			// current twin edge's face.
			hbFaceIndex_t adjacentFace;
			signed char clipped;

			if(referenceEdge->face == referenceFace){
				// The next edge associated with this face is not a twin.
				adjacentFace = referenceEdge->twinFace;
				referenceEdgeNext = &reference->edges[referenceEdge->next];
			}else{
				// The next edge associated with this face is a twin.
				adjacentFace = referenceEdge->face;
				referenceEdgeNext = &reference->edges[referenceEdge->twinNext];
			}

			clipped = hbMeshClipEdge(&reference->normals[adjacentFace],
			                         &reference->vertices[reference->edges[reference->faces[adjacentFace].edge].start],
			                         &start, &end);

			if(!clipped){
				startRegion = startRegionOld;
				goto SKIP_CURRENT_EDGE;
			}

			if(clipped == 1){
				// The starting vertex was clipped,
				// so it may potentially be a new
				// contact point.
				startClipped = 1;
				// Save the final edge it was clipped
				// on as somewhere to end when adding
				// clipping region vertices.
				endRegion = referenceEdge;
			}else{
				// The ending vertex was clipped, save
				// the final edge it was clipped on as
				// somewhere to start when adding
				// clipping region vertices.
				startRegion = referenceEdge;
			}
			referenceEdge = referenceEdgeNext;

		} while(referenceEdge != referenceFaceFirstEdge);

		// If new clipped faces were found but some were skipped,
		// add vertices from the clipping region to patch the gap.
		if(endRegion != NULL && startRegion != endRegion){

			// If the edge's normal (perpendicular to the edge and
			// the incident face normal) is pointing away from
			// endRegion's starting vertex, swap startRegion and
			// endRegion so the edges are looped through in the
			// correct order.
			vec3 local;
			vec3 edgeNormal;
			// Translate the edge direction into start's space.
			vec3SubVFromVR(&end, &start, &local);
			vec3Cross(&incident->normals[incidentFace], &local, &edgeNormal);
			// Calculate the starting vertex in local space.
			// First we need to find which vertex is the starting vertex.
			if(endRegion->end == reference->edges[endRegion->next].start || endRegion->end == reference->edges[endRegion->next].end){
				// Translate the starting vertex into start's space.
				vec3SubVFromVR(&reference->vertices[endRegion->start], &start, &local);
			}else{
				// Translate the starting vertex into local space.
				vec3SubVFromVR(&reference->vertices[endRegion->end], &start, &local);
			}

			if(startRegion == NULL){
				// Check whether or not startRegion and
				// endRegion should be swapped.
				if(vec3Dot(&edgeNormal, &local) < 0.f){
					swapEdgesFinal = 1;
				}
				// We've only found an ending edge.
				// Store it until we've finished this loop.
				endRegionFinal = endRegion;
			}else{
				// Check whether or not startRegion and
				// endRegion should be swapped.
				if(vec3Dot(&edgeNormal, &local) < 0.f){
					hbMeshEdge *swap = startRegion;
					startRegion = endRegion;
					endRegion = swap;
				}
				// Add all the vertices between startRegion
				// and endRegion as contact points.
				if(hbMeshClipPatchRegion(reference, referenceFace, incidentNormal, incidentVertex, startRegion, endRegion, cm)){
					return;
				}
				startRegion = NULL;
			}

		}

		// The vertices that lie behind the reference face
		// are valid contact points. Check if the edge's
		// vertices satisfy this condition.
		// Begin by checking the ending vertex.
		depthSquared = pointPlaneDistance(&cm->normal, referenceVertex, &end);
		if(depthSquared >= 0.f){
			cm->contacts[cm->contactNum].depthSquared = depthSquared;
			cm->contacts[cm->contactNum].position = end;
			++cm->contactNum;
			if(cm->contactNum == COLLISION_MAX_CONTACT_POINTS){
				return;
			}
		}
		// Only potentially add the starting vertex if it was clipped.
		// The reason for this is because if it wasn't clipped, it
		// either has already been added or will be added later as an
		// ending vertex, and we don't want duplicate contact points.
		if(startClipped){
			depthSquared = pointPlaneDistance(&cm->normal, referenceVertex, &start);
			if(depthSquared >= 0.f){
				cm->contacts[cm->contactNum].depthSquared = depthSquared;
				cm->contacts[cm->contactNum].position = start;
				++cm->contactNum;
				if(cm->contactNum == COLLISION_MAX_CONTACT_POINTS){
					return;
				}
			}
		}

		SKIP_CURRENT_EDGE:
		incidentEdge = incidentEdgeNext;

	} while(incidentEdge != incidentFaceFirstEdge);

	// If the start and final end edges are not null, we can
	// still add the vertices between the start edge and it.
	if(startRegion != NULL && endRegionFinal != NULL && startRegion != endRegionFinal){
		if(swapEdgesFinal){
			hbMeshEdge *swap = startRegion;
			startRegion = endRegionFinal;
			endRegionFinal = swap;
		}
		if(hbMeshClipPatchRegion(reference, referenceFace, incidentNormal, incidentVertex, startRegion, endRegionFinal, cm)){
			return;
		}
	}

	// If no contacts were added, just add the reference face vertices.
	if(cm->contactNum == 0){
		hbMeshClipPatchRegion(reference, referenceFace, incidentNormal, incidentVertex, referenceFaceFirstEdge, referenceFaceFirstEdge, cm);
	}

}
static void hbMeshClipFaceContactAlloc(const hbMesh *reference, const hbMesh *incident,
                                       const hbFaceIndex_t referenceFace, const hbFaceIndex_t incidentFace,
                                       hbCollisionContactManifold *cm){

	/*
	** Generates a contact manifold by clipping the edges of
	** the incident face against the faces adjacent to the
	** reference face.
	*/

	hbVertexIndex_t i = 0;

	const hbVertexIndex_t referenceVertexNum = reference->faces[referenceFace].edgeNum;
	hbVertexIndex_t vertexNum = incident->faces[incidentFace].edgeNum;

	const hbVertexIndex_t maxOutput = referenceVertexNum > vertexNum ?
	                                  referenceVertexNum + referenceVertexNum : vertexNum + vertexNum;
	vec3 *vertices = alloca((maxOutput + maxOutput) * sizeof(vec3));

	// Pointer to the array with the vertices to be clipped.
	vec3 *vertexArray = vertices;
	// Pointer to the array with the clipped vertices.
	vec3 *vertexClipArray = &vertices[maxOutput];
	vec3 *vertexArraySwap;

	hbMeshEdge *edgeFirst = &reference->edges[reference->faces[referenceFace].edge];
	hbMeshEdge *edge = &incident->edges[incident->faces[incidentFace].edge];
	hbMeshEdge *edgeNext;

	const vec3 *referenceVertex = &reference->vertices[reference->edges[reference->faces[referenceFace].edge].start];


	// Calculate the contact normal.
	cm->normal.x = -reference->normals[referenceFace].x;
	cm->normal.y = -reference->normals[referenceFace].y;
	cm->normal.z = -reference->normals[referenceFace].z;

	// Generate the contact tangents.
	hbCollisionGenerateContactTangents(&cm->normal, &cm->tangents[0], &cm->tangents[1]);


	// Add the vertex positions to vertexArray.
	while(i < vertexNum){

		// Get the next edge.
		if(edge->face == incidentFace){
			edgeNext = &incident->edges[edge->next];
		}else{
			edgeNext = &incident->edges[edge->twinNext];
		}

		// Only add starting vertices.
		// Swap the start and end vertices around to
		// stay consistent with previous edges.
		if(edge->end == edgeNext->start || edge->end == edgeNext->end){
			vertexArray[i] = incident->vertices[edge->start];
		}else{
			vertexArray[i] = incident->vertices[edge->end];
		}

		++i;
		edge = edgeNext;

	}


	// Loop through every edge on the reference face
	// to find its adjacent faces. Clip these against
	// the incident face vertices.
	edge = edgeFirst;
	do {

		// Clip loop variables.
		hbVertexIndex_t iStart = 0;
		signed char clipped;
		hbVertexIndex_t newVertexNum = 0;

		// Clip the vertex with the normal of the
		// current twin edge's face.
		hbFaceIndex_t adjacentFace;

		if(edge->face == referenceFace){
			// The next edge associated with this face is not a twin.
			adjacentFace = edge->twinFace;
			edgeNext = &reference->edges[edge->next];
		}else{
			// The next edge associated with this face is a twin.
			adjacentFace = edge->face;
			edgeNext = &reference->edges[edge->twinNext];
		}

		// Loop through every edge of the incident face,
		// clipping its vertices against the current
		// reference plane.
		i = 1;
		while(i < vertexNum){
			clipped =
			hbMeshClipEdgeAlloc(&reference->normals[adjacentFace],
			                    &reference->vertices[reference->edges[reference->faces[adjacentFace].edge].start],
			                    &vertexArray[iStart], &vertexArray[i], &vertexClipArray[newVertexNum]);
			if(clipped > 0){
				++newVertexNum;
				if(clipped == 1){
					// The starting vertex is in front of the plane
					// and the ending vertex is behind it. Add both.
					// The clipped starting vertex has already been
					// added, so add the ending vertex.
					vertexClipArray[newVertexNum] = vertexArray[i];
					++newVertexNum;
				}
			}
			iStart = i;
			++i;
		}
		// Final iteration between last and first vertices.
		clipped =
		hbMeshClipEdgeAlloc(&reference->normals[adjacentFace],
		                    &reference->vertices[reference->edges[reference->faces[adjacentFace].edge].start],
		                    &vertexArray[iStart], &vertexArray[0], &vertexClipArray[newVertexNum]);
		if(clipped > 0){
			++newVertexNum;
			if(clipped == 1){
				// The starting vertex is in front of the plane
				// and the ending vertex is behind it. Add both.
				// The clipped starting vertex has already been
				// added, so add the ending vertex.
				vertexClipArray[newVertexNum] = vertexArray[0];
				++newVertexNum;
			}
		}

		// Swap vertexArray and vertexClipArray so that we clip
		// the vertices that were clipped during this iteration.
		vertexArraySwap = vertexArray;
		vertexArray = vertexClipArray;
		vertexClipArray = vertexArraySwap;

		// Resize the array to represent our new,
		// culled collection of potential contacts.
		vertexNum = newVertexNum;

		edge = edgeNext;

	} while(edge != edgeFirst);


	// Loop through every vertex of the incident face,
	// checking which ones we can use as contact points.
	i = 0;
	while(i < vertexNum){
		const float depthSquared = pointPlaneDistance(&cm->normal, referenceVertex, &vertexArray[i]);
		if(depthSquared >= 0.f){
			cm->contacts[cm->contactNum].depthSquared = depthSquared;
			cm->contacts[cm->contactNum].position = vertexArray[i];
			++cm->contactNum;
			if(cm->contactNum == COLLISION_MAX_CONTACT_POINTS){
				return;
			}
		}
		++i;
	}

}

static inline void hbMeshCollisionSHClipping(const hbMesh *c1, const hbMesh *c2, const hbMeshPenetrationPlanes *planes, hbCollisionContactManifold *cm){

	/*
	** Implementation of the Sutherland-Hodgman clipping
	** algorithm for generating a contact manifold after
	** a collision.
	*/

	const float maxSeparation = planes->face2.depth > planes->face1.depth ? planes->face2.depth : planes->face1.depth;

	// Only create an edge contact if the edge penetration depth
	// is greater than both face penetration depths. Favours
	// face contacts over edge contacts.
	if(planes->edge.depth > 0.5f + maxSeparation * 0.9f){

		hbMeshClipEdgeContact(c1, c2, planes->edge.index1, planes->edge.index2, cm);

	}else{

		// If the second face penetration depth is greater than
		// the first, create a face contact with it. Favours the
		// first hull as the reference collider and the second
		// as the incident collider.
		if(planes->face2.depth > planes->face1.depth * 0.95f){

			hbMeshClipFaceContactAlloc(c2, c1, planes->face2.index, planes->face1.index, cm);

		}else{

			hbMeshClipFaceContactAlloc(c1, c2, planes->face1.index, planes->face2.index, cm);

		}

	}

}

static inline signed char hbMeshCollisionSATFaceQuery(const hbMesh *c1, const hbMesh *c2, hbMeshSHFaceHelper *r, axisIndex_t *index){

	/*
	** Find the maximum separation distance between
	** the faces of the first and the vertices of the
	** second collider.
	*/

	/** Should perform calculations in the second collider's local space? **/
	/** Might not be worth it, as our physics colliders are very simple.  **/

	hbFaceIndex_t i;
	for(i = 0; i < c1->faceNum; ++i){

		// Get the vertex in the second collider that's
		// farthest from the current face in the first
		// collider in the opposite direction that the
		// face is pointing.
		const vec3 invNormal = {.x = -c1->normals[i].x,
		                        .y = -c1->normals[i].y,
		                        .z = -c1->normals[i].z};
		// After this, find the distance between them.
		const float distance = pointPlaneDistance(&c1->normals[i],
		                                          &c1->vertices[c1->edges[c1->faces[i].edge].start],
		                                          &c2->vertices[hbMeshCollisionSupport(c2, &invNormal)]);

		if(distance > 0.f){
			// Early exit, a separating axis has been found.
			// Cache the separating axis.
			*index = i;
			return 0;
		}else if(distance > r->depth){
			r->depth = distance;
			r->index = i;
		}

	}

	return r->depth <= 0.f;

}

static inline signed char hbMeshCollisionSATMinkowskiFace(const hbMesh *c1, const hbMesh *c2,
                                                          const hbMeshEdge *e1, const vec3 *e1InvDir,
                                                          const hbMeshEdge *e2, const vec3 *e2InvDir){

	/*
	** Tests if the specified edges overlap
	** when projected onto a unit sphere.
	**
	** Overlapping arcs on the Gauss map create
	** a face on the Minkowski sum of the
	** polyhedras.
	**
	** Because we want the Minkowski difference,
	** we negate the normals of the second edge.
	*/

	float BDC;

	// Normals of one of the planes intersecting
	// the arcs. Instead of crossing the two
	// adjacent face normals, we can just
	// subtract one edge vertex from the other.
	//
	// D x C
	//vec3SubVFromVR(&c2->vertices[e2->start], &c2->vertices[e2->end], &normal);

	BDC = vec3Dot(&c1->normals[e1->twinFace], e2InvDir);

	// Check if the arcs are overlapping by testing
	// if the vertices of one arc are on opposite
	// sides of the plane intersecting the other arc.
	//
	// ADC * BDC
	if(vec3Dot(&c1->normals[e1->face], e2InvDir) * BDC < 0.f){

		float CBA;

		// Normals of the other plane intersecting
		// the arcs.
		//
		// B x A
		//vec3SubVFromVR(&c1->vertices[e1->start], &c1->vertices[e1->end], &normal);

		CBA = vec3Dot(&c2->normals[e2->face], e1InvDir);

		// Make sure both arcs are on the same
		// hemisphere by creating a plane from
		// one vertex of one arc and another
		// vertex of the other arc and checking
		// if the two remaining vertices are on
		// the same side of the plane.
		//
		// Because we want to check a face on the
		// Minkowski difference rather than the
		// Minkowski sum, we can either invert CBA
		// or change the comparison operator from
		// > to <.
		if(CBA * BDC < 0.f){

			// Final check to see if the vertices are overlapping.
			// We don't need to change the signs of CBA or DBA, as
			// they will cancel each other out.
			//
			// CBA * DBA
			return CBA * vec3Dot(&c2->normals[e2->twinFace], e1InvDir) < 0.f;

		}

	}

	return 0;

}

static inline float hbMeshCollisionSATEdgeSeparation(const hbMesh *c1, const hbMesh *c2, const vec3 *centroid,
                                                     const hbMeshEdge *e1, const vec3 *e1InvDir,
                                                     const hbMeshEdge *e2, const vec3 *e2InvDir){

	/*
	** Check the distance between the two edges.
	*/

	/** Should perform calculations in the second collider's local space? **/
	/** Might not be worth it, as our physics colliders are very simple.  **/

	const vec3 *pointA = &c1->vertices[e1->start];
	vec3 normal, offset;
	float magnitudeSquared;

	// Get the directions of the edges.
	//vec3SubVFromVR(&c1->vertices[e1->start], &c1->vertices[e1->end], &edgeA);
	//vec3SubVFromVR(&c2->vertices[e2->start], &c2->vertices[e2->end], &edgeB);

	vec3Cross(e1InvDir, e2InvDir, &normal);
	magnitudeSquared = vec3MagnitudeSquared(&normal);

	// Check if the edges are parallel enough to not be
	// considered a new face on the Minkowski difference.
	if(magnitudeSquared < COLLISION_PARALLEL_THRESHOLD *
	   (vec3MagnitudeSquared(e1InvDir) + vec3MagnitudeSquared(e2InvDir))){
		return -INFINITY;
	}

	// Normalize.
	vec3MultVByS(&normal, fastInvSqrt(magnitudeSquared));
	vec3SubVFromVR(pointA, centroid, &offset);

	// Ensure the normal points from A to B.
	if(vec3Dot(&normal, &offset) < 0.f){
		normal.x = -normal.x;
		normal.y = -normal.y;
		normal.z = -normal.z;
	}

	vec3SubVFromVR(&c2->vertices[e2->start], pointA, &offset);
	return vec3Dot(&normal, &offset);

	/*
	const vec3 *pointA = &c1->vertices[e1->start];
	const vec3 *pointB = &c2->vertices[e2->start];
	vec3 edgeA, edgeB;
	vec3 normal, offset;

	// Get the directions of the edges.
	vec3SubVFromVR(&c1->vertices[e1->end], &c1->vertices[e1->start], &edgeA);
	vec3SubVFromVR(&c2->vertices[e2->end], &c2->vertices[e2->start], &edgeB);

	vec3Cross(&edgeA, &edgeB, &normal);

	if(normal.x == 0.f && normal.y == 0.f && normal.z == 0.f){
		// The edges are parallel, so they can't create
		// a new face on the Minkowski difference.
		return -INFINITY;
	}

	vec3NormalizeFast(&normal);
	vec3SubVFromVR(pointA, centroid, &offset);

	if(vec3Dot(&normal, &offset) < 0.f){
		// Ensure the normal points from A to B.
		normal.x = -normal.x;
		normal.y = -normal.y;
		normal.z = -normal.z;
	}

	vec3SubVFromVR(pointB, pointA, &offset);
	return vec3Dot(&normal, &offset);
	*/

}

static inline signed char hbMeshCollisionSATEdgeQuery(const hbMesh *c1, const hbMesh *c2, const vec3 *centroid, hbMeshSHEdgeHelper *r, axisIndex_t *index){

	/*
	** Find the maximum separation distance between
	** the edges of the first and second colliders.
	*/

	hbEdgeIndex_t i, j;
	hbMeshEdge *e1, *e2;
	vec3 e1InvDir, e2InvDir;

	for(i = 0; i < c1->edgeNum; ++i){
		e1 = &c1->edges[i];
		for(j = 0; j < c2->edgeNum; ++j){
			e2 = &c2->edges[j];
			// Get the inverse direction vectors for the edges.
			vec3SubVFromVR(&c1->vertices[e1->start], &c1->vertices[e1->end], &e1InvDir);
			vec3SubVFromVR(&c2->vertices[e2->start], &c2->vertices[e2->end], &e2InvDir);
			// The inverse direction vectors are used in place of the cross product
			// between the edge's face normal and its twin's face normal.
			if(hbMeshCollisionSATMinkowskiFace(c1, c2, e1, &e1InvDir, e2, &e2InvDir)){
				// Now that we have a Minkowski face, we can
				// get the distance between the two edges.
				const float distance = hbMeshCollisionSATEdgeSeparation(c1, c2, centroid, e1, &e1InvDir, e2, &e2InvDir);
				if(distance > 0.f){
					// Early exit, a separating axis has been found.
					// Cache the separating axis.
					*index = i;
					return 0;
				}else if(distance > r->depth){
					// If the distance between these two edges
					// is larger than the previous greatest
					// distance, record it.
					r->depth = distance;
					r->index1 = i;
					r->index2 = j;
				}
			}
		}
	}

	return r->depth <= 0.f;

}

signed char hbMeshCollisionSAT(const hbMesh *c1, const hbMesh *c2, const vec3 *centroid, hbCollisionInfo *info, hbCollisionContactManifold *cm){

	/*
	** Implementation of the separating axis theorem
	** using Minkowski space and Gauss map optimizations.
	**
	** Credit to Erin Catto for the original idea and
	** Dirk Gregorius for his amazing presentation at
	** GDC 2013.
	*/

	hbMeshPenetrationPlanes planes;
	hbMeshPenetrationPlanesInit(&planes);
	axisIndex_t index = (axisIndex_t)-1;  // Index of the separating axis.

	if(hbMeshCollisionSATFaceQuery(c1, c2, &planes.face1, &index)){
		if(hbMeshCollisionSATFaceQuery(c2, c1, &planes.face2, &index)){
			if(hbMeshCollisionSATEdgeQuery(c1, c2, centroid, &planes.edge, &index)){
				if(cm != NULL){
					hbMeshCollisionSHClipping(c1, c2, &planes, cm);
				}
				return 1;
			}else if(info != NULL){
				info->type = COLLISION_SEPARATION_TYPE_EDGE;
				info->axisID = index;
			}
		}else if(info != NULL){
			info->type = COLLISION_SEPARATION_TYPE_FACE_2;
			info->axisID = index;
		}
	}else if(info != NULL){
		info->type = COLLISION_SEPARATION_TYPE_FACE_1;
		info->axisID = index;
	}

	return 0;

}

typedef struct {
	vec3 v;                  // The Minkowski "difference" of the two support points.
	hbVertexIndex_t i1, i2;  // The indices of the support points used to generate v.
} hbMeshSupportVertex;

typedef struct {
	// Three vertices and a normal.
	hbMeshSupportVertex vertex[3];
	vec3 normal;
} hbMeshEPAFaceHelper;

typedef struct {
	// An edge that will be removed.
	hbMeshSupportVertex vertex[2];
} hbMeshEPAEdgeHelper;

static inline void hbMeshEPAFaceInit(hbMeshEPAFaceHelper *face, const hbMeshSupportVertex *v0, const hbMeshSupportVertex *v1, const hbMeshSupportVertex *v2){
	// Create a new face.
	face->vertex[0] = *v0;
	face->vertex[1] = *v1;
	face->vertex[2] = *v2;
	faceNormal(&v0->v, &v1->v, &v2->v, &face->normal);
	vec3NormalizeFastAccurate(&face->normal);
}

static inline void hbMeshEPAEdgeInit(hbMeshEPAEdgeHelper *edge, const hbMeshSupportVertex *v0, const hbMeshSupportVertex *v1){
	// Create a new face.
	edge->vertex[0] = *v0;
	edge->vertex[1] = *v1;
}

static inline void hbMeshCollisionMinkowskiSupport(const hbMesh *c1, const hbMesh *c2, const vec3 *axis, hbMeshSupportVertex *r){
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
	r->i1 = hbMeshCollisionSupport(c1, axis);
	r->i2 = hbMeshCollisionSupport(c2, &axisNegative);
	// Get the "Minkowski Difference" of the two vertices. r = v1 - v2
	vec3SubVFromVR(&c1->vertices[r->i1], &c2->vertices[r->i2], &r->v);
}

static inline void hbMeshCollisionEPA(const hbMesh *c1, const hbMesh *c2, hbMeshSupportVertex *simplex, hbCollisionContactManifold *cm){

	/*
	** Implementation of the expanding polytope algorithm. Extrapolates
	** additional collision information from two convex polygons and the
	** simplex generated by the Gilbert-Johnson-Keerthi algorithm.
	*/

	unsigned int i;
	hbFaceIndex_t j;
	hbVertexIndex_t k;
	hbEdgeIndex_t l;
	hbFaceIndex_t closestFace;
	float distance;
	hbMeshSupportVertex point;

	// Create the face and edge arrays.
	hbFaceIndex_t faceNum = 4;
	hbMeshEPAFaceHelper faces[COLLISION_MAX_FACE_NUM_EPA];
	hbEdgeIndex_t edgeNum = 0;
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
		for(l = 0; l < edgeNum; ++l){

			if(faceNum >= COLLISION_MAX_FACE_NUM_EPA){
				break;
			}

			hbMeshEPAFaceInit(&faces[faceNum], &edges[l].vertex[0], &edges[l].vertex[1], &point);

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

	if(cm->contactNum < COLLISION_MAX_CONTACT_POINTS-1){

		const float depthSquared = distance*distance;

		// Store the contact normal and penetration depth.
		vec3 contact = faces[closestFace].normal;
		cm->normal = contact;

		// Generate the contact tangents.
		hbCollisionGenerateContactTangents(&contact, &cm->tangents[0], &cm->tangents[1]);

		// Project the origin onto the closest face.
		vec3MultVByS(&contact, distance);

		// Calculate the Barycentric coordinates of the projected origin.
		barycentric(&faces[closestFace].vertex[0].v, &faces[closestFace].vertex[1].v, &faces[closestFace].vertex[2].v, &contact, &contact);

		// The contact point on c1 is the linear combination of the original
		// vertices in c1 used to generate the support vertices for the
		// closest face, using the barycentric coordinates stored in contact.
		vec3CombineLinear(&c1->vertices[faces[closestFace].vertex[0].i1],
						  &c1->vertices[faces[closestFace].vertex[1].i1],
						  &c1->vertices[faces[closestFace].vertex[2].i1],
						  contact.x, contact.y, contact.z, &cm->contacts[cm->contactNum].position);

		cm->contacts[cm->contactNum].depthSquared = depthSquared;
		++cm->contactNum;

		// The contact point on c2 is the linear combination of the original
		// vertices in c2 used to generate the support vertices for the
		// closest face, using the barycentric coordinates stored in contact.
		vec3CombineLinear(&c2->vertices[faces[closestFace].vertex[0].i2],
						  &c2->vertices[faces[closestFace].vertex[1].i2],
						  &c2->vertices[faces[closestFace].vertex[2].i2],
						  contact.x, contact.y, contact.z, &cm->contacts[cm->contactNum].position);

		cm->contacts[cm->contactNum].depthSquared = depthSquared;
		++cm->contactNum;

	}

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

	}else{

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

	}

	return 1;

}

signed char hbMeshCollisionGJK(const hbMesh *c1, const vec3 *c1c, const hbMesh *c2, const vec3 *c2c, hbCollisionContactManifold *cm){

	/*
	** Implementation of the Gilbert-Johnson-Keerthi distance algorithm,
	** which is only compatible with convex polytopes.
	*/

	unsigned int i;
	hbMeshSupportVertex simplex[4];

	// Set the current number of vertices of our simplex. We will be
	// creating a line segment with vertices B and C before the main
	// loop, so we can set this to 2 preemptively.
	unsigned char simplexVertices = 2;

	// The first direction to check in is the direction of body2 from body1.
	vec3 axis = {.x = c2c->x - c1c->x,
	             .y = c2c->y - c1c->y,
	             .z = c2c->z - c1c->z};

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
			if(cm != NULL){
				hbMeshCollisionEPA(c1, c2, &simplex[0], cm);
			}
			return 1;
		}

	}

	return 0;

}

void hbMeshDelete(hbMesh *hbm){
	if(hbm->vertices != NULL){
		free(hbm->vertices);
	}
	if(hbm->normals != NULL){
		free(hbm->normals);
	}
	if(hbm->faces != NULL){
		free(hbm->faces);
	}
	if(hbm->edges != NULL){
		free(hbm->edges);
	}
}
