#ifndef GRAPHICSDEBUG_H
#define GRAPHICSDEBUG_H

#include "skeletonShared.h"
#include "mat4.h"
#include "return.h"

typedef struct {
	unsigned int fillMode;
	vec3 colour;
} gfxDebugInfo;

return_t gfxDebugLoadShaderProgram();
void gfxDebugDeleteShaderProgram();

gfxDebugInfo gfxDebugInfoInit(const unsigned int fillMode, const vec3 colour);

void gfxDebugDrawSkeleton(
	const vec3 *const __RESTRICT__ positions, const boneIndex_t *const __RESTRICT__ parents, const boneIndex_t boneNum,
	const gfxDebugInfo info, const mat4 *const __RESTRICT__ vpMatrix
);

#endif