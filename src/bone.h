#ifndef BONE_H
#define BONE_H

#include "mat4.h"

// Skeleton bone data.
typedef struct {
	vec3 position;
	quat orientation;
	vec3 scale;
} bone;

void boneInit(bone *const __RESTRICT__ b);
bone boneIdentity();
mat4 boneMatrix(const bone b);

bone boneInvert(const bone b);
void boneInvertP(bone *const __RESTRICT__ b);
void boneInvertPR(const bone *const __RESTRICT__ b, bone *const __RESTRICT__ r);

bone boneInvertFast(const bone b);
void boneInvertFastP(bone *const __RESTRICT__ b);
void boneInvertFastPR(const bone *const __RESTRICT__ b, bone *const __RESTRICT__ r);

bone boneInterpolate(const bone b1, const bone b2, const float t);
void boneInterpolateP1(bone *const __RESTRICT__ b1, const bone *const __RESTRICT__ b2, const float t);
void boneInterpolateP2(const bone *const __RESTRICT__ b1, bone *const __RESTRICT__ b2, const float t);
void boneInterpolatePR(const bone *const __RESTRICT__ b1, const bone *const __RESTRICT__ b2, const float t, bone *const __RESTRICT__ r);

vec3 boneTransformAppendPosition1(const bone b1, const bone b2);
vec3 boneTransformAppendPosition2(const bone b1, const bone b2);
void boneTransformAppendPositionP1(bone *const __RESTRICT__ b1, const bone *const __RESTRICT__ b2);
void boneTransformAppendPositionP2(const bone *const __RESTRICT__ b1, bone *const __RESTRICT__ b2);
void boneTransformAppendPositionPR(const bone *const __RESTRICT__ b1, const bone *const __RESTRICT__ b2, bone *const __RESTRICT__ r);

vec3 boneTransformAppendPositionVec(const bone b, const float x, const float y, const float z);
void boneTransformAppendPositionVecP(bone *const __RESTRICT__ b, const float x, const float y, const float z);
void boneTransformAppendPositionVecPR(const bone *const __RESTRICT__ b, const float x, const float y, const float z, vec3 *const __RESTRICT__ r);

quat boneTransformAppendOrientation(const bone b1, const bone b2);
void boneTransformAppendOrientationP1(bone *const __RESTRICT__ b1, const bone *const __RESTRICT__ b2);
void boneTransformAppendOrientationP2(const bone *const __RESTRICT__ b1, bone *const __RESTRICT__ b2);
void boneTransformAppendOrientationPR(const bone *const __RESTRICT__ b1, const bone *const __RESTRICT__ b2, bone *const __RESTRICT__ r);

vec3 boneTransformAppendScale(const bone b1, const bone b2);
void boneTransformAppendScaleP(bone *const __RESTRICT__ b1, const bone *const __RESTRICT__ b2);
void boneTransformAppendScalePR(const bone *const __RESTRICT__ b1, const bone *const __RESTRICT__ b2, bone *const __RESTRICT__ r);

bone boneTransformAppend(const bone b1, const bone b2);
void boneTransformAppendP1(bone *const __RESTRICT__ b1, const bone *const __RESTRICT__ b2);
void boneTransformAppendP2(const bone *const __RESTRICT__ b1, bone *const __RESTRICT__ b2);
void boneTransformAppendPR(const bone *const __RESTRICT__ b1, const bone *const __RESTRICT__ b2, bone *const __RESTRICT__ r);

bone boneTransformUndoPrepend(const bone b1, const bone b2);

bone boneTransformCombine(const bone b1, const bone b2);
void boneTransformCombineP1(bone *const __RESTRICT__ b1, const bone *const __RESTRICT__ b2);
void boneTransformCombineP2(const bone *const __RESTRICT__ b1, bone *const __RESTRICT__ b2);
void boneTransformCombinePR(const bone *const __RESTRICT__ b1, const bone *const __RESTRICT__ b2, bone *const __RESTRICT__ r);

#endif
