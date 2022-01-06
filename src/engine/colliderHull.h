#ifndef COLLIDERHULL_H
#define COLLIDERHULL_H

#include "colliderAABB.h"
#include <stddef.h>

// Convex mesh data structure optimized for collision
// detection. There is only one edge per unique
// vertex pair; this means a separate cHullEdge is
// not stored for twin edges. Instead, when using
// cHullFaces, you must check if its index is equal
// to the edge's face or twinFace.

#define COLLIDER_HULL_SEPARATION_FEATURE_NULL   0
#define COLLIDER_HULL_SEPARATION_FEATURE_FACE_1 1  // The separation contains a face from the first
                                                   // collider and a vertex from the second collider.
#define COLLIDER_HULL_SEPARATION_FEATURE_FACE_2 2  // The separation contains a face from the second
                                                   // collider and a vertex from the first collider.
#define COLLIDER_HULL_SEPARATION_FEATURE_EDGE   3  // The separation contains two edges.

///typedef uint_least8_t  cSeparationFeature_t;
typedef uint_least16_t cVertexIndex_t;
typedef uint_least16_t cFaceIndex_t;
typedef uint_least16_t cEdgeIndex_t;

/**
typedef struct {
	size_t featureA;
	size_t featureB;
	cSeparationFeature_t type;
} cHullSeparation;
**/

typedef struct {
	cEdgeIndex_t edge;
} cHullFace;

typedef struct {
	// The indices of the half-edge's vertices.
	cVertexIndex_t start;
	cVertexIndex_t end;
	// The index of the current half-edge's associated face.
	cFaceIndex_t face;
	// The index of the current half-edge's next half-edge.
	cEdgeIndex_t next;
	// The index of the opposite half-edge's associated face.
	cFaceIndex_t twinFace;
	// The index of the opposite half-edge's next half-edge.
	cEdgeIndex_t twinNext;
} cHullEdge;

typedef struct {

	vec3 *vertices;

	vec3 *normals;

	// Index of the first edge of each face.
	cHullFace *faces;

	// Edges are stored as a starting vertex index,
	// an ending vertex index, a normal index and
	// a twin normal index.
	cHullEdge *edges;

	cVertexIndex_t vertexNum;
	cFaceIndex_t faceNum;
	cEdgeIndex_t edgeNum;
	// The number of edges on the face with the
	// most edges. Used for preallocation in
	// certain collision functions.
	cEdgeIndex_t edgeMax;

	vec3 centroid;

} cHull;

typedef struct cContactPoint cHullContactPoint;
typedef struct cContact      cHullContact;
typedef struct cSeparation   cHullSeparation;

void cHullInit(cHull *const __RESTRICT__ c);
return_t cHullInstantiate(void *const __RESTRICT__ instance, const void *const __RESTRICT__ local);
cAABB cHullTransform(void *const instance, const vec3 instanceCentroid, const void *const local, const vec3 localCentroid, const transform configuration);
void cHullCentroidFromPosition(cHull *const __RESTRICT__ c, const cHull *const __RESTRICT__ l, const transform configuration);
return_t cHullCollisionSAT(const cHull *const __RESTRICT__ c1, const cHull *const __RESTRICT__ c2, cHullSeparation *const __RESTRICT__ sc, cHullContact *const __RESTRICT__ cm);
return_t cHullSeparationSAT(const cHull *const __RESTRICT__ c1, const cHull *const __RESTRICT__ c2, const cHullSeparation *const __RESTRICT__ sc);
return_t cHullCollisionGJK(const cHull *const __RESTRICT__ c1, const cHull *const __RESTRICT__ c2, cHullContact *const __RESTRICT__ cm);
void cHullDeleteBase(cHull *const __RESTRICT__ c);
void cHullDelete(cHull *const __RESTRICT__ c);

#endif
