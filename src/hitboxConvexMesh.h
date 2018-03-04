#ifndef HITBOXCONVEXMESH_H
#define HITBOXCONVEXMESH_H

#include "vec3.h"
#include <stdlib.h>

typedef struct {
	vec3 v;         // The Minkowski "difference" of the two support points.
	size_t i1, i2;  // The indices of the support points used to generate v.
} hbSupportVertex;

typedef struct {
	vec3 contactNormal;
	float penetrationDepth;
	vec3 contactPointA;
	vec3 contactPointB;
	vec3 contactTangentA;
	vec3 contactTangentB;
} hbMeshCollisionData;

typedef struct {
	size_t vertexNum;
	vec3 *vertices;
	size_t indexNum;
	size_t *indices;
} hbMesh;

void hbMeshInit(hbMesh *hbm);
signed char hbMeshLoad(hbMesh *hbm, const char *prgPath, const char *filePath);
signed char hbMeshCollisionGJK(const hbMesh *c1, const vec3 *c1c, const hbMesh *c2, const vec3 *c2c, hbSupportVertex *simplex);
signed char hbMeshCollisionMPR(const hbMesh *c1, const vec3 *c1c, const hbMesh *c2, const vec3 *c2c, hbSupportVertex *simplex);
signed char hbMeshCollisionEPA(const hbMesh *c1, const hbMesh *c2, const hbSupportVertex *simplex, hbMeshCollisionData *cd);
void hbMeshDelete(hbMesh *hbm);

#endif
