#ifndef COLLIDERCONVEXMESH_H
#define COLLIDERCONVEXMESH_H

#include "collision.h"

#define COLLIDER_MESH_MAX_ARRAY_SIZE 256

typedef uint_least16_t cVertexIndex_t;
typedef uint_least16_t cFaceIndex_t;
typedef uint_least16_t cEdgeIndex_t;

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

	//vec3 centroid;  /** Update HB_MAX_COLLIDER_SIZE **/
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

} cMesh;

void cMeshInit(cMesh *const restrict cm);
return_t cMeshCollisionSAT(const cMesh *const restrict c1, const cMesh *const restrict c2, const vec3 *const restrict centroid, cSeparationCache *const restrict info, cContactManifold *const restrict cm);
return_t cMeshCollisionGJK(const cMesh *const restrict c1, const vec3 *const restrict c1c, const cMesh *const restrict c2, const vec3 *const restrict c2c, cContactManifold *const restrict cm);
void cMeshDelete(cMesh *const restrict cm);

#endif
