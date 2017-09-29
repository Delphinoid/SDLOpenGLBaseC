#include "skeleton.h"
#include <string.h>
#include <stdio.h>

#define RADIAN_RATIO 0.017453292  // = PI / 180, used for converting degrees to radians

#define BONE_START_CAPACITY 1
#define ANIM_START_CAPACITY 1

void boneInit(sklBone *bone){
	vec3SetS(&bone->position, 0.f);
	quatSetIdentity(&bone->orientation);
	vec3SetS(&bone->scale, 1.f);
}

static unsigned char sklResizeToFit(skeleton *skl, const size_t boneCapacity){
	if(skl->boneNum != boneCapacity){
		skl->bones = realloc(skl->bones, skl->boneNum*sizeof(sklNode));
		if(skl->bones == NULL){
			printf("Error loading skeleton: Memory allocation failure.\n");
			return 0;
		}
	}
	return 1;
}
void sklInit(skeleton *skl){
	skl->name = NULL;
	skl->boneNum = 0;
	skl->bones = NULL;
}
unsigned char sklLoad(skeleton *skl, const char *prgPath, const char *filePath){

	sklInit(skl);

	size_t boneCapacity = BONE_START_CAPACITY;

	skl->bones = malloc(boneCapacity*sizeof(sklNode));
	if(skl->bones == NULL){
		printf("Error loading skeleton: Memory allocation failure.\n");
		return 0;
	}

	size_t parent = 0;
	int currentCommand = -1;       // The current multiline command type (-1 = none, >-1 = bone)
	unsigned int currentLine = 0;  // Current file line being read

	size_t pathLen = strlen(prgPath);
	size_t fileLen = strlen(filePath);
	char *fullPath = malloc((pathLen+fileLen+1)*sizeof(char));
	memcpy(fullPath, prgPath, pathLen);
	memcpy(fullPath+pathLen, filePath, fileLen);
	fullPath[pathLen+fileLen] = '\0';
	FILE *sklInfo = fopen(fullPath, "r");
	char lineFeed[1024];
	char *line;
	size_t lineLength;

	if(sklInfo != NULL){
		while(fgets(lineFeed, sizeof(lineFeed), sklInfo)){

			line = lineFeed;
			++currentLine;
			lineLength = strlen(line);

			// Remove new line and carriage return
			if(line[lineLength-1] == '\n'){
				line[--lineLength] = '\0';
			}
			if(line[lineLength-1] == '\r'){
				line[--lineLength] = '\0';
			}
			// Remove any comments from the line
			char *commentPos = strstr(line, "//");
			if(commentPos != NULL){
				lineLength -= commentPos-line;
				*commentPos = '\0';
			}
			// Remove any indentations from the line, as well as any trailing spaces and tabs
			unsigned char doneFront = 0, doneEnd = 0;
			size_t newOffset = 0;
			size_t i;
			for(i = 0; (i < lineLength && !doneFront && !doneEnd); ++i){
				if(!doneFront && line[i] != '\t' && line[i] != ' '){
					newOffset = i;
					doneFront = 1;
				}
				if(!doneEnd && i > 1 && i < lineLength && line[lineLength-i] != '\t' && line[lineLength-i] != ' '){
					lineLength -= i-1;
					line[lineLength] = '\0';
					doneEnd = 1;
				}
			}
			line += newOffset;
			lineLength -= newOffset;

			// Name
			if(lineLength >= 6 && strncmp(line, "name ", 5) == 0){
				if(currentCommand == -1){
					skl->name = malloc((lineLength-4) * sizeof(char));
					if(skl->name == NULL){
						printf("Error loading skeleton: Memory allocation failure.\n");
						sklDelete(skl);
						free(fullPath);
						fclose(sklInfo);
						return 0;
					}
					strncpy(skl->name, line+5, lineLength-5);
					skl->name[lineLength-5] = '\0';
				}else{
					printf("Error loading skeleton: Name command at line %u does not belong inside a multiline command.\n", currentLine);
				}

			// Close current multiline command
			}else if(lineLength > 0 && line[0] == '}'){
				if(currentCommand > -1){
					parent = skl->bones[parent].parent;
					if(skl->boneNum == parent){
						// Last bone was reached (equal number of opening and closing braces)
						break;
					}
					--currentCommand;
				}else{
					printf("Error loading skeleton: Invalid closing brace at line %u.\n", currentLine);
				}

			// New bone
			}else if(lineLength >= 18 && strncmp(line, "bone ", 5) == 0){

				char *token = strtok(line+5, " ");

				if(token != NULL){

					if(skl->boneNum >= boneCapacity){
						boneCapacity *= 2;
						sklNode *tempBuffer = realloc(skl->bones, boneCapacity*sizeof(sklNode));
						if(tempBuffer == NULL){
							printf("Error loading skeleton: Memory allocation failure.\n");
							sklDelete(skl);
							free(fullPath);
							fclose(sklInfo);
							return 0;
						}
						skl->bones = tempBuffer;
					}

					size_t nameLen = strlen(token);
					skl->bones[skl->boneNum].name = malloc((nameLen+1)*sizeof(char));
					if(skl->bones[skl->boneNum].name == NULL){
						printf("Error loading skeleton: Memory allocation failure.\n");
						sklDelete(skl);
						free(fullPath);
						fclose(sklInfo);
						return 0;
					}
					memcpy(skl->bones[skl->boneNum].name, token, nameLen);
					skl->bones[skl->boneNum].name[nameLen] = '\0';

					float data[3][3];  // Position, orientation (in Eulers) and scale
					size_t i, j;
					for(i = 0; i < 3; ++i){
						for(j = 0; j < 3; ++j){
							token = strtok(NULL, "/");
							data[i][j] = strtod(token, NULL);
						}
					}

					vec3Set(&skl->bones[skl->boneNum].defaultState.position, data[0][0], data[0][1], data[0][2]);
					quatSetEuler(&skl->bones[skl->boneNum].defaultState.orientation, data[1][0], data[1][1], data[1][2]);
					vec3Set(&skl->bones[skl->boneNum].defaultState.scale, data[2][0], data[2][1], data[2][2]);

					skl->bones[skl->boneNum].parent = parent;

					if(strrchr(token, '{')){
						parent = skl->boneNum;
						++currentCommand;
					}else if(skl->boneNum == parent){
						// Last bone was reached (equal number of opening and closing braces)
						++skl->boneNum;
						break;
					}

					++skl->boneNum;

				}else{
					printf("Error loading skeleton: Bone command at line %u does not specify a name for the bone.\n", currentLine);
				}

			}

		}

		fclose(sklInfo);
		free(fullPath);

	}else{
		printf("Error loading texture wrapper: Couldn't open %s\n", fullPath);
		free(fullPath);
		return 0;
	}

	// If no name was given, generate one based off the file name
	if(skl->name == NULL || skl->name[0] == '\0'){
		skl->name = malloc((fileLen+1)*sizeof(char));
		if(skl->name == NULL){
			printf("Error loading skeleton: Memory allocation failure.\n");
			sklDelete(skl);
			return 0;
		}
		memcpy(skl->name, filePath, fileLen);
		skl->name[fileLen] = '\0';
	}

	return sklResizeToFit(skl, boneCapacity);

}
static size_t sklFindBone(skeleton *skl, const char *name){
	size_t i;
	for(i = 0; i < skl->boneNum; ++i){
		if(skl->bones[i].name != NULL && strcmp(skl->bones[i].name, name) == 0){
			return i;
		}
	}
	return (size_t)-1;
}
void sklDelete(skeleton *skl){
	if(skl->name != NULL){
		free(skl->name);
	}
	if(skl->bones != NULL){
		size_t i;
		for(i = 0; i < skl->boneNum; ++i){
			if(skl->bones[i].name != NULL){
				free(skl->bones[i].name);
			}
		}
		free(skl->bones);
	}
}

void sklaInit(sklAnim *skla){
	//
}
unsigned char sklaLoad(sklAnim *skla, const char *prgPath, const char *filePath){

	sklaInit(skla);

	//

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
		size_t i;
		for(i = 0; i < skla->animData.frameNum; ++i){
			if(skla->frames[i] != NULL){
				free(skla->frames[i]);
			}
		}
		free(skla->frames);
	}
	if(skla->animData.frameDelays != NULL){
		free(skla->animData.frameDelays);
	}
}

void sklaiInit(sklAnimInstance *sklai, const sklAnim *skla){

}
static inline sklBone **sklaiGetAnimFrame(const sklAnimInstance *sklai, const size_t frame){
	//return (sklKeyframe *)cvGet(&sklai->anim->keyframes, frame);
	return &sklai->anim->frames[frame];
}
static inline sklBone *sklaiGetAnimBone(const sklAnimInstance *sklai, const size_t frame, const size_t bone){
	//return (sklBone *)cvGet(&sklaiGetAnimFrame(sklai, frame)->bones, bone);
	return &sklai->anim->frames[frame][bone];
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
		vec3 difference;
		vec3SubVFromVR(&sklai->animInterpEnd[bone].position, &sklai->animInterpStart[bone].position, &difference);
		vec3MultVByS(&difference, sklai->animInterpT);  // Divide it by animInterpT
		vec3AddVToVR(&sklai->animInterpStart[bone].position, &difference, &sklai->animState[bone].position);

		// Repeat for scale
		vec3SubVFromVR(&sklai->animInterpEnd[bone].scale, &sklai->animInterpStart[bone].scale, &difference);
		vec3MultVByS(&difference, sklai->animInterpT);  // Divide it by animInterpT
		vec3AddVToVR(&sklai->animInterpStart[bone].scale, &difference, &sklai->animState[bone].scale);

		// SLERP between the start orientation and end orientation
		quatSlerp(&sklai->animInterpStart[bone].orientation,
		          &sklai->animInterpEnd[bone].orientation,
		          sklai->animInterpT,
		          &sklai->animState[bone].orientation);

	}

}
static void sklaiGenerateState(sklAnimInstance *sklai){
	size_t i;
	for(i = 0; i < sklai->anim->boneNum; ++i){
		sklaiDeltaTransform(sklai, i);
	}
}
static void sklaiAnimate(sklAnimInstance *sklai, const float elapsedTime){
	/*if(sklai->animInst.currentFrame >= sklai->anim->animData.frameNum){
		sklai->animInst.currentFrame = 0;
	}
	if(sklai->animInst.nextFrame >= sklai->anim->animData.frameNum){
		sklai->animInst.nextFrame = 0;
	}*/
	animAdvance(&sklai->animInst, &sklai->anim->animData, elapsedTime);
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
void sklaiChangeAnim(sklAnimInstance *sklai, const sklAnim *anim, const size_t frame, const float blendTime){
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
	sklLoad(skl, prgPath, filePath);

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
	skla->animData.frameNum = 3;
	skla->frames = malloc(skla->animData.frameNum*sizeof(sklBone*));
	skla->animData.frameDelays = malloc(skla->animData.frameNum*sizeof(float));

	sklBone tempBoneRoot, tempBoneTop;
	boneInit(&tempBoneRoot); boneInit(&tempBoneTop);

	skla->frames[0] = malloc(skla->boneNum*sizeof(sklBone));
	skla->frames[0][0] = tempBoneRoot;
	skla->frames[0][1] = tempBoneTop;
	skla->animData.frameDelays[0] = 1000.f;

	skla->frames[1] = malloc(skla->boneNum*sizeof(sklBone));
	skla->frames[1][0] = tempBoneRoot;
	tempBoneTop.position.y = 0.5f;
	skla->frames[1][1] = tempBoneTop;
	skla->animData.frameDelays[1] = 1000.f;

	skla->frames[2] = malloc(skla->boneNum*sizeof(sklBone));
	tempBoneRoot.position.y = 0.5f;
	skla->frames[2][0] = tempBoneRoot;
	tempBoneTop.position.y = 0.f;
	tempBoneTop.orientation = quatNewEuler(0.f, 90.f*RADIAN_RATIO, 0.f);
	skla->frames[2][1] = tempBoneTop;
	skla->animData.frameDelays[2] = 1000.f;

	sklAnimInstance sklai;
	sklai.anim = skla;
	sklai.animInst.currentLoops = 0;
	sklai.animInst.currentFrame = 0;
	sklai.animInst.currentFrameProgress = 0.f;
	sklai.animInst.currentFrameLength = 1.f;
	sklai.animInst.nextFrame = 1;
	sklai.animInterpT = 0.f;
	sklai.animInterpStart = malloc(skla->boneNum*sizeof(sklBone));
	sklai.animInterpEnd = malloc(skla->boneNum*sizeof(sklBone));
	sklai.animState = malloc(skla->boneNum*sizeof(sklBone));
	sklai.animBoneLookup = malloc(2*sizeof(sklBone *));
	sklai.animBoneLookup[0] = &sklai.animState[0];
	sklai.animBoneLookup[1] = &sklai.animState[1];

	skli->animations[skli->animationNum] = sklai;
	++skli->animationNum;

	return 1;

}
void skliAddAnimation(sklInstance *skli, const sklAnim *skla, const size_t frame){
	//
}
void skliChangeSkeleton(sklInstance *skli, skeleton *skl){
	/** Re-calculate bone lookups for all animation instances. **/
}
void skliAnimate(sklInstance *skli, const float elapsedTime){
	size_t i;
	for(i = 0; i < skli->animationNum; ++i){
		sklaiAnimate(&skli->animations[i], elapsedTime*skli->timeMod);
	}
}
/*static void skliBoneState(const sklInstance *skli, mat4 *state, const sklNode *space, const sklNode *node, const size_t parent, const size_t bone){
	*
	** Update the transform state of the specified bone. Uses the delta transforms
	** in animState from each sklAnimInstance.
	*//*
	// Apply parent transforms first if possible
	if(bone != parent){
		state[bone] = state[parent];
	}else{
		mat4Identity(&state[bone]);
	}
	*
	** If the bone exists in the animated skeleton,
	** translate the vertices into its space.
	*//*
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
	** Find the bone's position in each sklAnimInstance by strcmping the names **//*
	** Later, set up a "lookup table" of sorts when animations are added to make this faster **//*
	size_t i;
	for(i = 0; i < skli->animationNum; ++i){
		mat4Translate(&state[bone], skli->animations[i].animBoneLookup[bone]->position.x,
		                            skli->animations[i].animBoneLookup[bone]->position.y,
		                            skli->animations[i].animBoneLookup[bone]->position.z);
		mat4Rotate(&state[bone], skli->animations[i].animBoneLookup[bone]->orientation);
		mat4Scale(&state[bone], skli->animations[i].animBoneLookup[bone]->scale.x,
			                    skli->animations[i].animBoneLookup[bone]->scale.y,
			                    skli->animations[i].animBoneLookup[bone]->scale.z);
	}
	// Translate them back by the model's bone position
	// This and the previous translation make more sense when diagrammed
	mat4Translate(&state[bone], -node->defaultState.position.x,
	                            -node->defaultState.position.y,
	                            -node->defaultState.position.z);
}
static size_t skliGenerateStateRecursive(sklInstance *skli, mat4 *state, sklNode *space, const sklNode *node, const size_t parent, const size_t bone){
	*
	** Depth-first traversal through each bone, running skliInterpolateBone on each.
	** Returns the position in skli->sklState of the next bone (the number of bones modified so-far).
	**
	** Passing in a root node of a skeleton other than skli->skl will transform it to fit skli->skl.
	** This can be used for "attaching" models to other skeletons.
	*//*
	size_t nextBone = bone;
	if(node != NULL){
		// Find a bone in skli->skl with the same name as node's bone
		** Could be better maybe? **//*
		space = sklFindBone(space, node->name);
		// Update the delta transform for the bone
		skliBoneState(skli, state, space, node, parent, bone);
		// Loop through each child
		size_t i;
		for(i = 0; i < node->childNum; ++i){
			nextBone = skliGenerateStateRecursive(skli, state, space, &(*node->children[i]), bone, nextBone+1);
		}
	}
	return nextBone;
}*/
static void skliGenerateBoneState(const sklInstance *skli, const skeleton *skl, mat4 *state, const size_t bone){
	/*
	** Update the transform state of the specified bone. Uses the delta transforms
	** in animState from each sklAnimInstance if possible.
	*/
	// Apply parent transforms first if possible.
	if(bone != skl->bones[bone].parent){
		state[bone] = state[skl->bones[bone].parent];
	}else{
		mat4Identity(&state[bone]);
	}
	/*
	** If the bone exists in the animated skeleton,
	** translate the vertices into its space and
	** apply each animation transform.
	*/
	/** Could definitely be better, using sklFindBone() is horrible. **/
	size_t animBone = sklFindBone(skli->skl, skl->bones[bone].name);
	if(animBone < skli->skl->boneNum){
		mat4Translate(&state[bone], skli->skl->bones[animBone].defaultState.position.x,
		                            skli->skl->bones[animBone].defaultState.position.y,
		                            skli->skl->bones[animBone].defaultState.position.z);
		size_t i;
		for(i = 0; i < skli->animationNum; ++i){
			if(skli->animations[i].animBoneLookup[animBone] != NULL){
				mat4Translate(&state[bone], skli->animations[i].animBoneLookup[animBone]->position.x,
				                            skli->animations[i].animBoneLookup[animBone]->position.y,
				                            skli->animations[i].animBoneLookup[animBone]->position.z);
				mat4Rotate(&state[bone], &skli->animations[i].animBoneLookup[animBone]->orientation);
				mat4Scale(&state[bone], skli->animations[i].animBoneLookup[animBone]->scale.x,
				                        skli->animations[i].animBoneLookup[animBone]->scale.y,
				                        skli->animations[i].animBoneLookup[animBone]->scale.z);
			}
		}
	}else{
		// If it doesn't exist, use the model's bone position.
		mat4Translate(&state[bone], skl->bones[bone].defaultState.position.x,
		                            skl->bones[bone].defaultState.position.y,
		                            skl->bones[bone].defaultState.position.z);
	}
	// Translate them back by the model's bone position.
	// This and the previous translation make more sense when diagrammed.
	mat4Translate(&state[bone], -skl->bones[bone].defaultState.position.x,
	                            -skl->bones[bone].defaultState.position.y,
	                            -skl->bones[bone].defaultState.position.z);
}
void skliGenerateState(const sklInstance *skli, const skeleton *skl, mat4 *state){
	/*
	** Depth-first traversal through skl, running skliGenerateBoneState() on each bone.
	**
	** Passing in a skeleton (skl) that is different to skli->skl will result in it
	** being transformed to fit skli->skl. This can be used for "attaching" models to
	** other models: for example, if you have a hat model, you can pass its skeleton
	** in as skl and the skeleton of the entity wearing it in as skli->skl and it will
	** move the hat onto that entity's head.
	*/
	size_t i;
	for(i = 0; i < skl->boneNum; ++i){
		// Update the skl bone's state.
		skliGenerateBoneState(skli, skl, state, i);
	}
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
