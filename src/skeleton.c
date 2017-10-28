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
void boneInterpolate(const sklBone *b1, const sklBone *b2, const float t, sklBone *r){

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
	if(skl != NULL){
		size_t i;
		for(i = 0; i < skl->boneNum; ++i){
			if(skl->bones[i].name != NULL && strcmp(skl->bones[i].name, name) == 0){
				return i;
			}
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
	// animDataInit(&skla->animData);
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
	animDataDelete(&skla->animData);
}

unsigned char sklaiInit(sklAnimInstance *sklai, skeleton *skl, const size_t stateNum){
	sklai->animState = malloc(skl->boneNum*sizeof(sklBone));
	if(sklai->animState == NULL){
		return 0;
	}
	size_t i;
	sklai->animBoneLookup = malloc(stateNum*sizeof(sklBone *));
	if(sklai->animBoneLookup == NULL){
		/** Memory allocation failure. **/
		free(sklai->animState);
		return 0;
	}
	for(i = 0; i < stateNum; ++i){
		/** Create a proper lookup. **/
		sklai->animBoneLookup[i] = malloc(skl->boneNum*sizeof(sklBone));
		if(sklai->animBoneLookup[i] == NULL){
			break;
		}
		boneInit(sklai->animBoneLookup[i]);
	}
	// Check for a memory allocation failure.
	if(i < stateNum){
		while(i > 0){
			free(sklai->animBoneLookup[i]);
			--i;
		}
		free(sklai->animState);
		free(sklai->animBoneLookup);
		return 0;
	}
	return animInstInit(&sklai->animInst, stateNum);
}
void sklaiChangeAnim(sklAnimInstance *sklai, const sklAnim *anim, const size_t frame, const float blendTime){
	/** Needs a special function for changing animations in order to handle blending correctly **/
}
void sklaiDelete(sklAnimInstance *sklai, const size_t boneNum){
	animInstDelete(&sklai->animInst);
	if(sklai->animState != NULL){
		free(sklai->animState);
	}
	if(sklai->animBoneLookup != NULL){
		size_t i;
		for(i = 0; i < boneNum; ++i){
			free(sklai->animBoneLookup[i]);
		}
	}
}

unsigned char skliInit(sklInstance *skli, skeleton *skl, const size_t stateNum){
	/** Use stateNum. **/
	skli->skl = skl;
	skli->timeMod = 1.f;
	skli->animationNum = 0;
	skli->animations = NULL;
	skli->animInstNum = 0;
	skli->animInstCapacity = ANIM_START_CAPACITY;
	skli->animInstances = malloc(skli->animInstCapacity*sizeof(sklAnimInstance));
	if(skli->animInstances == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	if(skl != NULL){
		size_t i;
		skli->customState = malloc(stateNum*sizeof(sklBone *));
		if(skli->customState == NULL){
			/** Memory allocation failure. **/
			free(skli->animations);
			return 0;
		}
		for(i = 0; i < stateNum; ++i){
			skli->customState[i] = malloc(skl->boneNum*sizeof(sklBone));
			if(skli->customState[i] == NULL){
				break;
			}
			boneInit(skli->customState[i]);
		}
		// Check for a memory allocation failure.
		if(i < stateNum){
			while(i > 0){
				free(skli->customState[i]);
				--i;
			}
			free(skli->animations);
			free(skli->customState);
			return 0;
		}
	}
	return 1;
}
unsigned char skliLoad(sklInstance *skli, const size_t stateNum, const char *prgPath, const char *filePath){

	/** stateNum is temporary. **/

	skeleton *skl = malloc(sizeof(skeleton));
	sklLoad(skl, prgPath, filePath);

	skliInit(skli, skl, stateNum);

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
	skla->animData.frameDelays[1] = 2000.f;

	skla->frames[2] = malloc(skla->boneNum*sizeof(sklBone));
	tempBoneRoot.position.y = 0.5f;
	skla->frames[2][0] = tempBoneRoot;
	tempBoneTop.position.y = 0.f;
	tempBoneTop.orientation = quatNewEuler(0.f, 90.f*RADIAN_RATIO, 0.f);
	skla->frames[2][1] = tempBoneTop;
	skla->animData.frameDelays[2] = 3000.f;

	++skli->animationNum;
	skli->animations = malloc(skli->animationNum*sizeof(sklAnim *));
	skli->animations[0] = skla;

	sklaiInit(&skli->animInstances[0], skl, stateNum);
	skli->animInstances[0].animBoneLookup[0] = &skli->animInstances[0].animState[0];
	skli->animInstances[0].animBoneLookup[1] = &skli->animInstances[0].animState[1];
	++skli->animInstNum;

	return 1;

}
void skliAddAnimation(sklInstance *skli, const sklAnim *skla, const size_t frame){
	//
}
void skliChangeSkeleton(sklInstance *skli, skeleton *skl){
	/** Re-calculate bone lookups for all animation instances. **/
}
void skliAnimate(sklInstance *skli, const size_t stateNum, const float elapsedTime){
	float elapsedTimeMod = elapsedTime*skli->timeMod;
	size_t i;
	for(i = 0; i < skli->animInstNum; ++i){
		animResetInterpolation(&skli->animInstances[i].animInst, stateNum);
		animAdvance(&skli->animInstances[i].animInst, &skli->animations[*skli->animInstances[i].animInst.currentAnim]->animData, elapsedTimeMod);
	}
}
void skliGenerateAnimStates(sklInstance *skli, const size_t state, const float interpT){

	size_t startAnim, startFrame;
	float startProgress;
	size_t endAnim, endFrame;
	float animInterpT;
	sklBone startBone, endBone;

	size_t i;
	for(i = 0; i < skli->animInstNum; ++i){

		animGetRenderData(&skli->animInstances[i].animInst, &skli->animations[*skli->animInstances[i].animInst.currentAnim]->animData, state, interpT,
		                  &startAnim, &startFrame, &startProgress, &endAnim, &endFrame, &animInterpT);

		size_t j;
		for(j = 0; j < skli->animations[endAnim]->boneNum; ++j){

			/* Find the start and end states to interpolate between for bone j of animation instance i. */
			if(startProgress > 0.f){
				// The start bone is in a transition between two other bones. Interpolate between them.
				if(startFrame + 1 >= skli->animations[startAnim]->animData.frameNum){
					boneInterpolate(&skli->animations[startAnim]->frames[startFrame][j],
					                &skli->animations[startAnim]->frames[0][j],
					                startProgress, &startBone);
				}else{
					boneInterpolate(&skli->animations[startAnim]->frames[startFrame][j],
					                &skli->animations[startAnim]->frames[startFrame+1][j],
					                startProgress, &startBone);
				}
			}else{
				// The start bone is just a regular bone.
				startBone = skli->animations[startAnim]->frames[startFrame][j];
			}
			// The end bone will never be in a transition.
			endBone = skli->animations[endAnim]->frames[endFrame][j];

			/* Interpolate between startBone and endBone. Store the result in animation instance i's animState. */
			boneInterpolate(&startBone, &endBone, animInterpT, &skli->animInstances[i].animState[j]);

		}

	}

}
void skliGenerateBoneState(const sklInstance *skli, const skeleton *skl, mat4 *state, const size_t bone){
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
		for(i = 0; i < skli->animInstNum; ++i){
			if(skli->animInstances[i].animBoneLookup[animBone] != NULL){
				mat4Translate(&state[bone], skli->animInstances[i].animBoneLookup[animBone]->position.x,
				                            skli->animInstances[i].animBoneLookup[animBone]->position.y,
				                            skli->animInstances[i].animBoneLookup[animBone]->position.z);
				mat4Rotate(&state[bone], &skli->animInstances[i].animBoneLookup[animBone]->orientation);
				mat4Scale(&state[bone], skli->animInstances[i].animBoneLookup[animBone]->scale.x,
				                        skli->animInstances[i].animBoneLookup[animBone]->scale.y,
				                        skli->animInstances[i].animBoneLookup[animBone]->scale.z);
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
void skliDelete(sklInstance *skli, const size_t stateNum){
	if(skli->animations != NULL){
		free(skli->animations);
	}
	if(skli->animInstances != NULL){
		size_t i;
		for(i = 0; i < skli->animInstNum; ++i){
			sklaiDelete(&skli->animInstances[i], skli->skl->boneNum);
		}
		free(skli->animInstances);
	}
	if(skli->customState != NULL){
		size_t i;
		for(i = 0; i < stateNum; ++i){
			free(skli->customState[i]);
		}
	}
}
