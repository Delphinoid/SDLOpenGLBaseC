#ifndef MODEL_H
#define MODEL_H

#include <SDL2/SDL_opengl.h>
#include "vertex.h"
#include "skeleton.h"
#include "physicsRigidBody.h"
#include "hitbox.h"
#include "cVector.h"

typedef uint32_t vertexIndex_t;
typedef uint32_t vertexIndexNum_t;

typedef struct {

	char *name;

	/* Skeleton associated with the model. */
	skeleton *skl;

	/* Model data for rendering. */
	vertexIndex_t vertexNum;
	vertexIndexNum_t indexNum;
	GLuint vaoID;  // Vertex array object ID
	GLuint vboID;  // Vertex buffer object ID
	GLuint iboID;  // Index buffer object ID

	/* Animation and collision data. */
	/**size_t boneNum;
	char **boneNames;       // An array of bone names.**/
	/**physRigidBody *bodies;  // An array of physics body descriptors, one for each bone.
	hitbox **hitboxes;      // An array of hitbox arrays, one for each bone.**/

} model;

void mdlInit(model *mdl);
/** I don't like the cVector being passed in here at all. **/
signed char mdlLoad(model *mdl, const char *prgPath, const char *filePath, cVector *allSkeletons);
signed char mdlDefault(model *mdl, cVector *allSkeletons);
signed char mdlCreateSprite(model *mdl, cVector *allSkeletons);
void mdlDelete(model *mdl);

#endif
