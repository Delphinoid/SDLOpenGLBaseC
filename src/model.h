#ifndef MODEL_H
#define MODEL_H

#include <SDL2/SDL_opengl.h>
#include "skeleton.h"
#include "cVector.h"

/** Models need to store a skeleton structure of their own (do they?). **/

typedef struct {
	vec3  position;
	float u, v;
	vec3 normal;
	int   bIDs[4];
	float bWeights[4];
} vertex;

typedef struct {
	char *name;
	skeleton *skl;
	size_t vertexNum;
	size_t indexNum;
	GLuint vaoID;  // Vertex array object ID
	GLuint vboID;  // Vertex buffer object ID
	GLuint iboID;  // Index buffer object ID
} model;

void vertInit(vertex *v);
void vertTransform(vertex *v, const vec3 *position, const quat *orientation, const vec3 *scale);
void mdlInit(model *mdl);
/** I don't like allSkeletons being passed in here either. **/
signed char mdlLoad(model *mdl, const char *prgPath, const char *filePath, cVector *allSkeletons);
signed char mdlCreateSprite(model *mdl, cVector *allSkeletons);
void mdlDelete(model *mdl);

#endif
