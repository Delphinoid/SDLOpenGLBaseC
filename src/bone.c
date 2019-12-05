#include "bone.h"

void boneInit(bone *const restrict b){
	vec3ZeroP(&b->position);
	b->orientation = quatIdentity();
	b->scale = vec3NewS(1.f);
}
bone boneIdentity(){
	const bone r = {
		.position = vec3Zero(),
		.orientation = quatIdentity(),
		.scale = vec3NewS(1.f)
	};
	return r;
}

mat4 boneMatrix(const bone b){
	// Don't ask.
	mat4 transform = mat4Scale(
		mat4RotationMatrix(b.orientation),
		b.scale.x, b.scale.y, b.scale.z
	);
	transform.m[3][0] = b.position.x;
	transform.m[3][1] = b.position.y;
	transform.m[3][2] = b.position.z;
	return transform;
}

bone boneNegate(const bone b){
	const bone r = {
		.position = vec3Negate(b.position),
		.orientation = quatNegate(b.orientation),
		.scale = vec3SDivV(1.f, b.scale)
	};
	return r;
}
void boneNegateP(bone *const restrict b){
	b->position.x = -b->position.x;
	b->position.y = -b->position.y;
	b->position.z = -b->position.z;
	b->orientation.w = -b->orientation.w;
	b->orientation.v = b->orientation.v;
	b->scale.x = 1.f/b->scale.x;
	b->scale.y = 1.f/b->scale.y;
	b->scale.z = 1.f/b->scale.z;
}
void boneNegatePR(const bone *const restrict b, bone *const restrict r){
	r->position.x = -b->position.x;
	r->position.y = -b->position.y;
	r->position.z = -b->position.z;
	r->orientation.w = -b->orientation.w;
	r->orientation.v = b->orientation.v;
	r->scale.x = 1.f/b->scale.x;
	r->scale.y = 1.f/b->scale.y;
	r->scale.z = 1.f/b->scale.z;
}

bone boneInvert(const bone b){
	bone r;
	r.orientation = quatConjugateFast(b.orientation);
	r.position = vec3Negate(quatRotateVec3FastApproximate(r.orientation, b.position));
	r.scale = vec3SDivV(1.f, b.scale);
	return r;
}
void boneInvertP(bone *const restrict b){
	quatConjugateFastP(&b->orientation);
	quatRotateVec3FastApproximateP(&b->orientation, &b->position);
	vec3NegateP(&b->position);
	vec3SDivVP(1.f, &b->scale);
}
void boneInvertPR(const bone *const restrict b, bone *const restrict r){
	quatConjugateFastPR(&b->orientation, &r->orientation);
	quatRotateVec3FastApproximatePR(&r->orientation, &b->position, &r->position);
	vec3NegateP(&r->position);
	r->scale.x = 1.f/b->scale.x;
	r->scale.y = 1.f/b->scale.y;
	r->scale.z = 1.f/b->scale.z;
}

bone boneInterpolate(const bone b1, const bone b2, const float t){

	/*
	** Calculate the interpolated delta transform for the bone.
	*/
	if(t == 0.f){

		// Only use the start frame if t exceeds the lower bounds.
		return b1;

	}else if(t == 1.f){

		// Only use the end frame if t exceeds the upper bounds.
		return b2;

	}else{

		const bone r = {

			// LERP between the start position and end position.
			.position = vec3Lerp(b1.position, b2.position, t),

			// SLERP between the start orientation and end orientation.
			.orientation = quatSlerp(b1.orientation, b2.orientation, t),

			// LERP once more for the scale.
			.scale = vec3Lerp(b1.scale, b2.scale, t)

		};

		return r;

	}

}
void boneInterpolateP1(bone *const restrict b1, const bone *const restrict b2, const float t){

	/*
	** Calculate the interpolated delta transform for the bone.
	*/
	if(t == 1.f){

		// Only use the end frame if t exceeds the upper bounds.
		*b1 = *b2;

	}else if(t != 0.f){

		// LERP between the start position and end position.
		vec3LerpP1(&b1->position, &b2->position, t);

		// SLERP between the start orientation and end orientation.
		quatSlerpP1(&b1->orientation, &b2->orientation, t);

		// LERP once more for the scale.
		vec3LerpP1(&b1->scale, &b2->scale, t);

	}

}
void boneInterpolateP2(const bone *const restrict b1, bone *const restrict b2, const float t){

	/*
	** Calculate the interpolated delta transform for the bone.
	*/
	if(t == 0.f){

		// Only use the start frame if t exceeds the lower bounds.
		*b2 = *b1;

	}else if(t != 1.f){

		// LERP between the start position and end position.
		vec3LerpP2(&b1->position, &b2->position, t);

		// SLERP between the start orientation and end orientation.
		quatSlerpP2(&b1->orientation, &b2->orientation, t);

		// LERP once more for the scale.
		vec3LerpP2(&b1->scale, &b2->scale, t);

	}

}
void boneInterpolatePR(const bone *const restrict b1, const bone *const restrict b2, const float t, bone *const restrict r){

	/*
	** Calculate the interpolated delta transform for the bone.
	*/
	if(t == 0.f){

		// Only use the start frame if t exceeds the lower bounds.
		*r = *b1;

	}else if(t == 1.f){

		// Only use the end frame if t exceeds the upper bounds.
		*r = *b2;

	}else{

		// LERP between the start position and end position.
		vec3LerpPR(&b1->position, &b2->position, t, &r->position);

		// SLERP between the start orientation and end orientation.
		quatSlerpPR(&b1->orientation, &b2->orientation, t, &r->orientation);

		// LERP once more for the scale.
		vec3LerpPR(&b1->scale, &b2->scale, t, &r->scale);

	}

}

vec3 boneTransformAppendPosition1(const bone b1, const bone b2){
	return vec3VAddV(b1.position, quatRotateVec3FastApproximate(b1.orientation, vec3VMultV(b2.position, b1.scale)));
}
vec3 boneTransformAppendPosition2(const bone b1, const bone b2){
	return vec3VAddV(b2.position, quatRotateVec3FastApproximate(b1.orientation, vec3VMultV(b2.position, b1.scale)));
}
void boneTransformAppendPositionP1(bone *const restrict b1, const bone *const restrict b2){
	// Calculate total translation.
	vec3 tempVec3;
	vec3VMultVPR(&b2->position, &b1->scale, &tempVec3);  // Scale
	quatRotateVec3FastApproximateP(&b1->orientation, &tempVec3);    // Rotate
	vec3VAddVP(&b1->position, &tempVec3);                // Translate
}
void boneTransformAppendPositionP2(const bone *const restrict b1, bone *const restrict b2){
	// Calculate total translation.
	vec3VMultVP(&b2->position, &b1->scale);                // Scale
	quatRotateVec3FastApproximateP(&b1->orientation, &b2->position);  // Rotate
	vec3VAddVP(&b2->position, &b1->position);              // Translate
}
void boneTransformAppendPositionPR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r){
	// Calculate total translation.
	vec3VMultVPR(&b2->position, &b1->scale, &r->position);  // Scale
	quatRotateVec3FastApproximateP(&b1->orientation, &r->position);    // Rotate
	vec3VAddVP(&r->position, &b1->position);               // Translate
}

vec3 boneTransformAppendPositionVec(const bone b, const float x, const float y, const float z){
	return vec3VAddV(b.position, quatRotateVec3FastApproximate(b.orientation, vec3VMultN(b.scale, x, y, z)));
}
void boneTransformAppendPositionVecP(bone *const restrict b, const float x, const float y, const float z){
	// Calculate total translation.
	vec3 tempVec3;
	tempVec3.x = x * b->scale.x;                      // Scale
	tempVec3.y = y * b->scale.y;
	tempVec3.z = z * b->scale.z;
	quatRotateVec3FastApproximateP(&b->orientation, &tempVec3);  // Rotate
	vec3VAddVP(&b->position, &tempVec3);              // Translate
}
void boneTransformAppendPositionVecPR(const bone *const restrict b, const float x, const float y, const float z, vec3 *const restrict r){
	// Calculate total translation.
	r->x = x * b->scale.x;                    // Scale
	r->y = y * b->scale.y;
	r->z = z * b->scale.z;
	quatRotateVec3FastApproximateP(&b->orientation, r);  // Rotate
	vec3VAddVP(r, &b->position);              // Translate
}

quat boneTransformAppendOrientation(const bone b1, const bone b2){
	return quatNormalizeFast(quatQMultQ(b1.orientation, b2.orientation));
}
void boneTransformAppendOrientationP1(bone *const restrict b1, const bone *const restrict b2){
	// Calculate total orientation.
	quatQMultQP1(&b1->orientation, &b2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&b1->orientation);
}
void boneTransformAppendOrientationP2(const bone *const restrict b1, bone *const restrict b2){
	// Calculate total orientation.
	quatQMultQP2(&b1->orientation, &b2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&b2->orientation);
}
void boneTransformAppendOrientationPR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r){
	// Calculate total orientation.
	quatQMultQPR(&b1->orientation, &b2->orientation, &r->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&r->orientation);
}

vec3 boneTransformAppendScale(const bone b1, const bone b2){
	return vec3VMultV(b1.scale, b2.scale);
}
void boneTransformAppendScaleP(bone *const restrict b1, const bone *const restrict b2){
	// Calculate total scale.
	vec3VMultVP(&b1->scale, &b2->scale);
}
void boneTransformAppendScalePR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r){
	// Calculate total scale.
	vec3VMultVPR(&b2->scale, &b1->scale, &r->scale);
}

bone boneTransformAppend(const bone b1, const bone b2){
	/*
	** Adds the transformations for b2 to b1 and stores the result in r.
	** Used for getting the total sum of all transformations of a bone.
	*/
	const bone r = {
		.position = vec3VAddV(b1.position, quatRotateVec3FastApproximate(b1.orientation, vec3VMultV(b2.position, b1.scale))),
		.orientation = quatNormalizeFast(quatQMultQ(b1.orientation, b2.orientation)),
		.scale = vec3VMultV(b1.scale, b2.scale)
	};
	return r;
}
void boneTransformAppendP1(bone *const restrict b1, const bone *const restrict b2){
	/*
	** Adds the transformations for b2 to b1 and stores the result in b1.
	** Used for getting the total sum of all transformations of a bone.
	*/
	// Calculate total translation.
	vec3 tempVec3;
	vec3VMultVPR(&b2->position, &b1->scale, &tempVec3);  // Scale
	quatRotateVec3FastApproximateP(&b1->orientation, &tempVec3);    // Rotate
	vec3VAddVP(&b1->position, &tempVec3);                // Translate
	// Calculate total orientation.
	quatQMultQP1(&b1->orientation, &b2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&b1->orientation);
	// Calculate total scale.
	vec3VMultVP(&b1->scale, &b2->scale);
}
void boneTransformAppendP2(const bone *const restrict b1, bone *const restrict b2){
	/*
	** Adds the transformations for b2 to b1 and stores the result in b2.
	** Used for getting the total sum of all transformations of a bone.
	*/
	// Calculate total translation.
	vec3VMultVP(&b2->position, &b1->scale);                // Scale
	quatRotateVec3FastApproximateP(&b1->orientation, &b2->position);  // Rotate
	vec3VAddVP(&b2->position, &b1->position);              // Translate
	// Calculate total orientation.
	quatQMultQP2(&b1->orientation, &b2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&b2->orientation);
	// Calculate total scale.
	vec3VMultVP(&b2->scale, &b1->scale);
}
void boneTransformAppendPR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r){
	/*
	** Adds the transformations for b2 to b1 and stores the result in r.
	** Used for getting the total sum of all transformations of a bone.
	*/
	// Calculate total translation.
	vec3VMultVPR(&b2->position, &b1->scale, &r->position);  // Scale
	quatRotateVec3FastApproximateP(&b1->orientation, &r->position);    // Rotate
	vec3VAddVP(&r->position, &b1->position);                // Translate
	// Calculate total orientation.
	quatQMultQPR(&b1->orientation, &b2->orientation, &r->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&r->orientation);
	// Calculate total scale.
	vec3VMultVPR(&b1->scale, &b2->scale, &r->scale);
}

bone boneTransformCombine(const bone b1, const bone b2){
	/*
	** Combines the transformations for b2 with b1 and stores the result in r.
	** Used for getting the total sum of all transformations of a bone.
	*/
	const bone r = {
		.position = vec3VAddV(b1.position, b2.position),
		.orientation = quatNormalizeFast(quatQMultQ(b1.orientation, b2.orientation)),
		.scale = vec3VMultV(b1.scale, b2.scale)
	};
	return r;
}
void boneTransformCombineP1(bone *const restrict b1, const bone *const restrict b2){
	/*
	** Combines the transformations for b2 with b1 and stores the result in b1.
	** Used for getting the total sum of all transformations of a bone.
	*/
	// Calculate total translation.
	vec3VAddVP(&b1->position, &b2->position);
	// Calculate total orientation.
	quatQMultQP1(&b1->orientation, &b2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&b1->orientation);
	// Calculate total scale.
	vec3VMultVP(&b1->scale, &b2->scale);
}
void boneTransformCombineP2(const bone *const restrict b1, bone *const restrict b2){
	/*
	** Combines the transformations for b2 with b1 and stores the result in b2.
	** Used for getting the total sum of all transformations of a bone.
	*/
	// Calculate total translation.
	vec3VAddVP(&b2->position, &b1->position);
	// Calculate total orientation.
	quatQMultQP2(&b1->orientation, &b2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&b2->orientation);
	// Calculate total scale.
	vec3VMultVP(&b2->scale, &b1->scale);
}
void boneTransformCombinePR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r){
	/*
	** Combines the transformations for b2 with b1 and stores the result in r.
	** Used for getting the total sum of all transformations of a bone.
	*/
	// Calculate total translation.
	vec3VAddVPR(&b1->position, &b2->position, &r->position);
	// Calculate total orientation.
	quatQMultQPR(&b1->orientation, &b2->orientation, &r->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFastP(&r->orientation);
	// Calculate total scale.
	vec3VMultVPR(&b1->scale, &b2->scale, &r->scale);
}
