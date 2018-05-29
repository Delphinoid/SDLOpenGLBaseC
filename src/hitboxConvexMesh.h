#ifndef HITBOXCONVEXMESH_H
#define HITBOXCONVEXMESH_H

//#include "hitboxCollision.h"
#include "vec3.h"
#include <stdlib.h>

typedef struct {
	vec3 v;         // The Minkowski "difference" of the two support points.
	size_t i1, i2;  // The indices of the support points used to generate v.
} hbMeshSupportVertex;

typedef struct {
	size_t vertexNum;
	vec3 *vertices;
	size_t indexNum;
	size_t *indices;
} hbMesh;

void hbMeshInit(hbMesh *hbm);
signed char hbMeshLoad(hbMesh *hbm, const char *prgPath, const char *filePath);
void hbMeshDelete(hbMesh *hbm);

#endif
