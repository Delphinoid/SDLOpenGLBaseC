#ifndef SPRITE_H
#define SPRITE_H

#include "mesh.h"

// Sprite state buffer ID. Required for batch rendering sprites.
extern GLuint sprStateBufferID;

// Sprites are just meshes that store vertices in a separate format.
return_t sprGenerateStateBuffer();
return_t sprDefaultInit();
void sprPackVertexBuffer(const vertexIndex_t vertexNum, vertex *vertices);
return_t sprGenerateBuffers(mesh *const restrict spr, const vertexIndex_t vertexNum, const vertex *const restrict vertices, const vertexIndexNum_t indexNum, const vertexIndexNum_t *const restrict indices);
void sprDeleteStateBuffer();

#endif
