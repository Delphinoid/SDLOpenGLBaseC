#ifndef SHADER_H
#define SHADER_H

#define GLEW_STATIC
#include <GL/glew.h>
#include "shaderSettings.h"
#include "skeletonSettings.h"
#include "spriteSettings.h"
#include "spriteState.h"
#include "transform.h"
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
	GLuint sdfTypeID;
	GLuint sdfColourID;
	GLuint sdfBackgroundID;

	// Texture sampler.
	GLuint textureSamplerID;

} shaderProgramSprite;

typedef struct {

	// Previously bound texture ID for more efficient binding.
	GLuint lastTexID;

	// Buffer data.
	transform skeletonBindAccumulator[SKELETON_MAX_BONE_NUM];  // Accumulates bind states for bones before rendering.
	mat4 skeletonTransformState[SKELETON_MAX_BONE_NUM];   // Stores the renderable's transform state before rendering.
	spriteState spriteTransformState[SPRITE_STATE_BUFFER_SIZE];

	// Current MIP and LODs biases.
	float biasMIP;
	size_t biasLOD;

	// Identity matrix.
	mat4 identityMatrix;

} shaderData;

void shdrDataInit(shaderData *const __RESTRICT__ shdrData);
return_t shdrLoad(GLuint *const __RESTRICT__ id, GLenum type, const char *const __RESTRICT__ filePath, const size_t filePathLength);
return_t shdrPrgLoad(GLuint *const __RESTRICT__ id, const char *const __RESTRICT__ vertexPath, const size_t vertexPathLength, const char *const __RESTRICT__ fragmentPath, const size_t fragmentPathLength);
return_t shdrPrgObjLink(shaderProgramObject *const __RESTRICT__ shdrPrg);
return_t shdrPrgSprLink(shaderProgramSprite *const __RESTRICT__ shdrPrg);
void shdrPrgDelete(const void *const __RESTRICT__ shdrPrg);

#endif
