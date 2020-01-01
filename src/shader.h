#ifndef SHADER_H
#define SHADER_H

#define GLEW_STATIC
#include <GL/glew.h>
#include "shaderSettings.h"
#include "skeletonSettings.h"
#include "spriteSettings.h"
#include "spriteState.h"
#include "bone.h"
#include "return.h"

typedef struct {

	// Shader program ID.
	GLuint id;

	// Uniform objects.
	GLuint vpMatrixID;
	GLuint alphaID;
	GLuint mipID;
	GLuint textureFragmentID[SHADER_TEXTURE_SAMPLER_NUM];
	GLuint boneArrayID[SKELETON_MAX_BONE_NUM];

	// Texture samplers.
	GLuint textureSamplerArrayID[SHADER_TEXTURE_SAMPLER_NUM];

} shaderProgramObject;

typedef struct {

	// Shader program ID.
	GLuint id;

	// Uniform objects.
	GLuint vpMatrixID;
	GLuint alphaID;
	GLuint mipID;

	// Texture sampler.
	GLuint textureSamplerID;

} shaderProgramSprite;

typedef struct {

	// Previously bound texture ID for more efficient binding.
	GLuint lastTexID;

	// Buffer data.
	bone skeletonBindAccumulator[SKELETON_MAX_BONE_NUM];  // Accumulates bind states for bones before rendering.
	mat4 skeletonTransformState[SKELETON_MAX_BONE_NUM];   // Stores the renderable's transform state before rendering.
	spriteState spriteTransformState[SPRITE_STATE_BUFFER_SIZE];

	// Current MIP and LODs biases.
	float biasMIP;
	size_t biasLOD;

	// Identity matrix.
	mat4 identityMatrix;

} shaderData;

void shdrDataInit(shaderData *const restrict shdrData);
return_t shdrPrgLoad(GLuint *const restrict id, const char *const restrict prgPath, const char *const restrict vertexPath, const char *const restrict fragmentPath);
return_t shdrPrgObjLink(shaderProgramObject *const restrict shdrPrg);
return_t shdrPrgSprLink(shaderProgramSprite *const restrict shdrPrg);
void shdrPrgDelete(const void *const restrict shdrPrg);

#endif
