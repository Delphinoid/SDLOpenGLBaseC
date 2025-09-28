#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "settingsTransform.h"
#include "transformRigid.h"
#include "mat4.h"
#include "mat3.h"

// Represents an affine transformation.
// To do this correctly without resorting to matrices,
// which cannot be interpolated, we introduce a "stretch
// rotation" in the form of another quaternion, "shear".
// This is the rotation such that the standard x, y and
// z axes correspond precisely to the shear basis.
///
// As a matrix A, this transformation may be written
//     A = TRQSQ^T,
// where T is a translation matrix, R is a rotation
// matrix and QSQ^T is the full affine scale matrix.
// Q here is the matrix corresponding to the stretch
// rotation, while S the diagonal scale matrix.
///
// When TRANSFORM_MATRIX_SHEAR is set, we use a matrix
// for scaling and shearing. This is faster than using
// a vector and quaternion and can give nicer results
// during interpolation.
typedef struct {
	vec3 position;
	quat orientation;
	#ifdef TRANSFORM_MATRIX_SHEAR
	mat3 scale;
	#else
	vec3 scale;
	quat shear;
	#endif
} transform;

extern transform g_tfIdentity;

void tfIdentityP(transform *const __RESTRICT__ tf);

transformRigid tfRigid(const transform tf);
void tfRigidPR(const transform *const __RESTRICT__ tf, transformRigid *const __RESTRICT__ r);
transform tfrAffine(const transformRigid tfr);
void tfrAffinePR(const transformRigid *const __RESTRICT__ tfr, transform *const __RESTRICT__ r);

mat4 tfMatrix4(const transform tf);
void tfMatrix4PR(const transform *const __RESTRICT__ tf, mat4 *const __RESTRICT__ r);
mat3x4 tfMatrix3x4(const transform tf);
void tfMatrix3x4PR(const transform *const __RESTRICT__ tf, mat3x4 *const __RESTRICT__ r);
mat3 tfMatrix3(const transform tf);
void tfMatrix3PR(const transform *const __RESTRICT__ tf, mat3 *const __RESTRICT__ r);

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

transform tfInvert(const transform tf);
void tfInvertP(transform *const __RESTRICT__ tf);
void tfInvertPR(const transform *const __RESTRICT__ tf, transform *const __RESTRICT__ r);

transform tfMultiply(const transform tf1, const transform tf2);
void tfMultiplyP1(transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2);
void tfMultiplyP2(const transform *const __RESTRICT__ tf1, transform *const __RESTRICT__ tf2);
void tfMultiplyPR(const transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, transform *const __RESTRICT__ r);

#endif
