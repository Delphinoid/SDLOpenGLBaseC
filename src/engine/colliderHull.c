#include "collision.h"
#include "memoryManager.h"
#include "helpersMath.h"
#include "constantsMath.h"
#include <string.h>

#if defined(_MSC_VER) || defined(__MINGW32__)
	#include <malloc.h>
#else
	#include <alloca.h>
#endif

#define COLLIDER_HULL_SAT_USE_STACK_ALLOCATION

#define COLLISION_MAX_ITERATIONS 64
#define COLLISION_MAX_FACE_NUM_EPA 64
#define COLLISION_MAX_EDGE_NUM_EPA 32

#define COLLISION_DISTANCE_THRESHOLD 0.0001f
#define COLLISION_PARALLEL_THRESHOLD 0.005f
#define COLLISION_PARALLEL_THRESHOLD_RECIPROCAL (1.f/COLLISION_PARALLEL_THRESHOLD)
#define COLLISION_PARALLEL_THRESHOLD_SQUARED    (COLLISION_PARALLEL_THRESHOLD*COLLISION_PARALLEL_THRESHOLD)

#define COLLISION_TOLERANCE_COEFFICIENT 0.95f
#define COLLISION_TOLERANCE_TERM        0.025f  // 0.5f * PHYSICS_PENETRATION_SLOP

void cHullInit(cHull *const __RESTRICT__ c){
	c->vertexNum = 0;
	c->vertices = NULL;
	c->faceNum = 0;
	c->normals = NULL;
	c->edgeMax = 0;
	c->faces = NULL;
	c->edgeNum = 0;
	c->edges = NULL;
}

return_t cHullInstantiate(void *const __RESTRICT__ instance, const void *const __RESTRICT__ local){

	cVertexIndex_t vertexArraySize;
	cFaceIndex_t normalArraySize;

	vertexArraySize = ((cHull *)local)->vertexNum * sizeof(vec3);
	((cHull *)instance)->vertices = memAllocate(vertexArraySize);
	if(((cHull *)instance)->vertices == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	normalArraySize = ((cHull *)local)->faceNum * sizeof(vec3);
	((cHull *)instance)->normals = memAllocate(normalArraySize);
	if(((cHull *)instance)->normals == NULL){
		/** Memory allocation failure. **/
		memFree(((cHull *)instance)->vertices);
		return -1;
	}

	((cHull *)instance)->vertexNum = ((cHull *)local)->vertexNum;
	((cHull *)instance)->edgeMax   = ((cHull *)local)->edgeMax;
	((cHull *)instance)->faceNum   = ((cHull *)local)->faceNum;
	((cHull *)instance)->edgeNum   = ((cHull *)local)->edgeNum;

	memcpy(((cHull *)instance)->vertices, ((cHull *)local)->vertices, vertexArraySize);
	memcpy(((cHull *)instance)->normals,  ((cHull *)local)->normals,  normalArraySize);

	// Re-use the faces and edges arrays. Vertices and
	// normals, however, are modified each update when the
	// collider's configuration changes.
	((cHull *)instance)->faces = ((cHull *)local)->faces;
	((cHull *)instance)->edges = ((cHull *)local)->edges;

	return 1;

}

__FORCE_INLINE__ void cHullCentroidFromPosition(cHull *const __RESTRICT__ c, const cHull *const __RESTRICT__ l, const vec3 position, const quat orientation, const vec3 scale){
	// Extrapolate the mesh's centroid from a configuration.
	c->centroid = vec3VAddV(vec3VMultV(quatRotateVec3FastApproximate(orientation, l->centroid), scale), position);
}

cAABB cHullTransform(void *const instance, const vec3 instanceCentroid, const void *const local, const vec3 localCentroid, const vec3 position, const quat orientation, const vec3 scale){

	cHull *const cInstance = instance;
	const cHull *const cLocal = local;

	const vec3 *vLocal = cLocal->vertices;
	vec3 *vGlobal = cInstance->vertices;
	const vec3 *vLast = &vGlobal[cInstance->vertexNum];

	cAABB tempAABB = {.min.x = 0.f, .min.y = 0.f, .min.z = 0.f, .max.x = 0.f, .max.y = 0.f, .max.z = 0.f};

	// Update each collider and find the total bounding box.
	if(vGlobal < vLast){

		// Extrapolate the collider's centroid from its position.
		cInstance->centroid = vec3VAddV(vec3VMultV(quatRotateVec3FastApproximate(orientation, cLocal->centroid), scale), position);

		// First iteration.
		// Transform the vertex.
		*vGlobal = vec3VAddV(quatRotateVec3FastApproximate(orientation, vec3VMultV(vec3VSubV(*vLocal, localCentroid), scale)), instanceCentroid);

		// Initialize the AABB to the first vertex.
		tempAABB.min = *vGlobal;
		tempAABB.max = *vGlobal;

		// Remaining iterations.
		// Update each vertex.
		for(++vLocal, ++vGlobal; vGlobal < vLast; ++vLocal, ++vGlobal){

			// Transform the vertex.
			*vGlobal = vec3VAddV(quatRotateVec3FastApproximate(orientation, vec3VMultV(vec3VSubV(*vLocal, localCentroid), scale)), instanceCentroid);

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

	// Update each normal. We actually do need to scale the normals.
	for(; vGlobal < vLast; ++vLocal, ++vGlobal){
		*vGlobal = vec3NormalizeFastAccurate(quatRotateVec3FastApproximate(orientation, vec3VMultV(*vLocal, scale)));

	}

	return tempAABB;

}

static __FORCE_INLINE__ const vec3 *cHullCollisionSupport(const cHull *const __RESTRICT__ c, const vec3 axis){
	// Finds the vertex in c that is farthest in
	// the direction of axis by projecting each
	// vertex onto axis.
	const vec3 *r = c->vertices;
	const vec3 *v = &r[1];
	const vec3 *const vLast = &r[c->vertexNum];
	float max = vec3Dot(*r, axis);
	for(; v < vLast; ++v){
		const float s = vec3Dot(*v, axis);
		if(s > max){
			r = v;
			max = s;
		}
	}
	return r;
}
/** UNUSED **/
static __FORCE_INLINE__ const vec3 *cHullCollisionSupportIndex(const cHull *const __RESTRICT__ c, const vec3 axis, cVertexIndex_t *const index){

	// Finds the vertex in c that is farthest in
	// the direction of axis by projecting each
	// vertex onto axis.

	cVertexIndex_t i;
	const vec3 *v;
	const vec3 *r = c->vertices;
	float max = vec3Dot(*r, axis);

	*index = 0;

	for(i = 1, v = &r[1]; i < c->vertexNum; ++i, ++v){
		const float s = vec3Dot(*v, axis);
		if(s > max){
			r = v;
			max = s;
			*index = i;
		}
	}

	return r;

}

typedef struct {
	float separation;
	cFaceIndex_t index;
} cHullSHFaceHelper;

typedef struct {
	float separation;
	cEdgeIndex_t edge1;
	cEdgeIndex_t edge2;
} cHullSHEdgeHelper;

typedef struct {
	// The first collider's deepest penetrating face.
	// Provides the minimum penetration vector.
	cHullSHFaceHelper face1;
	// The second collider's deepest penetrating face.
	// Provides the minimum penetration vector.
	cHullSHFaceHelper face2;
	// Two penetrating edges for edge-edge contact.
	cHullSHEdgeHelper edge;
} cHullPenetrationPlanes;

typedef struct {
	vec3 v;
	cContactKey key;
} cHullClipVertex;

typedef struct {
	vec3 v;
	float depth;
} cHullProjectVertex;

static __FORCE_INLINE__ void cHullSHFaceInit(cHullSHFaceHelper *e){
	e->separation = -INFINITY;
	e->index = (cFaceIndex_t)-1;
}

static __FORCE_INLINE__ void cHullSHEdgeInit(cHullSHEdgeHelper *e){
	e->separation = -INFINITY;
	e->edge1 = (cEdgeIndex_t)-1;
	e->edge2 = (cEdgeIndex_t)-1;
}

static __FORCE_INLINE__ void cHullPenetrationPlanesInit(cHullPenetrationPlanes *planes){
	cHullSHFaceInit(&planes->face1);
	cHullSHFaceInit(&planes->face2);
	cHullSHEdgeInit(&planes->edge);
}

static __HINT_INLINE__ void cHullClipEdgeContact(const cHull *const reference, const cHull *const incident,
                                                 const cHullSHEdgeHelper edges, cHullContact *const __RESTRICT__ cm){

	const cHullEdge *const referenceEdge = &reference->edges[edges.edge1];
	const vec3 referenceEdgeStart = reference->vertices[referenceEdge->start];
	const vec3 referenceEdgeEnd   = reference->vertices[referenceEdge->end];

	const cHullEdge *const incidentEdge = &incident->edges[edges.edge2];
	const vec3 incidentEdgeStart = incident->vertices[incidentEdge->start];
	const vec3 incidentEdgeEnd   = incident->vertices[incidentEdge->end];

	vec3 referencePoint, incidentPoint;
	vec3 separation;

	cHullContactPoint *contact = cm->contacts;

	const vec3 v2 = vec3VSubV(referenceEdgeEnd, referenceEdgeStart);
	const vec3 v3 = vec3VSubV(incidentEdgeEnd, incidentEdgeStart);

	// Get the closest points on the line segments.
	// Expansion of segmentClosestPoints() so we can
	// use v2 and v3 later when calculating the normal.
	// segmentClosestPoints(referenceEdgeStart, referenceEdgeEnd, incidentEdgeStart, incidentEdgeEnd, &referencePoint, &incidentPoint);
	{
		const vec3 v1 = vec3VSubV(referenceEdgeStart, incidentEdgeStart);
		const float d12 = vec3Dot(v1, v2);
		const float d13 = vec3Dot(v1, v3);
		const float d22 = vec3Dot(v2, v2);
		const float d32 = vec3Dot(v3, v2);
		const float d33 = vec3Dot(v3, v3);
		const float denom = d22 * d33 - d32 * d32;
		// If the denominator is 0, use 0.5 as the position
		// along the first line segment. This puts the closest
		// point in the very center.
		const float m1 = denom == 0.f ? 0.5f : (d13 * d32 - d12 * d33) / denom;
		const float m2 = (d13 + m1 * d32) / d33;
		// Calculate the point on the first line segment.
		referencePoint = vec3VAddV(referenceEdgeStart, vec3VMultS(v2, m1));
		// Calculate the point on the second line segment.
		incidentPoint = vec3VAddV(incidentEdgeStart, vec3VMultS(v3, m2));
	}

	// Get the offset of the incident contact from
	// the reference contact. This will be halved
	// as one of our contact points.
	contact->pointA = referencePoint;
	contact->pointB = incidentPoint;

	// Calculate the contact normal.
	contact->normal = vec3NormalizeFastAccurate(vec3Cross(v2, v3));

	// Check if the contact normal is in the right direction.
	separation = vec3VSubV(referencePoint, incidentPoint);
	if(vec3Dot(contact->normal, separation) < 0.f){
		contact->normal = vec3Negate(contact->normal);
	}

	// Retrieve the contact separation.
	contact->separation = edges.separation;

	#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
	contact->key.edgeA = edges.edge1;
	contact->key.edgeB = edges.edge2;
	#else
	contact->key.inEdgeR = edges.edge1;
	contact->key.outEdgeR = edges.edge1;
	contact->key.inEdgeI = edges.edge2;
	contact->key.outEdgeI = edges.edge2;
	#endif

	cm->contactNum = 1;

}

static __HINT_INLINE__ const cHullFace *cHullFindIncidentClipFace(const cHull *const incident, const vec3 referenceNormal, cFaceIndex_t *const __RESTRICT__ incidentFaceIndex){

	if(vec3Dot(referenceNormal, incident->normals[*incidentFaceIndex]) < 0.f){
		// If the suggested face is penetrating, use it.
		return &incident->faces[*incidentFaceIndex];
	}else{

		// Otherwise find the deepest penetrating face.
		cFaceIndex_t i;
		const vec3 *n = incident->normals;
		const cHullFace *f = incident->faces;

		float min = vec3Dot(referenceNormal, *n);

		const cHullFace *r = f;
		*incidentFaceIndex = 0;

		for(i = 1; i < incident->faceNum; ++i){
			++n; ++f;
			{
				const float sameness = vec3Dot(referenceNormal, *n);
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

static __HINT_INLINE__ void cHullReduceManifold(const cHullClipVertex *const vertices, const cHullClipVertex *const verticesLast, const cHullProjectVertex *const projections,
                                                const vec3 planeNormal, const int offset, cHullContact *const __RESTRICT__ cm){

	// Reduce the manifold to the combination of
	// contacts that provides the greatest area.

	cHullContactPoint *contact = cm->contacts;

	const cHullClipVertex *pointBest;
	const cHullProjectVertex *projectionBest;
	float distanceBest;

	const cHullClipVertex *pointWorst;
	const cHullProjectVertex *projectionWorst;
	float distanceWorst;

	const cHullClipVertex *point;
	const cHullProjectVertex *projection;

	const cHullProjectVertex *first;
	const cHullProjectVertex *second;

	vec3 normal;


	// Find the first two contacts.
	// The first will be the farthest point from the origin and
	// the second will be the farthest point from the first.
	pointBest = vertices;
	projectionBest = projections;
	distanceBest = vec3MagnitudeSquared(projections->v);

	pointWorst = vertices;
	projectionWorst = projections;
	distanceWorst = distanceBest;

	point = vertices+1;
	projection = projections+1;

	// Find the points with the greatest and smallest distances.
	for(; point < verticesLast; ++point, ++projection){
		const float distance = vec3MagnitudeSquared(projection->v);
		if(distance > distanceBest){
			pointBest = point;
			projectionBest = projection;
			distanceBest = distance;
		}else if(distance < distanceWorst){
			pointWorst = point;
			projectionWorst = projection;
			distanceWorst = distance;
		}
	}

	first = projectionBest;
	second = projectionWorst;

	if(offset == 0){

		// Add the first two contact points.
		contact->pointA = projectionBest->v;
		contact->pointB = pointBest->v;
		contact->separation = projectionBest->depth;
		contact->key = pointBest->key;
		contact->normal = planeNormal;
		++contact;

		contact->pointA = projectionWorst->v;
		contact->pointB = pointWorst->v;
		contact->separation = projectionWorst->depth;
		contact->key = pointWorst->key;
		contact->normal = planeNormal;
		++contact;

	}else{

		// Add the first two contact points.
		contact->pointA = projectionBest->v;
		contact->pointB = pointBest->v;
		contact->separation = projectionBest->depth;
		#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
		contact->key.edgeA = pointBest->key.edgeA;
		contact->key.edgeB = pointBest->key.edgeB;
		#else
		contact->key.inEdgeR  = pointBest->key.inEdgeR;
		contact->key.outEdgeR = pointBest->key.outEdgeR;
		contact->key.inEdgeI  = pointBest->key.inEdgeI;
		contact->key.outEdgeI = pointBest->key.outEdgeI;
		#endif
		contact->normal = vec3Negate(planeNormal);
		++contact;

		contact->pointA = projectionWorst->v;
		contact->pointB = pointWorst->v;
		contact->separation = projectionWorst->depth;
		#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
		contact->key.edgeA = pointWorst->key.edgeA;
		contact->key.edgeB = pointWorst->key.edgeB;
		#else
		contact->key.inEdgeR  = pointWorst->key.inEdgeR;
		contact->key.outEdgeR = pointWorst->key.outEdgeR;
		contact->key.inEdgeI  = pointWorst->key.inEdgeI;
		contact->key.outEdgeI = pointWorst->key.outEdgeI;
		#endif
		contact->normal = vec3Negate(planeNormal);
		++contact;

	}


	// Now find the two points farthest in the two directions
	// perpendicular to that of the "edge" we've just created.
	// This will be our new search normal.
	// These two vectors will also work as our contact tangents.
	normal = vec3Cross(planeNormal, vec3VSubV(projectionBest->v, projectionWorst->v));

	pointBest = vertices;
	projectionBest = projections;
	distanceBest = vec3Dot(normal, vec3VSubV(projections->v, first->v));

	pointWorst = vertices;
	projectionWorst = projections;
	distanceWorst = distanceBest;

	point = vertices+1;
	projection = projections+1;

	// Find the points with the greatest and smallest distances.
	for(; point < verticesLast; ++point, ++projection){
		if(projection != first && projection != second){
			const float distance = vec3Dot(normal, vec3VSubV(projection->v, first->v));
			if(distance > distanceBest){
				pointBest = point;
				projectionBest = projection;
				distanceBest = distance;
			}else if(distance < distanceWorst){
				pointWorst = point;
				projectionWorst = projection;
				distanceWorst = distance;
			}
		}
	}

	// Add the last two contact points.
	if(offset == 0){

		// Add the first two contact points.
		contact->pointA = projectionBest->v;
		contact->pointB = pointBest->v;
		contact->separation = projectionBest->depth;
		contact->key = pointBest->key;
		contact->normal = planeNormal;
		++contact;

		contact->pointA = projectionWorst->v;
		contact->pointB = pointWorst->v;
		contact->separation = projectionWorst->depth;
		contact->key = pointWorst->key;
		contact->normal = planeNormal;
		++contact;

	}else{

		// Add the first two contact points.
		contact->pointA = projectionBest->v;
		contact->pointB = pointBest->v;
		contact->separation = projectionBest->depth;
		#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
		contact->key.edgeA = pointBest->key.edgeA;
		contact->key.edgeB = pointBest->key.edgeB;
		#else
		contact->key.inEdgeR  = pointBest->key.inEdgeR;
		contact->key.outEdgeR = pointBest->key.outEdgeR;
		contact->key.inEdgeI  = pointBest->key.inEdgeI;
		contact->key.outEdgeI = pointBest->key.outEdgeI;
		#endif
		contact->normal = vec3Negate(planeNormal);
		++contact;

		contact->pointA = projectionWorst->v;
		contact->pointB = pointWorst->v;
		contact->separation = projectionWorst->depth;
		#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
		contact->key.edgeA = pointWorst->key.edgeA;
		contact->key.edgeB = pointWorst->key.edgeB;
		#else
		contact->key.inEdgeR  = pointWorst->key.inEdgeR;
		contact->key.outEdgeR = pointWorst->key.outEdgeR;
		contact->key.inEdgeI  = pointWorst->key.inEdgeI;
		contact->key.outEdgeI = pointWorst->key.outEdgeI;
		#endif
		contact->normal = vec3Negate(planeNormal);

	}

}

static __HINT_INLINE__ void cHullClipFaceContact(const cHull *const reference, const cHull *const incident,
                                                 const cFaceIndex_t referenceFaceIndex, cFaceIndex_t incidentFaceIndex,
                                                 const int offset, cHullContact *const __RESTRICT__ cm){

	// Generates a contact manifold by clipping the edges of
	// the incident face against the faces adjacent to the
	// reference face.
	//
	// Offset is used to swap the positions of the points in
	// the contact manifold when, for example, the first
	// mesh is the incident collider and the second is the
	// reference collider.
	//
	// This method is capable of performing manifold reduction.

	// Allocate two arrays of vertices for each edge on the face.
	cHullClipVertex *const vertices =
	#if defined(COLLIDER_HULL_SAT_USE_STACK_ALLOCATION) && !defined(_STDC_NO_VLA_)
	alloca(incident->edgeMax * 2 * sizeof(cHullClipVertex) * 2);
	#else
	memAllocate(incident->edgeMax * 2 * sizeof(cHullClipVertex) * 2);
	// Make sure the allocation didn't fail.
	if(vertices != NULL){
	#endif

		cHullContactPoint *contact = cm->contacts;

		const cEdgeIndex_t referenceFaceFirstEdgeIndex = reference->faces[referenceFaceIndex].edge;
		const cHullEdge *const referenceFaceFirstEdge = &reference->edges[referenceFaceFirstEdgeIndex];
		const cHullEdge *referenceFaceEdge;

		const vec3 referenceFaceVertex = reference->vertices[referenceFaceFirstEdge->start];
		const vec3 referenceFaceNormal = reference->normals[referenceFaceIndex];

		// Find the incident face to clip against,
		// if the test index is insufficient.
		const cEdgeIndex_t incidentFaceFirstEdgeIndex = cHullFindIncidentClipFace(incident, referenceFaceNormal, &incidentFaceIndex)->edge;
		const cHullEdge *incidentFaceFirstEdge = &incident->edges[incidentFaceFirstEdgeIndex];
		const cHullEdge *incidentFaceEdge;

		// Pointer to the array with the vertices to be clipped.
		cHullClipVertex *vertexArray = vertices;
		union {
			cHullClipVertex *vertices;
			cHullProjectVertex *projections;
		} clipArray;
		cHullClipVertex *vertexArraySwap;

		const cHullClipVertex *vertex;
		cHullClipVertex *vertexLast;

		cHullClipVertex *vertexNext;
		cHullProjectVertex *projectionNext;

		cEdgeIndex_t inEdgeIndex;
		cEdgeIndex_t outEdgeIndex;


		// Pointer to the array with the clipped vertices. Starts halfway through the vertex array.
		clipArray.vertices = vertices+incident->edgeMax*2;

		// We haven't found any contacts yet.
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

			cHullClipVertex *vertexClip;
			const cHullClipVertex *vertexPrevious;

			vec3 adjacentFaceNormal;
			vec3 adjacentFaceVertex;

			float startDistance;
			float endDistance;


			adjacentFaceVertex = reference->vertices[referenceFaceEdge->start];
			if(referenceFaceEdge->face == referenceFaceIndex){
				// The next edge associated with this face is not a twin.
				adjacentFaceNormal = reference->normals[referenceFaceEdge->twinFace];
				outEdgeIndex = referenceFaceEdge->next;
			}else{
				// The next edge associated with this face is a twin.
				adjacentFaceNormal = reference->normals[referenceFaceEdge->face];
				outEdgeIndex = referenceFaceEdge->twinNext;
			}
			referenceFaceEdge = &reference->edges[outEdgeIndex];


			// Loop through every edge of the incident face,
			// clipping its vertices against the current
			// reference plane.
			vertex = vertexArray;
			vertexClip = clipArray.vertices;
			vertexPrevious = vertexLast;
			startDistance = pointPlaneDistance(adjacentFaceNormal, adjacentFaceVertex, vertexPrevious->v);

			while(vertex <= vertexLast){

				// Clips an edge intersecting a plane.
				// Records the edges involved in the clip.
				endDistance = pointPlaneDistance(adjacentFaceNormal, adjacentFaceVertex, vertex->v);
				// Check if the starting vertex is behind the plane.
				if(startDistance <= 0.f){
					// If the starting vertex is behind the plane and
					// the ending vertex is in front of it, clip the
					// ending vertex. If both vertices are behind the
					// plane, there is no need to clip anything.
					if(endDistance > 0.f){
						vertexClip->v = vec3Lerp(vertexPrevious->v, vertex->v, startDistance / (startDistance - endDistance));
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
					vertexClip->v = vec3Lerp(vertexPrevious->v, vertex->v, startDistance / (startDistance - endDistance));
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
		projectionNext = clipArray.projections;
		for(; vertex <= vertexLast; ++vertex){
			const float separation = pointPlaneDistance(referenceFaceNormal, referenceFaceVertex, vertex->v);
			if(separation <= 0.f){
				// The current vertex is a valid contact.
				// Project the vertex onto the reference
				// face and store it away for later.
				vertexNext->v = vertex->v;
				vertexNext->key = vertex->key;
				projectionNext->v = pointPlaneProject(referenceFaceNormal, referenceFaceVertex, vertex->v);
				projectionNext->depth = separation;
				++projectionNext;
				++vertexNext;
				++cm->contactNum;
			}
		}

		if(cm->contactNum > COLLISION_MANIFOLD_MAX_CONTACT_POINTS){
			// Perform manifold reduction if we have too
			// many valid contact points. Calculates the
			// contact tangents during reduction.
			cHullReduceManifold(vertexArray, vertexNext, clipArray.projections, referenceFaceNormal, offset, cm);
			cm->contactNum = COLLISION_MANIFOLD_MAX_CONTACT_POINTS;
		}else{
			// Otherwise add each contact to the manifold.
			for(; vertexArray < vertexNext; ++vertexArray, ++clipArray.projections, ++contact){
				contact->separation = clipArray.projections->depth;
				if(offset == 0){
					contact->pointA = clipArray.projections->v;
					contact->pointB = vertexArray->v;
					contact->normal = referenceFaceNormal;
					contact->key = vertexArray->key;
				}else{
					contact->pointA = vertexArray->v;
					contact->pointB = clipArray.projections->v;
					contact->normal = vec3Negate(referenceFaceNormal);
					#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
					contact->key.edgeA = vertexArray->key.edgeB;
					contact->key.edgeB = vertexArray->key.edgeA;
					#else
					contact->key.inEdgeR  = vertexArray->key.inEdgeI;
					contact->key.outEdgeR = vertexArray->key.outEdgeI;
					contact->key.inEdgeI  = vertexArray->key.inEdgeR;
					contact->key.outEdgeI = vertexArray->key.outEdgeR;
					#endif
				}
			}
		}

		#else

		// Loop through every vertex of the incident face,
		// checking which ones we can use as contact points.
		for(; vertexArray <= vertexLast; ++vertexArray, ++clipArray.projections){
			const float separation = pointPlaneDistance(referenceFaceNormal, referenceFaceVertex, vertexArray->v);
			if(separation <= 0.f){
				contact->separation = separation;
				if(offset == 0){
					contact->pointA = pointPlaneProject(referenceFaceNormal, referenceFaceVertex, vertexArray->v);
					contact->pointB = vertexArray->v;
					contact->normal = referenceFaceNormal;
					contact->key = vertexArray->key;
				}else{
					contact->pointA = vertexArray->v;
					contact->pointB = pointPlaneProject(referenceFaceNormal, referenceFaceVertex, vertexArray->v);
					contact->normal = vec3Negate(referenceFaceNormal);
					#ifdef COLLISION_MANIFOLD_SIMPLE_CONTACT_KEYS
					contact->key.edgeA = vertexArray->key.edgeB;
					contact->key.edgeB = vertexArray->key.edgeA;
					#else
					contact->key.inEdgeR  = vertexArray->key.inEdgeI;
					contact->key.outEdgeR = vertexArray->key.outEdgeI;
					contact->key.inEdgeI  = vertexArray->key.inEdgeR;
					contact->key.outEdgeI = vertexArray->key.outEdgeR;
					#endif
				}
				++cm->contactNum;
				if(cm->contactNum < COLLISION_MANIFOLD_MAX_CONTACT_POINTS){
					++contact;
				}else{
					break;
				}

			}
		}

		#endif


	#if !defined(COLLIDER_HULL_SAT_USE_STACK_ALLOCATION) || defined(_STDC_NO_VLA_)
		memFree(vertices);

	}else{
		/** Memory allocation failure. **/
	}
	#endif

}

static __FORCE_INLINE__ void cHullCollisionSHClipping(const cHull *const __RESTRICT__ c1, const cHull *const __RESTRICT__ c2, const cHullPenetrationPlanes planes, cHullContact *const __RESTRICT__ cm){

	// Implementation of the Sutherland-Hodgman clipping
	// algorithm for generating a contact manifold after
	// a collision.

	const float maxSeparation = planes.face2.separation > planes.face1.separation ? planes.face2.separation : planes.face1.separation;

	// Only create an edge contact if the edge penetration depth
	// is greater than both face penetration depths. Favours
	// face contacts over edge contacts.
	if(planes.edge.separation > COLLISION_TOLERANCE_COEFFICIENT * maxSeparation + COLLISION_TOLERANCE_TERM){
		cHullClipEdgeContact(c1, c2, planes.edge, cm);
	}else{
		// If the second face penetration depth is greater than
		// the first, create a face contact with it. Favours the
		// first hull as the reference collider and the second
		// as the incident collider in order to prevent flip-flopping.
		if(planes.face2.separation > COLLISION_TOLERANCE_COEFFICIENT * planes.face1.separation + COLLISION_TOLERANCE_TERM){

			// Offset is 1 so pointA and pointB in the contact manifold are swapped.
			cHullClipFaceContact(c2, c1, planes.face2.index, planes.face1.index, 1, cm);

			// Make sure at least one contact point was generated.
			if(cm->contactNum == 0){
				// Couldn't generate contact points.
				// Try again, ignoring tolerances.
				if(planes.edge.separation > maxSeparation){
					cHullClipEdgeContact(c1, c2, planes.edge, cm);
				}else{
					cHullClipFaceContact(c1, c2, planes.face1.index, planes.face2.index, 0, cm);
					if(cm->contactNum == 0){
						// Clipping still failed. Treat the contact as an
						// edge contact, which guarantees at least two
						// contact points will be created.
						cHullClipEdgeContact(c1, c2, planes.edge, cm);
					}
				}
			}

		}else{

			// Offset is 0 so pointA and pointB in the contact manifold are NOT swapped.
			cHullClipFaceContact(c1, c2, planes.face1.index, planes.face2.index, 0, cm);

			// Make sure at least one contact point was generated.
			if(cm->contactNum == 0){
				// Couldn't generate contact points.
				// Try again, ignoring tolerances.
				if(planes.edge.separation > maxSeparation){
					cHullClipEdgeContact(c1, c2, planes.edge, cm);
				}else{
					cHullClipFaceContact(c2, c1, planes.face2.index, planes.face1.index, 1, cm);
					if(cm->contactNum == 0){
						// Clipping still failed. Treat the contact as an
						// edge contact, which guarantees at least two
						// contact points will be created.
						cHullClipEdgeContact(c1, c2, planes.edge, cm);
					}
				}
			}

		}
	}

}

static __FORCE_INLINE__ return_t cHullCollisionSATFaceQuery(const cHull *const __RESTRICT__ c1, const cHull *const __RESTRICT__ c2, cHullSHFaceHelper *const __RESTRICT__ r, cHullSeparation *const __RESTRICT__ sc, const cSeparationFeature_t type){

	// Find the maximum separation distance between
	// the faces of the first and the vertices of the
	// second collider.

	/** Should perform calculations in the second collider's local space? **/
	/** Might not be worth it, as our physics colliders are very simple.  **/

	cFaceIndex_t i;
	const vec3 *n;
	const cHullFace *f;

	for(i = 0, n = c1->normals, f = c1->faces; i < c1->faceNum; ++i, ++n, ++f){

		// Get the vertex in the second collider that's
		// farthest from the current face in the first
		// collider in the opposite direction that the
		// face is pointing.
		// After this, find the distance between them.
		const float distance = pointPlaneDistance(*n, c1->vertices[c1->edges[f->edge].start], *cHullCollisionSupport(c2, vec3Negate(*n)));

		if(distance > 0.f){
			// Early exit, a separating axis has been found.
			// Cache the separating axis.
			if(sc != NULL){
				sc->type = type;
				sc->featureA = i;
			}
			return 0;
		}else if(distance > r->separation){
			r->separation = distance;
			r->index = i;
		}

	}

	return 1;

}

static __FORCE_INLINE__ return_t cHullCollisionSATMinkowskiFace(const vec3 A, const vec3 B, const vec3 BxA,
																const vec3 C, const vec3 D, const vec3 DxC){

	// Tests if the specified edges overlap
	// when projected onto a unit sphere.
	//
	// Overlapping arcs on the Gauss map create
	// a face on the Minkowski sum of the
	// polyhedras.
	//
	// Because we want the Minkowski difference,
	// we negate the normals of the second edge.

	// Normals of one of the planes intersecting
	// the arcs. Instead of crossing the two
	// adjacent face normals, we can just
	// subtract one edge vertex from the other.
	const float BDC = vec3Dot(B, DxC);

	// Check if the arcs are overlapping by testing
	// if the vertices of one arc are on opposite
	// sides of the plane intersecting the other arc.
	//
	// ADC * BDC
	if(vec3Dot(A, DxC) * BDC < 0.f){

		// Normals of the other plane intersecting
		// the arcs.
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

static __FORCE_INLINE__ float cHullCollisionSATEdgeSeparation(const vec3 pointA, const vec3 e1InvDir,
                                                              const vec3 pointB, const vec3 e2InvDir,
                                                              const vec3 centroid){

	// Check the distance between the two edges.

	/** Should perform calculations in the second collider's local space? **/
	/** Might not be worth it, as our physics colliders are very simple.  **/

	vec3 normal = vec3Cross(e1InvDir, e2InvDir);
	const float normalMagnitudeSquared = vec3MagnitudeSquared(normal);

	// Check if the edges are parallel enough to not be
	// considered a new face on the Minkowski difference.
	if(normalMagnitudeSquared < vec3MagnitudeSquared(e1InvDir) * vec3MagnitudeSquared(e2InvDir) * COLLISION_PARALLEL_THRESHOLD_SQUARED){
		return -INFINITY;
	}

	// Normalize.
	normal = vec3VMultS(normal, rsqrtAccurate(normalMagnitudeSquared));

	// Ensure the normal points from A to B.
	if(vec3Dot(normal, vec3VSubV(pointA, centroid)) < 0.f){
		normal = vec3Negate(normal);
	}

	return vec3Dot(normal, vec3VSubV(pointB, pointA));

}

static __FORCE_INLINE__ return_t cHullCollisionSATEdgeQuery(const cHull *c1, const cHull *c2, cHullSHEdgeHelper *r, cHullSeparation *const __RESTRICT__ sc){

	// Find the maximum separation distance between
	// the edges of the first and second colliders.

	cEdgeIndex_t i;
	const cHullEdge *e1;

	for(i = 0, e1 = c1->edges; i < c1->edgeNum; ++i, ++e1){

		cEdgeIndex_t j;
		const cHullEdge *e2;

		const vec3 s1 = c1->vertices[e1->start];
		const vec3 e1InvDir = vec3VSubV(s1, c1->vertices[e1->end]);

		for(j = 0, e2 = c2->edges; j < c2->edgeNum; ++j, ++e2){

			const vec3 s2 = c2->vertices[e2->start];
			const vec3 e2InvDir = vec3VSubV(s2, c2->vertices[e2->end]);

			// The inverse direction vectors are used in place of the cross product
			// between the edge's face normal and its twin's face normal.
			if(cHullCollisionSATMinkowskiFace(
				c1->normals[e1->face], c1->normals[e1->twinFace], e1InvDir,
				c2->normals[e2->face], c2->normals[e2->twinFace], e2InvDir
			)){

				// Now that we have a Minkowski face, we can
				// get the distance between the two edges.
				const float distance = cHullCollisionSATEdgeSeparation(s1, e1InvDir, s2, e2InvDir, c1->centroid);
				if(distance > 0.f){
					// Early exit, a separating axis has been found.
					// Cache the separating axis.
					if(sc != NULL){
						sc->type = COLLIDER_HULL_SEPARATION_FEATURE_EDGE;
						sc->featureA = i;
						sc->featureB = j;
					}
					return 0;
				}else if(distance > r->separation){
					// If the distance between these two edges
					// is larger than the previous greatest
					// distance, record it.
					r->separation = distance;
					r->edge1 = i;
					r->edge2 = j;
				}

			}

		}

	}

	return 1;

}

return_t cHullCollisionSAT(const cHull *const __RESTRICT__ c1, const cHull *const __RESTRICT__ c2, cHullSeparation *const __RESTRICT__ sc, cHullContact *const __RESTRICT__ cm){

	// Implementation of the separating axis theorem
	// using Minkowski space and Gauss map optimizations.
	//
	// Credit to Dirk Gregorius for his amazing presentation
	// at GDC 2013. For a better understanding of the method,
	// I would definitely recommend going through the list
	// of references at the end of this presentation.

	cHullPenetrationPlanes planes;
	cHullPenetrationPlanesInit(&planes);

	if(cHullCollisionSATFaceQuery(c1, c2, &planes.face1, sc, COLLIDER_HULL_SEPARATION_FEATURE_FACE_1)){
		if(cHullCollisionSATFaceQuery(c2, c1, &planes.face2, sc, COLLIDER_HULL_SEPARATION_FEATURE_FACE_2)){
			if(cHullCollisionSATEdgeQuery(c1, c2, &planes.edge, sc)){
				if(cm != NULL){
					cHullCollisionSHClipping(c1, c2, planes, cm);
				}
				return 1;
			}
		}
	}

	return 0;

}

static __FORCE_INLINE__ return_t cHullSeparationSATFaceQuery(const cHull *const __RESTRICT__ c1, const cHull *const __RESTRICT__ c2, const cHullSeparation *const __RESTRICT__ sc){
	return pointPlaneDistance(c1->normals[sc->featureA], c1->vertices[c1->edges[c1->faces[sc->featureA].edge].start], *cHullCollisionSupport(c2, vec3Negate(c1->normals[sc->featureA]))/**&c2->vertices[sc->featureB]**/) > 0.f;
}

static __FORCE_INLINE__ return_t cHullSeparationSATEdgeQuery(const cHull *const __RESTRICT__ c1, const cHull *const __RESTRICT__ c2, const cHullSeparation *const __RESTRICT__ sc){
	const cHullEdge e1 = c1->edges[sc->featureA];
	const cHullEdge e2 = c2->edges[sc->featureB];
	const vec3 s1 = c1->vertices[e1.start];
	const vec3 s2 = c2->vertices[e2.start];
	const vec3 e1InvDir = vec3VSubV(s1, c1->vertices[e1.end]);
	const vec3 e2InvDir = vec3VSubV(s2, c2->vertices[e2.end]);
	return cHullCollisionSATMinkowskiFace(c1->normals[e1.face], c1->normals[e1.twinFace], e1InvDir,
	                                      c2->normals[e2.face], c2->normals[e2.twinFace], e2InvDir) &&
	       cHullCollisionSATEdgeSeparation(s1, e1InvDir, s2, e2InvDir, c1->centroid) > 0.f;
}

return_t cHullSeparationSAT(const cHull *const __RESTRICT__ c1, const cHull *const __RESTRICT__ c2, const cHullSeparation *const __RESTRICT__ sc){
	// Check the separation cache for possible early exits.
	switch(sc->type){
		case COLLIDER_HULL_SEPARATION_FEATURE_FACE_1:
			return cHullSeparationSATFaceQuery(c1, c2, sc);
		case COLLIDER_HULL_SEPARATION_FEATURE_FACE_2:
			return cHullSeparationSATFaceQuery(c2, c1, sc);
		case COLLIDER_HULL_SEPARATION_FEATURE_EDGE:
			return cHullSeparationSATEdgeQuery(c1, c2, sc);
	}
	return 0;
}

typedef struct {
	// The Minkowski "difference" of the two support points.
	vec3 v;
	// The support points used to generate v.
	const vec3 *s1;
	const vec3 *s2;
} cHullSupportVertex;

typedef struct {
	// Three vertices and a normal.
	cHullSupportVertex vertex[3];
	vec3 normal;
} cHullEPAFaceHelper;

typedef struct {
	// An edge that will be removed.
	cHullSupportVertex vertex[2];
} cHullEPAEdgeHelper;

static __FORCE_INLINE__ void cHullEPAFaceInit(cHullEPAFaceHelper *const __RESTRICT__ face, const cHullSupportVertex v0, const cHullSupportVertex v1, const cHullSupportVertex v2){
	// Create a new face.
	face->vertex[0] = v0;
	face->vertex[1] = v1;
	face->vertex[2] = v2;
	face->normal = vec3NormalizeFastAccurate(faceNormal(v0.v, v1.v, v2.v));
}

static __FORCE_INLINE__ void cHullEPAEdgeInit(cHullEPAEdgeHelper *const __RESTRICT__ edge, const cHullSupportVertex v0, const cHullSupportVertex v1){
	// Create a new face.
	edge->vertex[0] = v0;
	edge->vertex[1] = v1;
}

static __FORCE_INLINE__ void cHullCollisionMinkowskiSupport(const cHull *const __RESTRICT__ c1, const cHull *const __RESTRICT__ c2, const vec3 axis, cHullSupportVertex *const __RESTRICT__ r){
	// Returns a point in Minkowski space on the edge of
	// the polygons' "Minkowski difference".
	//
	// For the first polygon, find the vertex that is
	// farthest in the direction of axis. Use a
	// negative axis for the second polygon; this will
	// give us the two closest vertices along a given
	// axis.
	r->s1 = cHullCollisionSupport(c1, axis);
	r->s2 = cHullCollisionSupport(c2, vec3Negate(axis));
	// Get the "Minkowski Difference" of the two support points.
	r->v = vec3VSubV(*r->s1, *r->s2);
}

static __FORCE_INLINE__ void cHullCollisionEPA(const cHull *const __RESTRICT__ c1, const cHull *const __RESTRICT__ c2, cHullSupportVertex *const __RESTRICT__ simplex, cHullContact *const __RESTRICT__ cm){

	// Implementation of the expanding polytope algorithm. Extrapolates
	// additional collision information from two convex polygons and the
	// simplex generated by the Gilbert-Johnson-Keerthi algorithm.

	int i, j;
	cHullEPAFaceHelper *f;
	cHullEPAEdgeHelper *e;
	const cHullEPAFaceHelper *closestFace;
	float distance;
	cHullSupportVertex point;

	// Create the face and edge arrays.
	cHullEPAFaceHelper faces[COLLISION_MAX_FACE_NUM_EPA];
	cHullEPAFaceHelper *faceLast = &faces[4];
	cHullEPAEdgeHelper edges[COLLISION_MAX_EDGE_NUM_EPA];
	cHullEPAEdgeHelper *edgeLast = edges;

	// Ensure the vertex winding order of the simplex is CCW.
	if(vec3Dot(vec3VSubV(simplex[0].v, simplex[3].v), vec3Cross(vec3VSubV(simplex[1].v, simplex[3].v), vec3VSubV(simplex[2].v, simplex[3].v))) < 0.f){
		// If it's not, swap two vertices.
		const cHullSupportVertex swap = simplex[0];
		simplex[0] = simplex[1];
		simplex[1] = swap;
	}

	// Generate a starting tetrahedron from the given vertices.
	cHullEPAFaceInit(&faces[0], simplex[0], simplex[1], simplex[2]);  // Face 1 - ABC
	cHullEPAFaceInit(&faces[1], simplex[0], simplex[2], simplex[3]);  // Face 2 - ACD
	cHullEPAFaceInit(&faces[2], simplex[0], simplex[3], simplex[1]);  // Face 3 - ADB
	cHullEPAFaceInit(&faces[3], simplex[1], simplex[3], simplex[2]);  // Face 4 - BDC

	// Find the edge on the Minkowski difference closest to the origin.
	for(i = 0; i < COLLISION_MAX_ITERATIONS; ++i){

		// Find the closest face to the origin.
		f = &faces[1];
		closestFace = faces;
		distance = vec3Dot(faces[0].vertex[0].v, faces[0].normal);
		for(; f < faceLast; ++f){

			// Get the minimum distance between the current face and the origin.
			// This is the dot product between the face's normal and one of its vertices.
			const float tempDistance = vec3Dot(f->vertex[0].v, f->normal);

			if(tempDistance < distance){
				// New closest face found.
				closestFace = f;
				distance = tempDistance;
			}

		}

		// Search in the direction of the closest face to find
		// a point on the edge of the Minkowski difference.
		cHullCollisionMinkowskiSupport(c1, c2, closestFace->normal, &point);
		if(vec3Dot(point.v, closestFace->normal) - distance < COLLISION_DISTANCE_THRESHOLD){
			// The new point is not much further from the origin, break from the loop.
			break;
		}

		// For each face, if it is facing the new point, remove it and
		// create some new faces to join the new point to the simplex.
		f = faces;
		for(; f < faceLast; ++f){

			// Check if the face's normal is pointing in the direction of the new point.
			// If it is, the face can be "seen" by the new point.
			if(vec3Dot(f->normal, vec3VSubV(point.v, f->vertex[0].v)) > 0.f){

				// Add each of the face's edges to the edge array,
				// ready to be removed in the next section.
				for(j = 0; j < 3; ++j){

					int add = 1;
					cHullEPAEdgeHelper newEdge;
					cHullEPAEdgeInit(&newEdge, f->vertex[j], f->vertex[(j+1)%3]);

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

			cHullEPAFaceInit(faceLast, e->vertex[0], e->vertex[1], point);

			// Ensure the vertex winding order of the simplex is CCW.
			if(vec3Dot(faceLast->vertex[0].v, faceLast->normal) < 0.f){
				// If it's not, swap two vertices.
				cHullSupportVertex swap = faceLast->vertex[0];
				faceLast->vertex[0] = faceLast->vertex[1];
				faceLast->vertex[1] = swap;
				// Invert the face's normal as well.
				faceLast->normal = vec3Negate(faceLast->normal);
			}

			++faceLast;

		}
		++faceLast;
		edgeLast = edges;

	}

	// Now we can extrapolate various collision information
	// using face, normal and distance.
	{

		const float separation = distance*distance;

		// Store the contact normal and penetration depth.
		vec3 contact = closestFace->normal;
		cm->contactNum = 0;

		// Project the origin onto the closest face.
		// Calculate the Barycentric coordinates of the projected origin.
		contact = barycentric(closestFace->vertex[0].v, closestFace->vertex[1].v, closestFace->vertex[2].v, vec3VMultS(contact, distance));

		// The contact point on c1 is the linear combination of the original
		// vertices in c1 used to generate the support vertices for the
		// closest face, using the barycentric coordinates stored in contact.
		cm->contacts[cm->contactNum].pointA = vec3LinearCombination(
			*closestFace->vertex[0].s1,
			*closestFace->vertex[1].s1,
			*closestFace->vertex[2].s1,
			contact.x, contact.y, contact.z
		);
		cm->contacts[cm->contactNum].pointB = vec3LinearCombination(
			*closestFace->vertex[0].s2,
			*closestFace->vertex[1].s2,
			*closestFace->vertex[2].s2,
			contact.x, contact.y, contact.z
		);
		cm->contacts[cm->contactNum].normal = contact;
		cm->contacts[cm->contactNum].separation = -separation;
		++cm->contactNum;

	}

}

static __FORCE_INLINE__ void cHullCollisionGJKTriangle(cHullSupportVertex simplex[4], vec3 *const __RESTRICT__ axis){

	const vec3 AO = vec3Negate(simplex[0].v);
	const vec3 AB = vec3VSubV(simplex[1].v, simplex[0].v);
	const vec3 AC = vec3VSubV(simplex[2].v, simplex[0].v);
	const vec3 ABC = vec3Cross(AB, AC);

	if(vec3Dot(vec3Cross(AB, ABC), AO) > 0.f){

		// AB x ABC does not cross the origin, so we need a
		// new simplex fragment. Change the direction and
		// remove vertex C by replacing it with vertex A.
		*axis = vec3Cross(vec3Cross(AB, AO), AB);
		simplex[2] = simplex[0];
		///*simplexVertices = 2;

	}else{

		if(vec3Dot(vec3Cross(ABC, AC), AO) > 0.f){

			// ABC x AC does not cross the origin, so we need a
			// new simplex fragment. Change the direction and
			// remove vertex C by replacing it with vertex A.
			*axis = vec3Cross(vec3Cross(AC, AO), AC);
			simplex[1] = simplex[0];
			///*simplexVertices = 2;

		}else{

			// Checks have passed, a tetrahedron can be generated.
			if(vec3Dot(ABC, AO) > 0.f){
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
				*axis = vec3Negate(ABC);
			}

			// No need to update simplexVertices, as it will be 3
			// in preparation for the next iteration.

		}

	}

}

static __FORCE_INLINE__ return_t cHullCollisionGJKTetrahedron(cHullSupportVertex *const __RESTRICT__ simplex, vec3 *const __RESTRICT__ axis){

	// Check if the normal of ABC is crossing the origin.
	const vec3 AO = vec3Negate(simplex[0].v);
	const vec3 AB = vec3VSubV(simplex[1].v, simplex[0].v);
	const vec3 AC = vec3VSubV(simplex[2].v, simplex[0].v);
	vec3 tempCross = vec3Cross(AB, AC);

	if(vec3Dot(tempCross, AO) > 0.f){

		simplex[3] = simplex[2];
		simplex[2] = simplex[1];
		simplex[1] = simplex[0];
		*axis = tempCross;
		return 0;

	}else{

		// Check if the normal of ACD is crossing the origin.
		const vec3 AD = vec3VSubV(simplex[3].v, simplex[0].v);
		tempCross = vec3Cross(AC, AD);
		if(vec3Dot(tempCross, AO) > 0.f){
			simplex[1] = simplex[0];
			*axis = tempCross;
			return 0;
		}

		// Check if the normal of ADB is crossing the origin.
		tempCross = vec3Cross(AD, AB);
		if(vec3Dot(tempCross, AO) > 0.f){
			simplex[2] = simplex[3];
			simplex[3] = simplex[1];
			simplex[1] = simplex[0];
			*axis = tempCross;
			return 0;
		}

	}

	return 1;

}

return_t cHullCollisionGJK(const cHull *const __RESTRICT__ c1, const cHull *const __RESTRICT__ c2, cHullContact *const __RESTRICT__ cm){

	// Implementation of the Gilbert-Johnson-Keerthi distance algorithm,
	// which is only compatible with convex polytopes.

	int i;
	cHullSupportVertex simplex[4];

	// Set the current number of vertices of our simplex. We will be
	// creating a line segment with vertices B and C before the main
	// loop, so we can set this to 2 preemptively.
	// If vertex D is uninitialized, we will have 3 vertices in the
	// main loop, otherwise we will have 4.
	simplex[3].s1 = NULL;  //int simplexVertices = 2;

	// The first direction to check in is the direction of body2 from body1.
	vec3 axis = vec3VSubV(c2->centroid, c1->centroid);

	// Create an initial vertex for the simplex.
	cHullCollisionMinkowskiSupport(c1, c2, axis, &simplex[2]);

	// Create another vertex to form a line segment. B should be
	// a vertex in the opposite direction of C from the origin.
	axis = vec3Negate(simplex[2].v);
	cHullCollisionMinkowskiSupport(c1, c2, axis, &simplex[1]);

	// If the origin was not crossed, the Minkowski space is
	// entirely on one side of the origin, so the two
	// polygons cannot be overlapping.
	if(vec3Dot(simplex[1].v, axis) < 0.f){
		return 0;
	}else{
		const vec3 BO = vec3Negate(simplex[1].v);
		const vec3 BC = vec3VSubV(simplex[2].v, simplex[1].v);
		// Set the new search axis to the axis perpendicular
		// to the line segment BC, towards the origin.
		axis = vec3Cross(vec3Cross(BC, BO), BC);
		if(axis.x == 0.f && axis.y == 0.f && axis.z == 0.f){
			// The origin is on the line segment BC.
			axis = vec3Cross(BC, vec3New(1.f, 0.f, 0.f));
			if(axis.x == 0.f && axis.y == 0.f && axis.z == 0.f){
				axis = vec3Cross(BC, vec3New(0.f, 0.f, -1.f));
			}
		}
	}

	for(i = 0; i < COLLISION_MAX_ITERATIONS; ++i){

		// Add a new vertex to our simplex.
		// Vertex A is the "working vertex".
		cHullCollisionMinkowskiSupport(c1, c2, axis, &simplex[0]);
		if(vec3Dot(simplex[0].v, axis) < 0.f){
			// If the new vertex has not crossed the origin on the given axis,
			// the origin cannot lie within the "Minkowski difference" of the
			// polygons. Therefore, the two polygons are not colliding.
			return 0;
		}

		// Check if the origin is enclosed in our simplex.
		if(simplex[3].s1 == NULL){  //++simplexVertices == 3
			// We only have a triangle right now, find which
			// direction to search for our next vertex in.
			cHullCollisionGJKTriangle(simplex, &axis);
		}else if(cHullCollisionGJKTetrahedron(simplex, &axis)){
			// The origin lies within our simplex, so we know there was
			// a collision.
			if(cm != NULL){
				cHullCollisionEPA(c1, c2, &simplex[0], cm);
			}
			return 1;
		}

	}

	return 0;

}

void cHullDeleteBase(cHull *const __RESTRICT__ c){
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

void cHullDelete(cHull *const __RESTRICT__ c){
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