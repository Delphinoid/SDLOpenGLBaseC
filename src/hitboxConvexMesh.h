#ifndef HITBOXCONVEXMESH_H
#define HITBOXCONVEXMESH_H

#include "vec3.h"
#include "return.h"
#include <stdlib.h>
#include <stdint.h>

#define HB_MESH_MAX_ARRAY_SIZE 256

typedef uint_least8_t hbVertexIndex_t;
typedef uint_least8_t hbFaceIndex_t;
typedef uint_least8_t hbEdgeIndex_t;

/** Temporary? **/
typedef struct {
	hbEdgeIndex_t edgeNum;
	hbEdgeIndex_t edge;
} hbMeshFace;

typedef struct {
	// The indices of the half-edge's vertices.
	hbVertexIndex_t start;
	hbVertexIndex_t end;
	// The index of the current half-edge's associated face.
	hbFaceIndex_t face;
	// The index of the current half-edge's next half-edge.
	hbEdgeIndex_t next;
	// The index of the opposite half-edge's associated face.
	hbFaceIndex_t twinFace;
	// The index of the opposite half-edge's next half-edge.
	hbEdgeIndex_t twinNext;
} hbMeshEdge;

typedef struct {
	//vec3 centroid;  /** Update HB_MAX_COLLIDER_SIZE **/
	hbVertexIndex_t vertexNum;
	vec3 *vertices;
	hbFaceIndex_t faceNum;
	vec3 *normals;
	// Index of the first edge of each face.
	hbMeshFace *faces;
	// Edges are stored as a starting vertex index,
	// an ending vertex index, a normal index and
	// a twin normal index.
	hbEdgeIndex_t edgeNum;
	hbMeshEdge *edges;
} hbMesh;

void hbMeshInit(hbMesh *hbm);
return_t hbMeshLoad(hbMesh *hbm, const char *prgPath, const char *filePath);
void hbMeshDelete(hbMesh *hbm);

#endif
