#ifndef SHADER_H
#define SHADER_H

#define GLEW_STATIC
#include <GL/glew.h>
#include "shaderSettings.h"
#include "skeletonSettings.h"
#include "particleSettings.h"
#include "particleState.h"
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

	// Buffer data.
	bone skeletonBindAccumulator[SKELETON_MAX_BONE_NUM];  // Accumulates bind states for bones before rendering.
	mat4 skeletonTransformState[SKELETON_MAX_BONE_NUM];   // Stores the renderable's transform state before rendering.

	// Texture samplers.
	GLuint textureSamplerArrayID[SHADER_TEXTURE_SAMPLER_NUM];

} shaderProgramObject;

typedef struct {

	// Shader program ID.
	GLuint id;

	// Buffer objects.
	/** Extend for sprites (GUI elements)? **/
	GLuint stateBufferID;  // Holds PARTICLE_SYSTEM_MAX_PARTICLE_NUM particle states.

	// Uniform objects.
	GLuint vpMatrixID;
	GLuint alphaID;
	GLuint mipID;

	// Buffer data.
	particleState particleTransformState[PARTICLE_SYSTEM_MAX_PARTICLE_NUM];

	// Texture sampler.
	GLuint textureSamplerID;

} shaderProgramSprite;

typedef struct {

	// Previously bound texture ID for more efficient binding.
	GLuint lastTexID;

	// Identity matrix.
	mat4 identityMatrix;

	// Current MIP and LODs biases.
	float biasMIP;
	size_t biasLOD;

} shaderShared;

void shdrSharedInit(shaderShared *const restrict shdrShared);
return_t shdrPrgLoad(GLuint *const restrict id, const char *const restrict prgPath, const char *const restrict vertexPath, const char *const restrict fragmentPath);
return_t shdrPrgObjLink(shaderProgramObject *const restrict shdrPrg);
return_t shdrPrgSprLink(shaderProgramSprite *const restrict shdrPrg);
void shdrPrgDelete(const void *const restrict shdrPrg);

#endif
