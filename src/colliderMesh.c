#include "collision.h"
#include "memoryManager.h"
#include "helpersMath.h"
#include "constantsMath.h"
#include "inline.h"
#include <string.h>

#if defined(_MSC_VER) || defined(__MINGW32__)
	#include <malloc.h>
#else
	#include <alloca.h>
#endif

#define COLLISION_MAX_ITERATIONS 64
#define COLLISION_MAX_FACE_NUM_EPA 64
#define COLLISION_MAX_EDGE_NUM_EPA 32

#define COLLISION_DISTANCE_THRESHOLD 0.0001f
#define COLLISION_PARALLEL_THRESHOLD 0.005f
#define COLLISION_PARALLEL_THRESHOLD_RECIPROCAL (1.f/COLLISION_PARALLEL_THRESHOLD)
#define COLLISION_PARALLEL_THRESHOLD_SQUARED    (COLLISION_PARALLEL_THRESHOLD*COLLISION_PARALLEL_THRESHOLD)

#define COLLISION_TOLERANCE_COEFFICIENT 0.95f
#define COLLISION_TOLERANCE_TERM        0.025f  // 0.5f * PHYSICS_PENETRATION_SLOP

void cMeshInit(cMesh *const restrict c){
	c->vertexNum = 0;
	c->vertices = NULL;
	c->faceNum = 0;
	c->normals = NULL;
	c->edgeMax = 0;
	c->faces = NULL;
	c->edgeNum = 0;
	c->edges = NULL;
}

return_t cMeshInstantiate(cMesh *const restrict instance, const cMesh *const restrict local){

	cVertexIndex_t vertexArraySize;
	cFaceIndex_t normalArraySize;

	vertexArraySize = local->vertexNum * sizeof(vec3);
	instance->vertices = memAllocate(vertexArraySize);
	if(instance->vertices == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	normalArraySize = local->faceNum * sizeof(vec3);
	instance->normals = memAllocate(normalArraySize);
	if(instance->normals == NULL){
		/** Memory allocation failure. **/
		memFree(instance->vertices);
		return -1;
	}

	instance->vertexNum = local->vertexNum;
	instance->edgeMax   = local->edgeMax;
	instance->faceNum   = local->faceNum;
	instance->edgeNum   = local->edgeNum;

	memcpy(instance->vertices, local->vertices, vertexArraySize);
	memcpy(instance->normals,  local->normals,  normalArraySize);

	// Re-use the faces and edges arrays. Vertices and
	// normals, however, are modified each update when the
	// collider's configuration changes.
	instance->faces = local->faces;
	instance->edges = local->edges;

	return 1;

}

__FORCE_INLINE__ void cMeshCentroidFromPosition(cMesh *const restrict c, const cMesh *const restrict l, const vec3 *const position, const quat *const orientation, const vec3 *const scale){
	/*
	** Extrapolate the mesh's centroid from a configuration.
	*/
	quatRotateVec3FastR(orientation, &l->centroid, &c->centroid);
	vec3MultVByV(&c->centroid, scale);
	vec3AddVToV(&c->centroid, position);
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
	float penetrationDepth;
	cFaceIndex_t index;
} cMeshSHFaceHelper;

typedef struct {
	float penetrationDepth;
	cEdgeIndex_t edge1;
	cEdgeIndex_t edge2;
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

typedef struct {
	vec3 v;
	cContactKey key;
} cMeshClipVertex;

static __FORCE_INLINE__ void cMeshSHFaceInit(cMeshSHFaceHelper *e){
	e->penetrationDepth = -INFINITY;
	e->index = (cFaceIndex_t)-1;
}

static __FORCE_INLINE__ void cMeshSHEdgeInit(cMeshSHEdgeHelper *e){
	e->penetrationDepth = -INFINITY;
	e->edge1 = (cEdgeIndex_t)-1;
	e->edge2 = (cEdgeIndex_t)-1;
}

static __FORCE_INLINE__ void cMeshPenetrationPlanesInit(cMeshPenetrationPlanes *planes){
	cMeshSHFaceInit(&planes->face1);
	cMeshSHFaceInit(&planes->face2);
	cMeshSHEdgeInit(&planes->edge);
}

static __HINT_INLINE__ void cMeshClipEdgeContact(const cMesh *const reference, const cMesh *const incident,
                                                 const cEdgeIndex_t referenceEdgeIndex, const cEdgeIndex_t incidentEdgeIndex,
                                                 cMeshContact *const restrict cm){

	const cMeshEdge *const referenceEdge = &reference->edges[referenceEdgeIndex];
	const vec3 *const referenceEdgeStart = &reference->vertices[referenceEdge->start];
	const vec3 *const referenceEdgeEnd   = &reference->vertices[referenceEdge->end];

	const cMeshEdge *const incidentEdge = &incident->edges[incidentEdgeIndex];
	const vec3 *const incidentEdgeStart = &incident->vertices[incidentEdge->start];
	const vec3 *const incidentEdgeEnd   = &incident->vertices[incidentEdge->end];

	vec3 incidentPoint;

	cMeshContactPoint *contact = cm->contacts;

	// Get the closest points on the line segments.
	segmentClosestPoints(referenceEdgeStart, referenceEdgeEnd, incidentEdgeStart, incidentEdgeEnd, &contact->point, &incidentPoint);

	// Get the offset of the incident contact from
	// the reference contact. This will be halved
	// as one of our contact points.
	vec3SubVFromVR(&contact->point, &incidentPoint, &contact->normal);

	// Calculate a contact point.
	contact->penetrationDepth = vec3Magnitude(&contact->normal);

	// Convert the contact normal to a unit vector
	// using the magnitude we calculated earlier.
	vec3MultVByS(&contact->normal, 1.f/contact->penetrationDepth);

	#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
	contact->key.edgeA = referenceEdgeIndex;
	contact->key.edgeB = incidentEdgeIndex;
	#else
	contact->key.inEdgeR = referenceEdgeIndex;
	contact->key.outEdgeR = referenceEdgeIndex;
	contact->key.inEdgeI = incidentEdgeIndex;
	contact->key.outEdgeI = incidentEdgeIndex;
	#endif

	cm->contactNum = 1;

}

static __HINT_INLINE__ const cMeshFace *cMeshFindIncidentClipFace(const cMesh *const incident, const vec3 *const restrict referenceNormal, cFaceIndex_t *const restrict incidentFaceIndex){

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

static __HINT_INLINE__ void cMeshReduceManifold(const cMeshClipVertex *const vertices, const cMeshClipVertex *const verticesLast, const float *const depths,
                                                const vec3 *const restrict planeNormal, const vec3 *const restrict planeVertex,
                                                const int offset, cMeshContact *const restrict cm){

	/*
	** Reduce the manifold to the combination of
	** contacts that provides the greatest area.
	*/

	cMeshContactPoint *contact = cm->contacts;

	const cMeshClipVertex *pointBest;
	const float *depthBest;
	float distanceBest;

	const cMeshClipVertex *pointWorst;
	const float *depthWorst;
	float distanceWorst;

	const cMeshClipVertex *point;
	const float *depth;

	const cMeshClipVertex *first;
	const cMeshClipVertex *second;

	vec3 relative;
	vec3 normal;


	// Find the first two contacts.
	// The first will be the farthest point from the origin and
	// the second will be the farthest point from the first.
	pointBest = vertices;
	depthBest = depths;
	distanceBest = vec3Dot(&vertices->v, &vertices->v);

	pointWorst = vertices;
	depthWorst = depths;
	distanceWorst = distanceBest;

	point = vertices+1;
	depth = depths+1;

	// Find the points with the greatest and smallest distances.
	for(; point < verticesLast; ++point, ++depth){
		const float distance = vec3Dot(&point->v, &point->v);
		if(distance > distanceBest){
			pointBest = point;
			depthBest = depth;
			distanceBest = distance;
		}else if(distance < distanceWorst){
			pointWorst = point;
			depthWorst = depth;
			distanceWorst = distance;
		}
	}

	first = pointBest;
	second = pointWorst;

	if(offset == 0){

		// Add the first two contact points.
		contact->point = pointBest->v;
		contact->penetrationDepth = *depthBest;
		contact->key = pointBest->key;
		contact->normal = *planeNormal;
		++contact;

		contact->point = pointWorst->v;
		contact->penetrationDepth = *depthWorst;
		contact->key = pointWorst->key;
		contact->normal = *planeNormal;
		++contact;

	}else{

		// Add the first two contact points.
		contact->point = pointBest->v;
		contact->penetrationDepth = *depthBest;
		#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
		contact->key.edgeA = pointBest->key.edgeA;
		contact->key.edgeB = pointBest->key.edgeB;
		#else
		contact->key.inEdgeR  = pointBest->key.inEdgeR;
		contact->key.outEdgeR = pointBest->key.outEdgeR;
		contact->key.inEdgeI  = pointBest->key.inEdgeI;
		contact->key.outEdgeI = pointBest->key.outEdgeI;
		#endif
		vec3NegateR(planeNormal, &contact->normal);
		++contact;

		contact->point = pointWorst->v;
		contact->penetrationDepth = *depthWorst;
		#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
		contact->key.edgeA = pointWorst->key.edgeA;
		contact->key.edgeB = pointWorst->key.edgeB;
		#else
		contact->key.inEdgeR  = pointWorst->key.inEdgeR;
		contact->key.outEdgeR = pointWorst->key.outEdgeR;
		contact->key.inEdgeI  = pointWorst->key.inEdgeI;
		contact->key.outEdgeI = pointWorst->key.outEdgeI;
		#endif
		vec3NegateR(planeNormal, &contact->normal);
		++contact;

	}


	// Now find the two points farthest in the two directions
	// perpendicular to that of the "edge" we've just created.
	// This will be our new search normal.
	// These two vectors will also work as our contact tangents.
	vec3SubVFromVR(&pointBest->v, &pointWorst->v, &normal);
	vec3Cross2(planeNormal, &normal);

	pointBest = vertices;
	vec3SubVFromVR(&vertices->v, &first->v, &relative);
	distanceBest = vec3Dot(&normal, &relative);

	pointWorst = vertices;
	distanceWorst = distanceBest;

	point = vertices+1;

	// Find the points with the greatest and smallest distances.
	for(; point < verticesLast; ++point){
		if(point != first && point != second){
			vec3SubVFromVR(&point->v, &first->v, &relative);
			const float distance = vec3Dot(&normal, &relative);
			if(distance > distanceBest){
				pointBest = point;
				distanceBest = distance;
			}else if(distance < distanceWorst){
				pointWorst = point;
				distanceWorst = distance;
			}
		}
	}

	// Add the last two contact points.
	if(offset == 0){

		// Add the first two contact points.
		contact->point = pointBest->v;
		contact->penetrationDepth = *depthBest;
		contact->key = pointBest->key;
		contact->normal = *planeNormal;
		++contact;

		contact->point = pointWorst->v;
		contact->penetrationDepth = *depthWorst;
		contact->key = pointWorst->key;
		contact->normal = *planeNormal;
		++contact;

	}else{

		// Add the first two contact points.
		contact->point = pointBest->v;
		contact->penetrationDepth = *depthBest;
		#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
		contact->key.edgeA = pointBest->key.edgeA;
		contact->key.edgeB = pointBest->key.edgeB;
		#else
		contact->key.inEdgeR  = pointBest->key.inEdgeR;
		contact->key.outEdgeR = pointBest->key.outEdgeR;
		contact->key.inEdgeI  = pointBest->key.inEdgeI;
		contact->key.outEdgeI = pointBest->key.outEdgeI;
		#endif
		vec3NegateR(planeNormal, &contact->normal);
		++contact;

		contact->point = pointWorst->v;
		contact->penetrationDepth = *depthWorst;
		#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
		contact->key.edgeA = pointWorst->key.edgeA;
		contact->key.edgeB = pointWorst->key.edgeB;
		#else
		contact->key.inEdgeR  = pointWorst->key.inEdgeR;
		contact->key.outEdgeR = pointWorst->key.outEdgeR;
		contact->key.inEdgeI  = pointWorst->key.inEdgeI;
		contact->key.outEdgeI = pointWorst->key.outEdgeI;
		#endif
		vec3NegateR(planeNormal, &contact->normal);

	}

}

static __HINT_INLINE__ void cMeshClipFaceContact(const cMesh *const reference, const cMesh *const incident,
                                                 const cFaceIndex_t referenceFaceIndex, cFaceIndex_t incidentFaceIndex,
                                                 const int offset, cMeshContact *const restrict cm){

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
	** This method is capable of performing manifold reduction.
	*/

	// Allocate two arrays of vertices for each edge on the face.
	cMeshClipVertex *const vertices =
	#ifndef COLLIDER_MESH_SAT_USE_HEAP_ALLOCATION
	alloca(incident->edgeMax * 2 * sizeof(cMeshClipVertex) * 2);
	#else
	memAllocate(incident->edgeMax * 2 * sizeof(cMeshClipVertex) * 2);
	// Make sure the allocation didn't fail.
	if(vertices != NULL){
	#endif

		cMeshContactPoint *contact = cm->contacts;

		const cEdgeIndex_t referenceFaceFirstEdgeIndex = reference->faces[referenceFaceIndex].edge;
		const cMeshEdge *const referenceFaceFirstEdge = &reference->edges[referenceFaceFirstEdgeIndex];
		const cMeshEdge *referenceFaceEdge;

		const vec3 *const referenceFaceVertex = &reference->vertices[referenceFaceFirstEdge->start];
		const vec3 *const referenceFaceNormal = &reference->normals[referenceFaceIndex];

		// Find the incident face to clip against,
		// if the test index is insufficient.
		const cEdgeIndex_t incidentFaceFirstEdgeIndex = cMeshFindIncidentClipFace(incident, referenceFaceNormal, &incidentFaceIndex)->edge;
		const cMeshEdge *incidentFaceFirstEdge = &incident->edges[incidentFaceFirstEdgeIndex];
		const cMeshEdge *incidentFaceEdge;

		// Pointer to the array with the vertices to be clipped.
		cMeshClipVertex *vertexArray = vertices;
		union {
			cMeshClipVertex *vertices;
			float *depths;
		} clipArray;
		cMeshClipVertex *vertexArraySwap;

		const cMeshClipVertex *vertex;
		cMeshClipVertex *vertexLast;

		cMeshClipVertex *vertexNext;
		float *depthNext;

		cEdgeIndex_t inEdgeIndex;
		cEdgeIndex_t outEdgeIndex;


		// Pointer to the array with the clipped vertices. Starts halfway through the vertex array.
		clipArray.vertices = vertices+incident->edgeMax*2;


		// Set the contact normal.
		// Used for calculating the distance of
		// penetrating vertices from the reference face.
		// Contact tangents are calculated later on.
		cm->contactNum = 0;


		// Add the positions of all the vertices
		// for the incident edges to vertexArray.
		vertexLast = vertexArray;
		inEdgeIndex = 0;
		outEdgeIndex = incidentFaceFirstEdgeIndex;
		incidentFaceEdge = incidentFaceFirstEdge;

		for(;;){

			#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
			vertexLast->key.edgeA = inEdgeIndex;
			vertexLast->key.edgeB = outEdgeIndex;
			#else
			vertexLast->key.inEdgeR = (cEdgeIndex_t)-1;
			vertexLast->key.outEdgeR = (cEdgeIndex_t)-1;
			vertexLast->key.inEdgeI = inEdgeIndex;
			vertexLast->key.outEdgeI = outEdgeIndex;
			#endif

			inEdgeIndex = outEdgeIndex;

			// Add the edge's starting vertex and get the next edge.
			if(incidentFaceEdge->face == incidentFaceIndex){
				// The next edge associated with this face is not a twin.
				vertexLast->v = incident->vertices[incidentFaceEdge->start];
				outEdgeIndex = incidentFaceEdge->next;
			}else{
				// The next edge associated with this face is a twin.
				// Swap the start and end vertices around to
				// stay consistent with previous edges.
				vertexLast->v = incident->vertices[incidentFaceEdge->end];
				outEdgeIndex = incidentFaceEdge->twinNext;
			}
			incidentFaceEdge = &incident->edges[outEdgeIndex];

			if(incidentFaceEdge == incidentFaceFirstEdge){
				break;
			}

			++vertexLast;

		}

		// Set the first incident vertex's "in-edge".
		#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
		vertexArray->key.edgeA = inEdgeIndex;
		#else
		vertexArray->key.inEdgeI = inEdgeIndex;
		#endif


		// Loop through every edge on the reference face
		// to find its adjacent faces. Clip these against
		// the incident face vertices.
		referenceFaceEdge = referenceFaceFirstEdge;
		inEdgeIndex = referenceFaceFirstEdgeIndex;
		do {

			// Clip the vertex with the normal of the
			// current twin edge's face.

			cMeshClipVertex *vertexClip;
			const cMeshClipVertex *vertexPrevious;

			const vec3 *adjacentFaceNormal;
			const vec3 *adjacentFaceVertex;

			float startDistance;
			float endDistance;


			adjacentFaceVertex = &reference->vertices[referenceFaceEdge->start];
			if(referenceFaceEdge->face == referenceFaceIndex){
				// The next edge associated with this face is not a twin.
				adjacentFaceNormal = &reference->normals[referenceFaceEdge->twinFace];
				outEdgeIndex = referenceFaceEdge->next;
			}else{
				// The next edge associated with this face is a twin.
				adjacentFaceNormal = &reference->normals[referenceFaceEdge->face];
				outEdgeIndex = referenceFaceEdge->twinNext;
			}
			referenceFaceEdge = &reference->edges[outEdgeIndex];


			// Loop through every edge of the incident face,
			// clipping its vertices against the current
			// reference plane.
			vertex = vertexArray;
			vertexClip = clipArray.vertices;
			vertexPrevious = vertexLast;
			startDistance = pointPlaneDistance(adjacentFaceNormal, adjacentFaceVertex, &vertexPrevious->v);

			while(vertex <= vertexLast){

				/*
				** Clips an edge intersecting a plane.
				** Records the edges involved in the clip.
				*/
				endDistance = pointPlaneDistance(adjacentFaceNormal, adjacentFaceVertex, &vertex->v);
				// Check if the starting vertex is behind the plane.
				if(startDistance <= 0.f){
					// If the starting vertex is behind the plane and
					// the ending vertex is in front of it, clip the
					// ending vertex. If both vertices are behind the
					// plane, there is no need to clip anything.
					if(endDistance > 0.f){
						vec3LerpR(&vertexPrevious->v, &vertex->v, startDistance / (startDistance - endDistance), &vertexClip->v);
						#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
						vertexClip->key.edgeA = vertex->key.edgeA;
						vertexClip->key.edgeB = inEdgeIndex;  // Reference edge index.
						#else
						vertexClip->key.inEdgeR  = (cEdgeIndex_t)-1;
						vertexClip->key.outEdgeR = inEdgeIndex;  // Reference edge index.
						vertexClip->key.inEdgeI  = vertex->key.inEdgeI;
						vertexClip->key.outEdgeI = (cEdgeIndex_t)-1;
						#endif
					}else{
						*vertexClip = *vertex;
					}
					++vertexClip;
				}else if(endDistance <= 0.f){
					// If the starting vertex is in front of the plane
					// and the ending vertex is behind it, clip the
					// starting vertex.
					vec3LerpR(&vertexPrevious->v, &vertex->v, startDistance / (startDistance - endDistance), &vertexClip->v);
					#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
					vertexClip->key.edgeA = inEdgeIndex;  // Reference edge index.
					vertexClip->key.edgeB = vertexPrevious->key.edgeB;
					#else
					vertexClip->key.inEdgeR  = inEdgeIndex;  // Reference edge index.
					vertexClip->key.outEdgeR = (cEdgeIndex_t)-1;
					vertexClip->key.inEdgeI  = (cEdgeIndex_t)-1;
					vertexClip->key.outEdgeI = vertex->key.outEdgeI;
					#endif
					++vertexClip;
					// Now that the clipped starting vertex has been
					// added, add the ending vertex.
					*vertexClip = *vertex;
					++vertexClip;
				}
				startDistance = endDistance;
				vertexPrevious = vertex;
				++vertex;

			}


			// Calculate the new last vertex.
			// Since we increase vertexClip every time we add
			// a new vertex and we swap vertexArray and vertexClip,
			// vertexClip will be the new end of our array.
			vertexLast = vertexClip-1;

			// Swap vertexArray and vertexClipArray so that we clip
			// the vertices that were clipped during this iteration.
			vertexArraySwap = vertexArray;
			vertexArray = clipArray.vertices;
			clipArray.vertices = vertexArraySwap;

			// Update the current reference edge index.
			inEdgeIndex = outEdgeIndex;

		} while(referenceFaceEdge != referenceFaceFirstEdge);

		#ifndef COLLISION_MANIFOLD_REDUCTION_DISABLED

			// Loop through every vertex of the incident face,
			// checking which ones we can use as contact points.
			vertex = vertexArray;
			vertexNext = vertexArray;
			depthNext = clipArray.depths;
			for(; vertex <= vertexLast; ++vertex){
				const float penetrationDepth = pointPlaneDistance(referenceFaceNormal, referenceFaceVertex, &vertex->v);
				if(penetrationDepth <= 0.f){
					// The current vertex is a valid contact.
					// Clip the vertex onto the reference face
					// and store it away for later.
					pointPlaneProjectR(referenceFaceNormal, referenceFaceVertex, &vertex->v, &vertexNext->v);
					vertexNext->key = vertex->key;
					*depthNext = penetrationDepth;
					++depthNext;
					++vertexNext;
					++cm->contactNum;
				}
			}

			if(cm->contactNum > COLLISION_MANIFOLD_MAX_CONTACT_POINTS){
				// Perform manifold reduction if we have too
				// many valid contact points. Calculates the
				// contact tangents during reduction.
				cMeshReduceManifold(vertexArray, vertexNext, clipArray.depths, referenceFaceNormal, referenceFaceVertex, offset, cm);
				cm->contactNum = COLLISION_MANIFOLD_MAX_CONTACT_POINTS;
			}else{
				// Otherwise add each contact to the manifold.
				for(; vertexArray < vertexNext; ++vertexArray, ++clipArray.depths, ++contact){
					contact->point = vertexArray->v;
					contact->penetrationDepth = *clipArray.depths;
					if(offset == 0){
						contact->key = vertexArray->key;
						contact->normal = *referenceFaceNormal;
					}else{
						#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
						contact->key.edgeA = vertexArray->key.edgeB;
						contact->key.edgeB = vertexArray->key.edgeA;
						#else
						contact->key.inEdgeR  = vertexArray->key.inEdgeI;
						contact->key.outEdgeR = vertexArray->key.outEdgeI;
						contact->key.inEdgeI  = vertexArray->key.inEdgeR;
						contact->key.outEdgeI = vertexArray->key.outEdgeR;
						#endif
						vec3NegateR(referenceFaceNormal, &contact->normal);
					}
				}
			}

		#else

			// Loop through every vertex of the incident face,
			// checking which ones we can use as contact points
			// and adding them to the manifold.
			for(; vertexArray <= vertexLast; ++vertexArray){
				const float penetrationDepth = pointPlaneDistance(referenceFaceNormal, referenceFaceVertex, &vertexArray->v);
				if(penetrationDepth <= 0.f){
					// Project the vertex onto the reference face.
					pointPlaneProjectR(referenceFaceNormal, referenceFaceVertex, &vertexArray->v, &contact->point);
					contact->penetrationDepth = penetrationDepth;
					if(offset == 0){
						contact->key = vertexArray->key;
						contact->normal = *referenceFaceNormal;
					}else{
						pointPlaneProjectR(referenceFaceNormal, referenceFaceVertex, &vertexArray->v, &contact->pointA);
						pointPlaneProjectR(referenceFaceNormal, referenceFaceVertex, &vertexArray->v, &contact->pointB);
						#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
						contact->key.edgeA = vertexArray->key.edgeB;
						contact->key.edgeB = vertexArray->key.edgeA;
						#else
						contact->key.inEdgeR  = vertexArray->key.inEdgeI;
						contact->key.outEdgeR = vertexArray->key.outEdgeI;
						contact->key.inEdgeI  = vertexArray->key.inEdgeR;
						contact->key.outEdgeI = vertexArray->key.outEdgeR;
						#endif
						vec3NegateR(referenceFaceNormal, &contact->normal);
					}
					++cm->contactNum;
					if(cm->contactNum == COLLISION_MANIFOLD_MAX_CONTACT_POINTS){
						break;
					}
					++contact;
				}
			}

		#endif


	#ifdef COLLIDER_MESH_SAT_USE_HEAP_ALLOCATION
		memFree(vertices);

	}else{
		/** Memory allocation failure. **/
	}
	#endif

}

static __FORCE_INLINE__ void cMeshCollisionSHClipping(const cMesh *const restrict c1, const cMesh *const restrict c2, const cMeshPenetrationPlanes *const restrict planes, cMeshContact *const restrict cm){

	/*
	** Implementation of the Sutherland-Hodgman clipping
	** algorithm for generating a contact manifold after
	** a collision.
	*/

	const float maxSeparation = planes->face2.penetrationDepth > planes->face1.penetrationDepth ? planes->face2.penetrationDepth : planes->face1.penetrationDepth;

	// Only create an edge contact if the edge penetration depth
	// is greater than both face penetration depths. Favours
	// face contacts over edge contacts.
	if(planes->edge.penetrationDepth > COLLISION_TOLERANCE_COEFFICIENT * maxSeparation + COLLISION_TOLERANCE_TERM){

		cMeshClipEdgeContact(c1, c2, planes->edge.edge1, planes->edge.edge2, cm);

	}else{

		// If the second face penetration depth is greater than
		// the first, create a face contact with it. Favours the
		// first hull as the reference collider and the second
		// as the incident collider in order to prevent flip-flopping.
		if(planes->face2.penetrationDepth > COLLISION_TOLERANCE_COEFFICIENT * planes->face1.penetrationDepth + COLLISION_TOLERANCE_TERM){

			// Offset is sizeof(vec3) so pointA and
			// pointB in the contact manifold are swapped.
			cMeshClipFaceContact(c2, c1, planes->face2.index, planes->face1.index, 1, cm);

		}else{

			cMeshClipFaceContact(c1, c2, planes->face1.index, planes->face2.index, 0, cm);

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
		const vec3 invNormal = {.x = -n->x,
		                        .y = -n->y,
		                        .z = -n->z};
		const float distance = pointPlaneDistance(n, &c1->vertices[c1->edges[f->edge].start], cMeshCollisionSupport(c2, &invNormal));

		if(distance > 0.f){
			// Early exit, a separating axis has been found.
			// Cache the separating axis.
			if(sc != NULL){
				sc->type = type;
				sc->featureA = i;
			}
			return 0;
		}else if(distance > r->penetrationDepth){
			r->penetrationDepth = distance;
			r->index = i;
		}

	}

	return 1;

}

static __FORCE_INLINE__ return_t cMeshCollisionSATMinkowskiFace(const vec3 *const restrict A, const vec3 *const restrict B, const vec3 *const restrict BxA,
																const vec3 *const restrict C, const vec3 *const restrict D, const vec3 *const restrict DxC){

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

	// Normals of one of the planes intersecting
	// the arcs. Instead of crossing the two
	// adjacent face normals, we can just
	// subtract one edge vertex from the other.
	//
	// D x C
	//vec3SubVFromVR(&c2->vertices[e2->start], &c2->vertices[e2->end], &normal);

	const float BDC = vec3Dot(B, DxC);

	// Check if the arcs are overlapping by testing
	// if the vertices of one arc are on opposite
	// sides of the plane intersecting the other arc.
	//
	// ADC * BDC
	if(vec3Dot(A, DxC) * BDC < 0.f){

		// Normals of the other plane intersecting
		// the arcs.
		//
		// B x A
		//vec3SubVFromVR(&c1->vertices[e1->start], &c1->vertices[e1->end], &normal);

		const float CBA = vec3Dot(C, BxA);

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
		// > to <. Otherwise, we can invert D and C.
		if(CBA * BDC < 0.f){

			// Final check to see if the vertices are overlapping.
			// We don't need to change the signs of CBA or DBA, as
			// they will cancel each other out.
			//
			// CBA * DBA
			return CBA * vec3Dot(D, BxA) < 0.f;

		}

	}

	return 0;

}

static __FORCE_INLINE__ float cMeshCollisionSATEdgeSeparation(const vec3 *const restrict pointA, const vec3 *const restrict e1InvDir,
                                                              const vec3 *const restrict pointB, const vec3 *const restrict e2InvDir,
                                                              const vec3 *const restrict centroid){

	/*
	** Check the distance between the two edges.
	*/

	/** Should perform calculations in the second collider's local space? **/
	/** Might not be worth it, as our physics colliders are very simple.  **/

	vec3 normal, offset;
	float magnitudeSquared;

	vec3CrossR(e1InvDir, e2InvDir, &normal);
	magnitudeSquared = vec3MagnitudeSquared(&normal);

	// Check if the edges are parallel enough to not be
	// considered a new face on the Minkowski difference.
	if(magnitudeSquared < vec3MagnitudeSquared(e1InvDir) * vec3MagnitudeSquared(e2InvDir) * COLLISION_PARALLEL_THRESHOLD_SQUARED){
		return -INFINITY;
	}

	// Normalize.
	vec3MultVByS(&normal, fastInvSqrt(magnitudeSquared));
	vec3SubVFromVR(pointA, centroid, &offset);

	// Ensure the normal points from A to B.
	if(vec3Dot(&normal, &offset) < 0.f){
		vec3Negate(&normal);
	}

	vec3SubVFromVR(pointB, pointA, &offset);
	return vec3Dot(&normal, &offset);

}

static __FORCE_INLINE__ return_t cMeshCollisionSATEdgeQuery(const cMesh *c1, const cMesh *c2, cMeshSHEdgeHelper *r, cMeshSeparation *const restrict sc){

	/*
	** Find the maximum separation distance between
	** the edges of the first and second colliders.
	*/

	cEdgeIndex_t i;
	const cMeshEdge *e1;

	for(i = 0, e1 = c1->edges; i < c1->edgeNum; ++i, ++e1){

		cEdgeIndex_t j;
		const cMeshEdge *e2;

		const vec3 const *s1 = &c1->vertices[e1->start];

		vec3 e1InvDir;
		vec3SubVFromVR(s1, &c1->vertices[e1->end], &e1InvDir);

		for(j = 0, e2 = c2->edges; j < c2->edgeNum; ++j, ++e2){

			const vec3 const *s2 = &c2->vertices[e2->start];

			vec3 e2InvDir;
			vec3SubVFromVR(s2, &c2->vertices[e2->end], &e2InvDir);

			// The inverse direction vectors are used in place of the cross product
			// between the edge's face normal and its twin's face normal.
			if(cMeshCollisionSATMinkowskiFace(
				&c1->normals[e1->face], &c1->normals[e1->twinFace], &e1InvDir,
				&c2->normals[e2->face], &c2->normals[e2->twinFace], &e2InvDir
			)){

				// Now that we have a Minkowski face, we can
				// get the distance between the two edges.
				const float distance = cMeshCollisionSATEdgeSeparation(s1, &e1InvDir, s2, &e2InvDir, &c1->centroid);
				if(distance > 0.f){
					// Early exit, a separating axis has been found.
					// Cache the separating axis.
					if(sc != NULL){
						sc->type = COLLIDER_MESH_SEPARATION_FEATURE_EDGE;
						sc->featureA = i;
						sc->featureB = j;
					}
					return 0;
				}else if(distance > r->penetrationDepth){
					// If the distance between these two edges
					// is larger than the previous greatest
					// distance, record it.
					r->penetrationDepth = distance;
					r->edge1 = i;
					r->edge2 = j;
				}

			}

		}

	}

	return 1;

}

return_t cMeshCollisionSAT(const cMesh *const restrict c1, const cMesh *const restrict c2, cMeshSeparation *const restrict sc, cMeshContact *const restrict cm){

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
			if(cMeshCollisionSATEdgeQuery(c1, c2, &planes.edge, sc)){
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
	vec3 invNormal;
	vec3NegateR(&c1->normals[sc->featureA], &invNormal);
	return pointPlaneDistance(&c1->normals[sc->featureA], &c1->vertices[c1->edges[c1->faces[sc->featureA].edge].start], cMeshCollisionSupport(c2, &invNormal)/*&c2->vertices[sc->featureB]*/) > 0.f;
}

static __FORCE_INLINE__ return_t cMeshSeparationSATEdgeQuery(const cMesh *const restrict c1, const cMesh *const restrict c2, const cMeshSeparation *const restrict sc){
	const cMeshEdge *const e1 = &c1->edges[sc->featureA];
	const cMeshEdge *const e2 = &c2->edges[sc->featureB];
	const vec3 *const s1 = &c1->vertices[e1->start];
	const vec3 *const s2 = &c2->vertices[e2->start];
	vec3 e1InvDir, e2InvDir;
	vec3SubVFromVR(s1, &c1->vertices[e1->end], &e1InvDir);
	vec3SubVFromVR(s2, &c2->vertices[e2->end], &e2InvDir);
	return cMeshCollisionSATMinkowskiFace(&c1->normals[e1->face], &c1->normals[e1->twinFace], &e1InvDir,
	                                      &c2->normals[e2->face], &c2->normals[e2->twinFace], &e2InvDir) &&
	       cMeshCollisionSATEdgeSeparation(s1, &e1InvDir, s2, &e2InvDir, &c1->centroid) > 0.f;
}

return_t cMeshSeparationSAT(const cMesh *const restrict c1, const cMesh *const restrict c2, const cMeshSeparation *const restrict sc){
	// Check the separation cache for possible early exits.
	switch(sc->type){
		case COLLIDER_MESH_SEPARATION_FEATURE_FACE_1:
			return cMeshSeparationSATFaceQuery(c1, c2, sc);
		case COLLIDER_MESH_SEPARATION_FEATURE_FACE_2:
			return cMeshSeparationSATFaceQuery(c2, c1, sc);
		case COLLIDER_MESH_SEPARATION_FEATURE_EDGE:
			return cMeshSeparationSATEdgeQuery(c1, c2, sc);
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

static __FORCE_INLINE__ void cMeshCollisionEPA(const cMesh *const restrict c1, const cMesh *const restrict c2, cMeshSupportVertex *const restrict simplex, cMeshContact *const restrict cm){

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
	vec3CrossR(&temp1, &temp2, &temp3);
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

	{

		const float penetrationDepth = distance*distance;

		// Store the contact normal and penetration depth.
		vec3 contact = closestFace->normal;
		cm->contactNum = 0;

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
						  contact.x, contact.y, contact.z, &cm->contacts[cm->contactNum].point);
		cm->contacts[cm->contactNum].normal = contact;
		cm->contacts[cm->contactNum].penetrationDepth = penetrationDepth;
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
	vec3CrossR(&AB, &AC, &ABC);
	vec3 tempCross;

	vec3CrossR(&AB, &ABC, &tempCross);
	if(vec3Dot(&tempCross, &AO) > 0.f){

		// AB x ABC does not cross the origin, so we need a
		// new simplex fragment. Change the direction and
		// remove vertex C by replacing it with vertex A.
		vec3CrossR(&AB, &AO, &tempCross);
		vec3CrossR(&tempCross, &AB, axis);
		simplex[2] = simplex[0];

	}else{

		vec3CrossR(&ABC, &AC, &tempCross);
		if(vec3Dot(&tempCross, &AO) > 0.f){

			// ABC x AC does not cross the origin, so we need a
			// new simplex fragment. Change the direction and
			// remove vertex C by replacing it with vertex A.
			vec3CrossR(&AC, &AO, &tempCross);
			vec3CrossR(&tempCross, &AC, axis);
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
	vec3CrossR(&AB, &AC, &tempCross);

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
		vec3CrossR(&AC, &AD, &tempCross);
		if(vec3Dot(&tempCross, &AO) > 0.f){
			simplex[1] = simplex[0];
			*axis = tempCross;
			return 0;
		}

		// Check if the normal of ADB is crossing the origin.
		vec3CrossR(&AD, &AB, &tempCross);
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

return_t cMeshCollisionGJK(const cMesh *const restrict c1, const cMesh *const restrict c2, cMeshContact *const restrict cm){

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
	vec3 axis = {.x = c2->centroid.x - c1->centroid.x,
	             .y = c2->centroid.y - c1->centroid.y,
	             .z = c2->centroid.z - c1->centroid.z};

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
		vec3CrossR(&BC, &BO, &tempCross);
		vec3CrossR(&tempCross, &BC, &axis);
		if(axis.x == 0.f && axis.y == 0.f && axis.z == 0.f){
			// The origin is on the line segment BC.
			vec3Set(&tempCross, 1.f, 0.f, 0.f);
			vec3CrossR(&BC, &tempCross, &axis);
			if(axis.x == 0.f && axis.y == 0.f && axis.z == 0.f){
				vec3Set(&tempCross, 0.f, 0.f, -1.f);
				vec3CrossR(&BC, &tempCross, &axis);
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

void cMeshDeleteBase(cMesh *const restrict c){
	if(c->vertices != NULL){
		memFree(c->vertices);
	}
	if(c->normals != NULL){
		memFree(c->normals);
	}
	if(c->faces != NULL){
		memFree(c->faces);
	}
	if(c->edges != NULL){
		memFree(c->edges);
	}
}

void cMeshDelete(cMesh *const restrict c){
	// Meshes reuse the local collider's
	// arrays, except for vertices and
	// normals which need to be modified.
	if(c->vertices != NULL){
		memFree(c->vertices);
	}
	if(c->normals != NULL){
		memFree(c->normals);
	}
}