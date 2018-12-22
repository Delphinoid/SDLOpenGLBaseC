#include "colliderConvexMesh.h"
#include "memoryManager.h"
#include "collision.h"
#include "helpersMath.h"
#include "constantsMath.h"
#include "inline.h"
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

void cMeshInit(cMesh *const restrict cm){
	cm->vertexNum = 0;
	cm->vertices = NULL;
	cm->faceNum = 0;
	cm->normals = NULL;
	cm->edgeMax = 0;
	cm->faces = NULL;
	cm->edgeNum = 0;
	cm->edges = NULL;
}

static __FORCE_INLINE__ const vec3 *cMeshCollisionSupport(const cMesh *const restrict c, const vec3 *const restrict axis){
	/*
	** Finds the vertex in c that is farthest in
	** the direction of axis by projecting each
	** vertex onto axis.
	*/
	const vec3 *r = c->vertices;
	const vec3 *v = &r[1];
	const vec3 *const vLast = &r[c->vertexNum];
	float max = vec3Dot(r, axis);
	for(; v < vLast; ++v){
		const float s = vec3Dot(v, axis);
		if(s > max){
			r = v;
			max = s;
		}
	}
	return r;
}
static __FORCE_INLINE__ const vec3 *cMeshCollisionSupportIndex(const cMesh *const restrict c, const vec3 *const restrict axis, cVertexIndex_t *const index){

	/*
	** Finds the vertex in c that is farthest in
	** the direction of axis by projecting each
	** vertex onto axis.
	*/

	cVertexIndex_t i;
	const vec3 *v;
	const vec3 *r = c->vertices;
	float max = vec3Dot(r, axis);

	*index = 0;

	for(i = 1, v = &r[1]; i < c->vertexNum; ++i, ++v){
		const float s = vec3Dot(v, axis);
		if(s > max){
			r = v;
			max = s;
			*index = i;
		}
	}

	return r;

}

typedef struct {
	float depth;
	cFaceIndex_t index;
} cMeshSHFaceHelper;

typedef struct {
	float depth;
	const cMeshEdge *edge1;
	const cMeshEdge *edge2;
} cMeshSHEdgeHelper;

typedef struct {
	// The first collider's deepest penetrating face.
	// Provides the minimum penetration vector.
	cMeshSHFaceHelper face1;
	// The second collider's deepest penetrating face.
	// Provides the minimum penetration vector.
	cMeshSHFaceHelper face2;
	// Two penetrating edges for edge-edge contact.
	cMeshSHEdgeHelper edge;
} cMeshPenetrationPlanes;

static __FORCE_INLINE__ void cMeshSHFaceInit(cMeshSHFaceHelper *e){
	e->depth = -INFINITY;
	e->index = (cFaceIndex_t)-1;
}

static __FORCE_INLINE__ void cMeshSHEdgeInit(cMeshSHEdgeHelper *e){
	e->depth = -INFINITY;
	e->edge1 = NULL;
	e->edge2 = NULL;
}

static __FORCE_INLINE__ void cMeshPenetrationPlanesInit(cMeshPenetrationPlanes *planes){
	cMeshSHFaceInit(&planes->face1);
	cMeshSHFaceInit(&planes->face2);
	cMeshSHEdgeInit(&planes->edge);
}

static __FORCE_INLINE__ const cMeshFace *cMeshFindIncidentClipFace(const cMesh *const incident, const vec3 *const restrict referenceNormal, cFaceIndex_t *const restrict incidentFaceIndex){

	if(vec3Dot(referenceNormal, &incident->normals[*incidentFaceIndex]) < 0.f){
		// If the suggested face is penetrating, use it.
		return &incident->faces[*incidentFaceIndex];
	}else{

		// Otherwise find the deepest penetrating face.
		cFaceIndex_t i;
		const vec3 *n = incident->normals;
		const cMeshFace *f = incident->faces;

		float min = vec3Dot(referenceNormal, n);

		const cMeshFace *r = f;
		*incidentFaceIndex = 0;

		for(i = 1; i < incident->faceNum; ++i){
			++n; ++f;
			{
				const float sameness = vec3Dot(referenceNormal, n);
				if(sameness < min){
					min = sameness;
					*incidentFaceIndex = i;
					r = f;
				}
			}
		}

		return r;

	}

}
static return_t cMeshClipPatchRegion(const cMesh *const reference, const cFaceIndex_t referenceFaceIndex,
                                     const vec3 *const restrict incidentNormal, const vec3 *const restrict incidentVertex,
                                     const cMeshEdge *currentEdge, const cMeshEdge *const endEdge,
                                     const size_t offset, cContactManifold *cm){

	/*
	** Loops from currentEdge to endEdge, projecting the
	** ending vertex of each edge from the reference collider
	** onto the incident plane and adding it as a contact
	** point. This function is invoked if multiple edges in a
	** row lie outside the clipping region.
	*/

	cContact *contact = &cm->contacts[cm->contactNum];
	const cMeshEdge *nextEdge;
	float depth;

	// Loop through the edges inbetween the start and end edges, adding the end vertices.
	do {

		if(currentEdge->face == referenceFaceIndex){
			// The next edge is not a twin.
			nextEdge = &reference->edges[currentEdge->next];
		}else{
			// The next edge is a twin.
			nextEdge = &reference->edges[currentEdge->twinNext];
		}
		if(currentEdge->end == nextEdge->start || currentEdge->end == nextEdge->end){
			// The ending vertex is currentEdge->end.
			*(&contact->pointA+offset) = reference->vertices[currentEdge->end];
		}else{
			// The ending vertex is currentEdge->start.
			*(&contact->pointA+offset) = reference->vertices[currentEdge->start];
		}

		// Only add the vertex if it is behind the incident face.
		depth = pointPlaneDistance(incidentNormal, incidentVertex, &contact->pointA+offset);
		if(depth <= 0.f){
			// If the vertex is behind the incident face, project it onto it.
			pointPlaneProjectR(incidentNormal, incidentVertex, &contact->pointA+offset, &contact->pointB-offset);
			contact->depth = -depth;
			++cm->contactNum;
			if(cm->contactNum == COLLISION_MANIFOLD_MAX_CONTACT_POINTS){
				return 1;
			}
			++contact;
		}
		currentEdge = nextEdge;

	} while(currentEdge != endEdge);

	return 0;

}
static __FORCE_INLINE__ return_t cMeshClipEdge(const vec3 *const restrict normal, const vec3 *const restrict vertex, vec3 *const restrict start, vec3 *const restrict end){
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
			vec3Lerp2(start, end, startDistance / (startDistance - endDistance));
		}
		return 2;
	}else if(endDistance <= 0.f){
		// If the starting vertex is in front of the plane
		// and the ending vertex is behind it, clip the
		// starting vertex.
		vec3Lerp1(start, end, startDistance / (startDistance - endDistance));
		return 1;
	}
	return 0;
}

static __FORCE_INLINE__ return_t cMeshClipEdgeAlloc(const vec3 *const restrict normal, const vec3 *const restrict vertex, const vec3 *const restrict start, const vec3 *const restrict end, vec3 *const restrict clip){
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
			vec3LerpR(start, end, startDistance / (startDistance - endDistance), clip);
		}else{
			*clip = *end;
		}
		return 2;
	}else if(endDistance <= 0.f){
		// If the starting vertex is in front of the plane
		// and the ending vertex is behind it, clip the
		// starting vertex.
		vec3LerpR(start, end, startDistance / (startDistance - endDistance), clip);
		return 1;
	}
	return 0;
}

static __FORCE_INLINE__ void cMeshClipEdgeContact(const cMesh *const reference, const cMesh *const incident,
                                                  const cMeshEdge *const restrict referenceEdge, const cMeshEdge *const restrict incidentEdge,
                                                  cContactManifold *const restrict cm){

	const vec3 *const referenceEdgeStart = &reference->vertices[referenceEdge->start];
	const vec3 *const referenceEdgeEnd   = &reference->vertices[referenceEdge->end];
	vec3 referenceContact;

	const vec3 *const incidentEdgeStart = &incident->vertices[incidentEdge->start];
	const vec3 *const incidentEdgeEnd   = &incident->vertices[incidentEdge->end];
	vec3 incidentContact;

	float depth;

	cContact *contact = &cm->contacts[cm->contactNum];

	// Get the closest points on the line segments.
	segmentClosestPoints(referenceEdgeStart, referenceEdgeEnd, incidentEdgeStart, incidentEdgeEnd, &referenceContact, &incidentContact);

	// Calculate the contact normal.
	vec3SubVFromVR(&referenceContact, &incidentContact, &cm->normal);
	depth = vec3Magnitude(&cm->normal);

	// Add the contact point.
	if(depth >= 0.f){
		contact->depth = depth;
		contact->pointB.x = referenceContact.x + 0.5f * cm->normal.x;
		contact->pointB.y = referenceContact.y + 0.5f * cm->normal.y;
		contact->pointB.z = referenceContact.z + 0.5f * cm->normal.z;
		pointPlaneProjectR(&cm->normal, referenceEdgeStart, &contact->pointB, &contact->pointA);
		++cm->contactNum;
	}

	// Convert the contact normal to a unit vector
	// using the magnitude we calculated earlier.
	vec3MultVByS(&cm->normal, 1.f/depth);

}

static __FORCE_INLINE__ void cMeshClipFaceContact(const cMesh *const reference, const cMesh *const incident,
                                                  const cFaceIndex_t referenceFaceIndex, cFaceIndex_t incidentFaceIndex,
                                                  const size_t offset, cContactManifold *const restrict cm){

	/*
	** Generates a contact manifold by clipping the edges of
	** the incident face against the faces adjacent to the
	** reference face.
	**
	** Offset is used to swap the positions of the points in
	** the contact manifold when, for example, the first
	** mesh is the incident collider and the second is the
	** reference collider.
	**
	** No heap allocation is invoked.
	*/

	const cMeshEdge *const referenceFaceFirstEdge = &reference->edges[reference->faces[referenceFaceIndex].edge];
	const cMeshEdge *referenceFaceEdge;

	const vec3 *const referenceFaceNormal = &reference->normals[referenceFaceIndex];
	const vec3 *const referenceFaceVertex = &reference->vertices[referenceFaceFirstEdge->start];

	// Find the incident face to clip against,
	// if the test index is insufficient.
	const cMeshEdge *incidentFaceFirstEdge = &incident->edges[cMeshFindIncidentClipFace(incident, referenceFaceNormal, &incidentFaceIndex)->edge];
	const cMeshEdge *incidentFaceEdge;

	const vec3 *const incidentFaceNormal = &incident->normals[incidentFaceIndex];
	const vec3 *const incidentFaceVertex = &incident->vertices[incidentFaceFirstEdge->start];

	const cMeshEdge *startRegion = NULL;
	const cMeshEdge *endRegionFinal = NULL;
	int swapEdgesFinal;

	cContact *contact = &cm->contacts[cm->contactNum];

	vec3 referenceFaceNormalInverse;

	// Incident edge loop variables.
	int startClipped;
	float depth;
	vec3 start;
	vec3 end;
	const cMeshEdge *startRegionOld;
	const cMeshEdge *endRegion;


	// Set the contact normal.
	// Used for calculating the distance of
	// penetrating vertices from the reference face.
	cm->normal = reference->normals[referenceFaceIndex];

	// Generate the contact tangents.
	cGenerateContactTangents(&cm->normal, &cm->tangents[0], &cm->tangents[1]);

	// Uses the inverse reference face normal when
	// projecting potential contact points.
	referenceFaceNormalInverse.x = -referenceFaceNormal->x;
	referenceFaceNormalInverse.y = -referenceFaceNormal->y;
	referenceFaceNormalInverse.z = -referenceFaceNormal->z;


	//
	// Loop through every edge of the incident face.
	//
	incidentFaceEdge = incidentFaceFirstEdge;
	MESH_CLIP_INCIDENT_EDGE_LOOP:

	startClipped = 0;
	startRegionOld = startRegion;
	endRegion = NULL;

	// Add the edge's starting vertex and get the next edge.
	if(incidentFaceEdge->face == incidentFaceIndex){
		// The next edge associated with this face is not a twin.
		start = incident->vertices[incidentFaceEdge->start];
		end = incident->vertices[incidentFaceEdge->end];
		incidentFaceEdge = &incident->edges[incidentFaceEdge->next];
	}else{
		// The next edge associated with this face is a twin.
		// Swap the start and end vertices around to
		// stay consistent with previous edges.
		start = incident->vertices[incidentFaceEdge->end];
		end = incident->vertices[incidentFaceEdge->start];
		incidentFaceEdge = &incident->edges[incidentFaceEdge->twinNext];
	}

	// Loop through every edge on the reference face
	// to find its adjacent faces. Clip the edge of the
	// incident mesh against each of the adjacent face's
	// normals.
	referenceFaceEdge = referenceFaceFirstEdge;
	do {

		// Clip the vertex with the normal of the
		// current twin edge's face.

		const cMeshEdge *referenceEdgeNext;

		const vec3 *adjacentFaceNormal;
		const vec3 *adjacentFaceVertex;

		int clipped;

		adjacentFaceVertex = &reference->vertices[referenceFaceEdge->start];
		if(referenceFaceEdge->face == referenceFaceIndex){
			// The next edge associated with this face is not a twin.
			adjacentFaceNormal = &reference->normals[referenceFaceEdge->twinFace];
			referenceEdgeNext = &reference->edges[referenceFaceEdge->next];
		}else{
			// The next edge associated with this face is a twin.
			adjacentFaceNormal = &reference->normals[referenceFaceEdge->face];
			referenceEdgeNext = &reference->edges[referenceFaceEdge->twinNext];
		}

		clipped = cMeshClipEdge(adjacentFaceNormal, adjacentFaceVertex, &start, &end);

		if(!clipped){
			startRegion = startRegionOld;
			goto MESH_CLIP_SKIP_CURRENT_EDGE;
		}

		if(clipped == 1){
			// The starting vertex was clipped,
			// so it may potentially be a new
			// contact point.
			startClipped = 1;
			// Save the final edge it was clipped
			// on as somewhere to end when adding
			// clipping region vertices.
			endRegion = referenceFaceEdge;
		}else{
			// The ending vertex was clipped, save
			// the final edge it was clipped on as
			// somewhere to start when adding
			// clipping region vertices.
			startRegion = referenceFaceEdge;
		}

		referenceFaceEdge = referenceEdgeNext;

	} while(referenceFaceEdge != referenceFaceFirstEdge);

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
		vec3Cross(incidentFaceNormal, &local, &edgeNormal);

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
				const cMeshEdge *const swap = startRegion;
				startRegion = endRegion;
				endRegion = swap;
			}
			// Add all the vertices between startRegion
			// and endRegion as contact points.
			if(cMeshClipPatchRegion(reference, referenceFaceIndex, incidentFaceNormal, incidentFaceVertex, startRegion, endRegion, offset, cm)){
				return;
			}
			startRegion = NULL;
		}

	}

	// The vertices that lie behind the reference face
	// are valid contact points. Check if the edge's
	// vertices satisfy this condition.
	// Begin by checking the ending vertex.
	depth = pointPlaneDistance(&referenceFaceNormalInverse, referenceFaceVertex, &end);
	if(depth >= 0.f){
		// Project the vertex onto the reference face.
		pointPlaneProjectR(&referenceFaceNormalInverse, referenceFaceVertex, &end, &contact->pointA+offset);
		*(&contact->pointB-offset) = end;
		contact->depth = depth;
		++cm->contactNum;
		if(cm->contactNum == COLLISION_MANIFOLD_MAX_CONTACT_POINTS){
			return;
		}
		++contact;
	}
	// Only potentially add the starting vertex if it was clipped.
	// The reason for this is because if it wasn't clipped, it
	// either has already been added or will be added later as an
	// ending vertex, and we don't want duplicate contact points.
	if(startClipped){
		depth = pointPlaneDistance(&referenceFaceNormalInverse, referenceFaceVertex, &start);
		if(depth >= 0.f){
			// Project the vertex onto the reference face.
			pointPlaneProjectR(&referenceFaceNormalInverse, referenceFaceVertex, &start, &contact->pointA+offset);
			*(&contact->pointB-offset) = start;
			contact->depth = depth;
			++cm->contactNum;
			if(cm->contactNum == COLLISION_MANIFOLD_MAX_CONTACT_POINTS){
				return;
			}
			++contact;
		}
	}

	MESH_CLIP_SKIP_CURRENT_EDGE:
	if(incidentFaceEdge != incidentFaceFirstEdge){
		goto MESH_CLIP_INCIDENT_EDGE_LOOP;
	}
	//
	// End incident face edge loop.
	//


	// If the start and final end edges are not null, we can
	// still add the vertices between the start edge and it.
	if(startRegion != NULL && endRegionFinal != NULL && startRegion != endRegionFinal){
		if(swapEdgesFinal){
			const cMeshEdge *const swap = startRegion;
			startRegion = endRegionFinal;
			endRegionFinal = swap;
		}
		if(cMeshClipPatchRegion(reference, referenceFaceIndex, incidentFaceNormal, incidentFaceVertex, startRegion, endRegionFinal, offset, cm)){
			return;
		}
	}

	// If no contacts were added, just add the reference face vertices.
	if(cm->contactNum == 0){
		cMeshClipPatchRegion(reference, referenceFaceIndex, incidentFaceNormal, incidentFaceVertex, referenceFaceFirstEdge, referenceFaceFirstEdge, offset, cm);
	}

}
static __FORCE_INLINE__ void cMeshClipFaceContactAlloc(const cMesh *const reference, const cMesh *const incident,
                                                       const cFaceIndex_t referenceFaceIndex, cFaceIndex_t incidentFaceIndex,
                                                       const size_t offset, cContactManifold *const restrict cm){

	/*
	** Generates a contact manifold by clipping the edges of
	** the incident face against the faces adjacent to the
	** reference face.
	**
	** Offset is used to swap the positions of the points in
	** the contact manifold when, for example, the first
	** mesh is the incident collider and the second is the
	** reference collider.
	**
	** Heap allocation is invoked.
	*/

	//alloca(incident->edgeMax * 2 * sizeof(vec3) * 2);
	vec3 *const vertices = memAllocate(incident->edgeMax * 2 * sizeof(vec3) * 2);

	// Make sure the allocation didn't fail.
	if(vertices != NULL){

		cContact *contact = &cm->contacts[cm->contactNum];

		const cMeshEdge *const referenceFaceFirstEdge = &reference->edges[reference->faces[referenceFaceIndex].edge];
		const cMeshEdge *referenceFaceEdge;

		const vec3 *const referenceFaceVertex = &reference->vertices[referenceFaceFirstEdge->start];
		const vec3 *const referenceFaceNormal = &reference->normals[referenceFaceIndex];



		// Find the incident face to clip against,
		// if the test index is insufficient.
		const cMeshEdge *incidentFaceFirstEdge = &incident->edges[cMeshFindIncidentClipFace(incident, referenceFaceNormal, &incidentFaceIndex)->edge];
		const cMeshEdge *incidentFaceEdge;

		// Pointer to the array with the vertices to be clipped.
		vec3 *vertexArray = vertices;
		// Pointer to the array with the clipped vertices.
		vec3 *vertexClipArray;
		vec3 *vertexArraySwap;

		const vec3 *vertex;
		vec3 *vertexLast;

		vec3 referenceFaceNormalInverse;


		// Set the contact normal.
		// Used for calculating the distance of
		// penetrating vertices from the reference face.
		cm->normal = *referenceFaceNormal;

		// Generate the contact tangents.
		cGenerateContactTangents(&cm->normal, &cm->tangents[0], &cm->tangents[1]);

		// Uses the inverse reference face normal when
		// projecting potential contact points.
		referenceFaceNormalInverse.x = -referenceFaceNormal->x;
		referenceFaceNormalInverse.y = -referenceFaceNormal->y;
		referenceFaceNormalInverse.z = -referenceFaceNormal->z;

		// Add the positions of all the vertices
		// for the incident edges to vertexArray.
		vertexLast = vertexArray;
		incidentFaceEdge = incidentFaceFirstEdge;

		do {

			// Add the edge's starting vertex and get the next edge.
			if(incidentFaceEdge->face == incidentFaceIndex){
				// The next edge associated with this face is not a twin.
				*vertexLast = incident->vertices[incidentFaceEdge->start];
				incidentFaceEdge = &incident->edges[incidentFaceEdge->next];
			}else{
				// The next edge associated with this face is a twin.
				// Swap the start and end vertices around to
				// stay consistent with previous edges.
				*vertexLast = incident->vertices[incidentFaceEdge->end];
				incidentFaceEdge = &incident->edges[incidentFaceEdge->twinNext];
			}

			++vertexLast;

		} while(incidentFaceEdge != incidentFaceFirstEdge);

		// Set the start of the clip array, which
		// is the end of the vertex array.
		vertexClipArray = vertexLast;


		// Loop through every edge on the reference face
		// to find its adjacent faces. Clip these against
		// the incident face vertices.
		referenceFaceEdge = referenceFaceFirstEdge;
		do {

			// Clip the vertex with the normal of the
			// current twin edge's face.

			vec3 *vertexClip;
			const vec3 *vertexPrevious;

			const vec3 *adjacentFaceNormal;
			const vec3 *adjacentFaceVertex;

			float startDistance;
			float endDistance;


			adjacentFaceVertex = &reference->vertices[referenceFaceEdge->start];
			if(referenceFaceEdge->face == referenceFaceIndex){
				// The next edge associated with this face is not a twin.
				adjacentFaceNormal = &reference->normals[referenceFaceEdge->twinFace];
				referenceFaceEdge = &reference->edges[referenceFaceEdge->next];
			}else{
				// The next edge associated with this face is a twin.
				adjacentFaceNormal = &reference->normals[referenceFaceEdge->face];
				referenceFaceEdge = &reference->edges[referenceFaceEdge->twinNext];
			}


			// Loop through every edge of the incident face,
			// clipping its vertices against the current
			// reference plane.
			vertex = vertexArray+1;
			vertexClip = vertexClipArray;
			vertexPrevious = vertexArray;
			startDistance = pointPlaneDistance(adjacentFaceNormal, adjacentFaceVertex, vertexPrevious);
			incidentFaceEdge = incidentFaceFirstEdge;

			while(vertex < vertexLast){

				/*
				clipped = cMeshClipEdgeAlloc(
					adjacentFaceNormal, adjacentFaceVertex,
					vertexPrevious, vertex, vertexClip
				);
				if(clipped > 0){
					++vertexClip;
					if(clipped == 1){
						// The starting vertex is in front of the plane
						// and the ending vertex is behind it. Add both.
						// The clipped starting vertex has already been
						// added, so add the ending vertex.
						*vertexClip = *vertex;
						++vertexClip;
					}
				}
				*/

				/*
				** Clips an edge intersecting a plane. If there is
				** no intersection, the function returns 0.
				** If the starting vertex was clipped, it returns 1.
				** If the ending vertex was clipped, it returns 2.
				*/
				endDistance = pointPlaneDistance(adjacentFaceNormal, adjacentFaceVertex, vertex);
				// Check if the starting vertex is behind the plane.
				if(startDistance <= 0.f){
					// If the starting vertex is behind the plane and
					// the ending vertex is in front of it, clip the
					// ending vertex. If both vertices are behind the
					// plane, there is no need to clip anything.
					if(endDistance > 0.f){
						vec3LerpR(vertexPrevious, vertex, startDistance / (startDistance - endDistance), vertexClip);
					}else{
						*vertexClip = *vertex;
					}
					++vertexClip;
				}else if(endDistance <= 0.f){
					// If the starting vertex is in front of the plane
					// and the ending vertex is behind it, clip the
					// starting vertex.
					vec3LerpR(vertexPrevious, vertex, startDistance / (startDistance - endDistance), vertexClip);
					// Now that the clipped starting vertex has been
					// added, add the ending vertex.
					++vertexClip;
					*vertexClip = *vertex;
					++vertexClip;
				}
				startDistance = endDistance;
				++vertex;
				++vertexPrevious;

			}

			// Final iteration between last and first vertices.
			/*
			clipped = cMeshClipEdgeAlloc(
				adjacentFaceNormal, adjacentFaceVertex,
				vertexPrevious, &vertexArray[0], vertexClip
			);
			if(clipped > 0){
				++vertexClip;
				if(clipped == 1){
					// The starting vertex is in front of the plane
					// and the ending vertex is behind it. Add both.
					// The clipped starting vertex has already been
					// added, so add the ending vertex.
					*vertexClip = vertexArray[0];
					++vertexClip;
				}
			}
			*/

			/*
			** Final edge clip iteration between last and first vertices.
			*/
			endDistance = pointPlaneDistance(adjacentFaceNormal, adjacentFaceVertex, vertexArray);
			// Check if the starting vertex is behind the plane.
			if(startDistance <= 0.f){
				// If the starting vertex is behind the plane and
				// the ending vertex is in front of it, clip the
				// ending vertex. If both vertices are behind the
				// plane, there is no need to clip anything.
				if(endDistance > 0.f){
					vec3LerpR(vertexPrevious, vertexArray, startDistance / (startDistance - endDistance), vertexClip);
				}else{
					*vertexClip = *vertexArray;
				}
				++vertexClip;
			}else if(endDistance <= 0.f){
				// If the starting vertex is in front of the plane
				// and the ending vertex is behind it, clip the
				// starting vertex.
				vec3LerpR(vertexPrevious, vertexArray, startDistance / (startDistance - endDistance), vertexClip);
				// Now that the clipped starting vertex has been
				// added, add the ending vertex.
				++vertexClip;
				*vertexClip = *vertexArray;
				++vertexClip;
			}


			// Calculate the new last vertex.
			// Since we increase vertexClip every time we add
			// a new vertex and we swap vertexArray and vertexClip,
			// vertexClip will be the new end of our array.
			vertexLast = vertexClip;

			// Swap vertexArray and vertexClipArray so that we clip
			// the vertices that were clipped during this iteration.
			vertexArraySwap = vertexArray;
			vertexArray = vertexClipArray;
			vertexClipArray = vertexArraySwap;

		} while(referenceFaceEdge != referenceFaceFirstEdge);


		// Loop through every vertex of the incident face,
		// checking which ones we can use as contact points.
		vertex = vertexArray;
		for(; vertex < vertexLast; ++vertex){
			const float depth = pointPlaneDistance(&referenceFaceNormalInverse, referenceFaceVertex, vertex);
			if(depth >= 0.f){
				// Project the vertex onto the reference face.
				*(&contact->pointB-offset) = *vertex;
				pointPlaneProjectR(&referenceFaceNormalInverse, referenceFaceVertex, vertex, &contact->pointA+offset);
				contact->depth = depth;
				++cm->contactNum;
				if(cm->contactNum == COLLISION_MANIFOLD_MAX_CONTACT_POINTS){
					break;
				}
				++contact;
			}
		}


		memFree(vertices);

	}

}

static __FORCE_INLINE__ void cMeshCollisionSHClipping(const cMesh *const restrict c1, const cMesh *const restrict c2, const cMeshPenetrationPlanes *const restrict planes, cContactManifold *const restrict cm){

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

		cMeshClipEdgeContact(c1, c2, planes->edge.edge1, planes->edge.edge2, cm);

	}else{

		// If the second face penetration depth is greater than
		// the first, create a face contact with it. Favours the
		// first hull as the reference collider and the second
		// as the incident collider.
		if(planes->face2.depth > planes->face1.depth * 0.95f){

			// Offset is sizeof(vec3) so pointA and
			// pointB in the contact manifold are swapped.
			cMeshClipFaceContactAlloc(c2, c1, planes->face2.index, planes->face1.index, sizeof(vec3), cm);

		}else{

			cMeshClipFaceContactAlloc(c1, c2, planes->face1.index, planes->face2.index, 0, cm);

		}

	}

}


static __FORCE_INLINE__ return_t cMeshCollisionSATFaceQuery(const cMesh *const restrict c1, const cMesh *const restrict c2, cMeshSHFaceHelper *const restrict r, cMeshSeparation *const restrict sc, const cSeparationFeature_t type){

	/*
	** Find the maximum separation distance between
	** the faces of the first and the vertices of the
	** second collider.
	*/

	/** Should perform calculations in the second collider's local space? **/
	/** Might not be worth it, as our physics colliders are very simple.  **/

	cFaceIndex_t i;
	const vec3 *n;
	const cMeshFace *f;

	for(i = 0, n = c1->normals, f = c1->faces; i < c1->faceNum; ++i, ++n, ++f){

		// Get the vertex in the second collider that's
		// farthest from the current face in the first
		// collider in the opposite direction that the
		// face is pointing.
		// After this, find the distance between them.
		cVertexIndex_t c2VertexIndex;
		const vec3 invNormal = {.x = -n->x,
		                        .y = -n->y,
		                        .z = -n->z};
		const float distance = pointPlaneDistance(n, &c1->vertices[c1->edges[f->edge].start], cMeshCollisionSupportIndex(c2, &invNormal, &c2VertexIndex));

		if(distance > 0.f){
			// Early exit, a separating axis has been found.
			// Cache the separating axis.
			if(sc != NULL){
				sc->type = type;
				sc->featureA = i;
				sc->featureB = c2VertexIndex;
			}
			return 0;
		}else if(distance > r->depth){
			r->depth = distance;
			r->index = i;
		}

	}

	return 1;

}

static __FORCE_INLINE__ return_t cMeshCollisionSATMinkowskiFace(const cMesh *const restrict c1, const cMesh *const restrict c2,
																const cMeshEdge *const restrict e1, const vec3 *const restrict e1InvDir,
																const cMeshEdge *const restrict e2, const vec3 *const restrict e2InvDir){

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

static __FORCE_INLINE__ float cMeshCollisionSATEdgeSeparation(const cMesh *const restrict c1, const cMesh *const restrict c2, const vec3 *const restrict centroid,
                                                              const cMeshEdge *const restrict e1, const vec3 *const restrict e1InvDir,
                                                              const cMeshEdge *const restrict e2, const vec3 *const restrict e2InvDir){

	/*
	** Check the distance between the two edges.
	*/

	/** Should perform calculations in the second collider's local space? **/
	/** Might not be worth it, as our physics colliders are very simple.  **/

	const vec3 *const pointA = &c1->vertices[e1->start];
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

static __FORCE_INLINE__ return_t cMeshCollisionSATEdgeQuery(const cMesh *c1, const cMesh *c2, const vec3 *centroid, cMeshSHEdgeHelper *r, cMeshSeparation *const restrict sc){

	/*
	** Find the maximum separation distance between
	** the edges of the first and second colliders.
	*/

	cEdgeIndex_t i;
	const cMeshEdge *e1;

	for(i = 0, e1 = c1->edges; i < c1->edgeNum; ++i, ++e1){

		cEdgeIndex_t j;
		const cMeshEdge *e2;

		// Get the inverse direction vector for the first collider's edge.
		vec3 e1InvDir;
		vec3SubVFromVR(&c1->vertices[e1->start], &c1->vertices[e1->end], &e1InvDir);

		for(j = 0, e2 = c2->edges; j < c2->edgeNum; ++j, ++e2){

			// Get the inverse direction vector for the second collider's edge.
			vec3 e2InvDir;
			vec3SubVFromVR(&c2->vertices[e2->start], &c2->vertices[e2->end], &e2InvDir);

			// The inverse direction vectors are used in place of the cross product
			// between the edge's face normal and its twin's face normal.
			if(cMeshCollisionSATMinkowskiFace(c1, c2, e1, &e1InvDir, e2, &e2InvDir)){

				// Now that we have a Minkowski face, we can
				// get the distance between the two edges.
				const float distance = cMeshCollisionSATEdgeSeparation(c1, c2, centroid, e1, &e1InvDir, e2, &e2InvDir);
				if(distance > 0.f){
					// Early exit, a separating axis has been found.
					// Cache the separating axis.
					if(sc != NULL){
						sc->type = COLLIDER_MESH_SEPARATION_FEATURE_EDGE;
						sc->featureA = i;
						sc->featureB = j;
					}
					return 0;
				}else if(distance > r->depth){
					// If the distance between these two edges
					// is larger than the previous greatest
					// distance, record it.
					r->depth = distance;
					r->edge1 = e1;
					r->edge2 = e2;
				}

			}

		}

	}

	return 1;

}

return_t cMeshCollisionSAT(const cMesh *const restrict c1, const cMesh *const restrict c2, const vec3 *const restrict centroid, cMeshSeparation *const restrict sc, cContactManifold *const restrict cm){

	/*
	** Implementation of the separating axis theorem
	** using Minkowski space and Gauss map optimizations.
	**
	** Credit to Erin Catto for the original idea and
	** Dirk Gregorius for his amazing presentation at
	** GDC 2013.
	*/

	cMeshPenetrationPlanes planes;
	cMeshPenetrationPlanesInit(&planes);

	if(cMeshCollisionSATFaceQuery(c1, c2, &planes.face1, sc, COLLIDER_MESH_SEPARATION_FEATURE_FACE_1)){
		if(cMeshCollisionSATFaceQuery(c2, c1, &planes.face2, sc, COLLIDER_MESH_SEPARATION_FEATURE_FACE_2)){
			if(cMeshCollisionSATEdgeQuery(c1, c2, centroid, &planes.edge, sc)){
				if(cm != NULL){
					cMeshCollisionSHClipping(c1, c2, &planes, cm);
				}
				return 1;
			}
		}
	}

	return 0;

}

static __FORCE_INLINE__ return_t cMeshSeparationSATFaceQuery(const cMesh *const restrict c1, const cMesh *const restrict c2, const cMeshSeparation *const restrict sc){
	return pointPlaneDistance(&c1->normals[sc->featureA], &c1->vertices[c1->edges[c1->faces[sc->featureA].edge].start], &c2->vertices[sc->featureB]) <= 0.f;
}

static __FORCE_INLINE__ return_t cMeshSeparationSATEdgeQuery(const cMesh *const restrict c1, const cMesh *const restrict c2, const vec3 *const restrict centroid, const cMeshSeparation *const restrict sc){
	const cMeshEdge *const e1 = &c1->edges[sc->featureA];
	const cMeshEdge *const e2 = &c2->edges[sc->featureB];
	vec3 e1InvDir, e2InvDir;
	vec3SubVFromVR(&c1->vertices[e1->start], &c1->vertices[e1->end], &e1InvDir);
	vec3SubVFromVR(&c2->vertices[e2->start], &c2->vertices[e2->end], &e2InvDir);
	return cMeshCollisionSATMinkowskiFace(c1, c2, e1, &e1InvDir, e2, &e2InvDir) == 0 || cMeshCollisionSATEdgeSeparation(c1, c2, centroid, e1, &e1InvDir, e2, &e2InvDir) <= 0.f;
}

return_t cMeshSeparationSAT(const cMesh *const restrict c1, const cMesh *const restrict c2, const vec3 *const restrict centroid, const cMeshSeparation *const restrict sc){
	// Check the separation cache for possible early exits.
	if(sc->type != COLLIDER_MESH_SEPARATION_FEATURE_NULL){
		switch(sc->type){
			case COLLIDER_MESH_SEPARATION_FEATURE_FACE_1:
				if(cMeshSeparationSATFaceQuery(c1, c2, sc) == 0){
					return 1;
				}
			break;
			case COLLIDER_MESH_SEPARATION_FEATURE_FACE_2:
				if(cMeshSeparationSATFaceQuery(c2, c1, sc) == 0){
					return 1;
				}
			break;
			case COLLIDER_MESH_SEPARATION_FEATURE_EDGE:
				if(cMeshSeparationSATEdgeQuery(c1, c2, centroid, sc) == 0){
					return 1;
				}
			break;
		}
	}
	return 0;
}

typedef struct {
	// The Minkowski "difference" of the two support points.
	vec3 v;
	// The support points used to generate v.
	const vec3 *s1;
	const vec3 *s2;
} cMeshSupportVertex;

typedef struct {
	// Three vertices and a normal.
	cMeshSupportVertex vertex[3];
	vec3 normal;
} cMeshEPAFaceHelper;

typedef struct {
	// An edge that will be removed.
	cMeshSupportVertex vertex[2];
} cMeshEPAEdgeHelper;

static __FORCE_INLINE__ void cMeshEPAFaceInit(cMeshEPAFaceHelper *const restrict face, const cMeshSupportVertex *const restrict v0, const cMeshSupportVertex *const restrict v1, const cMeshSupportVertex *const restrict v2){
	// Create a new face.
	face->vertex[0] = *v0;
	face->vertex[1] = *v1;
	face->vertex[2] = *v2;
	faceNormal(&v0->v, &v1->v, &v2->v, &face->normal);
	vec3NormalizeFastAccurate(&face->normal);
}

static __FORCE_INLINE__ void cMeshEPAEdgeInit(cMeshEPAEdgeHelper *const restrict edge, const cMeshSupportVertex *const restrict v0, const cMeshSupportVertex *const restrict v1){
	// Create a new face.
	edge->vertex[0] = *v0;
	edge->vertex[1] = *v1;
}

static __FORCE_INLINE__ void cMeshCollisionMinkowskiSupport(const cMesh *const restrict c1, const cMesh *const restrict c2, const vec3 *const restrict axis, cMeshSupportVertex *const restrict r){
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
	r->s1 = cMeshCollisionSupport(c1, axis);
	r->s2 = cMeshCollisionSupport(c2, &axisNegative);
	// Get the "Minkowski Difference" of the two support points.
	vec3SubVFromVR(r->s1, r->s2, &r->v);
}

static __FORCE_INLINE__ void cMeshCollisionEPA(const cMesh *const restrict c1, const cMesh *const restrict c2, cMeshSupportVertex *const restrict simplex, cContactManifold *const restrict cm){

	/*
	** Implementation of the expanding polytope algorithm. Extrapolates
	** additional collision information from two convex polygons and the
	** simplex generated by the Gilbert-Johnson-Keerthi algorithm.
	*/

	int i, j;
	cMeshEPAFaceHelper *f;
	cMeshEPAEdgeHelper *e;
	const cMeshEPAFaceHelper *closestFace;
	float distance;
	cMeshSupportVertex point;

	// Create the face and edge arrays.
	cMeshEPAFaceHelper faces[COLLISION_MAX_FACE_NUM_EPA];
	cMeshEPAFaceHelper *faceLast = &faces[4];
	cMeshEPAEdgeHelper edges[COLLISION_MAX_EDGE_NUM_EPA];
	cMeshEPAEdgeHelper *edgeLast = edges;

	// Ensure the vertex winding order of the simplex is CCW.
	vec3 temp1, temp2, temp3;
	vec3SubVFromVR(&simplex[1].v, &simplex[3].v, &temp1);
	vec3SubVFromVR(&simplex[2].v, &simplex[3].v, &temp2);
	vec3Cross(&temp1, &temp2, &temp3);
	vec3SubVFromVR(&simplex[0].v, &simplex[3].v, &temp1);
	if(vec3Dot(&temp1, &temp3) < 0.f){
		// If it's not, swap two vertices.
		cMeshSupportVertex swap = simplex[0];
		simplex[0] = simplex[1];
		simplex[1] = swap;
	}

	// Generate a starting tetrahedron from the given vertices.
	cMeshEPAFaceInit(&faces[0], &simplex[0], &simplex[1], &simplex[2]);  // Face 1 - ABC
	cMeshEPAFaceInit(&faces[1], &simplex[0], &simplex[2], &simplex[3]);  // Face 2 - ACD
	cMeshEPAFaceInit(&faces[2], &simplex[0], &simplex[3], &simplex[1]);  // Face 3 - ADB
	cMeshEPAFaceInit(&faces[3], &simplex[1], &simplex[3], &simplex[2]);  // Face 4 - BDC

	/*
	** Find the edge on the Minkowski difference closest to the origin.
	*/
	for(i = 0; i < COLLISION_MAX_ITERATIONS; ++i){

		/*
		** Find the closest face to the origin.
		*/

		f = &faces[1];
		closestFace = faces;
		distance = vec3Dot(&faces[0].vertex[0].v, &faces[0].normal);
		for(; f < faceLast; ++f){

			// Get the minimum distance between the current face and the origin.
			// This is the dot product between the face's normal and one of its vertices.
			const float tempDistance = vec3Dot(&f->vertex[0].v, &f->normal);

			if(tempDistance < distance){
				// New closest face found.
				closestFace = f;
				distance = tempDistance;
			}

		}

		/*
		** Search in the direction of the closest face to find
		** a point on the edge of the Minkowski difference.
		*/
		cMeshCollisionMinkowskiSupport(c1, c2, &closestFace->normal, &point);
		if(vec3Dot(&point.v, &closestFace->normal) - distance < COLLISION_DISTANCE_THRESHOLD){
			// The new point is not much further from the origin, break from the loop.
			break;
		}

		/*
		** For each face, if it is facing the new point, remove it and
		** create some new faces to join the new point to the simplex.
		*/
		f = faces;
		for(; f < faceLast; ++f){

			// Check if the face's normal is pointing in the direction of the new point.
			// If it is, the face can be "seen" by the new point.
			vec3 dif;
			vec3SubVFromVR(&point.v, &f->vertex[0].v, &dif);
			if(vec3Dot(&f->normal, &dif) > 0.f){

				// Add each of the face's edges to the edge array,
				// ready to be removed in the next section.
				for(j = 0; j < 3; ++j){

					int add = 1;
					cMeshEPAEdgeHelper newEdge;
					cMeshEPAEdgeInit(&newEdge, &f->vertex[j], &f->vertex[(j+1)%3]);

					// Check if the new edge has already been added to the edge array.
					// An edge should never appear more than twice, so if it has already
					// been added both instances may be removed.
					e = edges;
					for(; e < edgeLast; ++e){

						// Because a consistent CCW winding order is used, any additional
						// occurrence of the edge will have reversed vertex indices.
						if(memcmp(&newEdge.vertex[0], &e->vertex[1], sizeof(vec3)) == 0 &&
						   memcmp(&newEdge.vertex[1], &e->vertex[0], sizeof(vec3)) == 0){

							// Replace this edge with the last edge in the edge array
							// in order to remove it.
							--edgeLast;
							*e = *edgeLast;
							add = 0;
							break;

						}

						++e;
					}

					// If the new edge did not already exist, add it to the edge array.
					if(add){
						if(edgeLast >= &edges[COLLISION_MAX_EDGE_NUM_EPA]){
							break;
						}
						*edgeLast = newEdge;
						++edgeLast;
					}
				}

				// Replace this face with the last face in the face array
				// in order to remove it.
				--faceLast;
				*f = *faceLast;
				--f;

			}

		}

		// Patch the holes in the polytope using the new point.
		--faceLast;
		e = edges;
		for(; e < edgeLast; ++e){

			if(faceLast >= &faces[COLLISION_MAX_FACE_NUM_EPA]){
				break;
			}

			cMeshEPAFaceInit(faceLast, &e->vertex[0], &e->vertex[1], &point);

			// Ensure the vertex winding order of the simplex is CCW.
			if(vec3Dot(&faceLast->vertex[0].v, &faceLast->normal) < 0.f){
				// If it's not, swap two vertices.
				cMeshSupportVertex swap = faceLast->vertex[0];
				faceLast->vertex[0] = faceLast->vertex[1];
				faceLast->vertex[1] = swap;
				// Invert the face's normal as well.
				faceLast->normal.x = -faceLast->normal.x;
				faceLast->normal.y = -faceLast->normal.y;
				faceLast->normal.z = -faceLast->normal.z;
			}

			++faceLast;

		}
		++faceLast;
		edgeLast = edges;

	}

	/*
	** Now we can extrapolate various collision information
	** using face, normal and distance.
	*/

	if(cm->contactNum < COLLISION_MANIFOLD_MAX_CONTACT_POINTS-1){

		const float depth = distance*distance;

		// Store the contact normal and penetration depth.
		vec3 contact = closestFace->normal;
		cm->normal = contact;

		// Generate the contact tangents.
		cGenerateContactTangents(&contact, &cm->tangents[0], &cm->tangents[1]);

		// Project the origin onto the closest face.
		vec3MultVByS(&contact, distance);

		// Calculate the Barycentric coordinates of the projected origin.
		barycentric(&closestFace->vertex[0].v, &closestFace->vertex[1].v, &closestFace->vertex[2].v, &contact);

		// The contact point on c1 is the linear combination of the original
		// vertices in c1 used to generate the support vertices for the
		// closest face, using the barycentric coordinates stored in contact.
		vec3CombineLinear(closestFace->vertex[0].s1,
						  closestFace->vertex[1].s1,
						  closestFace->vertex[2].s1,
						  contact.x, contact.y, contact.z, &cm->contacts[cm->contactNum].pointA);

		// The contact point on c2 is the linear combination of the original
		// vertices in c2 used to generate the support vertices for the
		// closest face, using the barycentric coordinates stored in contact.
		vec3CombineLinear(closestFace->vertex[0].s2,
						  closestFace->vertex[1].s2,
						  closestFace->vertex[2].s2,
						  contact.x, contact.y, contact.z, &cm->contacts[cm->contactNum].pointB);

		cm->contacts[cm->contactNum].depth = depth;
		++cm->contactNum;

	}

}

static __FORCE_INLINE__ void cMeshCollisionGJKTriangle(int *const restrict simplexVertices, cMeshSupportVertex simplex[4], vec3 *const restrict axis){

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

static __FORCE_INLINE__ return_t cMeshCollisionGJKTetrahedron(int *const restrict simplexVertices, cMeshSupportVertex *const restrict simplex, vec3 *const restrict axis){

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

return_t cMeshCollisionGJK(const cMesh *const restrict c1, const vec3 *const restrict c1c, const cMesh *const restrict c2, const vec3 *const restrict c2c, cContactManifold *const restrict cm){

	/*
	** Implementation of the Gilbert-Johnson-Keerthi distance algorithm,
	** which is only compatible with convex polytopes.
	*/

	int i;
	cMeshSupportVertex simplex[4];

	// Set the current number of vertices of our simplex. We will be
	// creating a line segment with vertices B and C before the main
	// loop, so we can set this to 2 preemptively.
	int simplexVertices = 2;

	// The first direction to check in is the direction of body2 from body1.
	vec3 axis = {.x = c2c->x - c1c->x,
	             .y = c2c->y - c1c->y,
	             .z = c2c->z - c1c->z};

	// Create an initial vertex for the simplex.
	cMeshCollisionMinkowskiSupport(c1, c2, &axis, &simplex[2]);

	// Create another vertex to form a line segment. B should be
	// a vertex in the opposite direction of C from the origin.
	axis.x = -simplex[2].v.x;
	axis.y = -simplex[2].v.y;
	axis.z = -simplex[2].v.z;
	cMeshCollisionMinkowskiSupport(c1, c2, &axis, &simplex[1]);

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
		cMeshCollisionMinkowskiSupport(c1, c2, &axis, &simplex[0]);
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
			cMeshCollisionGJKTriangle(&simplexVertices, simplex, &axis);
		}else if(cMeshCollisionGJKTetrahedron(&simplexVertices, simplex, &axis)){
			// The origin lies within our simplex, so we know there was
			// a collision.
			if(cm != NULL){
				cMeshCollisionEPA(c1, c2, &simplex[0], cm);
			}
			return 1;
		}

	}

	return 0;

}

void cMeshDelete(cMesh *const restrict cm){
	if(cm->vertices != NULL){
		memFree(cm->vertices);
	}
	if(cm->normals != NULL){
		memFree(cm->normals);
	}
	if(cm->faces != NULL){
		memFree(cm->faces);
	}
	if(cm->edges != NULL){
		memFree(cm->edges);
	}
}
