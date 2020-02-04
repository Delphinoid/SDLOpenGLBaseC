#ifndef COLLIDERMESH_H
#define COLLIDERMESH_H

#include "quat.h"
#include "return.h"
#include <stddef.h>

// Convex mesh data structure optimized for collision
// detection. There is only one edge per unique
// vertex pair; this means a separate cMeshEdge is
// not stored for twin edges. Instead, when using
// cMeshFaces, you must check if its index is equal
// to the edge's face or twinFace.

#define COLLIDER_MESH_SEPARATION_FEATURE_NULL   0
#define COLLIDER_MESH_SEPARATION_FEATURE_FACE_1 1  // The separation contains a face from the first
                                                   // collider and a vertex from the second collider.
#define COLLIDER_MESH_SEPARATION_FEATURE_FACE_2 2  // The separation contains a face from the second
                                                   // collider and a vertex from the first collider.
#define COLLIDER_MESH_SEPARATION_FEATURE_EDGE   3  // The separation contains two edges.

///typedef uint_least8_t  cSeparationFeature_t;
typedef uint_least16_t cVertexIndex_t;
typedef uint_least16_t cFaceIndex_t;
typedef uint_least16_t cEdgeIndex_t;

/**
typedef struct {
	size_t featureA;
	size_t featureB;
	cSeparationFeature_t type;
} cMeshSeparation;
**/

typedef struct {
	cEdgeIndex_t edge;
} cMeshFace;

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
} cMeshEdge;

typedef struct {

	vec3 *vertices;

	vec3 *normals;

	// Index of the first edge of each face.
	cMeshFace *faces;

	// Edges are stored as a starting vertex index,
	// an ending vertex index, a normal index and
	// a twin normal index.
	cMeshEdge *edges;

	cVertexIndex_t vertexNum;
	cFaceIndex_t faceNum;
	cEdgeIndex_t edgeNum;
	// The number of edges on the face with the
	// most edges. Used for preallocation in
	// certain collision functions.
	cEdgeIndex_t edgeMax;

	vec3 centroid;

} cMesh;

typedef struct cContactPoint cMeshContactPoint;
typedef struct cContact      cMeshContact;
typedef struct cSeparation   cMeshSeparation;

void cMeshInit(cMesh *const __RESTRICT__ c);
return_t cMeshInstantiate(cMesh *const __RESTRICT__ instance, const cMesh *const __RESTRICT__ local);
void cMeshCentroidFromPosition(cMesh *const __RESTRICT__ c, const cMesh *const __RESTRICT__ l, const vec3 position, const quat orientation, const vec3 scale);
return_t cMeshCollisionSAT(const cMesh *const __RESTRICT__ c1, const cMesh *const __RESTRICT__ c2, cMeshSeparation *const __RESTRICT__ sc, cMeshContact *const __RESTRICT__ cm);
return_t cMeshSeparationSAT(const cMesh *const __RESTRICT__ c1, const cMesh *const __RESTRICT__ c2, const cMeshSeparation *const __RESTRICT__ sc);
return_t cMeshCollisionGJK(const cMesh *const __RESTRICT__ c1, const cMesh *const __RESTRICT__ c2, cMeshContact *const __RESTRICT__ cm);
void cMeshDeleteBase(cMesh *const __RESTRICT__ c);
void cMeshDelete(cMesh *const __RESTRICT__ c);

#endif
