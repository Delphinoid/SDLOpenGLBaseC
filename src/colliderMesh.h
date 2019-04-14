#ifndef COLLIDERMESH_H
#define COLLIDERMESH_H

#include "quat.h"
#include "return.h"
#include <stddef.h>

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

void cMeshInit(cMesh *const restrict c);
return_t cMeshInstantiate(cMesh *const restrict instance, const cMesh *const restrict local);
void cMeshCentroidFromPosition(cMesh *const restrict c, const cMesh *const restrict l, const vec3 const position, const quat const orientation, const vec3 const scale);
return_t cMeshCollisionSAT(const cMesh *const restrict c1, const cMesh *const restrict c2, cMeshSeparation *const restrict sc, cMeshContact *const restrict cm);
return_t cMeshSeparationSAT(const cMesh *const restrict c1, const cMesh *const restrict c2, const cMeshSeparation *const restrict sc);
return_t cMeshCollisionGJK(const cMesh *const restrict c1, const cMesh *const restrict c2, cMeshContact *const restrict cm);
void cMeshDeleteBase(cMesh *const restrict c);
void cMeshDelete(cMesh *const restrict c);

#endif
