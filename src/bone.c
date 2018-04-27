#include "bone.h"

void boneInit(bone *b){
	vec3SetS(&b->position, 0.f);
	quatSetIdentity(&b->orientation);
	vec3SetS(&b->scale, 1.f);
}

void boneInvert(const bone *b, bone *r){
    r->position.x = -b->position.x;
    r->position.y = -b->position.y;
    r->position.z = -b->position.z;
    r->orientation.w = -b->orientation.w;
    r->orientation.v = b->orientation.v;
    r->scale.x = 1.f/b->scale.x;
    r->scale.y = 1.f/b->scale.y;
    r->scale.z = 1.f/b->scale.z;
}

void boneInterpolate(const bone *b1, const bone *b2, const float t, bone *r){

	/* Calculate the interpolated delta transform for the bone. */
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
		vec3Lerp(&b1->position, &b2->position, t, &r->position);

		// SLERP between the start orientation and end orientation.
		quatSlerp(&b1->orientation, &b2->orientation, t, &r->orientation);

		// LERP once more for the scale.
		vec3Lerp(&b1->scale, &b2->scale, t, &r->scale);

	}

}

void boneTransformAppendPosition(const bone *b1, const bone *b2, bone *r){
	// Calculate total translation.
	vec3 tempVec3;
	vec3MultVByVR(&b2->position, &b1->scale, &tempVec3);   // Scale
	quatRotateVec3Fast(&b1->orientation, &tempVec3);       // Rotate
	vec3AddVToVR(&tempVec3, &b1->position, &r->position);  // Translate
}

void boneTransformAppendPositionVec(bone *b, const float x, const float y, const float z, vec3 *r){
	// Calculate total translation.
	vec3 tempVec3;
	tempVec3.x = x * b->scale.x;                     // Scale
	tempVec3.y = y * b->scale.y;
	tempVec3.z = z * b->scale.z;
	quatRotateVec3Fast(&b->orientation, &tempVec3);  // Rotate
	vec3AddVToVR(&tempVec3, &b->position, r);        // Translate
}

void boneTransformAppendOrientation(const bone *b1, const bone *b2, bone *r){
	// Calculate total orientation.
	quat tempQuat;
	quatMultQByQR(&b1->orientation, &b2->orientation, &tempQuat);
	r->orientation = tempQuat;
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFast(&r->orientation);
}

void boneTransformAppendScale(const bone *b1, const bone *b2, bone *r){
	// Calculate total scale.
	vec3MultVByVR(&b2->scale, &b1->scale, &r->scale);
}

void boneTransformAppend(const bone *b1, const bone *b2, bone *r){
	/*
	** Adds the transformations for b2 to b1 and stores the result in r.
	** Used for getting the total sum of all transformations of a bone.
	*/
	// Calculate total translation.
	vec3 tempVec3;
	quat tempQuat;
	vec3MultVByVR(&b2->position, &b1->scale, &tempVec3);   // Scale
	quatRotateVec3Fast(&b1->orientation, &tempVec3);       // Rotate
	vec3AddVToVR(&tempVec3, &b1->position, &r->position);  // Translate
	// Calculate total orientation.
	quatMultQByQR(&b1->orientation, &b2->orientation, &tempQuat);
	r->orientation = tempQuat;
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFast(&r->orientation);
	// Calculate total scale.
	vec3MultVByVR(&b2->scale, &b1->scale, &r->scale);
}

void boneTransformCombine(const bone *b1, const bone *b2, bone *r){
	/*
	** Combines the transformations for b2 with b1 and stores the result in r.
	** Used for getting the total sum of all transformations of a bone.
	*/
	quat tempQuat;
	// Calculate total translation.
	vec3AddVToVR(&b1->position, &b2->position, &r->position);
	// Calculate total orientation.
	quatMultQByQR(&b1->orientation, &b2->orientation, &tempQuat);
	r->orientation = tempQuat;
	// Normalize the new orientation to prevent error build-ups.
	quatNormalizeFast(&r->orientation);
	// Calculate total scale.
	vec3MultVByVR(&b1->scale, &b2->scale, &r->scale);
}

//Invert a bone's state!
void boneStateInvert(const bone *b, bone *out){
	//Invert the bone's rotation!
	quatConjugateFastR(&b->orientation, &out->orientation);

	//Invert its position with respect to the new rotation!
	quatRotateVec3FastR(&b->orientation, &b->position, &out->position);
	out->position.x = -out->position.x;
	out->position.y = -out->position.y;
	out->position.z = -out->position.z;

	//Invert its scale by storing the reciprocal of each value!
	out->scale = b->scale;
	vec3DivSByV(1.f, &out->scale);
}
