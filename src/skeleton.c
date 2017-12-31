#include "skeleton.h"
#include <string.h>
#include <stdio.h>

#define RADIAN_RATIO 0.017453292  // = PI / 180, used for converting degrees to radians

#define BONE_START_CAPACITY 1
#define ANIM_FRAGMENT_START_CAPACITY 1

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

static unsigned char sklafInit(sklAnimFragment *sklaf, sklAnim *anim){
	sklaf->animBoneLookup = malloc(anim->boneNum * sizeof(sklBone *));
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

static unsigned char sklaiInit(sklAnimInstance *sklai, const skeleton *skl, sklAnim *anim){
	sklai->animFrags = malloc(ANIM_FRAGMENT_START_CAPACITY * sizeof(sklAnimFragment));
	if(sklai->animFrags == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	sklai->animState = malloc(skl->boneNum * sizeof(sklBone));
	if(sklai->animState == NULL){
		/** Memory allocation failure. **/
		free(sklai->animFrags);
		return 0;
	}
	if(!sklafInit(&sklai->animFrags[0], anim)){
		/** Memory allocation failure. **/
		free(sklai->animState);
		free(sklai->animFrags);
		return 0;
	}
	sklai->animFragNum = 1;
	sklai->animFragCapacity = ANIM_FRAGMENT_START_CAPACITY;
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
	if(sklai->animState != NULL){
		free(sklai->animState);
	}
}
static unsigned char sklaiStateCopy(const sklAnimInstance *osklai, sklAnimInstance *csklai, const skeleton *oskl, const skeleton *cskl){

	if(oskl != NULL && (cskl == NULL || cskl->boneNum != oskl->boneNum)){
		if(csklai->animState != NULL){
			free(csklai->animState);
		}
		csklai->animState = malloc(oskl->boneNum * sizeof(sklBone));
		if(csklai->animState == NULL){
			/** Memory allocation failure. **/
			return 0;
		}
	}

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
			csklai->animFrags[fragmentID].animBoneLookup = malloc(osklai->animFrags[fragmentID].currentAnim->boneNum * sizeof(sklBone *));
			if(csklai->animFrags[fragmentID].animBoneLookup == NULL){
				/** Memory allocation failure. **/
				break;
			}
		}else if(csklai->animFrags[fragmentID].currentAnim->boneNum != osklai->animFrags[fragmentID].currentAnim->boneNum){
			// Bone lookup size has changed.
			free(csklai->animFrags[fragmentID].animBoneLookup);
			csklai->animFrags[fragmentID].animBoneLookup = malloc(osklai->animFrags[fragmentID].currentAnim->boneNum * sizeof(sklBone *));
			if(csklai->animFrags[fragmentID].animBoneLookup == NULL){
				/** Memory allocation failure. **/
				break;
			}
		}
		for(i = 0; i < osklai->animFrags[fragmentID].currentAnim->boneNum; ++i){
			/** This is pretty awful. **/
			csklai->animFrags[fragmentID].animBoneLookup[i] = osklai->animFrags[fragmentID].animBoneLookup[i] - osklai->animState + csklai->animState;
		}
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

	for(i = 0; i < oskl->boneNum; ++i){
		csklai->animState[i] = osklai->animState[i];
	}

	return 1;

}
static void sklaiAnimate(sklAnimInstance *sklai, const float elapsedTime){

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
				animAdvance(&sklai->animFrags[fragmentID].animator, &sklai->animFrags[fragmentID].currentAnim->animData, elapsedTime);
				sklai->animFrags[fragmentID].animBlendProgress += elapsedTime;
				++fragmentID;
			}
		}else{
			// Advance animator and exit.
			animAdvance(&sklai->animFrags[fragmentID].animator, &sklai->animFrags[fragmentID].currentAnim->animData, elapsedTime);
			break;
		}

	}

}
static void sklaiGenerateAnimState(sklAnimInstance *sklai, const float interpT){

	float blendTime = 1.f;
	float blendProgress = 1.f;

	/* Loop through each animation fragment in order, blending between them and generating bone states. */
	size_t fragmentID = 0;
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

			/* Loop through each bone in the current animation fragment, running interpolation and writing to animState. */
			size_t i;
			for(i = 0; i < sklai->animFrags[fragmentID].currentAnim->boneNum; ++i){
				// Only continue if the current bone exists in the skeleton.
				if(sklai->animFrags[fragmentID].animBoneLookup[i] != NULL){

					// Interpolate between startFrame and endFrame, storing the result in interpBoneEnd.
					sklBone interpBoneEnd;
					boneInterpolate(&sklai->animFrags[fragmentID].currentAnim->frames[startFrame][i],
					                &sklai->animFrags[fragmentID].currentAnim->frames[endFrame][i],
					                animInterpT, &interpBoneEnd);

					// Interpolate between animState and interpBoneEnd.
					sklBone *interpBoneStart = sklai->animFrags[fragmentID].animBoneLookup[i];
					const float animInterpTBlend = blendProgress / blendTime;
					boneInterpolate(interpBoneStart, &interpBoneEnd, animInterpTBlend, sklai->animFrags[fragmentID].animBoneLookup[i]);

				}
			}

			if(fragmentID+1 < sklai->animFragNum){
				// If this fragment marks the beginning of a valid blend, set blendTime and blendProgress for later.
				blendTime = sklai->animFrags[fragmentID].animBlendTime;
				blendProgress = sklai->animFrags[fragmentID].animBlendProgress;
			}

		}

		++fragmentID;

	}

}
unsigned char sklaiChangeAnim(sklAnimInstance *sklai, const skeleton *skl, sklAnim *anim, const size_t frame, const float blendTime){
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

unsigned char skliInit(sklInstance *skli, skeleton *skl, const size_t animationCapacity){

	size_t i;

	if(animationCapacity > 0){
		skli->animations = malloc(animationCapacity*sizeof(sklAnimInstance));
		if(skli->animations == NULL){
			/** Memory allocation failure. **/
			return 0;
		}
		for(i = 0; i < animationCapacity; ++i){
			skli->animations[i].animFragNum = 0;
			skli->animations[i].animFragCapacity = 0;
			skli->animations[i].animFrags = NULL;
			skli->animations[i].animState = NULL;
		}
	}else{
		skli->animations = NULL;
	}

	if(skl != NULL){
		/** Remove this NULL check. Also remove it from skliStateCopy. **/
		skli->customState = malloc(skl->boneNum*sizeof(sklBone));
		if(skli->customState == NULL){
			/** Memory allocation failure. **/
			free(skli->animations);
			return 0;
		}
		for(i = 0; i < skl->boneNum; ++i){
			boneInit(&skli->customState[i]);
		}
	}else{
		skli->customState = NULL;
	}

	skli->skl = skl;
	skli->timeMod = 1.f;
	skli->animationNum = 0;
	skli->animationCapacity = animationCapacity;

	return 1;
}
unsigned char skliLoad(sklInstance *skli, const char *prgPath, const char *filePath){

	/** stateNum is temporary. **/

	skeleton *skl = malloc(sizeof(skeleton));
	sklLoad(skl, prgPath, filePath);

	skliInit(skli, skl, 1);

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

	sklaiInit(&skli->animations[0], skl, skla);
	skli->animations[0].animFrags[0].animBoneLookup[0] = &skli->animations[0].animState[0];
	skli->animations[0].animFrags[0].animBoneLookup[1] = &skli->animations[0].animState[1];
	++skli->animationNum;

	return 1;

}
unsigned char skliStateCopy(const sklInstance *o, sklInstance *c){

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
			tempBuffer[c->animationCapacity].animState = NULL;
			++c->animationCapacity;
		}

		c->animations = tempBuffer;

	}

	/* Copy each sklAnimInstance over. */
	for(i = 0; i < o->animationNum; ++i){
		if(!sklaiStateCopy(&o->animations[i], &c->animations[i], o->skl, c->skl)){
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

	if(o->skl != NULL){
		if(c->skl == NULL || c->skl->boneNum != o->skl->boneNum){
			/*
			** We need to allocate more or less memory so that
			** the memory allocated for both custom states match.
			*/
			sklBone *tempBuffer = malloc(o->skl->boneNum*sizeof(sklBone));
			if(tempBuffer == NULL){
				/** Memory allocation failure. **/
				return 0;
			}
			if(c->customState != NULL){
				free(c->customState);
			}
			c->customState = tempBuffer;
		}
		memcpy(c->customState, o->customState, o->skl->boneNum*sizeof(sklBone));
	}else{
		c->customState = NULL;
	}
	c->skl = o->skl;

	c->timeMod = o->timeMod;
	return 1;

}
void skliAddAnimation(sklInstance *skli, const sklAnim *skla, const size_t frame){
	//
}
void skliChangeSkeleton(sklInstance *skli, const skeleton *skl){
	/** Re-calculate bone lookups for all animation fragments. **/
}
void skliAnimate(sklInstance *skli, const float elapsedTime){
	const float elapsedTimeMod = elapsedTime * skli->timeMod;
	size_t i;
	for(i = 0; i < skli->animationNum; ++i){
		sklaiAnimate(&skli->animations[i], elapsedTimeMod);
	}
}
void skliGenerateAnimStates(sklInstance *skli, const float interpT){
	size_t i;
	for(i = 0; i < skli->animationNum; ++i){
		sklaiGenerateAnimState(&skli->animations[i], interpT);
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
		for(i = 0; i < skli->animationNum; ++i){
			mat4Translate(&state[bone], skli->animations[i].animState[animBone].position.x,
			                            skli->animations[i].animState[animBone].position.y,
			                            skli->animations[i].animState[animBone].position.z);
			mat4Rotate(&state[bone], &skli->animations[i].animState[animBone].orientation);
			mat4Scale(&state[bone], skli->animations[i].animState[animBone].scale.x,
			                        skli->animations[i].animState[animBone].scale.y,
			                        skli->animations[i].animState[animBone].scale.z);
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
}
