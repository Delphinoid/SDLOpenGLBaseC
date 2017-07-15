#include "skeleton.h"
#include <string.h>

void boneInit(sklBone *bone){
	bone->name = NULL;
	vec3SetS(&bone->position, 0.f);
	quatSetIdentity(&bone->orientation);
	vec3SetS(&bone->scale, 1.f);
}

void sklInit(skeleton *skl){
	skl->name = NULL;
	skl->root = NULL;
	skl->boneNum = 0;
}

unsigned char sklLoad(skeleton *skl, const char *prgPath, const char *filePath){

	sklInit(skl);
	return 1;

}

static void sklDeleteRecursive(sklNode *node){
	if(node != NULL){
		while(node->childNum > 0){
			sklDeleteRecursive(&node->children[--node->childNum]);
		}
		free(node);
	}
}

void sklDelete(skeleton *skl){
	sklDeleteRecursive(skl->root);
}

static sklKeyframe *sklaiGetAnimFrame(sklAnimInstance *sklai, size_t frame){
	return (sklKeyframe *)cvGet(&sklai->anim->keyframes, frame);
}

static sklBone *sklaiGetAnimBone(sklAnimInstance *sklai, size_t frame, size_t bone){
	return (sklBone *)cvGet(&sklaiGetAnimFrame(sklai, frame)->bones, bone);
}

/** Merge with skliBoneState? **/
/**static void sklaiDeltaTransform(sklAnimInstance *sklai, size_t bone){

	/*
	** Loops through each bone in the current and next animation keyframes, looking for the ones
	** that represent the current bone being worked on. If found in the current keyframe, the
	** start transform for interpolation is set. If found in the next keyframe, the end transform
	** for interpolation is set.
	*
	size_t i;
	for(i = 0; i < sklai->anim->boneNum; i++){

		// Try and find the bone transformation for the current frame
		sklBone *transform = sklaiGetAnimBone(sklai, sklai->currentFrame, i);
		if(transform->name != NULL && strcmp(transform->name, sklai->animInterpStart[bone].name)){
			// A successful match was found, set the bone's start frame for interpolation
			// to the animation's current frame's delta transform
			sklai->animInterpStart[bone] = *transform;
		}

		// Try and find the bone transformation for the next frame
		transform = sklaiGetAnimBone(sklai, sklai->nextFrame, i);
		if(transform->name != NULL && strcmp(transform->name, sklai->animInterpEnd[bone].name)){
			// A successful match was found, set the bone's end frame for interpolation
			// to the animation's next frame's delta transform
			sklai->animInterpEnd[bone] = *transform;
		}

	}


	/*
	** Finally, calculate the interpolated transform for the bone
	*
	if(sklai->animInterpT <= 0.f){

		// Only use the start frame if animInterpT exceeds the lower bounds
		sklai->animState[bone].position = sklai->animInterpStart[bone].position;
		sklai->animState[bone].orientation = sklai->animInterpStart[bone].orientation;
		sklai->animState[bone].scale = sklai->animInterpStart[bone].scale;

	}else if(sklai->animInterpT >= 1.f){

		// Only use the end frame if animInterpT exceeds the upper bounds
		sklai->animState[bone].position = sklai->animInterpEnd[bone].position;
		sklai->animState[bone].orientation = sklai->animInterpEnd[bone].orientation;
		sklai->animState[bone].scale = sklai->animInterpEnd[bone].scale;

	}else{
		// Get the difference between the start position and end position
		sklai->animState[bone].position = vec3VSubV(sklai->animInterpEnd[bone].position,
		                                             sklai->animInterpStart[bone].position);
		// Divide it by animInterpT
		vec3DivVByS(&sklai->animState[bone].position, sklai->animInterpT);

		// Repeat for scale
		sklai->animState[bone].scale = vec3VSubV(sklai->animInterpEnd[bone].scale,
		                                         sklai->animInterpStart[bone].scale);
		vec3DivVByS(&sklai->animState[bone].scale, sklai->animInterpT);

		// SLERP between the start orientation and end orientation
		sklai->animState[bone].orientation = quatSlerp(sklai->animInterpStart[bone].orientation,
		                                                sklai->animInterpEnd[bone].orientation,
		                                                sklai->animInterpT);
	}

}**/

/**static size_t sklaiGenerateState(sklAnimInstance *sklai, sklNode *node, size_t bone){
	/*
	** Depth-first traversal through each bone, running sklaiDeltaTransform on each.
	** Returns the position in sklai->animState of the next bone (aka the number of
	** bones modified so-far).
	*
	if(node != NULL){
		// Update the delta transform for the bone
		sklaiDeltaTransform(sklai, bone);
		// Loop through each child
		size_t i = node->childNum;
		while(i > 0){
			bone = sklaiGenerateState(sklai, &node->children[i], bone+1);
		}
	}
	return bone;
}**/

static void sklaiDeltaTransform(sklAnimInstance *sklai, size_t bone){

	// If the current frame's bone has a valid state change, use it to start interpolation
	sklBone *transform = sklaiGetAnimBone(sklai, sklai->currentFrame, bone);
	if(transform->name != NULL){
		sklai->animInterpStart[bone] = *transform;
	}
	// If the next frame's bone has a valid state change, use it to end interpolation
	transform = sklaiGetAnimBone(sklai, sklai->nextFrame, bone);
	if(transform->name != NULL){
		sklai->animInterpEnd[bone] = *transform;
	}

	// Calculate the interpolated delta transform for the bone
	if(sklai->animInterpT <= 0.f){

		// Only use the start frame if animInterpT exceeds the lower bounds
		sklai->animState[bone].position = sklai->animInterpStart[bone].position;
		sklai->animState[bone].orientation = sklai->animInterpStart[bone].orientation;
		sklai->animState[bone].scale = sklai->animInterpStart[bone].scale;

	}else if(sklai->animInterpT >= 1.f){

		// Only use the end frame if animInterpT exceeds the upper bounds
		sklai->animState[bone].position = sklai->animInterpEnd[bone].position;
		sklai->animState[bone].orientation = sklai->animInterpEnd[bone].orientation;
		sklai->animState[bone].scale = sklai->animInterpEnd[bone].scale;

	}else{
		// Get the difference between the start position and end position
		sklai->animState[bone].position = vec3VSubV(sklai->animInterpEnd[bone].position,
		                                             sklai->animInterpStart[bone].position);
		// Divide it by animInterpT
		vec3DivVByS(&sklai->animState[bone].position, sklai->animInterpT);

		// Repeat for scale
		sklai->animState[bone].scale = vec3VSubV(sklai->animInterpEnd[bone].scale,
		                                         sklai->animInterpStart[bone].scale);
		vec3DivVByS(&sklai->animState[bone].scale, sklai->animInterpT);

		// SLERP between the start orientation and end orientation
		sklai->animState[bone].orientation = quatSlerp(sklai->animInterpStart[bone].orientation,
		                                                sklai->animInterpEnd[bone].orientation,
		                                                sklai->animInterpT);
	}

}

static void sklaiGenerateState(sklAnimInstance *sklai){
	size_t i;
	for(i = 0; i < sklai->anim->boneNum; i++){
		sklaiDeltaTransform(sklai, i);
	}
}

static void sklaiAnimate(sklAnimInstance *sklai, /**skeleton *skl,**/ uint32_t currentTick, float globalDelayMod){

	// Make sure lastUpdate has been set
	if(sklai->lastUpdate == 0.f){
		sklai->lastUpdate = currentTick;
	}

	float totalDelayMod = sklai->delayMod * globalDelayMod;

	// Only animate if the animation has more than one
	// frame and can still be animated
	if(totalDelayMod != 0.f && sklai->anim->frameDelays.size > 1 &&
	   (sklai->currentLoops < sklai->anim->desiredLoops ||
	    sklai->anim->desiredLoops < 0)){

		// Time passed since last update
		float deltaTime = currentTick - sklai->lastUpdate;
		// Multiplier applied to the current frame's delay in order to slow down / speed up the animation
		float currentFrameDelay = *((float *)cvGet(&sklai->anim->frameDelays, sklai->currentFrame))*totalDelayMod;
		// animInterpT is temporarily set to 0 for sklaiDeltaTransform()
		sklai->animInterpT = 0.f;

		/* While deltaTime exceeds the time that the current frame should last and the
		texture can still be animated, advance the animation */
		while(deltaTime >= currentFrameDelay &&
		      (sklai->currentLoops < sklai->anim->desiredLoops ||
		       sklai->anim->desiredLoops < 0)){

			// Add the delay to lastUpdate and advance the animation
			deltaTime -= currentFrameDelay;
			sklai->lastUpdate += currentFrameDelay;

			// Increase currentFrame and check if it exceeds the number of frames
			if(++sklai->currentFrame == sklai->anim->frameDelays.size){
				// currentFrame has exceeded the number of frames, increase the loop counter
				sklai->currentLoops++;
				if(sklai->currentLoops < sklai->anim->desiredLoops ||
				   sklai->anim->desiredLoops < 0){
					// If the animation can continue to loop, reset it to the first frame
					sklai->currentFrame = 0;
				}else{
					// Otherwise set it to the final frame
					sklai->currentFrame = sklai->anim->frameDelays.size-1;
					sklai->lastUpdate = currentTick;
				}
			}

			// Calculate nextFrame
			if(sklai->currentFrame < sklai->anim->frameDelays.size-1){
				sklai->nextFrame = sklai->currentFrame+1;
			}else if(sklai->currentLoops < sklai->anim->desiredLoops ||
			         sklai->anim->desiredLoops < 0){
				sklai->nextFrame = 0;
			}

			// Update currentFrameDelay based on the new value of currentFrame
			currentFrameDelay = *((float *)cvGet(&sklai->anim->frameDelays, sklai->currentFrame))*totalDelayMod;

			// Generate boneState for the new frame
			// With the current way animations work, boneState must be updated every time
			// the current frame changes so certain bone transformations aren't "lost" if
			// skipped over
			if(deltaTime < currentFrameDelay){
				// If statement exists so we can use animInterpT on the final call
				// (it is temporarily set to 0 for these calls)
				/**sklaiGenerateState(sklai, skl->root, 0);**/
				sklaiGenerateState(sklai);
			}

		}

		// Set animInterpT to a number between 0 and 1, where 0 is the current frame and 1 is the next frame
		sklai->animInterpT = deltaTime / *((float *)cvGet(&sklai->anim->frameDelays, sklai->currentFrame));
		// Final state update
		/**sklaiGenerateState(sklai, skl->root, 0);**/
		sklaiGenerateState(sklai);

	}

}

void skliInit(sklInstance *skli, skeleton *skl){
	skli->skl = skl;
	if(skl != NULL){
		cvInit(&skli->animations, skl->boneNum);
		skli->customState = malloc(skl->boneNum*sizeof(sklBone));
		skli->skeletonState = malloc(skl->boneNum*sizeof(mat4));
	}
}

unsigned char skliLoad(sklInstance *skli, const char *prgPath, const char *filePath){
	//
}

static void skliBoneState(sklInstance *skli, sklNode *node, size_t parent, size_t bone){
	/*
	** Update skeletonState for the specified bone. Uses the delta transforms
	** in animState from each sklAnimInstance.
	*/
	// Apply parent transforms first if possible
	if(bone != parent){
		skli->skeletonState[bone] = skli->skeletonState[parent];
	}else{
		mat4Identity(&skli->skeletonState[bone]);
	}
	// Apply base transforms
	mat4Translate(&skli->skeletonState[bone], node->bone.position.x,
	                                          node->bone.position.y,
	                                          node->bone.position.z);
	mat4Rotate(&skli->skeletonState[bone], node->bone.orientation);
	mat4Scale(&skli->skeletonState[bone], node->bone.scale.x,
	                                      node->bone.scale.y,
	                                      node->bone.scale.z);
	/** Find the bone's position in each sklAnimInstance by strcmping the names **/
	/** Later, set up a "lookup table" of sorts when animations are added to make this faster **/
	size_t i, j;
	for(i = 0; i < skli->animations.size; i++){
		// Loop through each bone modified by the animation
		for(j = 0; j < ((sklAnimInstance *)cvGet(&skli->animations, i))->anim->boneNum; j++){
			sklBone *currentAnimState = ((sklAnimInstance *)cvGet(&skli->animations, i))->animState;
			/** Use a lookup here instead of strcmp() **/
			if(strcmp(node->bone.name, currentAnimState[j].name)){
				mat4Translate(&skli->skeletonState[bone], currentAnimState[j].position.x,
				                                          currentAnimState[j].position.y,
				                                          currentAnimState[j].position.z);
				mat4Rotate(&skli->skeletonState[bone], currentAnimState[j].orientation);
				mat4Scale(&skli->skeletonState[bone], currentAnimState[j].scale.x,
				                                      currentAnimState[j].scale.y,
				                                      currentAnimState[j].scale.z);
				break;
			}
		}

	}
}

static size_t skliGenerateState(sklInstance *skli, sklNode *node, size_t parent, size_t bone){
	/*
	** Depth-first traversal through each bone, running skliInterpolateBone on each.
	** Returns the position in skli->sklState of the next bone (the number of bones modified so-far).
	*/
	size_t nextBone = bone;
	if(node != NULL){
		// Update the delta transform for the bone
		skliBoneState(skli, node, parent, bone);
		// Loop through each child
		size_t i = node->childNum;
		while(i > 0){
			nextBone = skliGenerateState(skli, &node->children[i], bone, nextBone+1);
		}
	}
	return nextBone;
}

void skliAnimate(sklInstance *skli, uint32_t currentTick, float globalDelayMod){
	size_t i;
	for(i = 0; i < skli->animations.size; i++){
		sklaiAnimate((sklAnimInstance *)cvGet(&skli->animations, i), currentTick, globalDelayMod);
	}
	skliGenerateState(skli, skli->skl->root, 0, 0);
}

void skliDelete(sklInstance *skli){
	cvClear(&skli->animations);
	if(skli->customState != NULL){
		free(skli->customState);
	}
	if(skli->skeletonState != NULL){
		free(skli->skeletonState);
	}
}
