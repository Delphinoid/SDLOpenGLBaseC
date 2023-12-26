#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "transformRigid.h"
#include "mat4.h"
#include "mat3.h"

// Represents an affine transformation.
// To do this correctly without resorting to matrices,
// which cannot be interpolated, we introduce a "stretch
// rotation" in the form of another quaternion, "shear".
// This is the rotation such that the standard x, y and
// z axes correspond precisely to the shear basis.
//
// As a matrix A, this transformation may be written
//     A = TRQSQ^T,
// where T is a translation matrix, R is a rotation
// matrix and QSQ^T is the full affine scale matrix.
// Q here is the matrix corresponding to the stretch
// rotation, while S the diagonal scale matrix.
typedef struct {
	vec3 position;
	quat orientation;
	vec3 scale;
	quat shear;
} transform;

extern transform g_tfIdentity;

void tfIdentityP(transform *const __RESTRICT__ tf);
transformRigid tfRigid(const transform tf);
transform tfrAffine(const transformRigid tf);
mat4 tfMatrix4(const transform tf);
mat3x4 tfMatrix3x4(const transform tf);
mat3 tfMatrix3(const transform tf);

vec3 tfTransformPoint(const transform tf, const vec3 v);
void tfTransformPointP(const transform *const __RESTRICT__ tf, vec3 *const __RESTRICT__ v);
void tfTransformPointPR(const transform *const __RESTRICT__ tf, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r);

vec3 tfTransformDirection(const transform tf, const vec3 v);
void tfTransformDirectionP(const transform *const __RESTRICT__ tf, vec3 *const __RESTRICT__ v);
void tfTransformDirectionPR(const transform *const __RESTRICT__ tf, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r);

transform tfInterpolate(const transform tf1, const transform tf2, const float t);
void tfInterpolateP1(transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, const float t);
void tfInterpolateP2(const transform *const __RESTRICT__ tf1, transform *const __RESTRICT__ tf2, const float t);
void tfInterpolatePR(const transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, const float t, transform *const __RESTRICT__ r);

transform tfInverse(const transform tf);
void tfInverseP(transform *const __RESTRICT__ tf);
void tfInversePR(const transform *const __RESTRICT__ tf, transform *const __RESTRICT__ r);

transform tfMultiply(const transform tf1, const transform tf2);
void tfMultiplyP1(transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2);
void tfMultiplyP2(const transform *const __RESTRICT__ tf1, transform *const __RESTRICT__ tf2);
void tfMultiplyPR(const transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, transform *const __RESTRICT__ r);

#endif
