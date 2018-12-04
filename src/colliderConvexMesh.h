#ifndef COLLIDERCONVEXMESH_H
#define COLLIDERCONVEXMESH_H

#include "vec3.h"
#include "return.h"
#include <stdlib.h>
#include <stdint.h>

#define COLLIDER_MESH_MAX_ARRAY_SIZE 256

typedef uint_least8_t cVertexIndex_t;
typedef uint_least8_t cFaceIndex_t;
typedef uint_least8_t cEdgeIndex_t;

/** Temporary? **/
typedef struct {
	cEdgeIndex_t edgeNum;
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
	cVertexIndex_t vertexNum;
	vec3 *vertices;
	cFaceIndex_t faceNum;
	vec3 *normals;
	// Index of the first edge of each face.
	cMeshFace *faces;
	// Edges are stored as a starting vertex index,
	// an ending vertex index, a normal index and
	// a twin normal index.
	cEdgeIndex_t edgeNum;
	cMeshEdge *edges;
} cMesh;

void cMeshInit(cMesh *cm);
return_t cMeshLoad(cMesh *cm, const char *prgPath, const char *filePath);
void cMeshDelete(cMesh *cm);

#endif
