#ifndef BONE_H
#define BONE_H

#include "quat.h"

// Skeleton bone data.
typedef struct {
	vec3 position;
	quat orientation;
	vec3 scale;
} bone;

void boneInit(bone *const restrict b);

void boneInvert(bone *const restrict b);
void boneInvertR(const bone *const restrict b, bone *const restrict r);

void boneInterpolate1(bone *const restrict b1, const bone *const restrict b2, const float t);
void boneInterpolate2(const bone *const restrict b1, bone *const restrict b2, const float t);
void boneInterpolateR(const bone *const restrict b1, const bone *const restrict b2, const float t, bone *const restrict r);

void boneTransformAppendPosition1(bone *const restrict b1, const bone *const restrict b2);
void boneTransformAppendPosition2(const bone *const restrict b1, bone *const restrict b2);
void boneTransformAppendPositionR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r);

void boneTransformAppendPositionVec(bone *const restrict b, const float x, const float y, const float z);
void boneTransformAppendPositionVecR(const bone *const restrict b, const float x, const float y, const float z, vec3 *const restrict r);

void boneTransformAppendOrientation1(bone *const restrict b1, const bone *const restrict b2);
void boneTransformAppendOrientation2(const bone *const restrict b1, bone *const restrict b2);
void boneTransformAppendOrientationR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r);

void boneTransformAppendScale(bone *const restrict b1, const bone *const restrict b2);
void boneTransformAppendScaleR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r);

void boneTransformAppend1(bone *const restrict b1, const bone *const restrict b2);
void boneTransformAppend2(const bone *const restrict b1, bone *const restrict b2);
void boneTransformAppendR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r);

void boneTransformCombine1(bone *const restrict b1, const bone *const restrict b2);
void boneTransformCombine2(const bone *const restrict b1, bone *const restrict b2);
void boneTransformCombineR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r);

#endif
