#include "transform.h"

void tfInit(transform *const __RESTRICT__ tf){
	vec3ZeroP(&tf->position);
	tf->orientation = quatIdentity();
	tf->scale = vec3NewS(1.f);
}
transform tfIdentity(){
	transform r;
	vec3ZeroP(&r.position);
	r.orientation = quatIdentity();
	r.scale = vec3NewS(1.f);
	return r;
}

mat4 tfMatrix(const transform tf){
	// Translate, rotate and scale.
	// It looks a bit weird because it's ultra optimized.
	// Note that mat4Scale is a right multiplication.
	mat4 transform = mat4Scale(
		mat4RotationMatrix(tf.orientation),
		tf.scale.x, tf.scale.y, tf.scale.z
	);
	transform.m[3][0] = tf.position.x;
	transform.m[3][1] = tf.position.y;
	transform.m[3][2] = tf.position.z;
	return transform;
}

/**transform tfInverse(const transform tf){
	// WARNING: This is technically incorrect, and may
	// result in singularities during interpolation.
	// See quatConjugateFast for more information.
	transform r;
	r.orientation = quatConjugate(tf.orientation);
	r.position = vec3Negate(quatRotateVec3FastApproximate(r.orientation, tf.position));
	r.scale = vec3SDivV(1.f, tf.scale);
	return r;
}
void tfInverseP(transform *const __RESTRICT__ tf){
	// WARNING: This is technically incorrect, and may
	// result in singularities during interpolation.
	// See quatConjugateFast for more information.
	quatConjugateP(&tf->orientation);
	quatRotateVec3FastApproximateP(&tf->orientation, &tf->position);
	vec3NegateP(&tf->position);
	vec3SDivVP(1.f, &tf->scale);
}
void tfInversePR(const transform *const __RESTRICT__ tf, transform *const __RESTRICT__ r){
	// WARNING: This is technically incorrect, and may
	// result in singularities during interpolation.
	// See quatConjugateFast for more information.
	quatConjugatePR(&tf->orientation, &r->orientation);
	quatRotateVec3FastApproximatePR(&r->orientation, &tf->position, &r->position);
	vec3NegateP(&r->position);
	r->scale.x = 1.f/tf->scale.x;
	r->scale.y = 1.f/tf->scale.y;
	r->scale.z = 1.f/tf->scale.z;
}**/

transform tfInterpolate(const transform tf1, const transform tf2, const float t){

	// Calculate the interpolated delta transform for the transform.
	if(t == 0.f){

		// Only use the start frame if t exceeds the lower bounds.
		return tf1;

	}else if(t == 1.f){

		// Only use the end frame if t exceeds the upper bounds.
		return tf2;

	}else{

		const transform r = {

			// LERP between the start position and end position.
			.position = vec3Lerp(tf1.position, tf2.position, t),

			// SLERP between the start orientation and end orientation.
			.orientation = quatSlerpFast(tf1.orientation, tf2.orientation, t),

			// LERP once more for the scale.
			.scale = vec3Lerp(tf1.scale, tf2.scale, t)

		};

		return r;

	}

}
void tfInterpolateP1(transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, const float t){

	// Calculate the interpolated delta transform for the transform.
	if(t == 1.f){

		// Only use the end frame if t exceeds the upper bounds.
		*tf1 = *tf2;

	}else if(t != 0.f){

		// LERP between the start position and end position.
		vec3LerpP1(&tf1->position, &tf2->position, t);

		// SLERP between the start orientation and end orientation.
		quatSlerpFastP1(&tf1->orientation, &tf2->orientation, t);

		// LERP once more for the scale.
		vec3LerpP1(&tf1->scale, &tf2->scale, t);

	}

}
void tfInterpolateP2(const transform *const __RESTRICT__ tf1, transform *const __RESTRICT__ tf2, const float t){

	// Calculate the interpolated delta transform for the transform.
	if(t == 0.f){

		// Only use the start frame if t exceeds the lower bounds.
		*tf2 = *tf1;

	}else if(t != 1.f){

		// LERP between the start position and end position.
		vec3LerpP2(&tf1->position, &tf2->position, t);

		// SLERP between the start orientation and end orientation.
		quatSlerpFastP2(&tf1->orientation, &tf2->orientation, t);

		// LERP once more for the scale.
		vec3LerpP2(&tf1->scale, &tf2->scale, t);

	}

}
void tfInterpolatePR(const transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, const float t, transform *const __RESTRICT__ r){

	// Calculate the interpolated delta transform for the transform.
	if(t == 0.f){

		// Only use the start frame if t exceeds the lower bounds.
		*r = *tf1;

	}else if(t == 1.f){

		// Only use the end frame if t exceeds the upper bounds.
		*r = *tf2;

	}else{

		// LERP between the start position and end position.
		vec3LerpPR(&tf1->position, &tf2->position, t, &r->position);

		// SLERP between the start orientation and end orientation.
		quatSlerpFastPR(&tf1->orientation, &tf2->orientation, t, &r->orientation);

		// LERP once more for the scale.
		vec3LerpPR(&tf1->scale, &tf2->scale, t, &r->scale);

	}

}

vec3 tfTransform(const transform tf, const vec3 v){
	return vec3VAddV(tf.position, quatRotateVec3FastApproximate(tf.orientation, vec3VMultV(v, tf.scale)));
}

transform tfInverse(const transform tf){
	// WARNING: This is technically incorrect, and may
	// result in singularities during interpolation.
	// See quatConjugateFast for more information.
	const transform r = {
		.position = vec3Negate(tf.position),
		.orientation = quatConjugateFast(tf.orientation),
		.scale = vec3SDivV(1.f, tf.scale)
	};
	return r;
}
void tfInverseP(transform *const __RESTRICT__ tf){
	// WARNING: This is technically incorrect, and may
	// result in singularities during interpolation.
	// See quatConjugateFast for more information.
	vec3NegateP(&tf->position);
	quatConjugateFastP(&tf->orientation);
	vec3SDivVP(1.f, &tf->scale);
}
void tfInversePR(const transform *const __RESTRICT__ tf, transform *const __RESTRICT__ r){
	// WARNING: This is technically incorrect, and may
	// result in singularities during interpolation.
	// See quatConjugateFast for more information.
	vec3NegatePR(&tf->position, &r->position);
	quatConjugateFastPR(&tf->orientation, &r->orientation);
	vec3SDivVPR(1.f, &tf->scale, &r->scale);
}

vec3 tfAppendPosition1(const transform tf1, const transform tf2){
	return vec3VAddV(tf1.position, quatRotateVec3FastApproximate(tf1.orientation, vec3VMultV(tf2.position, tf1.scale)));
}
vec3 tfAppendPosition2(const transform tf1, const transform tf2){
	return vec3VAddV(tf2.position, quatRotateVec3FastApproximate(tf1.orientation, vec3VMultV(tf2.position, tf1.scale)));
}
void tfAppendPositionP1(transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2){
	// Calculate total translation.
	vec3 tempVec3;
	vec3VMultVPR(&tf2->position, &tf1->scale, &tempVec3);          // Scale
	quatRotateVec3FastApproximateP(&tf1->orientation, &tempVec3);  // Rotate
	vec3VAddVP(&tf1->position, &tempVec3);                         // Translate
}
void tfAppendPositionP2(const transform *const __RESTRICT__ tf1, transform *const __RESTRICT__ tf2){
	// Calculate total translation.
	vec3VMultVP(&tf2->position, &tf1->scale);                           // Scale
	quatRotateVec3FastApproximateP(&tf1->orientation, &tf2->position);  // Rotate
	vec3VAddVP(&tf2->position, &tf1->position);                         // Translate
}
void tfAppendPositionPR(const transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, transform *const __RESTRICT__ r){
	// Calculate total translation.
	vec3VMultVPR(&tf2->position, &tf1->scale, &r->position);          // Scale
	quatRotateVec3FastApproximateP(&tf1->orientation, &r->position);  // Rotate
	vec3VAddVP(&r->position, &tf1->position);                         // Translate
}

vec3 tfAppendPositionVec(const transform tf, const float x, const float y, const float z){
	return vec3VAddV(tf.position, quatRotateVec3FastApproximate(tf.orientation, vec3VMultN(tf.scale, x, y, z)));
}
void tfAppendPositionVecP(transform *const __RESTRICT__ tf, const float x, const float y, const float z){
	// Calculate total translation.
	vec3 tempVec3;
	tempVec3.x = x * tf->scale.x;                                 // Scale
	tempVec3.y = y * tf->scale.y;
	tempVec3.z = z * tf->scale.z;
	quatRotateVec3FastApproximateP(&tf->orientation, &tempVec3);  // Rotate
	vec3VAddVP(&tf->position, &tempVec3);                         // Translate
}
void tfAppendPositionVecPR(const transform *const __RESTRICT__ tf, const float x, const float y, const float z, vec3 *const __RESTRICT__ r){
	// Calculate total translation.
	r->x = x * tf->scale.x;                               // Scale
	r->y = y * tf->scale.y;
	r->z = z * tf->scale.z;
	quatRotateVec3FastApproximateP(&tf->orientation, r);  // Rotate
	vec3VAddVP(r, &tf->position);                         // Translate
}

quat tfAppendOrientation(const transform tf1, const transform tf2){
	return quatNormalizeFast(quatQMultQ(tf1.orientation, tf2.orientation));
}
void tfAppendOrientationP1(transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2){
	// Calculate total orientation.
	quatQMultQP1(&tf1->orientation, &tf2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&tf1->orientation);
}
void tfAppendOrientationP2(const transform *const __RESTRICT__ tf1, transform *const __RESTRICT__ tf2){
	// Calculate total orientation.
	quatQMultQP2(&tf1->orientation, &tf2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&tf2->orientation);
}
void tfAppendOrientationPR(const transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, transform *const __RESTRICT__ r){
	// Calculate total orientation.
	quatQMultQPR(&tf1->orientation, &tf2->orientation, &r->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&r->orientation);
}

vec3 tfAppendScale(const transform tf1, const transform tf2){
	return vec3VMultV(tf1.scale, tf2.scale);
}
void tfAppendScaleP(transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2){
	// Calculate total scale.
	vec3VMultVP(&tf1->scale, &tf2->scale);
}
void tfAppendScalePR(const transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, transform *const __RESTRICT__ r){
	// Calculate total scale.
	vec3VMultVPR(&tf2->scale, &tf1->scale, &r->scale);
}

transform tfAppend(const transform tf1, const transform tf2){
	// Adds the transformations for tf2 to tf1 and stores the result in r.
	// Used for getting the total sum of all transformations of a basis.
	const transform r = {
		.position = vec3VAddV(tf1.position, quatRotateVec3FastApproximate(tf1.orientation, vec3VMultV(tf2.position, tf1.scale))),
		.orientation = quatNormalizeFast(quatQMultQ(tf1.orientation, tf2.orientation)),
		.scale = vec3VMultV(tf1.scale, tf2.scale)
	};
	return r;
}
void tfAppendP1(transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2){
	// Adds the transformations for tf2 to tf1 and stores the result in tf1.
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
void tfAppendP2(const transform *const __RESTRICT__ tf1, transform *const __RESTRICT__ tf2){
	// Adds the transformations for tf2 to tf1 and stores the result in tf2.
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
void tfAppendPR(const transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, transform *const __RESTRICT__ r){
	// Adds the transformations for tf2 to tf1 and stores the result in r.
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

transform tfPrepend(const transform tf1, const transform tf2){
	// Just performs the reverse of tfAppend,
	// such that the following equality holds:
	//     tf = tfPrepend(tfInverse(T), tfAppend(T, tf)),
	// for some matrix T.
	const transform r = {
		.position = vec3VMultV(quatRotateVec3FastApproximate(tf1.orientation, vec3VAddV(tf1.position, tf2.position)), tf1.scale),
		.orientation = quatNormalizeFast(quatQMultQ(tf1.orientation, tf2.orientation)),
		.scale = vec3VMultV(tf2.scale, tf1.scale)
	};
	return r;
}

transform tfCombine(const transform tf1, const transform tf2){
	// Combines the transformations for tf2 with tf1 and stores the result in r.
	// Used for getting the total sum of all transformations of a basis.
	const transform r = {
		.position = vec3VAddV(tf1.position, tf2.position),
		.orientation = quatNormalizeFast(quatQMultQ(tf1.orientation, tf2.orientation)),
		.scale = vec3VMultV(tf1.scale, tf2.scale)
	};
	return r;
}
void tfCombineP1(transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2){
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
void tfCombineP2(const transform *const __RESTRICT__ tf1, transform *const __RESTRICT__ tf2){
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
void tfCombinePR(const transform *const __RESTRICT__ tf1, const transform *const __RESTRICT__ tf2, transform *const __RESTRICT__ r){
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
