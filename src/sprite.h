#ifndef SPRITE_H
#define SPRITE_H

#include "mesh.h"

// Sprites are just meshes that store vertices in a separate format.
return_t sprDefaultInit(const GLuint stateBufferID);
void sprPackVertexBuffer(const vertexIndex_t vertexNum, vertex *vertices);
return_t sprGenerateBuffers(mesh *const restrict spr, const GLuint stateBufferID, const vertexIndex_t vertexNum, const vertex *const restrict vertices, const vertexIndexNum_t indexNum, const vertexIndexNum_t *const restrict indices);

#endif
