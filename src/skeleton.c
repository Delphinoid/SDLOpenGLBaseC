#include "skeleton.h"
#include <string.h>

#define RADIAN_RATIO 0.017453292  // = PI / 180, used for converting degrees to radians

#define ANIM_START_CAPACITY 1

void boneInit(sklBone *bone){
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
static sklNode *sklFindBone(sklNode *node, const char *name){
	sklNode *r = NULL;
	if(node != NULL && strcmp(node->name, name) != 0){
		size_t i;
		for(i = 0; r == NULL && i < node->childNum; ++i){
			r = sklFindBone(&node->children[i], name);
		}
	}else{
		r = node;
	}
	return r;
}
static void sklDeleteRecursive(sklNode *node){
	if(node != NULL){
		if(node->name != NULL){
			free(node->name);
		}
		while(node->childNum > 0){
			sklDeleteRecursive(&node->children[--node->childNum]);
		}
		free(node);
	}
}
void sklDelete(skeleton *skl){
	sklDeleteRecursive(skl->root);
}

void sklaInit(sklAnim *skla){
	//
}
unsigned char sklaLoad(sklAnim *skla, const char *prgPath, const char *filePath){

	sklaInit(skla);
	return 1;

}
void sklaDelete(sklAnim *skla){
	if(skla->name != NULL){
		free(skla->name);
	}
	if(skla->bones != NULL){
		size_t i;
		for(i = 0; i < skla->boneNum; ++i){
			if(skla->bones[i] != NULL){
				free(skla->bones[i]);
			}
		}
		free(skla->bones);
	}
	if(skla->frames != NULL){
		size_t i, j;
		for(i = 0; i < skla->animData.frameNum; ++i){
			if(skla->frames[i] != NULL){
				for(j = 0; j < skla->boneNum; ++j){
					if(skla->frames[i][j] != NULL){
						free(skla->frames[i][j]);
					}
				}
				free(skla->frames[i]);
			}
		}
		free(skla->frames);
	}
	if(skla->animData.frameDelays != NULL){
		free(skla->animData.frameDelays);
	}
}

static inline sklBone **sklaiGetAnimFrame(const sklAnimInstance *sklai, const size_t frame){
	//return (sklKeyframe *)cvGet(&sklai->anim->keyframes, frame);
	return sklai->anim->frames[frame];
}
static inline sklBone *sklaiGetAnimBone(const sklAnimInstance *sklai, const size_t frame, const size_t bone){
	//return (sklBone *)cvGet(&sklaiGetAnimFrame(sklai, frame)->bones, bone);
	return sklai->anim->frames[frame][bone];
}
static void sklaiDeltaTransform(sklAnimInstance *sklai, const size_t bone){

	// If the current frame's bone has a valid state change, use it to start interpolation
	sklBone *transform = sklaiGetAnimBone(sklai, sklai->animInst.currentFrame, bone);
	if(transform != NULL){
		sklai->animInterpStart[bone] = *transform;
	}
	// If the next frame's bone has a valid state change, use it to end interpolation
	transform = sklaiGetAnimBone(sklai, sklai->animInst.nextFrame, bone);
	if(transform != NULL){
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
	for(i = 0; i < sklai->anim->boneNum; ++i){
		sklaiDeltaTransform(sklai, i);
	}
}
static void sklaiAnimate(sklAnimInstance *sklai, const float timeElapsed){
	/*if(sklai->animInst.currentFrame >= sklai->anim->animData.frameNum){
		sklai->animInst.currentFrame = 0;
	}
	if(sklai->animInst.nextFrame >= sklai->anim->animData.frameNum){
		sklai->animInst.nextFrame = 0;
	}*/
	animAdvance(&sklai->animInst, &sklai->anim->animData, timeElapsed);
	sklai->animInterpT = sklai->animInst.currentFrameProgress / sklai->animInst.currentFrameLength;
	sklaiGenerateState(sklai);
}
/**static void sklaiAnimate(sklAnimInstance *sklai, const uint32_t currentTick, const float globalDelayMod){

	// Make sure lastUpdate has been set
	if(sklai->lastUpdate == 0.f){
		sklai->lastUpdate = currentTick;
	}

	const float totalDelayMod = sklai->delayMod * globalDelayMod;

	// Only animate if the animation has more than one
	// frame and can still be animated
	if(totalDelayMod != 0.f && sklai->anim->frameNum > 1 &&
	   (sklai->currentLoops < sklai->anim->desiredLoops ||
	    sklai->anim->desiredLoops < 0)){

		// Time passed since last update
		float deltaTime = currentTick - sklai->lastUpdate;
		// Multiplier applied to the current frame's delay in order to slow down / speed up the animation
		float currentFrameDelay = sklai->anim->frameDelays[sklai->currentFrame]*totalDelayMod;
		// animInterpT is temporarily set to 0 for sklaiDeltaTransform()
		sklai->animInterpT = 0.f;

		* While deltaTime exceeds the time that the current frame should last and the
		texture can still be animated, advance the animation *
		while(deltaTime >= currentFrameDelay &&
		      (sklai->currentLoops < sklai->anim->desiredLoops ||
		       sklai->anim->desiredLoops < 0)){

			// Add the delay to lastUpdate and advance the animation
			deltaTime -= currentFrameDelay;
			sklai->lastUpdate += currentFrameDelay;
			// Increase currentFrame and check if it exceeds the number of frames
			if(++sklai->currentFrame == sklai->anim->frameNum){
				// currentFrame has exceeded the number of frames, increase the loop counter
				++sklai->currentLoops;
				if(sklai->currentLoops < sklai->anim->desiredLoops ||
				   sklai->anim->desiredLoops < 0){
					// If the animation can continue to loop, reset it to the first frame
					sklai->currentFrame = 0;
				}else{
					// Otherwise set it to the final frame
					sklai->currentFrame = sklai->anim->frameNum-1;
					sklai->lastUpdate = currentTick;
				}
			}

			// Calculate nextFrame
			if(sklai->currentFrame < sklai->anim->frameNum-1){
				sklai->nextFrame = sklai->currentFrame+1;
			}else if(sklai->currentLoops < sklai->anim->desiredLoops ||
			         sklai->anim->desiredLoops < 0){
				sklai->nextFrame = 0;
			}

			// Update currentFrameDelay based on the new value of currentFrame
			currentFrameDelay = sklai->anim->frameDelays[sklai->currentFrame]*totalDelayMod;

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
		sklai->animInterpT = deltaTime / sklai->anim->frameDelays[sklai->currentFrame];
		// Final state update
		sklaiGenerateState(sklai);

	}

}**/
void sklaiChangeAnim(sklAnimInstance *sklai, const sklAnim *anim){
	/** Needs a special function for changing animations in order to handle blending correctly **/
}
void sklaiDelete(sklAnimInstance *sklai){
	if(sklai->animInterpStart != NULL){
		free(sklai->animInterpStart);
	}
	if(sklai->animInterpEnd != NULL){
		free(sklai->animInterpEnd);
	}
	if(sklai->animState != NULL){
		free(sklai->animState);
	}
}

void skliInit(sklInstance *skli, skeleton *skl){
	skli->skl = skl;
	skli->timeMod = 1.f;
	skli->animationNum = 0;
	skli->animationCapacity = ANIM_START_CAPACITY;
	skli->animations = malloc(skli->animationCapacity*sizeof(sklAnim));
	if(skl != NULL){
		skli->customState = malloc(skl->boneNum*sizeof(sklBone));
		/**skli->skeletonState = malloc(skl->boneNum*sizeof(mat4));**/
		/** Temp? **/
		sklBone tempBone; boneInit(&tempBone);
		mat4 identityMatrix; mat4Identity(&identityMatrix);
		size_t i;
		for(i = 0; i < skl->boneNum; ++i){
			skli->customState[i] = tempBone;
			/**skli->skeletonState[i] = identityMatrix;**/
		}
	}
}
unsigned char skliLoad(sklInstance *skli, const char *prgPath, const char *filePath){

	skeleton *skl = malloc(sizeof(skeleton));
	skl->root = malloc(sizeof(sklNode));
	skl->root->name = malloc(5*sizeof(char));
	memcpy(skl->root->name, "root\0", 5);
	skl->root->defaultState.position = vec3New(0.5f, -1.f, 0.5f);
	skl->root->defaultState.orientation = quatNew(1.f, 0.f, 0.f, 0.f);
	skl->root->defaultState.scale = vec3New(1.f, 1.f, 1.f);
	skl->root->parent = NULL;
	skl->root->childNum = 1;
	skl->root->children = malloc(sizeof(sklNode));
	skl->root->children[0].parent = skl->root;
	skl->root->children[0].childNum = 0;
	skl->root->children[0].name = malloc(4*sizeof(char));
	memcpy(skl->root->children[0].name, "top\0", 4);
	skl->root->children[0].defaultState.position = vec3New(0.f, 2.f, 0.f);
	skl->root->children[0].defaultState.orientation = quatNew(1.f, 0.f, 0.f, 0.f);
	skl->root->children[0].defaultState.scale = vec3New(1.f, 1.f, 1.f);
	skl->boneNum = 2;

	skliInit(skli, skl);

	sklAnim *skla = malloc(sizeof(sklAnim));
	skla->name = malloc(5*sizeof(char));
	memcpy(skla->name, "test\0", 5);
	skla->animData.desiredLoops = -1;
	skla->boneNum = 2;
	skla->bones = malloc(skla->boneNum*sizeof(char*));
	skla->bones[0] = malloc(5*sizeof(char));
	memcpy(skla->bones[0], "root\0", 5);
	skla->bones[1] = malloc(4*sizeof(char));
	memcpy(skla->bones[1], "top\0", 4);
	skla->animData.frameNum = 4;
	skla->frames = malloc(skla->animData.frameNum*sizeof(sklBone**));
	skla->animData.frameDelays = malloc(skla->animData.frameNum*sizeof(float));

	sklBone tempBoneRoot, tempBoneTop;
	boneInit(&tempBoneRoot); boneInit(&tempBoneTop);

	skla->frames[0] = malloc(skla->boneNum*sizeof(sklBone*));
	skla->frames[0][0] = NULL;
	skla->frames[0][1] = NULL;
	skla->animData.frameDelays[0] = 1000.f;

	skla->frames[1] = malloc(skla->boneNum*sizeof(sklBone*));
	skla->frames[1][0] = malloc(sizeof(sklBone));
	*skla->frames[1][0] = tempBoneRoot;
	skla->frames[1][1] = malloc(sizeof(sklBone));
	tempBoneTop.position.y = 0.5f;
	*skla->frames[1][1] = tempBoneTop;
	skla->animData.frameDelays[1] = 1000.f;

	skla->frames[2] = malloc(skla->boneNum*sizeof(sklBone*));
	skla->frames[2][0] = malloc(sizeof(sklBone));
	tempBoneRoot.position.y = 0.5f;
	*skla->frames[2][0] = tempBoneRoot;
	skla->frames[2][1] = malloc(sizeof(sklBone));
	tempBoneTop.position.y = 0.f;
	tempBoneTop.orientation = quatNewEuler(0.f, 90.f*RADIAN_RATIO, 0.f);
	*skla->frames[2][1] = tempBoneTop;
	skla->animData.frameDelays[2] = 1000.f;

	skla->frames[3] = malloc(skla->boneNum*sizeof(sklBone*));
	skla->frames[3][0] = malloc(sizeof(sklBone));
	tempBoneRoot.position.y = 0.f;
	*skla->frames[3][0] = tempBoneRoot;
	skla->frames[3][1] = malloc(sizeof(sklBone));
	tempBoneTop.position.y = 0.f;
	tempBoneTop.orientation = quatNew(1.f, 0.f, 0.f, 0.f);
	*skla->frames[3][1] = tempBoneTop;
	skla->animData.frameDelays[3] = 1000.f;

	sklAnimInstance sklai;
	sklai.anim = skla;
	sklai.animInst.currentLoops = 0;
	sklai.animInst.currentFrame = 0;
	sklai.animInst.currentFrameProgress = 0.f;
	sklai.animInst.currentFrameLength = 0.f;
	sklai.animInst.nextFrame = 1;
	sklai.animInterpT = 0.f;
	sklai.animInterpStart = malloc(skla->boneNum*sizeof(sklBone));
	sklai.animInterpStart[0] = tempBoneRoot;
	sklai.animInterpStart[1] = tempBoneTop;
	sklai.animInterpEnd = malloc(skla->boneNum*sizeof(sklBone));
	sklai.animInterpEnd[0] = tempBoneRoot;
	sklai.animInterpEnd[1] = tempBoneTop;
	sklai.animState = malloc(skla->boneNum*sizeof(sklBone));
	sklai.animState[0] = tempBoneRoot;
	sklai.animState[1] = tempBoneTop;

	skli->animations[skli->animationNum] = sklai;
	++skli->animationNum;

	return 1;
}
void skliAddAnimation(sklInstance *skli, sklAnimInstance *sklai){
	// Should be similar to a vector push function
}
static void skliBoneState(sklInstance *skli, mat4 *state, const sklNode *space, const sklNode *node, const size_t parent, const size_t bone){
	/*
	** Update the transform state of the specified bone. Uses the delta transforms
	** in animState from each sklAnimInstance.
	*/
	// Apply parent transforms first if possible
	if(bone != parent){
		state[bone] = state[parent];
	}else{
		mat4Identity(&state[bone]);
	}
	/*
	** If the bone exists in the animated skeleton,
	** translate the vertices into its space.
	*/
	if(space != NULL){
		mat4Translate(&state[bone], space->defaultState.position.x,
		                            space->defaultState.position.y,
		                            space->defaultState.position.z);
	}else{
		// If it doesn't exist, use the model's bone position
		mat4Translate(&state[bone], node->defaultState.position.x,
		                            node->defaultState.position.y,
		                            node->defaultState.position.z);
	}
	/** Find the bone's position in each sklAnimInstance by strcmping the names **/
	/** Later, set up a "lookup table" of sorts when animations are added to make this faster **/
	size_t i, j;
	for(i = 0; i < skli->animationNum; ++i){
		sklBone *currentAnimState = skli->animations[i].animState;
		// Loop through each bone modified by the animation
		for(j = 0; j < skli->animations[i].anim->boneNum; ++j){
			/** Use a lookup here instead of strcmp() **/
			if(strcmp(node->name, skli->animations[i].anim->bones[j]) == 0){
				mat4Translate(&state[bone], currentAnimState[j].position.x,
				                            currentAnimState[j].position.y,
				                            currentAnimState[j].position.z);
				mat4Rotate(&state[bone], currentAnimState[j].orientation);
				mat4Scale(&state[bone], currentAnimState[j].scale.x,
				                        currentAnimState[j].scale.y,
				                        currentAnimState[j].scale.z);
				break;
			}
		}

	}
	// Translate them back by the model's bone position
	// This and the previous translation make more sense when diagrammed
	mat4Translate(&state[bone], -node->defaultState.position.x,
	                            -node->defaultState.position.y,
	                            -node->defaultState.position.z);
}
void skliAnimate(sklInstance *skli, const float timeElapsed){
	size_t i;
	for(i = 0; i < skli->animationNum; ++i){
		sklaiAnimate(&skli->animations[i], timeElapsed*skli->timeMod);
	}
}
static size_t skliGenerateStateRecursive(sklInstance *skli, mat4 *state, sklNode *space, const sklNode *node, const size_t parent, const size_t bone){
	/*
	** Depth-first traversal through each bone, running skliInterpolateBone on each.
	** Returns the position in skli->sklState of the next bone (the number of bones modified so-far).
	**
	** Passing in a root node of a skeleton other than skli->skl will transform it to fit skli->skl.
	** This can be used for "attaching" models to other skeletons.
	*/
	size_t nextBone = bone;
	if(node != NULL){
		// Find a bone in skli->skl with the same name as node's bone
		/** Could be better maybe? **/
		space = sklFindBone(space, node->name);
		// Update the delta transform for the bone
		skliBoneState(skli, state, space, node, parent, bone);
		// Loop through each child
		size_t i;
		for(i = 0; i < node->childNum; ++i){
			nextBone = skliGenerateStateRecursive(skli, state, space, &node->children[i], bone, nextBone+1);
		}
	}
	return nextBone;
}
void skliGenerateState(sklInstance *skli, mat4 *state, const skeleton *skl){
	skliGenerateStateRecursive(skli, state, skli->skl->root, skl->root, 0, 0);
}
void skliDelete(sklInstance *skli){
	if(skli->animations != NULL){
		size_t i;
		for(i = 0; i < skli->animationNum; ++i){
			sklaiDelete(&skli->animations[i]);
		}
		free(skli->animations);
	}
	if(skli->customState != NULL){
		free(skli->customState);
	}
	/**if(skli->skeletonState != NULL){
		free(skli->skeletonState);
	}**/
}
