#include "transform.h"
#include "mat3.h"

transform g_tfIdentity = {
	.position.x    = 0.f, .position.y    = 0.f, .position.z    = 0.f,
	.orientation.x = 0.f, .orientation.y = 0.f, .orientation.z = 0.f, .orientation.w = 1.f,
	#ifdef TRANSFORM_MATRIX_SHEAR
	.scale.m       = {{1.f, 0.f, 0.f},
	                 {0.f, 1.f, 0.f},
	                 {0.f, 0.f, 1.f}}
	#else
	.scale.x       = 1.f, .scale.y       = 1.f, .scale.z       = 1.f,
	.shear.x       = 0.f, .shear.y       = 0.f, .shear.z       = 0.f, .shear.w       = 1.f
	#endif
};

void tfIdentityP(transform *const __RESTRICT__ tf){
	*tf = g_tfIdentity;
}

transformRigid tfRigid(const transform tf){
	#ifdef TRANSFORM_MATRIX_SHEAR
	const transformRigid r = {
		.position = tf.position,
		.orientation = tf.orientation,
		.scale.x = tf.scale.m[0][0], .scale.y = tf.scale.m[1][1], .scale.z = tf.scale.m[2][2],
	};
	#else
	const transformRigid r = {
		.position = tf.position,
		.orientation = tf.orientation,
		.scale = tf.scale
	};
	#endif
	return r;
}
void tfRigidPR(const transform *const __RESTRICT__ tf, transformRigid *const __RESTRICT__ r){
	#ifdef TRANSFORM_MATRIX_SHEAR
	r->position = tf->position;
	r->orientation = tf->orientation;
	r->scale.x = tf->scale.m[0][0];
	r->scale.y = tf->scale.m[1][1];
	r->scale.z = tf->scale.m[2][2];
	#else
	r->position = tf->position;
	r->orientation = tf->orientation;
	r->scale = tf->scale;
	#endif
}
transform tfrAffine(const transformRigid tfr){
	#ifdef TRANSFORM_MATRIX_SHEAR
	const transform r = {
		.position = tfr.position,
		.orientation = tfr.orientation,
		.scale.m = {{tfr.scale.x, 0.f, 0.f},
	                {0.f, tfr.scale.y, 0.f},
	                {0.f, 0.f, tfr.scale.z}}
	};
	#else
	const transform r = {
		.position = tfr.position,
		.orientation = tfr.orientation,
		.scale = tfr.scale,
		.shear = g_quatIdentity
	};
	#endif
	return r;
}
void tfrAffinePR(const transformRigid *const __RESTRICT__ tfr, transform *const __RESTRICT__ r){
	#ifdef TRANSFORM_MATRIX_SHEAR
	r->position = tfr->position;
	r->orientation = tfr->orientation;
	mat3DiagonalVP(&r->scale, &tfr->scale);
	#else
	r->position = tfr->position;
	r->orientation = tfr->orientation;
	r->scale = tfr->scale;
	r->shear = g_quatIdentity;
	#endif
}

mat4 tfMatrix4(const transform tf){
	#ifdef TRANSFORM_MATRIX_SHEAR
	// Translate, rotate and scale.
	return mat4Translate(
		tf.position.x, tf.position.y, tf.position.z,
		mat4MMultM3(
			mat4Quaternion(tf.orientation),
			tf.scale
		)
	);
	#else
	// Translate, rotate and scale.
	return mat4Translate(
		tf.position.x, tf.position.y, tf.position.z,
		mat4Rotate(
			tf.orientation,
			mat4ShearMatrix(tf.shear, tf.scale)
		)
	);
	#endif
}
void tfMatrix4PR(const transform *const __RESTRICT__ tf, mat4 *const __RESTRICT__ r){
	#ifdef TRANSFORM_MATRIX_SHEAR
	// Translate, rotate and scale.
	mat4QuaternionPR(&tf->orientation, r);
	mat4MMultM3P(r, &tf->scale);
	mat4TranslateP(tf->position.x, tf->position.y, tf->position.z, r);
	#else
	// Translate, rotate and scale.
	mat4ShearMatrixPR(&tf->shear, &tf->scale, r);
	mat4RotatePR(&tf->orientation, r);
	mat4TranslateP(tf->position.x, tf->position.y, tf->position.z, r);
	#endif
}
mat3x4 tfMatrix3x4(const transform tf){
	#ifdef TRANSFORM_MATRIX_SHEAR
	// Translate, rotate and scale.
	return mat3x4Translate(
		tf.position.x, tf.position.y, tf.position.z,
		mat3x4MMultM3(
			mat3x4Quaternion(tf.orientation),
			tf.scale
		)
	);
	#else
	// Translate, rotate and scale.
	return mat3x4Translate(
		tf.position.x, tf.position.y, tf.position.z,
		mat3x4Rotate(
			tf.orientation,
			mat3x4ShearMatrix(tf.shear, tf.scale)
		)
	);
	#endif
}
void tfMatrix3x4PR(const transform *const __RESTRICT__ tf, mat3x4 *const __RESTRICT__ r){
	#ifdef TRANSFORM_MATRIX_SHEAR
	// Translate, rotate and scale.
	mat3x4QuaternionPR(&tf->orientation, r);
	mat3x4MMultM3P(r, &tf->scale);
	mat3x4TranslateP(tf->position.x, tf->position.y, tf->position.z, r);
	#else
	// Translate, rotate and scale.
	mat3x4ShearMatrixPR(&tf->shear, &tf->scale, r);
	mat3x4RotatePR(&tf->orientation, r);
	mat3x4TranslateP(tf->position.x, tf->position.y, tf->position.z, r);
	#endif
}
mat3 tfMatrix3(const transform tf){
	#ifdef TRANSFORM_MATRIX_SHEAR
	// Rotate and scale.
	return mat3MMultM(
		mat3Quaternion(tf.orientation),
		tf.scale
	);
	#else
	// Rotate and scale.
	return mat3MMultM(
		mat3Quaternion(tf.orientation),
		mat3ShearMatrix(tf.shear, tf.scale)
	);
	#endif
}
void tfMatrix3PR(const transform *const __RESTRICT__ tf, mat3 *const __RESTRICT__ r){
	#ifdef TRANSFORM_MATRIX_SHEAR
	// Rotate and scale.
	mat3QuaternionPR(&tf->orientation, r);
	mat3MMultMP1(r, &tf->scale);
	#else
	// Rotate and scale.
	mat3 orientation;
	mat3QuaternionPR(&tf->orientation, &orientation);
	mat3ShearMatrixPR(&tf->shear, &tf->scale, r);
	mat3MMultMP2(&orientation, r);
	#endif
}

transform tfInterpolate(const transform tf1, const transform tf2, const float t){
	// Interpolate between two transformed states.
	const transform r = {
		// LERP between the start position and end position.
		.position = vec3Lerp(tf1.position, tf2.position, t),
		// SLERP between the start orientation and end orientation.
		.orientation = quatSlerpFast(tf1.orientation, tf2.orientation, t),
		#ifdef TRANSFORM_MATRIX_SHEAR
		// LERP once more for the scale.
		.scale = mat3Lerp(tf1.scale, tf2.scale, t)
		#else
		// SLERP between the start stretch rotation and end stretch rotation.
		.shear = quatSlerpFast(tf1.shear, tf2.shear, t),
		// LERP once more for the scale.
		.scale = vec3Lerp(tf1.scale, tf2.scale, t)
		#endif
	};
	return r;
}
void tfInterpolateP1(transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, const float t){
	// Interpolate between two transformed states.
	// LERP between the start position and end position.
	vec3LerpP1(&tf1->position, &tf2->position, t);
	// SLERP between the start orientation and end orientation.
	quatSlerpFastP1(&tf1->orientation, &tf2->orientation, t);
	#ifdef TRANSFORM_MATRIX_SHEAR
	// LERP once more for the scale.
	mat3LerpP1(&tf1->scale, &tf2->scale, t);
	#else
	// SLERP between the start stretch rotation and end stretch rotation.
	quatSlerpFastP1(&tf1->shear, &tf2->shear, t);
	// LERP once more for the scale.
	vec3LerpP1(&tf1->scale, &tf2->scale, t);
	#endif
}
void tfInterpolateP2(const transform *const __RESTRICT__ tf1, transform *const __RESTRICT__ tf2, const float t){
	// Interpolate between two transformed states.
	// LERP between the start position and end position.
	vec3LerpP2(&tf1->position, &tf2->position, t);
	// SLERP between the start orientation and end orientation.
	quatSlerpFastP2(&tf1->orientation, &tf2->orientation, t);
	#ifdef TRANSFORM_MATRIX_SHEAR
	// LERP once more for the scale.
	mat3LerpP2(&tf1->scale, &tf2->scale, t);
	#else
	// SLERP between the start stretch rotation and end stretch rotation.
	quatSlerpFastP2(&tf1->shear, &tf2->shear, t);
	// LERP once more for the scale.
	vec3LerpP2(&tf1->scale, &tf2->scale, t);
	#endif
}
void tfInterpolatePR(const transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, const float t, transform *const __RESTRICT__ r){
	// Interpolate between two transformed states.
	// LERP between the start position and end position.
	vec3LerpPR(&tf1->position, &tf2->position, t, &r->position);
	// SLERP between the start orientation and end orientation.
	quatSlerpFastPR(&tf1->orientation, &tf2->orientation, t, &r->orientation);
	#ifdef TRANSFORM_MATRIX_SHEAR
	// LERP once more for the scale.
	mat3LerpPR(&tf1->scale, &tf2->scale, t, &r->scale);
	#else
	// SLERP between the start stretch rotation and end stretch rotation.
	quatSlerpFastPR(&tf1->shear, &tf2->shear, t, &r->shear);
	// LERP once more for the scale.
	vec3LerpPR(&tf1->scale, &tf2->scale, t, &r->scale);
	#endif
}

vec3 tfTransformPoint(const transform tf, const vec3 v){
	#ifdef TRANSFORM_MATRIX_SHEAR
	// v' = Av = (TRQSQ^T)v
	return vec3VAddV(
		tf.position,
		// (RQSQ^T)v
		quatRotateVec3FastApproximate(
			tf.orientation,
			// (QSQ^T)v
			mat3MMultV(tf.scale, v)
		)
	);
	#else
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
	#endif
}
void tfTransformPointP(const transform *const __RESTRICT__ tf, vec3 *const __RESTRICT__ v){
	#ifdef TRANSFORM_MATRIX_SHEAR
	// (QSQ^T)v
	mat3MMultVP(&tf->scale, v);
	// (RQSQ^T)v
	quatRotateVec3FastApproximateP(&tf->orientation, v);
	// v' = Av = (TRQSQ^T)v
	vec3VAddVP(v, &tf->position);
	#else
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
	#endif
}
void tfTransformPointPR(const transform *const __RESTRICT__ tf, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r){
	#ifdef TRANSFORM_MATRIX_SHEAR
	// (QSQ^T)v
	mat3MMultVPR(&tf->scale, v, r);
	// (RQSQ^T)v
	quatRotateVec3FastApproximateP(&tf->orientation, r);
	// v' = Av = (TRQSQ^T)v
	vec3VAddVP(r, &tf->position);
	#else
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
	#endif
}

vec3 tfTransformDirection(const transform tf, const vec3 v){
	#ifdef TRANSFORM_MATRIX_SHEAR
	// v' = (RQSQ^T)v
	return quatRotateVec3FastApproximate(
		tf.orientation,
		// (QSQ^T)v
		mat3MMultV(tf.scale, v)
	);
	#else
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
	#endif
}
void tfTransformDirectionP(const transform *const __RESTRICT__ tf, vec3 *const __RESTRICT__ v){
	#ifdef TRANSFORM_MATRIX_SHEAR
	// (QSQ^T)v
	mat3MMultVP(&tf->scale, v);
	// v' = (RQSQ^T)v
	quatRotateVec3FastApproximateP(&tf->orientation, v);
	#else
	quat RQ;
	// (Q^T)v
	quatConjugateRotateVec3FastApproximateP(&tf->shear, v);
	// (SQ^T)v
	vec3VMultVP(v, &tf->scale);
	// v' = (RQSQ^T)v
	quatQMultQPR(&tf->orientation, &tf->shear, &RQ);
	quatRotateVec3FastApproximateP(&RQ, v);
	#endif
}
void tfTransformDirectionPR(const transform *const __RESTRICT__ tf, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r){
	#ifdef TRANSFORM_MATRIX_SHEAR
	// (QSQ^T)v
	mat3MMultVPR(&tf->scale, v, r);
	// v' = (RQSQ^T)v
	quatRotateVec3FastApproximateP(&tf->orientation, r);
	#else
	quat RQ;
	// (Q^T)v
	quatConjugateRotateVec3FastApproximatePR(&tf->shear, v, r);
	// (SQ^T)v
	vec3VMultVP(r, &tf->scale);
	// v' = (RQSQ^T)v
	quatQMultQPR(&tf->orientation, &tf->shear, &RQ);
	quatRotateVec3FastApproximateP(&RQ, r);
	#endif
}

transform tfInvert(const transform tf){

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

	#ifdef TRANSFORM_MATRIX_SHEAR
	const mat3 orientation = mat3Quaternion(tf.orientation);
	// QS^{-1}Q^T
	const mat3 QSQTInverse = mat3Invert(tf.scale);
	// Q' = RQS^{-1}Q^T R^T
	r.scale = mat3MMultMT(
		// RQS^{-1}Q^T
		mat3MMultM(orientation, QSQTInverse),
		orientation
	);
	#else
	// S' = S^{-1}
	r.scale = vec3SDivV(1.f, tf.scale);
	// Q' = RQ
	r.shear = quatQMultQ(tf.orientation, tf.shear);
	#endif

	// R' = R^T
	r.orientation = quatConjugateFast(tf.orientation);

	#ifdef TRANSFORM_MATRIX_SHEAR
	// T' = -QS^{-1}Q^T R^T T
	r.position = vec3Negate(
		// QS^{-1}Q^T R^T T
		mat3MMultV(
			QSQTInverse,
			// R^T T
			quatRotateVec3FastApproximate(r.orientation, tf.position)
		)
	);
	#else
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
	#endif

	return r;

}
void tfInvertP(transform *const __RESTRICT__ tf){

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

	#ifdef TRANSFORM_MATRIX_SHEAR
	mat3 orientation, QSQTInverse;
	mat3QuaternionPR(&tf->orientation, &orientation);
	// QS^{-1}Q^T
	mat3InvertPR(&tf->scale, &QSQTInverse);
	// RQS^{-1}Q^T
	mat3MMultMPR(&orientation, &QSQTInverse, &tf->scale);
	// Q' = RQS^{-1}Q^T R^T
	mat3MMultMTP1(&tf->scale, &orientation);
	#else
	const quat shear = tf->shear;
	// S' = S^{-1}
	vec3SDivVP(1.f, &tf->scale);
	// Q' = RQ
	quatQMultQP2(&tf->orientation, &tf->shear);
	#endif

	// R' = R^T
	quatConjugateFastP(&tf->orientation);

	#ifdef TRANSFORM_MATRIX_SHEAR
	// R^T T
	quatRotateVec3FastApproximateP(&tf->orientation, &tf->position);
	// QS^{-1}Q^T R^T T
	mat3MMultVP(&QSQTInverse, &tf->position);
	// T' = -QS^{-1}Q^T R^T T
	vec3NegateP(&tf->position);
	#else
	// T' = -Q S^{-1} Q^T R^T T
	// Note that we use Q^T R^T = (RQ)^T = (Q')^T.
	quatConjugateRotateVec3FastApproximateP(&tf->shear, &tf->position);
	vec3VMultVP(&tf->position, &tf->scale);
	quatRotateVec3FastApproximateP(&shear, &tf->position);
	vec3NegateP(&tf->position);
	#endif

}
void tfInvertPR(const transform *const __RESTRICT__ tf, transform *const __RESTRICT__ r){

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

	#ifdef TRANSFORM_MATRIX_SHEAR
	mat3 orientation, QSQTInverse;
	mat3QuaternionPR(&tf->orientation, &orientation);
	// QS^{-1}Q^T
	mat3InvertPR(&tf->scale, &QSQTInverse);
	// RQS^{-1}Q^T
	mat3MMultMPR(&orientation, &QSQTInverse, &r->scale);
	// Q' = RQS^{-1}Q^T R^T
	mat3MMultMTP1(&r->scale, &orientation);
	#else
	const quat shear = tf->shear;
	// S' = S^{-1}
	vec3SDivVPR(1.f, &tf->scale, &r->scale);
	// Q' = RQ
	quatQMultQPR(&tf->orientation, &tf->shear, &r->shear);
	#endif

	// R' = R^T
	quatConjugateFastPR(&tf->orientation, &r->orientation);

	#ifdef TRANSFORM_MATRIX_SHEAR
	// R^T T
	quatRotateVec3FastApproximatePR(&r->orientation, &tf->position, &r->position);
	// QS^{-1}Q^T R^T T
	mat3MMultVP(&QSQTInverse, &r->position);
	// T' = -QS^{-1}Q^T R^T T
	vec3NegateP(&r->position);
	#else
	// T' = -Q S^{-1} Q^T R^T T
	// Note that we use Q^T R^T = (RQ)^T = (Q')^T.
	quatConjugateRotateVec3FastApproximatePR(&tf->shear, &tf->position, &r->position);
	vec3VMultVP(&r->position, &tf->scale);
	quatRotateVec3FastApproximateP(&shear, &r->position);
	vec3NegateP(&r->position);
	#endif

}

transform tfMultiply(const transform tf1, const transform tf2){

	// We wish to append tf1 to tf2. If we let A_1 and A_2 be the
	// matrices for tf1 and tf2 respectively, this is equivalent
	// to evaluating the matrix product A = A_1 A_2.
	///
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

	#ifdef TRANSFORM_MATRIX_SHEAR

	const mat3 orientation2 = mat3Quaternion(tf2.orientation);

	// Position
	// T = T_1 R_1 Q_1 S_1 Q_1^T T_2
	r.position = vec3VAddV(
		tf1.position,
		// R_1 Q_1 S_1 Q_1^T T_2
		quatRotateVec3FastApproximate(
			tf1.orientation,
			// Q_1 S_1 Q_1^T T_2
			mat3MMultV(tf1.scale, tf2.position)
		)
	);

	// Scale
	// QSQ^T = R_2^T Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T
	r.scale = mat3MTMultM(
		orientation2,
		// Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T
		mat3MMultM(
			// Q_1 S_1 Q_1^T R_2
			mat3MMultM(tf1.scale, orientation2),
			tf2.scale
		)
	);

	// Orientation
	// R = R_1 R_2
	r.orientation = quatNormalizeFast(quatQMultQ(tf1.orientation, tf2.orientation));

	#else

	// Q_1' S_1 (Q_1')^T
	const mat3 shear1 = mat3ShearMatrix(
		// Q_1' = R_2^T Q_1
		quatQConjugateMultQ(tf2.orientation, tf1.shear),
		tf1.scale
	);
	// Q_2 S_2 Q_2^T
	const mat3 shear2 = mat3ShearMatrix(tf2.shear, tf2.scale);

	// Position
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

	// Scale
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

	// Orientation
	// R = R_1 R_2
	r.orientation = quatNormalizeFast(quatQMultQ(tf1.orientation, tf2.orientation));

	#endif

	return r;

}
void tfMultiplyP1(transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2){

	// We wish to append tf1 to tf2. If we let A_1 and A_2 be the
	// matrices for tf1 and tf2 respectively, this is equivalent
	// to evaluating the matrix product A = A_1 A_2.
	///
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

	#ifdef TRANSFORM_MATRIX_SHEAR

	vec3 position2;
	mat3 orientation2;

	// Position
	// Q_1 S_1 Q_1^T T_2
	mat3MMultVPR(&tf1->scale, &tf2->position, &position2);
	// R_1 Q_1 S_1 Q_1^T T_2
	quatRotateVec3FastApproximateP(&tf1->orientation, &position2);
	// T = T_1 R_1 Q_1 S_1 Q_1^T T_2
	vec3VAddVP(&tf1->position, &position2);

	// Scale
	mat3QuaternionPR(&tf2->orientation, &orientation2);
	// Q_1 S_1 Q_1^T R_2
	mat3MMultMP1(&tf1->scale, &orientation2);
	// Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T
	mat3MMultMP1(&tf1->scale, &tf2->scale);
	// QSQ^T = R_2^T Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T
	mat3MTMultMP2(&orientation2, &tf1->scale);

	// Orientation
	// R = R_1 R_2
	quatQMultQP1(&tf1->orientation, &tf2->orientation);
	quatNormalizeFastP(&tf1->orientation);

	#else

	quat temp;
	mat3 shear1, shear2;
	vec3 position2;

	// Q_1' = R_2^T Q_1
	quatQConjugateMultQPR(&tf2->orientation, &tf1->shear, &temp);
	// Q_1' S_1 (Q_1')^T
	mat3ShearMatrixPR(&temp, &tf1->scale, &shear1);
	// Q_2 S_2 Q_2^T
	mat3ShearMatrixPR(&tf2->shear, &tf2->scale, &shear2);

	// Position
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

	// Scale
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

	// Orientation
	// R = R_1 R_2
	quatQMultQP1(&tf1->orientation, &tf2->orientation);
	quatNormalizeFastP(&tf1->orientation);

	#endif

}
void tfMultiplyP2(const transform *const __RESTRICT__ tf1, transform *const __RESTRICT__ tf2){

	// We wish to append tf1 to tf2. If we let A_1 and A_2 be the
	// matrices for tf1 and tf2 respectively, this is equivalent
	// to evaluating the matrix product A = A_1 A_2.
	///
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

	#ifdef TRANSFORM_MATRIX_SHEAR

	mat3 orientation2;

	// Position
	// Q_1 S_1 Q_1^T T_2
	mat3MMultVP(&tf1->scale, &tf2->position);
	// R_1 Q_1 S_1 Q_1^T T_2
	quatRotateVec3FastApproximateP(&tf1->orientation, &tf2->position);
	// T = T_1 R_1 Q_1 S_1 Q_1^T T_2
	vec3VAddVP(&tf2->position, &tf1->position);

	// Scale
	mat3QuaternionPR(&tf2->orientation, &orientation2);
	// R_2 Q_2 S_2 Q_2^T
	mat3MMultMP1(&orientation2, &tf2->scale);
	// Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T
	mat3MMultMP2(&tf1->scale, &tf2->scale);
	// QSQ^T = R_2^T Q_2 S_2 Q_2^T R_2 Q_2 S_2 Q_2^T
	mat3MTMultMP2(&orientation2, &tf2->scale);

	// Orientation
	// R = R_1 R_2
	quatQMultQP2(&tf1->orientation, &tf2->orientation);
	quatNormalizeFastP(&tf2->orientation);

	#else

	quat temp;
	mat3 shear1, shear2;

	// Q_1' = R_2^T Q_1
	quatQConjugateMultQPR(&tf2->orientation, &tf1->shear, &temp);
	// Q_1' S_1 (Q_1')^T
	mat3ShearMatrixPR(&temp, &tf1->scale, &shear1);
	// Q_2 S_2 Q_2^T
	mat3ShearMatrixPR(&tf2->shear, &tf2->scale, &shear2);

	// Position
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

	// Scale
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

	// Orientation
	// R = R_1 R_2
	quatQMultQP2(&tf1->orientation, &tf2->orientation);
	quatNormalizeFastP(&tf2->orientation);

	#endif

}
void tfMultiplyPR(const transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, transform *const __RESTRICT__ r){

	// We wish to append tf1 to tf2. If we let A_1 and A_2 be the
	// matrices for tf1 and tf2 respectively, this is equivalent
	// to evaluating the matrix product A = A_1 A_2.
	///
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

	#ifdef TRANSFORM_MATRIX_SHEAR

	mat3 orientation2;

	// Position
	// Q_1 S_1 Q_1^T T_2
	mat3MMultVPR(&tf1->scale, &tf2->position, &r->position);
	// R_1 Q_1 S_1 Q_1^T T_2
	quatRotateVec3FastApproximateP(&tf1->orientation, &r->position);
	// T = T_1 R_1 Q_1 S_1 Q_1^T T_2
	vec3VAddVP(&r->position, &tf1->position);

	// Scale
	mat3QuaternionPR(&tf2->orientation, &orientation2);
	// Q_1 S_1 Q_1^T R_2
	mat3MMultMPR(&tf1->scale, &orientation2, &r->scale);
	// Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T
	mat3MMultMP1(&r->scale, &tf2->scale);
	// QSQ^T = R_2^T Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T
	mat3MTMultMP2(&orientation2, &r->scale);

	// Orientation
	// R = R_1 R_2
	quatQMultQPR(&tf1->orientation, &tf2->orientation, &r->orientation);
	quatNormalizeFastP(&r->orientation);

	#else

	quat temp;
	mat3 shear1, shear2;

	// Q_1' = R_2^T Q_1
	quatQConjugateMultQPR(&tf2->orientation, &tf1->shear, &temp);
	// Q_1' S_1 (Q_1')^T
	mat3ShearMatrixPR(&temp, &tf1->scale, &shear1);
	// Q_2 S_2 Q_2^T
	mat3ShearMatrixPR(&tf2->shear, &tf2->scale, &shear2);

	// Position
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

	// Scale
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

	// Orientation
	// R = R_1 R_2
	quatQMultQPR(&tf1->orientation, &tf2->orientation, &r->orientation);
	quatNormalizeFastP(&r->orientation);

	#endif

}
