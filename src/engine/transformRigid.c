#include "transformRigid.h"

transformRigid g_tfrIdentity = {
	.position.x    = 0.f, .position.y    = 0.f, .position.z    = 0.f,
	.orientation.x = 0.f, .orientation.y = 0.f, .orientation.z = 0.f, .orientation.w = 1.f,
	.scale.x       = 1.f, .scale.y       = 1.f, .scale.z       = 1.f
};

void tfrInit(transformRigid *const __RESTRICT__ tf){
	*tf = g_tfrIdentity;
}
mat3x4 tfrMatrix(const transformRigid tf){
	// Translate, rotate and scale.
	// It looks a bit weird because it's ultra optimized.
	// Note that mat3x4Scale is a right multiplication.
	return mat3x4Translate(
		tf.position.x, tf.position.y, tf.position.z,
		mat3x4Scale(
			tf.scale.x, tf.scale.y, tf.scale.z,
			mat3x4RotationMatrix(tf.orientation)
		)
	);
}

transformRigid tfrInterpolate(const transformRigid tf1, const transformRigid tf2, const float t){
	// Interpolate between two transformed states.
	const transformRigid r = {
		// LERP between the start position and end position.
		.position = vec3Lerp(tf1.position, tf2.position, t),
		// SLERP between the start orientation and end orientation.
		.orientation = quatSlerpFast(tf1.orientation, tf2.orientation, t),
		// LERP once more for the scale.
		.scale = vec3Lerp(tf1.scale, tf2.scale, t)
	};
	return r;
}
void tfrInterpolateP1(transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2, const float t){
	// Interpolate between two transformed states.
	// LERP between the start position and end position.
	vec3LerpP1(&tf1->position, &tf2->position, t);
	// SLERP between the start orientation and end orientation.
	quatSlerpFastP1(&tf1->orientation, &tf2->orientation, t);
	// LERP once more for the scale.
	vec3LerpP1(&tf1->scale, &tf2->scale, t);
}
void tfrInterpolateP2(const transformRigid *const __RESTRICT__ tf1, transformRigid *const __RESTRICT__ tf2, const float t){
	// Interpolate between two transformed states.
	// LERP between the start position and end position.
	vec3LerpP2(&tf1->position, &tf2->position, t);
	// SLERP between the start orientation and end orientation.
	quatSlerpFastP2(&tf1->orientation, &tf2->orientation, t);
	// LERP once more for the scale.
	vec3LerpP2(&tf1->scale, &tf2->scale, t);
}
void tfrInterpolatePR(const transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2, const float t, transformRigid *const __RESTRICT__ r){
	// Interpolate between two transformed states.
	// LERP between the start position and end position.
	vec3LerpPR(&tf1->position, &tf2->position, t, &r->position);
	// SLERP between the start orientation and end orientation.
	quatSlerpFastPR(&tf1->orientation, &tf2->orientation, t, &r->orientation);
	// LERP once more for the scale.
	vec3LerpPR(&tf1->scale, &tf2->scale, t, &r->scale);
}

vec3 tfrTransform(const transformRigid tf, const vec3 v){
	return vec3VAddV(tf.position, quatRotateVec3FastApproximate(tf.orientation, vec3VMultV(v, tf.scale)));
}
void tfrTransformP(const transformRigid *const __RESTRICT__ tf, vec3 *const __RESTRICT__ v){
	vec3VMultVP(v, &tf->scale);
	quatRotateVec3FastApproximateP(&tf->orientation, v);
	vec3VAddVP(v, &tf->position);
}
void tfrTransformPR(const transformRigid *const __RESTRICT__ tf, const vec3 *const __RESTRICT__ v, vec3 *const __RESTRICT__ r){
	vec3VMultVPR(v, &tf->scale, r);
	quatRotateVec3FastApproximateP(&tf->orientation, r);
	vec3VAddVP(r, &tf->position);
}

transformRigid tfrInverse(const transformRigid tf){
	// WARNING: This is technically incorrect, and may
	// result in singularities during interpolation.
	// See quatConjugateFast for more information.
	const transformRigid r = {
		.position = vec3Negate(quatRotateVec3FastApproximate(tf.orientation, tf.position)),
		.orientation = quatConjugateFast(tf.orientation),
		.scale = vec3SDivV(1.f, tf.scale)
	};
	return r;
}
void tfrInverseP(transformRigid *const __RESTRICT__ tf){
	// WARNING: This is technically incorrect, and may
	// result in singularities during interpolation.
	// See quatConjugateFast for more information.
	quatRotateVec3FastApproximateP(&tf->orientation, &tf->position);
	vec3NegateP(&tf->position);
	quatConjugateFastP(&tf->orientation);
	vec3SDivVP(1.f, &tf->scale);
}
void tfrInversePR(const transformRigid *const __RESTRICT__ tf, transformRigid *const __RESTRICT__ r){
	// WARNING: This is technically incorrect, and may
	// result in singularities during interpolation.
	// See quatConjugateFast for more information.
	quatRotateVec3FastApproximatePR(&tf->orientation, &tf->position, &r->position);
	vec3NegateP(&r->position);
	quatConjugateFastPR(&tf->orientation, &r->orientation);
	vec3SDivVPR(1.f, &tf->scale, &r->scale);
}

vec3 tfrAppendPosition1(const transformRigid tf1, const transformRigid tf2){
	return vec3VAddV(tf1.position, quatRotateVec3FastApproximate(tf1.orientation, vec3VMultV(tf2.position, tf1.scale)));
}
vec3 tfrAppendPosition2(const transformRigid tf1, const transformRigid tf2){
	return vec3VAddV(tf2.position, quatRotateVec3FastApproximate(tf1.orientation, vec3VMultV(tf2.position, tf1.scale)));
}
void tfrAppendPositionP1(transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2){
	// Calculate total translation.
	vec3 tempVec3;
	vec3VMultVPR(&tf2->position, &tf1->scale, &tempVec3);          // Scale
	quatRotateVec3FastApproximateP(&tf1->orientation, &tempVec3);  // Rotate
	vec3VAddVP(&tf1->position, &tempVec3);                         // Translate
}
void tfrAppendPositionP2(const transformRigid *const __RESTRICT__ tf1, transformRigid *const __RESTRICT__ tf2){
	// Calculate total translation.
	vec3VMultVP(&tf2->position, &tf1->scale);                           // Scale
	quatRotateVec3FastApproximateP(&tf1->orientation, &tf2->position);  // Rotate
	vec3VAddVP(&tf2->position, &tf1->position);                         // Translate
}
void tfrAppendPositionPR(const transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2, transformRigid *const __RESTRICT__ r){
	// Calculate total translation.
	vec3VMultVPR(&tf2->position, &tf1->scale, &r->position);          // Scale
	quatRotateVec3FastApproximateP(&tf1->orientation, &r->position);  // Rotate
	vec3VAddVP(&r->position, &tf1->position);                         // Translate
}

vec3 tfrAppendPositionVec(const transformRigid tf, const float x, const float y, const float z){
	return vec3VAddV(tf.position, quatRotateVec3FastApproximate(tf.orientation, vec3VMultN(tf.scale, x, y, z)));
}
void tfrAppendPositionVecP(transformRigid *const __RESTRICT__ tf, const float x, const float y, const float z){
	// Calculate total translation.
	vec3 tempVec3;
	tempVec3.x = x * tf->scale.x;                                 // Scale
	tempVec3.y = y * tf->scale.y;
	tempVec3.z = z * tf->scale.z;
	quatRotateVec3FastApproximateP(&tf->orientation, &tempVec3);  // Rotate
	vec3VAddVP(&tf->position, &tempVec3);                         // Translate
}
void tfrAppendPositionVecPR(const transformRigid *const __RESTRICT__ tf, const float x, const float y, const float z, vec3 *const __RESTRICT__ r){
	// Calculate total translation.
	r->x = x * tf->scale.x;                               // Scale
	r->y = y * tf->scale.y;
	r->z = z * tf->scale.z;
	quatRotateVec3FastApproximateP(&tf->orientation, r);  // Rotate
	vec3VAddVP(r, &tf->position);                         // Translate
}

quat tfrAppendOrientation(const transformRigid tf1, const transformRigid tf2){
	return quatNormalizeFast(quatQMultQ(tf1.orientation, tf2.orientation));
}
void tfrAppendOrientationP1(transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2){
	// Calculate total orientation.
	quatQMultQP1(&tf1->orientation, &tf2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&tf1->orientation);
}
void tfrAppendOrientationP2(const transformRigid *const __RESTRICT__ tf1, transformRigid *const __RESTRICT__ tf2){
	// Calculate total orientation.
	quatQMultQP2(&tf1->orientation, &tf2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&tf2->orientation);
}
void tfrAppendOrientationPR(const transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2, transformRigid *const __RESTRICT__ r){
	// Calculate total orientation.
	quatQMultQPR(&tf1->orientation, &tf2->orientation, &r->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&r->orientation);
}

vec3 tfrAppendScale(const transformRigid tf1, const transformRigid tf2){
	return vec3VMultV(tf1.scale, tf2.scale);
}
void tfrAppendScaleP(transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2){
	// Calculate total scale.
	vec3VMultVP(&tf1->scale, &tf2->scale);
}
void tfrAppendScalePR(const transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2, transformRigid *const __RESTRICT__ r){
	// Calculate total scale.
	vec3VMultVPR(&tf2->scale, &tf1->scale, &r->scale);
}

transformRigid tfrAppend(const transformRigid tf1, const transformRigid tf2){
	// Adds the transformations for tf1 to tf2 and stores the result in r.
	// Used for getting the total sum of all transformations of a basis.
	const transformRigid r = {
		.position = vec3VAddV(tf1.position, quatRotateVec3FastApproximate(tf1.orientation, vec3VMultV(tf2.position, tf1.scale))),
		.orientation = quatNormalizeFast(quatQMultQ(tf1.orientation, tf2.orientation)),
		.scale = vec3VMultV(tf1.scale, tf2.scale)
	};
	return r;
}
void tfrAppendP1(transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2){
	// Adds the transformations for tf1 to tf2 and stores the result in tf1.
	// Used for getting the total sum of all transformations of a basis.
	// Calculate total translation.
	vec3 tempVec3;
	vec3VMultVPR(&tf2->position, &tf1->scale, &tempVec3);          // Scale
	quatRotateVec3FastApproximateP(&tf1->orientation, &tempVec3);  // Rotate
	vec3VAddVP(&tf1->position, &tempVec3);                         // Translate
	// Calculate total orientation.
	quatQMultQP1(&tf1->orientation, &tf2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&tf1->orientation);
	// Calculate total scale.
	vec3VMultVP(&tf1->scale, &tf2->scale);
}
void tfrAppendP2(const transformRigid *const __RESTRICT__ tf1, transformRigid *const __RESTRICT__ tf2){
	// Adds the transformations for tf1 to tf2 and stores the result in tf2.
	// Used for getting the total sum of all transformations of a basis.
	// Calculate total translation.
	vec3VMultVP(&tf2->position, &tf1->scale);                           // Scale
	quatRotateVec3FastApproximateP(&tf1->orientation, &tf2->position);  // Rotate
	vec3VAddVP(&tf2->position, &tf1->position);                         // Translate
	// Calculate total orientation.
	quatQMultQP2(&tf1->orientation, &tf2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&tf2->orientation);
	// Calculate total scale.
	vec3VMultVP(&tf2->scale, &tf1->scale);
}
void tfrAppendPR(const transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2, transformRigid *const __RESTRICT__ r){
	// Adds the transformations for tf1 to tf2 and stores the result in r.
	// Used for getting the total sum of all transformations of a basis.
	// Calculate total translation.
	vec3VMultVPR(&tf2->position, &tf1->scale, &r->position);          // Scale
	quatRotateVec3FastApproximateP(&tf1->orientation, &r->position);  // Rotate
	vec3VAddVP(&r->position, &tf1->position);                         // Translate
	// Calculate total orientation.
	quatQMultQPR(&tf1->orientation, &tf2->orientation, &r->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&r->orientation);
	// Calculate total scale.
	vec3VMultVPR(&tf1->scale, &tf2->scale, &r->scale);
}

transformRigid tfrPrepend(const transformRigid tf1, const transformRigid tf2){
	// Just performs the reverse of tfrAppend,
	// such that the following equality holds:
	//     tf = tfPrepend(tfInverse(T), tfAppend(T, tf)),
	// for some matrix T.
	const transformRigid r = {
		.position = vec3VMultV(quatRotateVec3FastApproximate(tf1.orientation, vec3VAddV(tf1.position, tf2.position)), tf1.scale),
		.orientation = quatNormalizeFast(quatQMultQ(tf1.orientation, tf2.orientation)),
		.scale = vec3VMultV(tf2.scale, tf1.scale)
	};
	return r;
}

transformRigid tfrCombine(const transformRigid tf1, const transformRigid tf2){
	// Combines the transformations for tf2 with tf1 and stores the result in r.
	// Used for getting the total sum of all transformations of a basis.
	const transformRigid r = {
		.position = vec3VAddV(tf1.position, tf2.position),
		.orientation = quatNormalizeFast(quatQMultQ(tf1.orientation, tf2.orientation)),
		.scale = vec3VMultV(tf1.scale, tf2.scale)
	};
	return r;
}
void tfrCombineP1(transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2){
	// Combines the transformations for tf2 with tf1 and stores the result in tf1.
	// Used for getting the total sum of all transformations of a basis.
	// Calculate total translation.
	vec3VAddVP(&tf1->position, &tf2->position);
	// Calculate total orientation.
	quatQMultQP1(&tf1->orientation, &tf2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&tf1->orientation);
	// Calculate total scale.
	vec3VMultVP(&tf1->scale, &tf2->scale);
}
void tfrCombineP2(const transformRigid *const __RESTRICT__ tf1, transformRigid *const __RESTRICT__ tf2){
	// Combines the transformations for tf2 with tf1 and stores the result in tf2.
	// Used for getting the total sum of all transformations of a basis.
	// Calculate total translation.
	vec3VAddVP(&tf2->position, &tf1->position);
	// Calculate total orientation.
	quatQMultQP2(&tf1->orientation, &tf2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&tf2->orientation);
	// Calculate total scale.
	vec3VMultVP(&tf2->scale, &tf1->scale);
}
void tfrCombinePR(const transformRigid *const __RESTRICT__ tf1, const transformRigid *const __RESTRICT__ tf2, transformRigid *const __RESTRICT__ r){
	// Combines the transformations for tf2 with tf1 and stores the result in r.
	// Used for getting the total sum of all transformations of a basis.
	// Calculate total translation.
	vec3VAddVPR(&tf1->position, &tf2->position, &r->position);
	// Calculate total orientation.
	quatQMultQPR(&tf1->orientation, &tf2->orientation, &r->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&r->orientation);
	// Calculate total scale.
	vec3VMultVPR(&tf1->scale, &tf2->scale, &r->scale);
}
