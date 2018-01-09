#ifndef MESH_H
#define MESH_H

#include "vec3.h"
#include <stdlib.h>

typedef struct {
	vec3  position;
	float u, v;
	vec3 normal;
	int   bIDs[4];
	float bWeights[4];
} vertex;

typedef struct {

	/** Should meshes really be storing physical properties? **/
	/**float area;
	float mass;
	float inertia;
	vec3 centroid;  // The mesh's center of mass.**/

	size_t vertexNum;
	vertex *vertices;
	size_t indexNum;
	size_t *indices;

} mesh;

void vertInit(vertex *v);
size_t meshGetFarthestVertex(const mesh *m, const vec3 *axis);
/** Should meshes really be storing physical properties? **/
signed char meshLoadWavefrontObj(mesh *m, const char *filePath/**, const signed char generatePhysProperties**/);
void meshDelete(mesh *m);

#endif
