#include "bone.h"

void boneInit(bone *b){
	vec3SetS(&b->position, 0.f);
	quatSetIdentity(&b->orientation);
	vec3SetS(&b->scale, 1.f);
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

void boneTransformAppend(const bone *b1, const bone *b2, bone *r){
	/*
	** Adds the transformations for b2 to b1 and store the result in r.
	** Used for getting the total sum of all transformations of a bone.
	*/
	// Calculate total translation.
	vec3 tempVec3 = b2->position;
	vec3MultVByV(&tempVec3, &b1->scale);          // Scale
	quatRotateVec3(&b1->orientation, &tempVec3);  // Rotate
	vec3AddVToV(&tempVec3, &b1->position);        // Translate
	r->position = tempVec3;
	// Calculate total orientation.
	quat tempQuat;
	quatMultQByQR(&b2->orientation, &b1->orientation, &tempQuat);
	r->orientation = tempQuat;
	// Calculate total scale.
	vec3MultVByVR(&b2->scale, &b1->scale, &r->scale);
}

void boneTransformDifference(const bone *b1, const bone *b2, bone *r){
	/*
	** Finds the difference in translation, orientation and scale between two bones.
	*/

}
