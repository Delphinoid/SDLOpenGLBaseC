#include "bone.h"

void boneInit(bone *const restrict b){
	vec3Zero(&b->position);
	quatSetIdentity(&b->orientation);
	vec3SetS(&b->scale, 1.f);
}

void boneInvert(bone *const restrict b){
    b->position.x = -b->position.x;
    b->position.y = -b->position.y;
    b->position.z = -b->position.z;
    b->orientation.w = -b->orientation.w;
    b->orientation.v = b->orientation.v;
    b->scale.x = 1.f/b->scale.x;
    b->scale.y = 1.f/b->scale.y;
    b->scale.z = 1.f/b->scale.z;
}

void boneInvertR(const bone *const restrict b, bone *const restrict r){
    r->position.x = -b->position.x;
    r->position.y = -b->position.y;
    r->position.z = -b->position.z;
    r->orientation.w = -b->orientation.w;
    r->orientation.v = b->orientation.v;
    r->scale.x = 1.f/b->scale.x;
    r->scale.y = 1.f/b->scale.y;
    r->scale.z = 1.f/b->scale.z;
}

void boneInterpolate1(bone *const restrict b1, const bone *const restrict b2, const float t){

	/*
	** Calculate the interpolated delta transform for the bone.
	*/
	if(t >= 1.f){

		// Only use the end frame if t exceeds the upper bounds.
		b1->position    = b2->position;
		b1->orientation = b2->orientation;
		b1->scale       = b2->scale;

	}else if(t > 0.f){

		// LERP between the start position and end position.
		vec3Lerp1(&b1->position, &b2->position, t);

		// SLERP between the start orientation and end orientation.
		quatSlerp1(&b1->orientation, &b2->orientation, t);

		// LERP once more for the scale.
		vec3Lerp1(&b1->scale, &b2->scale, t);

	}

}

void boneInterpolate2(const bone *const restrict b1, bone *const restrict b2, const float t){

	/*
	** Calculate the interpolated delta transform for the bone.
	*/
	if(t <= 0.f){

		// Only use the start frame if t exceeds the lower bounds.
		b2->position    = b1->position;
		b2->orientation = b1->orientation;
		b2->scale       = b1->scale;

	}else if(t < 1.f){

		// LERP between the start position and end position.
		vec3Lerp2(&b1->position, &b2->position, t);

		// SLERP between the start orientation and end orientation.
		quatSlerp2(&b1->orientation, &b2->orientation, t);

		// LERP once more for the scale.
		vec3Lerp2(&b1->scale, &b2->scale, t);

	}

}

void boneInterpolateR(const bone *const restrict b1, const bone *const restrict b2, const float t, bone *const restrict r){

	/*
	** Calculate the interpolated delta transform for the bone.
	*/
	if(t <= 0.f){

		// Only use the start frame if t exceeds the lower bounds.
		r->position    = b1->position;
		r->orientation = b1->orientation;
		r->scale       = b1->scale;

	}else if(t >= 1.f){

		// Only use the end frame if t exceeds the upper bounds.
		r->position    = b2->position;
		r->orientation = b2->orientation;
		r->scale       = b2->scale;

	}else{

		// LERP between the start position and end position.
		vec3LerpR(&b1->position, &b2->position, t, &r->position);

		// SLERP between the start orientation and end orientation.
		quatSlerpR(&b1->orientation, &b2->orientation, t, &r->orientation);

		// LERP once more for the scale.
		vec3LerpR(&b1->scale, &b2->scale, t, &r->scale);

	}

}

void boneTransformAppendPosition1(bone *const restrict b1, const bone *const restrict b2){
	// Calculate total translation.
	vec3 tempVec3;
	vec3MultVByVR(&b2->position, &b1->scale, &tempVec3);  // Scale
	quatRotateVec3Fast(&b1->orientation, &tempVec3);      // Rotate
	vec3AddVToV(&b1->position, &tempVec3);                // Translate
}

void boneTransformAppendPosition2(const bone *const restrict b1, bone *const restrict b2){
	// Calculate total translation.
	vec3MultVByV(&b2->position, &b1->scale);              // Scale
	quatRotateVec3Fast(&b1->orientation, &b2->position);  // Rotate
	vec3AddVToV(&b2->position, &b1->position);            // Translate
}

void boneTransformAppendPositionR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r){
	// Calculate total translation.
	vec3MultVByVR(&b2->position, &b1->scale, &r->position);  // Scale
	quatRotateVec3Fast(&b1->orientation, &r->position);      // Rotate
	vec3AddVToV(&r->position, &b1->position);                // Translate
}

void boneTransformAppendPositionVec(bone *const restrict b, const float x, const float y, const float z){
	// Calculate total translation.
	vec3 tempVec3;
	tempVec3.x = x * b->scale.x;                     // Scale
	tempVec3.y = y * b->scale.y;
	tempVec3.z = z * b->scale.z;
	quatRotateVec3Fast(&b->orientation, &tempVec3);  // Rotate
	vec3AddVToV(&b->position, &tempVec3);            // Translate
}

void boneTransformAppendPositionVecR(const bone *const restrict b, const float x, const float y, const float z, vec3 *const restrict r){
	// Calculate total translation.
	r->x = x * b->scale.x;                   // Scale
	r->y = y * b->scale.y;
	r->z = z * b->scale.z;
	quatRotateVec3Fast(&b->orientation, r);  // Rotate
	vec3AddVToV(r, &b->position);            // Translate
}

void boneTransformAppendOrientation1(bone *const restrict b1, const bone *const restrict b2){
	// Calculate total orientation.
	quatMultQByQ1(&b1->orientation, &b2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFast(&b1->orientation);
}

void boneTransformAppendOrientation2(const bone *const restrict b1, bone *const restrict b2){
	// Calculate total orientation.
	quatMultQByQ2(&b1->orientation, &b2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFast(&b2->orientation);
}

void boneTransformAppendOrientationR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r){
	// Calculate total orientation.
	quatMultQByQR(&b1->orientation, &b2->orientation, &r->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFast(&r->orientation);
}

void boneTransformAppendScale(bone *const restrict b1, const bone *const restrict b2){
	// Calculate total scale.
	vec3MultVByV(&b1->scale, &b2->scale);
}

void boneTransformAppendScaleR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r){
	// Calculate total scale.
	vec3MultVByVR(&b2->scale, &b1->scale, &r->scale);
}

void boneTransformAppend1(bone *const restrict b1, const bone *const restrict b2){
	/*
	** Adds the transformations for b2 to b1 and stores the result in r.
	** Used for getting the total sum of all transformations of a bone.
	*/
	// Calculate total translation.
	vec3 tempVec3;
	vec3MultVByVR(&b2->position, &b1->scale, &tempVec3);  // Scale
	quatRotateVec3Fast(&b1->orientation, &tempVec3);      // Rotate
	vec3AddVToV(&b1->position, &tempVec3);                // Translate
	// Calculate total orientation.
	quatMultQByQ1(&b1->orientation, &b2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFast(&b1->orientation);
	// Calculate total scale.
	vec3MultVByV(&b1->scale, &b2->scale);
}

void boneTransformAppend2(const bone *const restrict b1, bone *const restrict b2){
	/*
	** Adds the transformations for b2 to b1 and stores the result in r.
	** Used for getting the total sum of all transformations of a bone.
	*/
	// Calculate total translation.
	vec3MultVByV(&b2->position, &b1->scale);              // Scale
	quatRotateVec3Fast(&b1->orientation, &b2->position);  // Rotate
	vec3AddVToV(&b2->position, &b1->position);            // Translate
	// Calculate total orientation.
	quatMultQByQ2(&b1->orientation, &b2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFast(&b2->orientation);
	// Calculate total scale.
	vec3MultVByV(&b2->scale, &b1->scale);
}

void boneTransformAppendR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r){
	/*
	** Adds the transformations for b2 to b1 and stores the result in r.
	** Used for getting the total sum of all transformations of a bone.
	*/
	// Calculate total translation.
	vec3MultVByVR(&b2->position, &b1->scale, &r->position);  // Scale
	quatRotateVec3Fast(&b1->orientation, &r->position);      // Rotate
	vec3AddVToV(&r->position, &b1->position);                // Translate
	// Calculate total orientation.
	quatMultQByQR(&b1->orientation, &b2->orientation, &r->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFast(&r->orientation);
	// Calculate total scale.
	vec3MultVByVR(&b1->scale, &b2->scale, &r->scale);
}

void boneTransformCombine1(bone *const restrict b1, const bone *const restrict b2){
	/*
	** Combines the transformations for b2 with b1 and stores the result in r.
	** Used for getting the total sum of all transformations of a bone.
	*/
	// Calculate total translation.
	vec3AddVToV(&b1->position, &b2->position);
	// Calculate total orientation.
	quatMultQByQ1(&b1->orientation, &b2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFast(&b1->orientation);
	// Calculate total scale.
	vec3MultVByV(&b1->scale, &b2->scale);
}

void boneTransformCombine2(const bone *const restrict b1, bone *const restrict b2){
	/*
	** Combines the transformations for b2 with b1 and stores the result in r.
	** Used for getting the total sum of all transformations of a bone.
	*/
	// Calculate total translation.
	vec3AddVToV(&b2->position, &b1->position);
	// Calculate total orientation.
	quatMultQByQ2(&b1->orientation, &b2->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFast(&b2->orientation);
	// Calculate total scale.
	vec3MultVByV(&b2->scale, &b1->scale);
}

void boneTransformCombineR(const bone *const restrict b1, const bone *const restrict b2, bone *const restrict r){
	/*
	** Combines the transformations for b2 with b1 and stores the result in r.
	** Used for getting the total sum of all transformations of a bone.
	*/
	// Calculate total translation.
	vec3AddVToVR(&b1->position, &b2->position, &r->position);
	// Calculate total orientation.
	quatMultQByQR(&b1->orientation, &b2->orientation, &r->orientation);
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFast(&r->orientation);
	// Calculate total scale.
	vec3MultVByVR(&b1->scale, &b2->scale, &r->scale);
}
