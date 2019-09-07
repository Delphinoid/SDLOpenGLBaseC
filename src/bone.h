#ifndef BONE_H
#define BONE_H

#include "mat4.h"

// Skeleton bone data.
typedef struct {
	vec3 position;
	quat orientation;
	vec3 scale;
} bone;

void boneInit(bone *const restrict b);
mat4 boneMatrix(const bone b);

bone boneInvert(const bone b);
void boneInvertP(bone *const restrict b);
void boneInvertPR(const bone *const restrict b, bone *const restrict r);

bone boneInterpolate(const bone b1, const bone b2, const float t);
void boneInterpolateP1(bone *const restrict b1, const bone *const restrict b2, const float t);
void boneInterpolateP2(const bone *const restrict b1, bone *const restrict b2, const float t);
void boneInterpolatePR(const bone *const restrict b1, const bone *const restrict b2, const float t, bone *const restrict r);

vec3 boneTransformAppendPosition1(const bone b1, const bone b2);
vec3 boneTransformAppendPosition2(const bone b1, const bone b2);
void boneTransformAppendPositionP1(bone *const restrict b1, const bone *const restrict b2);
void boneTransformAppendPositionP2(const bone *const restrict b1, bone *const restrict b2);
void boneTransformAppendPositionPR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r);

vec3 boneTransformAppendPositionVec(const bone b, const float x, const float y, const float z);
void boneTransformAppendPositionVecP(bone *const restrict b, const float x, const float y, const float z);
void boneTransformAppendPositionVecPR(const bone *const restrict b, const float x, const float y, const float z, vec3 *const restrict r);

quat boneTransformAppendOrientation(const bone b1, const bone b2);
void boneTransformAppendOrientationP1(bone *const restrict b1, const bone *const restrict b2);
void boneTransformAppendOrientationP2(const bone *const restrict b1, bone *const restrict b2);
void boneTransformAppendOrientationPR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r);

vec3 boneTransformAppendScale(const bone b1, const bone b2);
void boneTransformAppendScaleP(bone *const restrict b1, const bone *const restrict b2);
void boneTransformAppendScalePR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r);

bone boneTransformAppend(const bone b1, const bone b2);
void boneTransformAppendP1(bone *const restrict b1, const bone *const restrict b2);
void boneTransformAppendP2(const bone *const restrict b1, bone *const restrict b2);
void boneTransformAppendPR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r);

bone boneTransformCombine(const bone b1, const bone b2);
void boneTransformCombineP1(bone *const restrict b1, const bone *const restrict b2);
void boneTransformCombineP2(const bone *const restrict b1, bone *const restrict b2);
void boneTransformCombinePR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r);

#endif
