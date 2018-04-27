#ifndef BONE_H
#define BONE_H

#include "quat.h"

// Skeleton bone data.
typedef struct {
	vec3 position;
	quat orientation;
	vec3 scale;
} bone;

void boneInit(bone *b);
void boneInvert(const bone *b, bone *r);
void boneInterpolate(const bone *b1, const bone *b2, const float t, bone *r);
void boneTransformAppendPosition(const bone *b1, const bone *b2, bone *r);
void boneTransformAppendPositionVec(bone *b, const float x, const float y, const float z, vec3 *r);
void boneTransformAppendOrientation(const bone *b1, const bone *b2, bone *r);
void boneTransformAppendScale(const bone *b1, const bone *b2, bone *r);
void boneTransformAppend(const bone *b1, const bone *b2, bone *r);
void boneTransformCombine(const bone *b1, const bone *b2, bone *r);

void boneStateInvert(const bone *b, bone *out);
#endif
