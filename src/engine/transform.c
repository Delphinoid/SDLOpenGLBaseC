#include "transform.h"
#include "mat3.h"

transform g_tfIdentity = {
	.position.x    = 0.f, .position.y    = 0.f, .position.z    = 0.f,
	.orientation.x = 0.f, .orientation.y = 0.f, .orientation.z = 0.f, .orientation.w = 1.f,
	.scale.x       = 1.f, .scale.y       = 1.f, .scale.z       = 1.f,
	.shear.x       = 0.f, .shear.y       = 0.f, .shear.z       = 0.f, .shear.w       = 1.f
};

void tfIdentityP(transform *const __RESTRICT__ tf){
	*tf = g_tfIdentity;
}
transformRigid tfRigid(const transform tf){
	const transformRigid r = {
		.position = tf.position,
		.orientation = tf.orientation,
		.scale = tf.scale
	};
	return r;
}
transform tfrAffine(const transformRigid tf){
	const transform r = {
		.position = tf.position,
		.orientation = tf.orientation,
		.scale = tf.scale,
		.shear = g_quatIdentity
	};
	return r;
}
mat4 tfMatrix4(const transform tf){
	// Translate, rotate and scale.
	return mat4Translate(
		tf.position.x, tf.position.y, tf.position.z,
		mat4Rotate(
			tf.orientation,
			mat4ShearMatrix(tf.shear, tf.scale)
		)
	);
}
mat3x4 tfMatrix3x4(const transform tf){
	// Translate, rotate and scale.
	return mat3x4Translate(
		tf.position.x, tf.position.y, tf.position.z,
		mat3x4Rotate(
			tf.orientation,
			mat3x4ShearMatrix(tf.shear, tf.scale)
		)
	);
}
mat3 tfMatrix3(const transform tf){
	// Rotate and scale.
	return mat3MMultM(
		mat3Quaternion(tf.orientation),
		mat3ShearMatrix(tf.shear, tf.scale)
	);
}

transform tfInterpolate(const transform tf1, const transform tf2, const float t){
	// Interpolate between two transformed states.
	const transform r = {
		// LERP between the start position and end position.
		.position = vec3Lerp(tf1.position, tf2.position, t),
		// SLERP between the start orientation and end orientation.
		.orientation = quatSlerpFast(tf1.orientation, tf2.orientation, t),
		// SLERP between the start stretch rotation and end stretch rotation.
		.shear = quatSlerpFast(tf1.shear, tf2.shear, t),
		// LERP once more for the scale.
		.scale = vec3Lerp(tf1.scale, tf2.scale, t)
	};
	return r;
}
void tfInterpolateP1(transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, const float t){
	// Interpolate between two transformed states.
	// LERP between the start position and end position.
	vec3LerpP1(&tf1->position, &tf2->position, t);
	// SLERP between the start orientation and end orientation.
	quatSlerpFastP1(&tf1->orientation, &tf2->orientation, t);
	// SLERP between the start stretch rotation and end stretch rotation.
	quatSlerpFastP1(&tf1->shear, &tf2->shear, t);
	// LERP once more for the scale.
	vec3LerpP1(&tf1->scale, &tf2->scale, t);
}
void tfInterpolateP2(const transform *const __RESTRICT__ tf1, transform *const __RESTRICT__ tf2, const float t){
	// Interpolate between two transformed states.
	// LERP between the start position and end position.
	vec3LerpP2(&tf1->position, &tf2->position, t);
	// SLERP between the start orientation and end orientation.
	quatSlerpFastP2(&tf1->orientation, &tf2->orientation, t);
	// SLERP between the start stretch rotation and end stretch rotation.
	quatSlerpFastP2(&tf1->shear, &tf2->shear, t);
	// LERP once more for the scale.
	vec3LerpP2(&tf1->scale, &tf2->scale, t);
}
void tfInterpolatePR(const transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, const float t, transform *const __RESTRICT__ r){
	// Interpolate between two transformed states.
	// LERP between the start position and end position.
	vec3LerpPR(&tf1->position, &tf2->position, t, &r->position);
	// SLERP between the start orientation and end orientation.
	quatSlerpFastPR(&tf1->orientation, &tf2->orientation, t, &r->orientation);
	// SLERP between the start stretch rotation and end stretch rotation.
	quatSlerpFastPR(&tf1->shear, &tf2->shear, t, &r->shear);
	// LERP once more for the scale.
	vec3LerpPR(&tf1->scale, &tf2->scale, t, &r->scale);
}

vec3 tfTransformPoint(const transform tf, const vec3 v){
	// v' = Av = (TRQSQ^T)v
	return vec3VAddV(
		tf.position,
		// (RQSQ^T)v
		quatRotateVec3FastApproximate(
			quatQMultQ(tf.orientation, tf.shear),
			// (SQ^T)v
			vec3VMultV(
				// (Q^T)v
				tf.scale,
				quatConjugateRotateVec3FastApproximate(tf.shear, v)
			)
		)
	);
}
void tfTransformPointP(const transform *const __RESTRICT__ tf, vec3 *const __RESTRICT__ v){
	quat RQ;
	// (Q^T)v
	quatConjugateRotateVec3FastApproximateP(&tf->shear, v);
	// (SQ^T)v
	vec3VMultVP(v, &tf->scale);
	// (RQSQ^T)v
	quatQMultQPR(&tf->orientation, &tf->shear, &RQ);
	quatRotateVec3FastApproximateP(&RQ, v);
	// v' = Av = (TRQSQ^T)v
	vec3VAddVP(v, &tf->position);
}
void tfTransformPointPR(const transform *const __RESTRICT__ tf, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r){
	quat RQ;
	// (Q^T)v
	quatConjugateRotateVec3FastApproximatePR(&tf->shear, v, r);
	// (SQ^T)v
	vec3VMultVP(r, &tf->scale);
	// (RQSQ^T)v
	quatQMultQPR(&tf->orientation, &tf->shear, &RQ);
	quatRotateVec3FastApproximateP(&RQ, r);
	// v' = Av = (TRQSQ^T)v
	vec3VAddVP(r, &tf->position);
}

vec3 tfTransformDirection(const transform tf, const vec3 v){
	// v' = (RQSQ^T)v
	return quatRotateVec3FastApproximate(
		quatQMultQ(tf.orientation, tf.shear),
		// (SQ^T)v
		vec3VMultV(
			// (Q^T)v
			tf.scale,
			quatConjugateRotateVec3FastApproximate(tf.shear, v)
		)
	);
}
void tfTransformDirectionP(const transform *const __RESTRICT__ tf, vec3 *const __RESTRICT__ v){
	quat RQ;
	// (Q^T)v
	quatConjugateRotateVec3FastApproximateP(&tf->shear, v);
	// (SQ^T)v
	vec3VMultVP(v, &tf->scale);
	// v' = (RQSQ^T)v
	quatQMultQPR(&tf->orientation, &tf->shear, &RQ);
	quatRotateVec3FastApproximateP(&RQ, v);
}
void tfTransformDirectionPR(const transform *const __RESTRICT__ tf, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r){
	quat RQ;
	// (Q^T)v
	quatConjugateRotateVec3FastApproximatePR(&tf->shear, v, r);
	// (SQ^T)v
	vec3VMultVP(r, &tf->scale);
	// v' = (RQSQ^T)v
	quatQMultQPR(&tf->orientation, &tf->shear, &RQ);
	quatRotateVec3FastApproximateP(&RQ, r);
}

transform tfInverse(const transform tf){

	// Recall that the full affine transformation A is given by
	//     A = TRQSQ^T.
	// The inverse of A is then
	//     A^{-1} = (TRQSQ^T)^{-1} = -Q S^{-1} Q^T R^T T.
	// Therefore, if we want some affine transformation A' such
	// that A' = A^{-1} = T'R'Q'S'(Q')^T, we require
	//     T'           = -Q S^{-1} Q^T R^T T,
	//     R'Q'S'(Q')^T = Q S^{-1} Q^T R^T.
	// By letting Q'' = RQ, we can solve for the remaining components:
	//     R'Q'S'(Q')^T = R^T Q'' S^{-1} (Q'')^T,
	//     {R' = R^T,
	//  => {Q' = RQ,
	//     {S' = S^(-1).

	transform r;

	// S' = S^{-1}
	r.scale = vec3SDivV(1.f, tf.scale);

	// Q' = RQ
	r.shear = quatQMultQ(tf.orientation, tf.shear);

	// R' = R^T
	r.orientation = quatConjugateFast(tf.orientation);

	// T' = -Q S^{-1} Q^T R^T T
	// Note that we use Q^T R^T = (RQ)^T = (Q')^T.
	r.position = vec3Negate(
		quatRotateVec3FastApproximate(
			tf.shear,
			vec3VMultV(
				r.scale,
				quatConjugateRotateVec3FastApproximate(r.shear, tf.position)
			)
		)
	);

	return r;

}
void tfInverseP(transform *const __RESTRICT__ tf){

	// Recall that the full affine transformation A is given by
	//     A = TRQSQ^T.
	// The inverse of A is then
	//     A^{-1} = (TRQSQ^T)^{-1} = -Q S^{-1} Q^T R^T T.
	// Therefore, if we want some affine transformation A' such
	// that A' = A^{-1} = T'R'Q'S'(Q')^T, we require
	//     T'           = -Q S^{-1} Q^T R^T T,
	//     R'Q'S'(Q')^T = Q S^{-1} Q^T R^T.
	// By letting Q'' = RQ, we can solve for the remaining components:
	//     R'Q'S'(Q')^T = R^T Q'' S^{-1} (Q'')^T,
	//     {R' = R^T,
	//  => {Q' = RQ,
	//     {S' = S^(-1).

	const quat shear = tf->shear;

	// S' = S^{-1}
	vec3SDivVP(1.f, &tf->scale);

	// Q' = RQ
	quatQMultQP2(&tf->orientation, &tf->shear);

	// R' = R^T
	quatConjugateFastP(&tf->orientation);

	// T' = -Q S^{-1} Q^T R^T T
	// Note that we use Q^T R^T = (RQ)^T = (Q')^T.
	quatConjugateRotateVec3FastApproximateP(&tf->shear, &tf->position);
	vec3VMultVP(&tf->position, &tf->scale);
	quatRotateVec3FastApproximateP(&shear, &tf->position);
	vec3NegateP(&tf->position);

}
void tfInversePR(const transform *const __RESTRICT__ tf, transform *const __RESTRICT__ r){

	// Recall that the full affine transformation A is given by
	//     A = TRQSQ^T.
	// The inverse of A is then
	//     A^{-1} = (TRQSQ^T)^{-1} = -Q S^{-1} Q^T R^T T.
	// Therefore, if we want some affine transformation A' such
	// that A' = A^{-1} = T'R'Q'S'(Q')^T, we require
	//     T'           = -Q S^{-1} Q^T R^T T,
	//     R'Q'S'(Q')^T = Q S^{-1} Q^T R^T.
	// By letting Q'' = RQ, we can solve for the remaining components:
	//     R'Q'S'(Q')^T = R^T Q'' S^{-1} (Q'')^T,
	//     {R' = R^T,
	//  => {Q' = RQ,
	//     {S' = S^(-1).

	const quat shear = tf->shear;

	// S' = S^{-1}
	vec3SDivVPR(1.f, &tf->scale, &r->scale);

	// Q' = RQ
	quatQMultQPR(&tf->orientation, &tf->shear, &r->shear);

	// R' = R^T
	quatConjugateFastPR(&tf->orientation, &r->orientation);

	// T' = -Q S^{-1} Q^T R^T T
	// Note that we use Q^T R^T = (RQ)^T = (Q')^T.
	quatConjugateRotateVec3FastApproximatePR(&tf->shear, &tf->position, &r->position);
	vec3VMultVP(&r->position, &tf->scale);
	quatRotateVec3FastApproximateP(&shear, &r->position);
	vec3NegateP(&r->position);

}

transform tfMultiply(const transform tf1, const transform tf2){

	// We wish to append tf1 to tf2. If we let A_1 and A_2 be the
	// matrices for tf1 and tf2 respectively, this is equivalent
	// to evaluating the matrix product A = A_1 A_2.
	//
	// As matrices, we may write our affine transformations as
	//     A_k = T_k R_k Q_k S_k Q_k^T,
	// for k in {1, 2}. By expanding A = A_1 A_2, we find that
	//     T      = T_1 R_1 Q_1 S_1 Q_1^T T_2,
	//     RQSQ^T = R_1 Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q^2^T.
	// Note that, although Q_k S_k Q_k^T is symmetric for k in
	// {1, 2}, we have inserted an R_2 between them, which in
	// general breaks symmetry. However, post-rotations (such as
	// R_1) do not affect shearing; we can use this to our
	// advantage. In particular, by writing Q_1' = R_2^T Q_1,
	// we can actually write
	//     RQSQ^T = R_1 R_2 Q_1' S_1 (Q_1')^T Q_2 S_2 Q_2^T,
	//     {R     = R_1 R_2,
	//  => {QSQ^T = Q_1' S_1 (Q_1')^T Q_2 S_2 Q_2^T.
	// This happily gives us a symmetric QSQ^T as expected!

	transform r;

	// Q_1' S_1 (Q_1')^T
	const mat3 shear1 = mat3ShearMatrix(
		// Q_1' = R_2^T Q_1
		quatQConjugateMultQ(tf2.orientation, tf1.shear),
		tf1.scale
	);
	// Q_2 S_2 Q_2^T
	const mat3 shear2 = mat3ShearMatrix(tf2.shear, tf2.scale);

	// T = T_1 R_1 Q_1 S_1 Q_1^T T_2
	r.position = vec3VAddV(
		tf1.position,
		// R_1 Q_1 S_1 Q_1^T T_2
		quatRotateVec3FastApproximate(
			// R_1 Q_1
			quatQMultQ(tf1.orientation, tf1.shear),
			// S_1 Q_1^T T_2
			vec3VMultV(
				tf1.scale,
				// Q_1^T T_2
				quatConjugateRotateVec3FastApproximate(tf1.shear, tf2.position)
			)
		)
	);

	// R = R_1 R_2
	r.orientation = quatNormalizeFast(quatQMultQ(tf1.orientation, tf2.orientation));

	// QSQ^T = Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T
	// The shear matrices are symmetric, so we
	// do the multiplication here to save time.
	mat3DiagonalizeSymmetric(
		shear1.m[0][0]*shear2.m[0][0] + shear1.m[1][0]*shear2.m[0][1] + shear1.m[2][0]*shear2.m[0][2],
		shear1.m[0][0]*shear2.m[1][0] + shear1.m[1][0]*shear2.m[1][1] + shear1.m[2][0]*shear2.m[1][2],
		shear1.m[0][0]*shear2.m[2][0] + shear1.m[1][0]*shear2.m[2][1] + shear1.m[2][0]*shear2.m[2][2],
		shear1.m[0][1]*shear2.m[1][0] + shear1.m[1][1]*shear2.m[1][1] + shear1.m[2][1]*shear2.m[1][2],
		shear1.m[0][1]*shear2.m[2][0] + shear1.m[1][1]*shear2.m[2][1] + shear1.m[2][1]*shear2.m[2][2],
		shear1.m[0][2]*shear2.m[2][0] + shear1.m[1][2]*shear2.m[2][1] + shear1.m[2][2]*shear2.m[2][2],
		&r.scale, &r.shear
	);

	return r;

}
void tfMultiplyP1(transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2){

	// We wish to append tf1 to tf2. If we let A_1 and A_2 be the
	// matrices for tf1 and tf2 respectively, this is equivalent
	// to evaluating the matrix product A = A_1 A_2.
	//
	// As matrices, we may write our affine transformations as
	//     A_k = T_k R_k Q_k S_k Q_k^T,
	// for k in {1, 2}. By expanding A = A_1 A_2, we find that
	//     T      = T_1 R_1 Q_1 S_1 Q_1^T T_2,
	//     RQSQ^T = R_1 Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q^2^T.
	// Note that, although Q_k S_k Q_k^T is symmetric for k in
	// {1, 2}, we have inserted an R_2 between them, which in
	// general breaks symmetry. However, post-rotations (such as
	// R_1) do not affect shearing; we can use this to our
	// advantage. In particular, by writing Q_1' = R_2^T Q_1,
	// we can actually write
	//     RQSQ^T = R_1 R_2 Q_1' S_1 (Q_1')^T Q_2 S_2 Q_2^T,
	//     {R     = R_1 R_2,
	//  => {QSQ^T = Q_1' S_1 (Q_1')^T Q_2 S_2 Q_2^T.
	// This happily gives us a symmetric QSQ^T as expected!

	quat temp;
	mat3 shear1, shear2;
	vec3 position2;

	// Q_1' = R_2^T Q_1
	quatQConjugateMultQPR(&tf2->orientation, &tf1->shear, &temp);
	// Q_1' S_1 (Q_1')^T
	mat3ShearMatrixPR(&temp, &tf1->scale, &shear1);
	// Q_2 S_2 Q_2^T
	mat3ShearMatrixPR(&tf2->shear, &tf2->scale, &shear2);

	// Q_1^T T_2
	quatConjugateRotateVec3FastApproximatePR(&tf1->shear, &tf2->position, &position2);
	// S_1 Q_1^T T_2
	vec3VMultVP(&position2, &tf1->scale);
	// R_1 Q_1
	quatQMultQPR(&tf1->orientation, &tf1->shear, &temp);
	// R_1 Q_1 S_1 Q_1^T T_2
	quatRotateVec3FastApproximateP(&temp, &position2);
	// T = T_1 R_1 Q_1 S_1 Q_1^T T_2
	vec3VAddVP(&tf1->position, &position2);

	// R = R_1 R_2
	quatQMultQP1(&tf1->orientation, &tf2->orientation);
	quatNormalizeFastP(&tf1->orientation);

	// QSQ^T = Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T
	// The shear matrices are symmetric, so we
	// do the multiplication here to save time.
	mat3DiagonalizeSymmetric(
		shear1.m[0][0]*shear2.m[0][0] + shear1.m[1][0]*shear2.m[0][1] + shear1.m[2][0]*shear2.m[0][2],
		shear1.m[0][0]*shear2.m[1][0] + shear1.m[1][0]*shear2.m[1][1] + shear1.m[2][0]*shear2.m[1][2],
		shear1.m[0][0]*shear2.m[2][0] + shear1.m[1][0]*shear2.m[2][1] + shear1.m[2][0]*shear2.m[2][2],
		shear1.m[0][1]*shear2.m[1][0] + shear1.m[1][1]*shear2.m[1][1] + shear1.m[2][1]*shear2.m[1][2],
		shear1.m[0][1]*shear2.m[2][0] + shear1.m[1][1]*shear2.m[2][1] + shear1.m[2][1]*shear2.m[2][2],
		shear1.m[0][2]*shear2.m[2][0] + shear1.m[1][2]*shear2.m[2][1] + shear1.m[2][2]*shear2.m[2][2],
		&tf1->scale, &tf1->shear
	);

}
void tfMultiplyP2(const transform *const __RESTRICT__ tf1, transform *const __RESTRICT__ tf2){

	// We wish to append tf1 to tf2. If we let A_1 and A_2 be the
	// matrices for tf1 and tf2 respectively, this is equivalent
	// to evaluating the matrix product A = A_1 A_2.
	//
	// As matrices, we may write our affine transformations as
	//     A_k = T_k R_k Q_k S_k Q_k^T,
	// for k in {1, 2}. By expanding A = A_1 A_2, we find that
	//     T      = T_1 R_1 Q_1 S_1 Q_1^T T_2,
	//     RQSQ^T = R_1 Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q^2^T.
	// Note that, although Q_k S_k Q_k^T is symmetric for k in
	// {1, 2}, we have inserted an R_2 between them, which in
	// general breaks symmetry. However, post-rotations (such as
	// R_1) do not affect shearing; we can use this to our
	// advantage. In particular, by writing Q_1' = R_2^T Q_1,
	// we can actually write
	//     RQSQ^T = R_1 R_2 Q_1' S_1 (Q_1')^T Q_2 S_2 Q_2^T,
	//     {R     = R_1 R_2,
	//  => {QSQ^T = Q_1' S_1 (Q_1')^T Q_2 S_2 Q_2^T.
	// This happily gives us a symmetric QSQ^T as expected!

	quat temp;
	mat3 shear1, shear2;

	// Q_1' = R_2^T Q_1
	quatQConjugateMultQPR(&tf2->orientation, &tf1->shear, &temp);
	// Q_1' S_1 (Q_1')^T
	mat3ShearMatrixPR(&temp, &tf1->scale, &shear1);
	// Q_2 S_2 Q_2^T
	mat3ShearMatrixPR(&tf2->shear, &tf2->scale, &shear2);

	// Q_1^T T_2
	quatConjugateRotateVec3FastApproximateP(&tf1->shear, &tf2->position);
	// S_1 Q_1^T T_2
	vec3VMultVP(&tf2->position, &tf1->scale);
	// R_1 Q_1
	quatQMultQPR(&tf1->orientation, &tf1->shear, &temp);
	// R_1 Q_1 S_1 Q_1^T T_2
	quatRotateVec3FastApproximateP(&temp, &tf2->position);
	// T = T_1 R_1 Q_1 S_1 Q_1^T T_2
	vec3VAddVP(&tf2->position, &tf1->position);

	// R = R_1 R_2
	quatQMultQP2(&tf1->orientation, &tf2->orientation);
	quatNormalizeFastP(&tf2->orientation);

	// QSQ^T = Q_2' S_2 Q_2'^T Q_1 S_1 Q_1^T
	// The shear matrices are symmetric, so we
	// do the multiplication here to save time.
	mat3DiagonalizeSymmetric(
		shear1.m[0][0]*shear2.m[0][0] + shear1.m[1][0]*shear2.m[0][1] + shear1.m[2][0]*shear2.m[0][2],
		shear1.m[0][0]*shear2.m[1][0] + shear1.m[1][0]*shear2.m[1][1] + shear1.m[2][0]*shear2.m[1][2],
		shear1.m[0][0]*shear2.m[2][0] + shear1.m[1][0]*shear2.m[2][1] + shear1.m[2][0]*shear2.m[2][2],
		shear1.m[0][1]*shear2.m[1][0] + shear1.m[1][1]*shear2.m[1][1] + shear1.m[2][1]*shear2.m[1][2],
		shear1.m[0][1]*shear2.m[2][0] + shear1.m[1][1]*shear2.m[2][1] + shear1.m[2][1]*shear2.m[2][2],
		shear1.m[0][2]*shear2.m[2][0] + shear1.m[1][2]*shear2.m[2][1] + shear1.m[2][2]*shear2.m[2][2],
		&tf2->scale, &tf2->shear
	);

}
void tfMultiplyPR(const transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, transform *const __RESTRICT__ r){

	// We wish to append tf1 to tf2. If we let A_1 and A_2 be the
	// matrices for tf1 and tf2 respectively, this is equivalent
	// to evaluating the matrix product A = A_1 A_2.
	//
	// As matrices, we may write our affine transformations as
	//     A_k = T_k R_k Q_k S_k Q_k^T,
	// for k in {1, 2}. By expanding A = A_1 A_2, we find that
	//     T      = T_1 R_1 Q_1 S_1 Q_1^T T_2,
	//     RQSQ^T = R_1 Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q^2^T.
	// Note that, although Q_k S_k Q_k^T is symmetric for k in
	// {1, 2}, we have inserted an R_2 between them, which in
	// general breaks symmetry. However, post-rotations (such as
	// R_1) do not affect shearing; we can use this to our
	// advantage. In particular, by writing Q_1' = R_2^T Q_1,
	// we can actually write
	//     RQSQ^T = R_1 R_2 Q_1' S_1 (Q_1')^T Q_2 S_2 Q_2^T,
	//     {R     = R_1 R_2,
	//  => {QSQ^T = Q_1' S_1 (Q_1')^T Q_2 S_2 Q_2^T.
	// This happily gives us a symmetric QSQ^T as expected!

	quat temp;
	mat3 shear1, shear2;

	// Q_1' = R_2^T Q_1
	quatQConjugateMultQPR(&tf2->orientation, &tf1->shear, &temp);
	// Q_1' S_1 (Q_1')^T
	mat3ShearMatrixPR(&temp, &tf1->scale, &shear1);
	// Q_2 S_2 Q_2^T
	mat3ShearMatrixPR(&tf2->shear, &tf2->scale, &shear2);

	// Q_1^T T_2
	quatConjugateRotateVec3FastApproximatePR(&tf1->shear, &tf2->position, &r->position);
	// S_1 Q_1^T T_2
	vec3VMultVP(&r->position, &tf1->scale);
	// R_1 Q_1
	quatQMultQPR(&tf1->orientation, &tf1->shear, &temp);
	// R_1 Q_1 S_1 Q_1^T T_2
	quatRotateVec3FastApproximateP(&temp, &r->position);
	// T = T_1 R_1 Q_1 S_1 Q_1^T T_2
	vec3VAddVP(&r->position, &tf1->position);

	// R = R_1 R_2
	quatQMultQPR(&tf1->orientation, &tf2->orientation, &r->orientation);
	quatNormalizeFastP(&r->orientation);

	// QSQ^T = Q_2' S_2 Q_2'^T Q_1 S_1 Q_1^T
	// The shear matrices are symmetric, so we
	// do the multiplication here to save time.
	mat3DiagonalizeSymmetric(
		shear1.m[0][0]*shear2.m[0][0] + shear1.m[1][0]*shear2.m[0][1] + shear1.m[2][0]*shear2.m[0][2],
		shear1.m[0][0]*shear2.m[1][0] + shear1.m[1][0]*shear2.m[1][1] + shear1.m[2][0]*shear2.m[1][2],
		shear1.m[0][0]*shear2.m[2][0] + shear1.m[1][0]*shear2.m[2][1] + shear1.m[2][0]*shear2.m[2][2],
		shear1.m[0][1]*shear2.m[1][0] + shear1.m[1][1]*shear2.m[1][1] + shear1.m[2][1]*shear2.m[1][2],
		shear1.m[0][1]*shear2.m[2][0] + shear1.m[1][1]*shear2.m[2][1] + shear1.m[2][1]*shear2.m[2][2],
		shear1.m[0][2]*shear2.m[2][0] + shear1.m[1][2]*shear2.m[2][1] + shear1.m[2][2]*shear2.m[2][2],
		&r->scale, &r->shear
	);

}
