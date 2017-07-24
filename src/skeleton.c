#include "skeleton.h"
#include <string.h>

#define radianRatio 0.017453292  // = PI / 180, used for converting degrees to radians

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
		vec3 difference = vec3VSubV(sklai->animInterpEnd[bone].position, sklai->animInterpStart[bone].position);
		vec3MultVByS(&difference, sklai->animInterpT);  // Divide it by animInterpT
		sklai->animState[bone].position = vec3VAddV(sklai->animInterpStart[bone].position, difference);

		// Repeat for scale
		difference = vec3VSubV(sklai->animInterpEnd[bone].scale, sklai->animInterpStart[bone].scale);
		vec3MultVByS(&difference, sklai->animInterpT);  // Divide it by animInterpT
		sklai->animState[bone].scale = vec3VAddV(sklai->animInterpStart[bone].scale, difference);

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

static void sklaiAnimate(sklAnimInstance *sklai, uint32_t currentTick, float globalDelayMod){

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
			if(deltaTime >= currentFrameDelay){
				// If statement exists so we can use animInterpT on the final call
				// (it is temporarily set to 0 for these calls)
				sklaiGenerateState(sklai);
			}

		}

		// Set animInterpT to a number between 0 and 1, where 0 is the current frame and 1 is the next frame
		sklai->animInterpT = deltaTime / *((float *)cvGet(&sklai->anim->frameDelays, sklai->currentFrame));
		// Final state update
		sklaiGenerateState(sklai);

	}

}

void skliaChangeAnim(sklAnimInstance *sklia, sklAnim *anim){
	/** Needs a special function for changing animations in order to handle blending correctly **/
}

void skliInit(sklInstance *skli, skeleton *skl){
	skli->skl = skl;
	if(skl != NULL){
		cvInit(&skli->animations, skl->boneNum);
		skli->customState = malloc(skl->boneNum*sizeof(sklBone));
		skli->skeletonState = malloc(skl->boneNum*sizeof(mat4));
		/** Temp? **/
		sklBone tempBone; boneInit(&tempBone);
		mat4 identityMatrix; mat4Identity(&identityMatrix);
		size_t i;
		for(i = 0; i < skl->boneNum; i++){
			skli->customState[i] = tempBone;
			skli->skeletonState[i] = identityMatrix;
		}
	}
}

unsigned char skliLoad(sklInstance *skli, const char *prgPath, const char *filePath){

	skeleton *skel = malloc(sizeof(skeleton));
	skel->root = malloc(sizeof(sklNode));
	skel->root->bone.name = malloc(5*sizeof(char));
	skel->root->bone.name = "root\0";
	skel->root->bone.position = vec3New(0.5f, -1.f, 0.5f);
	skel->root->bone.orientation = quatNew(1.f, 0.f, 0.f, 0.f);
	skel->root->bone.scale = vec3New(1.f, 1.f, 1.f);
	skel->root->parent = NULL;
	skel->root->childNum = 1;
	skel->root->children = malloc(sizeof(sklNode));
	skel->root->children[0].parent = skel->root;
	skel->root->children[0].childNum = 0;
	skel->root->children[0].bone.name = malloc(4*sizeof(char));
	skel->root->children[0].bone.name = "top\0";
	skel->root->children[0].bone.position = vec3New(0.f, 2.f, 0.f);
	skel->root->children[0].bone.orientation = quatNew(1.f, 0.f, 0.f, 0.f);
	skel->root->children[0].bone.scale = vec3New(1.f, 1.f, 1.f);
	skel->boneNum = 2;

	skliInit(skli, skel);

	sklAnim *anim = malloc(sizeof(sklAnim));
	anim->desiredLoops = -1;
	anim->boneNum = 2;
	cvInit(&anim->keyframes, 4);
	sklKeyframe tempKeyframe;
	sklBone tempBoneRoot, tempBoneTop;

	cvInit(&tempKeyframe.bones, 2);
	boneInit(&tempBoneRoot); boneInit(&tempBoneTop);
	cvPush(&tempKeyframe.bones, &tempBoneRoot, sizeof(tempBoneRoot));
	cvPush(&tempKeyframe.bones, &tempBoneTop, sizeof(tempBoneTop));
	cvPush(&anim->keyframes, &tempKeyframe, sizeof(tempKeyframe));

	cvInit(&tempKeyframe.bones, 2);
	tempBoneRoot.name = malloc(5*sizeof(char));
	tempBoneRoot.name = "root\0";
	tempBoneTop.name = malloc(4*sizeof(char));
	tempBoneTop.name = "top\0";
	tempBoneTop.position.y = 0.5f;
	cvPush(&tempKeyframe.bones, &tempBoneRoot, sizeof(tempBoneRoot));
	cvPush(&tempKeyframe.bones, &tempBoneTop, sizeof(tempBoneTop));
	cvPush(&anim->keyframes, &tempKeyframe, sizeof(tempKeyframe));

	cvInit(&tempKeyframe.bones, 2);
	tempBoneRoot.position.y = 0.5f;
	tempBoneTop.position.y = 0.f;
	tempBoneTop.orientation = quatNewEuler(0.f, 90.f*radianRatio, 0.f);
	cvPush(&tempKeyframe.bones, &tempBoneRoot, sizeof(tempBoneRoot));
	cvPush(&tempKeyframe.bones, &tempBoneTop, sizeof(tempBoneTop));
	cvPush(&anim->keyframes, &tempKeyframe, sizeof(tempKeyframe));

	cvInit(&tempKeyframe.bones, 2);
	tempBoneRoot.position.y = 0.f;
	tempBoneTop.position.y = 0.f;
	tempBoneTop.orientation = quatNew(1.f, 0.f, 0.f, 0.f);
	cvPush(&tempKeyframe.bones, &tempBoneRoot, sizeof(tempBoneRoot));
	cvPush(&tempKeyframe.bones, &tempBoneTop, sizeof(tempBoneTop));
	cvPush(&anim->keyframes, &tempKeyframe, sizeof(tempKeyframe));

	cvInit(&anim->frameDelays, 4);
	float delay = 1000.f;
	cvPush(&anim->frameDelays, &delay, sizeof(delay));
	cvPush(&anim->frameDelays, &delay, sizeof(delay));
	cvPush(&anim->frameDelays, &delay, sizeof(delay));
	delay = 0.f;
	cvPush(&anim->frameDelays, &delay, sizeof(delay));

	sklAnimInstance animInst;
	animInst.anim = anim;
	animInst.delayMod = 1.f;
	animInst.currentFrame = 0;
	animInst.nextFrame = 1;
	animInst.currentLoops = 0;
	animInst.lastUpdate = 0;
	animInst.animInterpT = 0.f;
	animInst.animInterpStart = malloc(2*sizeof(sklBone));
	animInst.animInterpStart[0] = tempBoneRoot;
	animInst.animInterpStart[1] = tempBoneTop;
	animInst.animInterpEnd = malloc(2*sizeof(sklBone));
	animInst.animInterpEnd[0] = tempBoneRoot;
	animInst.animInterpEnd[1] = tempBoneTop;
	animInst.animState = malloc(2*sizeof(sklBone));
	animInst.animState[0] = tempBoneRoot;
	animInst.animState[1] = tempBoneTop;

	cvPush(&skli->animations, &animInst, sizeof(animInst));

	return 1;
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
	mat4Translate(&skli->skeletonState[bone], node->bone.position.x,
	                                          node->bone.position.y,
	                                          node->bone.position.z);
	/** Find the bone's position in each sklAnimInstance by strcmping the names **/
	/** Later, set up a "lookup table" of sorts when animations are added to make this faster **/
	size_t i, j;
	for(i = 0; i < skli->animations.size; i++){
		// Loop through each bone modified by the animation
		for(j = 0; j < ((sklAnimInstance *)cvGet(&skli->animations, i))->anim->boneNum; j++){
			sklBone *currentAnimState = ((sklAnimInstance *)cvGet(&skli->animations, i))->animState;
			/** Use a lookup here instead of strcmp() **/
			if(strcmp(node->bone.name, currentAnimState[j].name) == 0){
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
	mat4Translate(&skli->skeletonState[bone], -node->bone.position.x,
	                                          -node->bone.position.y,
	                                          -node->bone.position.z);
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
		size_t i;
		for(i = 0; i < node->childNum; i++){
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
