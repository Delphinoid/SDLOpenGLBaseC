#include "skeleton.h"
#include <string.h>
#include <stdio.h>

#define RADIAN_RATIO 0.017453292  // = PI / 180, used for converting degrees to radians

#define ANIM_FRAGMENT_START_CAPACITY 1

static signed char sklResizeToFit(skeleton *skl){
	bone *tempBuffer = realloc(skl->bones, skl->boneNum*sizeof(sklNode));
	if(tempBuffer == NULL){
		/** Memory allocation failure. **/
		sklDelete(skl);
		return 0;
	}
	return 1;
}
void sklInit(skeleton *skl){
	skl->name = NULL;
	skl->boneNum = 0;
	skl->bones = NULL;
}
signed char sklLoad(skeleton *skl, const char *prgPath, const char *filePath){

	sklInit(skl);

	skl->bones = malloc(SKL_MAX_BONE_NUM*sizeof(sklNode));
	if(skl->bones == NULL){
		/** Memory allocation failure. **/
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
		while(fgets(lineFeed, sizeof(lineFeed), sklInfo) && skl->boneNum < SKL_MAX_BONE_NUM){

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
						/** Memory allocation failure. **/
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
			}else if(lineLength >= 24 && strncmp(line, "bone ", 5) == 0){

				char *token = strtok(line+5, " ");

				if(token != NULL){

					size_t nameLen = strlen(token);
					skl->bones[skl->boneNum].name = malloc((nameLen+1)*sizeof(char));
					if(skl->bones[skl->boneNum].name == NULL){
						/** Memory allocation failure. **/
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
					quatSetEuler(&skl->bones[skl->boneNum].defaultState.orientation, data[1][0]*RADIAN_RATIO, data[1][1]*RADIAN_RATIO, data[1][2]*RADIAN_RATIO);
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
			/** Memory allocation failure. **/
			sklDelete(skl);
			return 0;
		}
		memcpy(skl->name, filePath, fileLen);
		skl->name[fileLen] = '\0';
	}

	return sklResizeToFit(skl);

}
signed char sklDefault(skeleton *skl){
	skl->bones = malloc(sizeof(sklNode));
	if(skl->bones == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	skl->bones[0].name = malloc(5*sizeof(char));
	if(skl->bones[0].name == NULL){
		/** Memory allocation failure. **/
		free(skl->bones);
		return 0;
	}
	skl->name = NULL;
	skl->boneNum = 1;
	skl->bones[0].name[0] = 'r';
	skl->bones[0].name[1] = 'o';
	skl->bones[0].name[2] = 'o';
	skl->bones[0].name[3] = 't';
	skl->bones[0].name[4] = '\0';
	boneInit(&skl->bones[0].defaultState);
	skl->bones[0].parent = 0;
	return 1;
}
static signed char sklNodeCopy(const sklNode *onode, sklNode *cnode){
	if(onode->name != NULL){
		size_t length = strlen(onode->name);
		cnode->name = malloc(length*sizeof(char));
		if(cnode->name == NULL){
			/** Memory allocation failure. **/
			return 0;
		}
		strncpy(cnode->name, onode->name, length);
	}else{
		cnode->name = NULL;
	}
	cnode->defaultState = onode->defaultState;
	cnode->parent = onode->parent;
	return 1;
}
signed char sklCopy(const skeleton *oskl, skeleton *cskl){
	size_t i;
	cskl->bones = malloc(oskl->boneNum*sizeof(sklNode));
	if(cskl->bones == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	if(oskl->name != NULL){
		i = strlen(oskl->name);
		cskl->name = malloc(i*sizeof(char));
		if(cskl->name == NULL){
			/** Memory allocation failure. **/
			free(cskl->bones);
			return 0;
		}
		strncpy(cskl->name, oskl->name, i);
	}else{
		cskl->name = NULL;
	}
	for(i = 0; i < oskl->boneNum; ++i){
		if(!sklNodeCopy(&oskl->bones[i], &cskl->bones[i])){
			/** Memory allocation failure. **/
			break;
		}
	}
	if(i != oskl->boneNum){
		// Free the copied names after a memory allocation failure.
		while(i > 0){
			if(cskl->bones[i].name != NULL){
				free(cskl->bones[i].name);
			}
		}
		if(cskl->bones[0].name != NULL){
			free(cskl->bones[0].name);
		}
		// Free the other skeleton data.
		if(cskl->name != NULL){
			free(cskl->name);
		}
		free(cskl->bones);
		return 0;
	}
	cskl->boneNum = oskl->boneNum;
	return 1;
}
signed char sklGenerateLookup(const skeleton *skl1, const skeleton *skl2, size_t **lookup){
	/** **/
}
size_t sklFindBone(const skeleton *skl, const char *name){
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
	// animDataInit(&skla->animData);
}
signed char sklaLoad(sklAnim *skla, const char *prgPath, const char *filePath){

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

static signed char sklafInit(sklAnimFragment *sklaf, sklAnim *anim){
	sklaf->animBoneLookup = malloc(anim->boneNum * sizeof(size_t));
	if(sklaf->animBoneLookup == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	/* Initialize animBoneLookup. */
	size_t i;
	for(i = 0; i < anim->boneNum; ++i){
		/** Create a proper lookup below. **/
		//
	}
	sklaf->currentAnim = anim;
	animInstInit(&sklaf->animator);
	sklaf->animBlendTime = -1.f;
	sklaf->animBlendProgress = -1.f;
	return 1;
}

static void sklafDelete(sklAnimFragment *sklaf){
	if(sklaf->animBoneLookup != NULL){
		free(sklaf->animBoneLookup);
	}
}

static signed char sklaiInit(sklAnimInstance *sklai, sklAnim *anim){
	sklai->animFrags = malloc(ANIM_FRAGMENT_START_CAPACITY * sizeof(sklAnimFragment));
	if(sklai->animFrags == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	if(!sklafInit(&sklai->animFrags[0], anim)){
		/** Memory allocation failure. **/
		free(sklai->animFrags);
		return 0;
	}
	sklai->timeMod = 1.f;
	sklai->animFragNum = 1;
	sklai->animFragCapacity = ANIM_FRAGMENT_START_CAPACITY;
	//sklai->additive = 1;
	return 1;
}
static void sklaiDelete(sklAnimInstance *sklai){
	if(sklai->animFrags != NULL){
		/* Loop through each sklAnimFragment, deleting them. */
		while(sklai->animFragNum > 0){
			--sklai->animFragNum;
			sklafDelete(&sklai->animFrags[sklai->animFragNum]);
		}
		free(sklai->animFrags);
	}
}
static signed char sklaiStateCopy(sklAnimInstance *osklai, sklAnimInstance *csklai){

	while(csklai->animFragNum > osklai->animFragNum){
		--csklai->animFragNum;
		sklafDelete(&csklai->animFrags[csklai->animFragNum]);
	}

	size_t i;
	if(csklai->animFragCapacity != osklai->animFragCapacity){
		sklAnimFragment *tempBuffer = malloc(osklai->animFragCapacity * sizeof(sklAnimFragment));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return 0;
		}
		for(i = 0; i < csklai->animFragCapacity; ++i){
			tempBuffer[i] = csklai->animFrags[i];
		}
		csklai->animFragCapacity = osklai->animFragCapacity;
		csklai->animFrags = tempBuffer;
	}

	/* Loop through each sklAnimFragment, copying everything. */
	size_t fragmentID = 0;
	while(fragmentID < osklai->animFragNum){
		if(fragmentID >= csklai->animFragNum){
			// New animation fragment.
			csklai->animFrags[fragmentID].animBoneLookup = malloc(osklai->animFrags[fragmentID].currentAnim->boneNum * sizeof(size_t));
			if(csklai->animFrags[fragmentID].animBoneLookup == NULL){
				/** Memory allocation failure. **/
				break;
			}
		}else if(csklai->animFrags[fragmentID].currentAnim->boneNum != osklai->animFrags[fragmentID].currentAnim->boneNum){
			// Bone lookup size has changed.
			free(csklai->animFrags[fragmentID].animBoneLookup);
			csklai->animFrags[fragmentID].animBoneLookup = malloc(osklai->animFrags[fragmentID].currentAnim->boneNum * sizeof(size_t));
			if(csklai->animFrags[fragmentID].animBoneLookup == NULL){
				/** Memory allocation failure. **/
				break;
			}
		}
		memcpy(csklai->animFrags[fragmentID].animBoneLookup, osklai->animFrags[fragmentID].animBoneLookup, osklai->animFrags[fragmentID].currentAnim->boneNum * sizeof(size_t));
		csklai->animFrags[fragmentID].currentAnim = osklai->animFrags[fragmentID].currentAnim;
		csklai->animFrags[fragmentID].animator = osklai->animFrags[fragmentID].animator;
		csklai->animFrags[fragmentID].animBlendTime = osklai->animFrags[fragmentID].animBlendTime;
		csklai->animFrags[fragmentID].animBlendProgress = osklai->animFrags[fragmentID].animBlendProgress;
		++fragmentID;
	}
	if(fragmentID < osklai->animFragNum){
		/** Memory allocation failure. **/
		while(fragmentID > 0){
			--fragmentID;
			sklafDelete(&csklai->animFrags[fragmentID]);
		}
		return 0;
	}
	csklai->animFragNum = osklai->animFragNum;

	csklai->timeMod = osklai->timeMod;
	//csklai->additive = osklai->additive;
	return 1;

}
void sklaiAnimate(sklAnimInstance *sklai, const float elapsedTime){

	const float elapsedTimeMod = elapsedTime * sklai->timeMod;

	/* Loop through each sklAnimFragment, updating them. */
	size_t fragmentID = 0;
	while(fragmentID < sklai->animFragNum){

		// Check if the fragment is blending into another.
		if(fragmentID+1 < sklai->animFragNum){
			/** **/
			// Check if the animation has finished blending on its oldest state.
			if(sklai->animFrags[fragmentID].animBlendProgress >= sklai->animFrags[fragmentID].animBlendTime){
				// Since it's no longer being used in any state, it can be safely freed.
				sklafDelete(&sklai->animFrags[fragmentID]);
				// Shift all the following fragments over.
				size_t tempID = fragmentID+1;
				while(tempID < sklai->animFragNum){
					sklai->animFrags[tempID-1] = sklai->animFrags[tempID];
					++tempID;
				}
			}else{
				// Advance animator, update the blend and go to the next fragment.
				animAdvance(&sklai->animFrags[fragmentID].animator, &sklai->animFrags[fragmentID].currentAnim->animData, elapsedTimeMod);
				sklai->animFrags[fragmentID].animBlendProgress += elapsedTimeMod;
				++fragmentID;
			}
		}else{
			// Advance animator and exit.
			animAdvance(&sklai->animFrags[fragmentID].animator, &sklai->animFrags[fragmentID].currentAnim->animData, elapsedTimeMod);
			break;
		}

	}

}
void sklaiGenerateAnimState(sklAnimInstance *sklai, bone *skeletonState, const bone *baseState, const size_t boneNum, const float interpT){

	size_t i;
	size_t fragmentID = 0;
	float animInterpTBlend = 1.f;
	bone lastState[SKL_MAX_BONE_NUM];  // Temporarily holds states of bones from previous animation fragments.

	/* Loop through each animation fragment in order, blending between them and generating bone states. */
	while(fragmentID < sklai->animFragNum){
		/** **/
		// If the animation exists in the current state, generate render data for it.
		if(fragmentID+1 >= sklai->animFragNum ||
		   sklai->animFrags[fragmentID].animBlendProgress < sklai->animFrags[fragmentID].animBlendTime){

			size_t startFrame;
			size_t endFrame;
			float animInterpT;
			animGetRenderData(&sklai->animFrags[fragmentID].animator, &sklai->animFrags[fragmentID].currentAnim->animData, interpT,
			                  &startFrame, &endFrame, &animInterpT);

			/* Loop through each bone in the current animation fragment, running interpolation and writing to skeletonState. */
			for(i = 0; i < sklai->animFrags[fragmentID].currentAnim->boneNum; ++i){
				// Only continue if the current bone exists in the skeleton.
				if(sklai->animFrags[fragmentID].animBoneLookup[i] != (size_t)-1){

					// Interpolate between startFrame and endFrame, storing the result in interpBoneEnd.
					bone interpBoneEnd;
					boneInterpolate(&sklai->animFrags[fragmentID].currentAnim->frames[startFrame][i],
					                &sklai->animFrags[fragmentID].currentAnim->frames[endFrame][i],
					                animInterpT, &interpBoneEnd);

					// Blend from the previous animation fragment. animInterpTBlend is always 1.f for the first animation fragment.
					boneInterpolate(&lastState[sklai->animFrags[fragmentID].animBoneLookup[i]], &interpBoneEnd, animInterpTBlend,
					                &lastState[sklai->animFrags[fragmentID].animBoneLookup[i]]);

					if(sklai->animFrags[fragmentID].currentAnim->additive){
						// Add the changes in lastState to skeletonState if the animation is additive.
						boneTransformAppend(&skeletonState[sklai->animFrags[fragmentID].animBoneLookup[i]],
						                    &lastState[sklai->animFrags[fragmentID].animBoneLookup[i]],
						                    &skeletonState[sklai->animFrags[fragmentID].animBoneLookup[i]]);
					}else{
						// Set if the animation is not additive. Start from the
						// base state so custom transformations aren't lost.
						boneTransformAppend(&baseState[sklai->animFrags[fragmentID].animBoneLookup[i]],
						                    &lastState[sklai->animFrags[fragmentID].animBoneLookup[i]],
						                    &skeletonState[sklai->animFrags[fragmentID].animBoneLookup[i]]);
					}

				}
			}

			if(fragmentID+1 < sklai->animFragNum){
				// If this fragment marks the beginning of a valid blend, set animInterpTBlend for later.
				animInterpTBlend = sklai->animFrags[fragmentID].animBlendProgress / sklai->animFrags[fragmentID].animBlendTime;
			}

		}

		++fragmentID;

	}

}
signed char sklaiChangeAnim(sklAnimInstance *sklai, const skeleton *skl, sklAnim *anim, const size_t frame, const float blendTime){
	if(blendTime <= 0.f){
		sklafDelete(&sklai->animFrags[sklai->animFragNum-1]);
		return sklafInit(&sklai->animFrags[sklai->animFragNum-1], anim);
	}
	if(sklai->animFragNum >= sklai->animFragCapacity){
		size_t tempCapacity = sklai->animFragCapacity * 2;
		sklAnimFragment *tempBuffer = realloc(sklai->animFrags, tempCapacity);
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return 0;
		}
		sklai->animFragCapacity = tempCapacity;
		sklai->animFrags = tempBuffer;
	}
	if(!sklafInit(&sklai->animFrags[sklai->animFragNum], anim)){
		/** Memory allocation failure. **/
		return 0;
	}
	++sklai->animFragNum;
	return 1;
}

signed char skliInit(sklInstance *skli, const size_t animationCapacity){
	if(animationCapacity > 0){
		size_t i;
		skli->animations = malloc(animationCapacity*sizeof(sklAnimInstance));
		if(skli->animations == NULL){
			/** Memory allocation failure. **/
			return 0;
		}
		for(i = 0; i < animationCapacity; ++i){
			skli->animations[i].animFragNum = 0;
			skli->animations[i].animFragCapacity = 0;
			skli->animations[i].animFrags = NULL;
		}
	}else{
		skli->animations = NULL;
	}
	skli->timeMod = 1.f;
	skli->animationNum = 0;
	skli->animationCapacity = animationCapacity;
	return 1;
}
signed char skliLoad(sklInstance *skli, const char *prgPath, const char *filePath){

	/** stateNum is temporary. **/

	skliInit(skli, 1);

	sklAnim *skla = malloc(sizeof(sklAnim));
	skla->name = malloc(5*sizeof(char));
	memcpy(skla->name, "test\0", 5);
	//skla->additive = 1;
	skla->animData.desiredLoops = -1;
	skla->boneNum = 2;
	skla->bones = malloc(skla->boneNum*sizeof(char*));
	skla->bones[0] = malloc(5*sizeof(char));
	memcpy(skla->bones[0], "root\0", 5);
	skla->bones[1] = malloc(4*sizeof(char));
	memcpy(skla->bones[1], "top\0", 4);
	skla->animData.frameNum = 3;
	skla->frames = malloc(skla->animData.frameNum*sizeof(bone*));
	skla->animData.frameDelays = malloc(skla->animData.frameNum*sizeof(float));

	bone tempBoneRoot, tempBoneTop;
	boneInit(&tempBoneRoot); boneInit(&tempBoneTop);

	skla->frames[0] = malloc(skla->boneNum*sizeof(bone));
	skla->frames[0][0] = tempBoneRoot;
	skla->frames[0][1] = tempBoneTop;
	skla->animData.frameDelays[0] = 1000.f;

	skla->frames[1] = malloc(skla->boneNum*sizeof(bone));
	tempBoneRoot.orientation = quatNewEuler(-90.f*RADIAN_RATIO, 0.f, 0.f);
	skla->frames[1][0] = tempBoneRoot;
	tempBoneTop.position.y = 0.5f;
	skla->frames[1][1] = tempBoneTop;
	skla->animData.frameDelays[1] = 2000.f;

	skla->frames[2] = malloc(skla->boneNum*sizeof(bone));
	tempBoneRoot.position.y = 0.5f;
	skla->frames[2][0] = tempBoneRoot;
	tempBoneTop.position.y = 0.f;
	tempBoneTop.orientation = quatNewEuler(0.f, -90.f*RADIAN_RATIO, 0.f);
	skla->frames[2][1] = tempBoneTop;
	skla->animData.frameDelays[2] = 3000.f;

	sklaiInit(&skli->animations[0], skla);
	skli->animations[0].animFrags[0].animBoneLookup[0] = 0;
	skli->animations[0].animFrags[0].animBoneLookup[1] = 1;
	++skli->animationNum;

	return 1;

}
signed char skliStateCopy(sklInstance *o, sklInstance *c){

	size_t i;
	if(c->animationCapacity != o->animationCapacity || c->animations == NULL){

		/*
		** We need to allocate more or less memory so that
		** the memory allocated for both animation arrays match.
		*/
		sklAnimInstance *tempBuffer = malloc(o->animationCapacity*sizeof(sklAnimInstance));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return 0;
		}

		if(c->animations != NULL){

			/* Delete each animation instance that is no longer needed. */
			while(c->animationCapacity > o->animationCapacity){
				--c->animationCapacity;
				sklaiDelete(&c->animations[c->animationCapacity]);
			}

			for(i = 0; i < c->animationCapacity; ++i){
				tempBuffer[i] = c->animations[i];
			}

			free(c->animations);

		}

		/* Initialize each animation that needs to be created. */
		while(c->animationCapacity < o->animationCapacity){
			tempBuffer[c->animationCapacity].animFragNum = 0;
			tempBuffer[c->animationCapacity].animFragCapacity = 0;
			tempBuffer[c->animationCapacity].animFrags = NULL;
			++c->animationCapacity;
		}

		c->animations = tempBuffer;

	}

	/* Copy each sklAnimInstance over. */
	for(i = 0; i < o->animationNum; ++i){
		if(!sklaiStateCopy(&o->animations[i], &c->animations[i])){
			break;
		}
	}
	// Check if every sklAnimInstance was copied properly. If not, free and return.
	if(i < o->animationNum){
		sklaiDelete(&c->animations[i]);
		while(i > 0){
			--i;
			sklaiDelete(&c->animations[i]);
		}
		/** Memory allocation failure. **/
		free(c->animations);
		return 0;
	}
	c->animationNum = o->animationNum;

	c->timeMod = o->timeMod;
	return 1;

}
void skliAddAnimation(sklInstance *skli, const sklAnim *skla, const size_t frame){
	//
}
void skliChangeSkeleton(sklInstance *skli, const skeleton *skl){
	/** Re-calculate bone lookups for all animation fragments. **/
}
void skliGenerateDefaultState(const skeleton *skl, mat4 *state, const size_t boneID){

	// Apply parent transformations first if possible.
	if(boneID != skl->bones[boneID].parent){
		state[boneID] = state[skl->bones[boneID].parent];
	}else{
		mat4Identity(&state[boneID]);
	}

	// Apply default state transformations.
	mat4Translate(&state[boneID], skl->bones[boneID].defaultState.position.x,
	                              skl->bones[boneID].defaultState.position.y,
	                              skl->bones[boneID].defaultState.position.z);
	mat4Rotate(&state[boneID], &skl->bones[boneID].defaultState.orientation);
	mat4Scale(&state[boneID], skl->bones[boneID].defaultState.scale.x,
	                          skl->bones[boneID].defaultState.scale.y,
	                          skl->bones[boneID].defaultState.scale.z);

}
void skliGenerateBoneStateFromLocal(const bone *skeletonState, const skeleton *oskl, const skeleton *mskl, mat4 *state, const size_t boneID){

	// Apply parent transformations first if possible.
	// This optimization is only relevant if the bones are in local space.
	if(boneID != mskl->bones[boneID].parent){
		state[boneID] = state[mskl->bones[boneID].parent];
	}else{
		mat4Identity(&state[boneID]);
	}

	size_t animBone = sklFindBone(oskl, mskl->bones[boneID].name);
	if(animBone < oskl->boneNum){

		//quat inverseOrientation;

		// Apply animation transformations.
		mat4Translate(&state[boneID], skeletonState[animBone].position.x,
		                              skeletonState[animBone].position.y,
		                              skeletonState[animBone].position.z);
		mat4Rotate(&state[boneID], &skeletonState[animBone].orientation);
		mat4Scale(&state[boneID], skeletonState[animBone].scale.x,
		                          skeletonState[animBone].scale.y,
		                          skeletonState[animBone].scale.z);

		// Apply model's negative default state transformations.
		mat4Translate(&state[boneID], -mskl->bones[boneID].defaultState.position.x,
		                              -mskl->bones[boneID].defaultState.position.y,
		                              -mskl->bones[boneID].defaultState.position.z);
		/** Probably won't keep what's below. **/
		/*quatInverseR(&mskl->bones[boneID].defaultState.orientation, &inverseOrientation);
		mat4Rotate(&state[boneID], &inverseOrientation);
		mat4Scale(&state[boneID], 1.f/mskl->bones[boneID].defaultState.scale.x,
		                          1.f/mskl->bones[boneID].defaultState.scale.y,
		                          1.f/mskl->bones[boneID].defaultState.scale.z);*/

		// Apply object's default state transformations.
		// This makes more sense when diagrammed.
		mat4Translate(&state[boneID], oskl->bones[animBone].defaultState.position.x,
		                              oskl->bones[animBone].defaultState.position.y,
		                              oskl->bones[animBone].defaultState.position.z);
		mat4Rotate(&state[boneID], &oskl->bones[animBone].defaultState.orientation);
		/** Probably won't keep what's below. **/
		mat4Scale(&state[boneID], oskl->bones[animBone].defaultState.scale.x,//*oskl->bones[animBone].defaultState.scale.x,
		                          oskl->bones[animBone].defaultState.scale.y,//*oskl->bones[animBone].defaultState.scale.y,
		                          oskl->bones[animBone].defaultState.scale.z);//*oskl->bones[animBone].defaultState.scale.z);

	}

}
void skliGenerateBoneStateFromGlobal(const bone *skeletonState, const skeleton *oskl, const skeleton *mskl, mat4 *state, const size_t boneID){

	size_t animBone = sklFindBone(oskl, mskl->bones[boneID].name);
	if(animBone < oskl->boneNum){

		// Apply animation transformations.
		mat4SetTranslationMatrix(&state[boneID], skeletonState[animBone].position.x,
		                                         skeletonState[animBone].position.y,
		                                         skeletonState[animBone].position.z);
		mat4Rotate(&state[boneID], &skeletonState[animBone].orientation);
		mat4Scale(&state[boneID], skeletonState[animBone].scale.x,
		                          skeletonState[animBone].scale.y,
		                          skeletonState[animBone].scale.z);

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
}
