#include "skeleton.h"
#include "memoryManager.h"
#include "moduleSkeleton.h"
#include "constantsMath.h"
#include "mat4.h"
#include "inline.h"
#include "helpersFileIO.h"
#include "helpersMisc.h"
#include <string.h>
#include <stdio.h>

#define SKL_ANIM_BONE_START_CAPACITY 1
#define SKL_ANIM_FRAME_START_CAPACITY 1

#define SKL_ANIM_FRAGMENT_START_CAPACITY 1

static void sklDefragment(skeleton *skl){
	frameIndex_t i;
	skl->bones = memReallocateForced(skl->bones, skl->boneNum     *sizeof(sklNode));
	skl->name  = memReallocateForced(skl->name,  strlen(skl->name)*sizeof(char   ));
	for(i = 0; i < skl->boneNum; ++i){
		skl->bones[i].name =
		memReallocateForced(
			skl->bones[i].name,
			strlen(skl->bones[i].name)*sizeof(char)
		);
	}
	skl->name = memReallocateForced(skl->name, strlen(skl->name)*sizeof(char));
}

static return_t sklResizeToFit(skeleton *skl){
	/*bone *tempBuffer = realloc(skl->bones, skl->boneNum*sizeof(sklNode));
	if(tempBuffer == NULL){
		** Memory allocation failure. **
		sklDelete(skl);
		return 0;
	}*/
	/**
	*** Defrag until I create a new
	*** binary skeleton file format
	*** where the sizes are all known
	*** beforehand.
	**/
	sklDefragment(skl);
	return 1;
}
void sklInit(skeleton *skl){
	skl->name = NULL;
	skl->boneNum = 0;
	skl->bones = NULL;
}
return_t sklLoad(skeleton *skl, const char *prgPath, const char *filePath){

	char fullPath[FILE_MAX_PATH_LENGTH];
	size_t fileLength = strlen(filePath);

	FILE *sklInfo;

	sklInit(skl);

	fileGenerateFullPath(&fullPath[0], prgPath, strlen(prgPath), filePath, fileLength);
	sklInfo = fopen(&fullPath[0], "r");

	if(sklInfo != NULL){

		char lineFeed[1024];
		char *line;
		size_t lineLength;

		size_t parent = 0;
		int currentCommand = -1;     // The current multiline command type (-1 = none, >-1 = bone).
		fileLine_t currentLine = 0;  // Current file line being read.

		skl->bones = memAllocate(SKL_MAX_BONE_NUM*sizeof(sklNode));
		if(skl->bones == NULL){
			/** Memory allocation failure. **/
			return -1;
		}

		while(fileParseNextLine(sklInfo, lineFeed, sizeof(lineFeed), &line, &lineLength) && skl->boneNum < SKL_MAX_BONE_NUM){

			++currentLine;

			// Name
			if(lineLength >= 6 && strncmp(line, "name ", 5) == 0){
				if(currentCommand == -1){
					while(line[5] == ' ' || line[5] == '\t'){
						++line;
						--lineLength;
					}
					if(line[5] == '"' && line[lineLength-1] == '"'){
						++line;
						lineLength -= 2;
					}
					skl->name = memAllocate((lineLength-4) * sizeof(char));
					if(skl->name == NULL){
						/** Memory allocation failure. **/
						sklDelete(skl);
						fclose(sklInfo);
						return -1;
					}
					strncpy(skl->name, line+5, lineLength-5);
					skl->name[lineLength-5] = '\0';
				}else{
					printf("Error loading skeleton \"%s\": Name command at line %u does not belong inside a multiline command.\n", &fullPath[0], currentLine);
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
					printf("Error loading skeleton \"%s\": Invalid closing brace at line %u.\n", &fullPath[0], currentLine);
				}

			// New bone
			}else if(lineLength >= 24 && strncmp(line, "bone ", 5) == 0){

				char *token = strtok(line+5, " ");

				if(token != NULL){

					size_t nameLen = strlen(token);
					skl->bones[skl->boneNum].name = memAllocate((nameLen+1)*sizeof(char));
					if(skl->bones[skl->boneNum].name == NULL){
						/** Memory allocation failure. **/
						sklDelete(skl);
						fclose(sklInfo);
						return -1;
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
					printf("Error loading skeleton \"%s\": Bone command at line %u does not specify a name for the bone.\n", &fullPath[0], currentLine);
				}

			}

		}

		fclose(sklInfo);

	}else{
		printf("Error loading skeleton \"%s\": Could not open file.\n", &fullPath[0]);
		return 0;
	}

	// If no name was given, generate one based off the file path.
	if(skl->name == NULL || skl->name[0] == '\0'){
		if(skl->name != NULL){
			free(skl->name);
		}
		skl->name = memAllocate((fileLength+1)*sizeof(char));
		if(skl->name == NULL){
			/** Memory allocation failure. **/
			sklDelete(skl);
			return -1;
		}
		memcpy(skl->name, filePath, fileLength);
		skl->name[fileLength] = '\0';
	}

	return sklResizeToFit(skl);

}
return_t sklDefault(skeleton *skl){
	skl->name = memAllocate(8*sizeof(char));
	if(skl->name == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	skl->bones = memAllocate(sizeof(sklNode));
	if(skl->bones == NULL){
		/** Memory allocation failure. **/
		free(skl->name);
		return -1;
	}
	skl->bones[0].name = memAllocate(5*sizeof(char));
	if(skl->bones[0].name == NULL){
		/** Memory allocation failure. **/
		free(skl->bones);
		free(skl->name);
		return -1;
	}
	skl->name[0] = 'd';
	skl->name[1] = 'e';
	skl->name[2] = 'f';
	skl->name[3] = 'a';
	skl->name[4] = 'u';
	skl->name[5] = 'l';
	skl->name[6] = 't';
	skl->name[7] = '\0';
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
static return_t sklNodeCopy(const sklNode *onode, sklNode *cnode){
	if(onode->name != NULL){
		const size_t length = strlen(onode->name);
		cnode->name = memAllocate(length*sizeof(char));
		if(cnode->name == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		strncpy(cnode->name, onode->name, length);
	}else{
		cnode->name = NULL;
	}
	cnode->defaultState = onode->defaultState;
	cnode->parent = onode->parent;
	return 1;
}
return_t sklCopy(const skeleton *oskl, skeleton *cskl){
	boneIndex_t i;
	cskl->bones = memAllocate(oskl->boneNum*sizeof(sklNode));
	if(cskl->bones == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	if(oskl->name != NULL){
		i = strlen(oskl->name);
		cskl->name = memAllocate(i*sizeof(char));
		if(cskl->name == NULL){
			/** Memory allocation failure. **/
			free(cskl->bones);
			return -1;
		}
		strncpy(cskl->name, oskl->name, i);
	}else{
		cskl->name = NULL;
	}
	for(i = 0; i < oskl->boneNum; ++i){
		if(sklNodeCopy(&oskl->bones[i], &cskl->bones[i]) == -1){
			/** Memory allocation failure. **/
			break;
		}
	}
	if(i < oskl->boneNum){
		/** Memory allocation failure. **/
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
		return -1;
	}
	cskl->boneNum = oskl->boneNum;
	return 1;
}
boneIndex_t sklFindBone(const skeleton *skl, const boneIndex_t id, const char *name){
	/*boneIndex_t i;
	for(i = 0; i < skl->boneNum; ++i){
		if(skl->bones[i].name != NULL && strcmp(skl->bones[i].name, name) == 0){
			return i;
		}
	}
	return (boneIndex_t)-1;*/
	boneIndex_t i;
	if(id < skl->boneNum){
		i = id;
		if(strcmp(skl->bones[i].name, name) == 0){
			return i;
		}
		while(i > 0){
			--i;
			if(strcmp(skl->bones[i].name, name) == 0){
				return i;
			}
		}
		for(i = id; i < skl->boneNum; ++i){
			if(strcmp(skl->bones[i].name, name) == 0){
				return i;
			}
		}
	}else{
		for(i = 0; i < skl->boneNum; ++i){
			if(strcmp(skl->bones[i].name, name) == 0){
				return i;
			}
		}
	}
	return (boneIndex_t)-1;
}
void sklDelete(skeleton *skl){
	if(skl->name != NULL){
		memFree(skl->name);
	}
	if(skl->bones != NULL){
		boneIndex_t i;
		for(i = 0; i < skl->boneNum; ++i){
			if(skl->bones[i].name != NULL){
				memFree(skl->bones[i].name);
			}
		}
		memFree(skl->bones);
	}
}

void sklaInit(sklAnim *skla){
	skla->name = NULL;
	//skla->additive = 0;
	animDataInit(&skla->animData);
	skla->boneNum = 0;
	skla->bones = NULL;
	skla->frames = NULL;
}
static void sklaDefragment(sklAnim *skla){
	frameIndex_t i;
	skla->bones = memReallocateForced(skla->bones, skla->boneNum*sizeof(char *));
	skla->frames = memReallocateForced(skla->frames, skla->animData.frameNum*sizeof(bone *));
	skla->animData.frameDelays = memReallocateForced(skla->animData.frameDelays, skla->animData.frameNum*sizeof(float));
	skla->name = memReallocateForced(skla->name, strlen(skla->name)*sizeof(char));
	for(i = 0; i < skla->boneNum; ++i){
		skla->bones[i] = memReallocateForced(skla->bones[i], strlen(skla->bones[i])*sizeof(char));
	}
	for(i = 0; i < skla->animData.frameNum; ++i){
		skla->frames[i] = memReallocateForced(skla->frames[i], skla->boneNum*sizeof(bone));
	}
	skla->name = memReallocateForced(skla->name, strlen(skla->name)*sizeof(char));
}
static return_t sklaResizeToFit(sklAnim *skla, const size_t boneCapacity, const size_t frameCapacity){
	/*if(skla->boneNum != boneCapacity){
		skla->bones = realloc(skla->bones, skla->boneNum*sizeof(bone *));
		if(skla->bones == NULL){
			** Memory allocation failure. **
			sklaDelete(skla);
			return -1;
		}
	}
	if(skla->animData.frameNum != frameCapacity){
		skla->frames = realloc(skla->frames, skla->animData.frameNum*sizeof(char *));
		if(skla->frames == NULL){
			** Memory allocation failure. **
			sklaDelete(skla);
			return -1;
		}
		skla->animData.frameDelays = realloc(skla->animData.frameDelays, skla->animData.frameNum*sizeof(char *));
		if(skla->animData.frameDelays == NULL){
			** Memory allocation failure. **
			sklaDelete(skla);
			return -1;
		}
	}*/
	/**
	*** Defrag until I create a new
	*** binary skeletal animation
	*** file format where the sizes
	*** are all known beforehand.
	**/
	sklaDefragment(skla);
	return 1;
}
return_t sklaLoad(sklAnim *skla, const char *prgPath, const char *filePath){

	boneIndex_t boneCapacity = SKL_ANIM_BONE_START_CAPACITY;
	frameIndex_t frameCapacity = SKL_ANIM_FRAME_START_CAPACITY;

	char fullPath[FILE_MAX_PATH_LENGTH];
	const size_t fileLength = strlen(filePath);

	FILE *sklaInfo;

	sklaInit(skla);

	fileGenerateFullPath(&fullPath[0], prgPath, strlen(prgPath), filePath, fileLength);
	sklaInfo = fopen(&fullPath[0], "r");

	if(sklaInfo != NULL){

		char lineFeed[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;

		int currentCommand = -1;     // The current multiline command type (-1 = none, 0 = bones, 1 = frame).
		fileLine_t currentLine = 0;  // Current file line being read.

		skla->bones = memAllocate(SKL_ANIM_BONE_START_CAPACITY*sizeof(char *));
		if(skla->bones == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		skla->frames = memAllocate(SKL_ANIM_FRAME_START_CAPACITY*sizeof(bone *));
		if(skla->frames == NULL){
			/** Memory allocation failure. **/
			free(skla->bones);
			return -1;
		}
		skla->animData.frameDelays = memAllocate(SKL_ANIM_FRAME_START_CAPACITY*sizeof(float));
		if(skla->animData.frameDelays == NULL){
			/** Memory allocation failure. **/
			free(skla->animData.frameDelays);
			free(skla->bones);
			return -1;
		}

		while(fileParseNextLine(sklaInfo, lineFeed, sizeof(lineFeed), &line, &lineLength)){

			++currentLine;

			// Name
			if(lineLength >= 6 && strncmp(line, "name ", 5) == 0){
				if(currentCommand != 1){
					while(line[5] == ' ' || line[5] == '\t'){
						++line;
						--lineLength;
					}
					if(line[5] == '"' && line[lineLength-1] == '"'){
						++line;
						lineLength -= 2;
					}
					skla->name = memAllocate((lineLength-4) * sizeof(char));
					if(skla->name == NULL){
						/** Memory allocation failure. **/
						sklaDelete(skla);
						fclose(sklaInfo);
						return -1;
					}
					strncpy(skla->name, line+5, lineLength-5);
					skla->name[lineLength-5] = '\0';
				}else{
					printf("Error loading skeletal animation \"%s\": Name command at line %u does not belong inside a multiline command.\n", &fullPath[0], currentLine);
				}


			// Bone
			}else if(lineLength >= 6 && strncmp(line, "bone ", 5) == 0){
				if(currentCommand != 1){
					// Resize the bone name array if necessary.
					if(skla->boneNum == boneCapacity){
						boneCapacity *= 2;
						char **tempBuffer = memReallocateForced(skla->bones, boneCapacity*sizeof(char *));
						if(tempBuffer == NULL){
							/** Memory allocation failure. **/
							sklaDelete(skla);
							fclose(sklaInfo);
							return -1;
						}
						skla->bones = tempBuffer;
					}
					// Add the new bone name.
					if(line[5] == '"' && line[lineLength-1] == '"'){
						++line;
						lineLength -= 2;
					}
					skla->bones[skla->boneNum] = memAllocate((lineLength-5)*sizeof(char));
					if(skla->bones[skla->boneNum] == NULL){
						/** Memory allocation failure. **/
						sklaDelete(skla);
						fclose(sklaInfo);
						return -1;
					}
					strncpy(skla->bones[skla->boneNum], line+5, lineLength-5);
					skla->bones[skla->boneNum][lineLength-5] = '\0';
					++skla->boneNum;
					currentCommand = 0;
				}else{
					printf("Error loading skeletal animation \"%s\": Bone at line %u must be specified before any frames.\n", &fullPath[0], currentLine);
				}


			// Make the current animation loop.
			}else if(lineLength >= 6 && strncmp(line, "loop ", 5) == 0){
				if(currentCommand != 1){
					skla->animData.desiredLoops = strtol(line+5, NULL, 0);
				}else{
					printf("Error loading skeletal animation \"%s\": Loop command at line %u does not belong inside a multiline command.\n", &fullPath[0], currentLine);
				}


			// New frame
			}else if(lineLength >= 6 && strncmp(line, "frame", 5) == 0){
				if(strchr(line+5, '{')){
					boneIndex_t i;
					if(currentCommand == 0){
						// Make sure at least one bone has been loaded.
						if(skla->boneNum == 0){
							printf("Error loading skeletal animation \"%s\": Attempting to load frames when no bones have been specified. Aborting.\n", &fullPath[0]);
							sklaDelete(skla);
							fclose(sklaInfo);
							return 0;
						}
					}else if(currentCommand == 1){
						printf("Error loading skeletal animation \"%s\": Trying to start a multiline command at line %u while another is already in progress. "
						       "Closing the current command.\n", &fullPath[0], currentLine);
					}
					// Resize the bone name array if necessary.
					if(skla->animData.frameNum == frameCapacity){
						frameCapacity *= 2;
						bone **tempBuffer1 = memReallocateForced(skla->frames, frameCapacity*sizeof(bone *));
						if(tempBuffer1 == NULL){
							/** Memory allocation failure. **/
							sklaDelete(skla);
							fclose(sklaInfo);
							return -1;
						}
						skla->frames = tempBuffer1;
						float *tempBuffer2 = memReallocateForced(skla->animData.frameDelays, frameCapacity*sizeof(float));
						if(tempBuffer2 == NULL){
							/** Memory allocation failure. **/
							sklaDelete(skla);
							fclose(sklaInfo);
							return -1;
						}
						skla->animData.frameDelays = tempBuffer2;
					}
					// Allocate bones for the new frame.
					bone *tempBuffer = memAllocate(skla->boneNum*sizeof(bone));
					if(tempBuffer == NULL){
						/** Memory allocation failure. **/
						sklaDelete(skla);
						fclose(sklaInfo);
						return -1;
					}
					skla->frames[skla->animData.frameNum] = tempBuffer;
					// Initialize each bone.
					if(skla->animData.frameNum == 0){
						for(i = 0; i < skla->boneNum; ++i){
							boneInit(&skla->frames[skla->animData.frameNum][i]);
						}
					}else{
						for(i = 0; i < skla->boneNum; ++i){
							skla->frames[skla->animData.frameNum][i] = skla->frames[skla->animData.frameNum-1][i];
						}
					}
					++skla->animData.frameNum;
					currentCommand = 1;
				}else{
					// Worth it?
					printf("Error loading skeletal animation \"%s\": Frame command at line %u does not contain a brace.\n", &fullPath[0], currentLine);
				}


			// Frame duration
			}else if(lineLength >= 8 && strncmp(line, "length ", 7) == 0){
				if(currentCommand == 1){
					skla->animData.frameDelays[skla->animData.frameNum-1] = strtof(line+7, NULL);
					if(skla->animData.frameNum > 1){
						skla->animData.frameDelays[skla->animData.frameNum-1] += skla->animData.frameDelays[skla->animData.frameNum-2];
					}
				}else{
					printf("Error loading skeletal animation \"%s\": Frame sub-command \"length\" invoked on line %u without specifying a frame.\n", &fullPath[0], currentLine);
				}


			// Bone transform
			}else if(lineLength >= 29 && strncmp(line, "transform ", 10) == 0){
				if(currentCommand == 1){

					char *token = strtok(line+10, " ");

					if(token != NULL){

						boneIndex_t boneID = strtoul(token, NULL, 0);

						float data[3][3];  // Position, orientation (in Eulers) and scale
						size_t i, j;
						for(i = 0; i < 3; ++i){
							for(j = 0; j < 3; ++j){
								token = strtok(NULL, "/");
								data[i][j] = strtod(token, NULL);
							}
						}

						vec3Set(&skla->frames[skla->animData.frameNum-1][boneID].position, data[0][0], data[0][1], data[0][2]);
						quatSetEuler(&skla->frames[skla->animData.frameNum-1][boneID].orientation, data[1][0]*RADIAN_RATIO, data[1][1]*RADIAN_RATIO, data[1][2]*RADIAN_RATIO);
						vec3Set(&skla->frames[skla->animData.frameNum-1][boneID].scale, data[2][0], data[2][1], data[2][2]);

					}else{
						printf("Error loading skeletal animation \"%s\": Frame sub-command \"transform\" at line %u does not specify a bone ID.\n", &fullPath[0], currentLine);
					}

				}else{
					printf("Error loading skeletal animation \"%s\": Frame sub-command \"transform\" invoked on line %u without specifying a frame.\n", &fullPath[0], currentLine);
				}


			}

			if(lineLength > 0 && line[lineLength-1] == '}'){
				if(currentCommand == 1){
					currentCommand = -1;
				}else{
					printf("Error loading skeletal animation \"%s\": Stray brace on line %u.\n", &fullPath[0], currentLine);
				}
			}

		}

		fclose(sklaInfo);

	}else{
		printf("Error loading skeletal animation \"%s\": Could not open file.\n", &fullPath[0]);
		return 0;
	}

	// If no name was given, generate one based off the file path.
	if(skla->name == NULL || skla->name[0] == '\0'){
		if(skla->name != NULL){
			free(skla->name);
		}
		skla->name = memAllocate((fileLength+1)*sizeof(char));
		if(skla->name == NULL){
			/** Memory allocation failure. **/
			sklaDelete(skla);
			return -1;
		}
		memcpy(skla->name, filePath, fileLength);
		skla->name[fileLength] = '\0';
	}

	return sklaResizeToFit(skla, boneCapacity, frameCapacity);

}
/** TEMPORARY **/
return_t sklaLoadSMD(sklAnim *skla, const skeleton *skl, const char *prgPath, const char *filePath){
	/*
	** Temporary function by 8426THMY.
	*/
	//Create and initialize the animation!
	sklaInit(skla);

	//Find the full path for the model!
	size_t pathLen = strlen(prgPath);
	size_t fileLen = strlen(filePath);
	char *fullPath = malloc((pathLen+fileLen+1)*sizeof(char));
	if(fullPath == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	memcpy(fullPath, prgPath, pathLen);
	memcpy(fullPath+pathLen, filePath, fileLen);
	fullPath[pathLen+fileLen] = '\0';

	//Load the textureGroup!
	FILE *skeleAnimFile = fopen(fullPath, "r");
	if(skeleAnimFile != NULL){
		size_t tempCapacity = 1;
		//Temporarily stores bones.
		size_t tempBonesSize = 0;
		sklNode *tempBones = malloc(tempCapacity * sizeof(*tempBones));
		//This indicates what sort of data we're currently supposed to be reading.
		unsigned char dataType = 0;
		//This variable stores data specific to the type we're currently loading.
		unsigned int data = 0;

		char lineBuffer[1024];
		char *line;
		size_t lineLength;


		while(fileParseNextLine(skeleAnimFile, lineBuffer, sizeof(lineBuffer), &line, &lineLength)){
			if(dataType == 0){
				if(strcmp(line, "nodes") == 0){
					dataType = 1;
				}else if(strcmp(line, "skeleton") == 0){
					dataType = 2;

				//If this isn't the version number and the line isn't empty, it's something we can't handle!
				}else if(lineLength > 0 && strcmp(line, "version 1") != 0){
					printf("Error loading skeletal animtion!\n"
					       "Path: %s\n"
					       "Line: %s\n"
					       "Error: Unexpected identifier!\n", fullPath, line);
					sklaDelete(skla);
					free(tempBones);
					free(fullPath);
					return 0;
				}
			}else{
				if(strcmp(line, "end") == 0){
					//If we've finished identifying the skeleton's bones, shrink the vector!
					if(dataType == 1){
						tempBones = realloc(tempBones, tempBonesSize * sizeof(*tempBones));
						skla->bones = malloc(tempBonesSize * sizeof(*skla->bones));
						skla->boneNum = tempBonesSize;
						skla->animData.frameDelays = malloc(sizeof(*skla->animData.frameDelays));
						skla->frames = malloc(sizeof(*skla->frames));

						size_t i;
						for(i = 0; i < tempBonesSize; ++i){
							skla->bones[i] = tempBones[i].name;
						}

						tempCapacity = 1;

					//If we've finished loading the animation, shrink the vectors!
					}else if(dataType == 2){
						skla->animData.frameDelays = realloc(skla->animData.frameDelays, skla->animData.frameNum * sizeof(*skla->animData.frameDelays));
						skla->frames = realloc(skla->frames, skla->animData.frameNum * sizeof(*skla->frames));
						skla->animData.desiredLoops = -1;
					}

					dataType = 0;
					data = 0;
				}else{
					if(dataType == 1){
						char *tokPos = line;

						sklNode tempBone;

						//Get this bone's ID.
						size_t boneID = strtoul(tokPos, &tokPos, 10);
						//Make sure a bone with this ID actually exists.
						if(boneID == tempBonesSize){
							//Get the bone's name.
							size_t boneNameLength;
							getDelimitedString(tokPos, line + lineLength - tokPos, "\" ", &tokPos, &boneNameLength);
							tempBone.name = malloc(boneNameLength + 1);
							memcpy(tempBone.name, tokPos, boneNameLength);
							tempBone.name[boneNameLength] = '\0';

							//Get the ID of this bone's parent.
							tempBone.parent = strtoul(tokPos + boneNameLength + 1, NULL, 10);


							//If we're out of space, allocate some more!
							if(tempBonesSize >= tempCapacity){
								tempCapacity = tempBonesSize * 2;
								tempBones = realloc(tempBones, tempCapacity * sizeof(*tempBones));
							}
							//Add the bone to our vector!
							tempBones[tempBonesSize] = tempBone;
							++tempBonesSize;
						}else{
							printf("Error loading skeletal animtion!\n"
							       "Path: %s\n"
							       "Line: %s\n"
							       "Error: Found node %u when expecting node %u!\n",
							       fullPath, line, boneID, tempBonesSize);
							sklaDelete(skla);
							free(tempBones);
							free(fullPath);
							return 0;
						}
					}else if(dataType == 2){
						//If the line begins with time, get the frame's timestamp!
						if(memcmp(line, "time ", 5) == 0){
							unsigned int newTime = strtoul(&line[5], NULL, 10);
							if(newTime >= data){
								data = newTime;

								//Allocate memory for the new frame if we have to!
								if(skla->animData.frameNum >= tempCapacity){
									tempCapacity = skla->animData.frameNum * 2;
									skla->animData.frameDelays = realloc(skla->animData.frameDelays, tempCapacity * sizeof(*skla->animData.frameDelays));

									//Resize the frame arrays!
									skla->frames = realloc(skla->frames, tempCapacity * sizeof(*skla->frames));
								}

								skla->frames[skla->animData.frameNum] = malloc(skla->boneNum * sizeof(**skla->frames));
								skla->animData.frameDelays[skla->animData.frameNum] = (data + 1) * (1000.f / 24.f);
								++skla->animData.frameNum;
							}else{
								printf("Error loading skeletal animtion!\n"
								       "Path: %s\n"
								       "Line: %s\n"
								       "Error: Frame timestamps do not increment sequentially!\n",
								       fullPath, line);
								sklaDelete(skla);
								free(tempBones);
								free(fullPath);
								return 0;
							}

						//Otherwise, we're setting the bone's state!
						}else{
							char *tokPos = line;

							//Get this bone's ID.
							size_t boneID = strtoul(tokPos, &tokPos, 10);
							if(boneID < tempBonesSize){
								bone *currentState = &skla->frames[skla->animData.frameNum - 1][boneID];

								//Load the bone's position!
								float x = strtod(tokPos, &tokPos) * 0.05f;
								float y = strtod(tokPos, &tokPos) * 0.05f;
								float z = strtod(tokPos, &tokPos) * 0.05f;
								/*The Source Engine uses Z as its up axis, so we need to fix that with the root bone.
								if(boneID == 0){
									vec3InitSet(&currentState->pos, x, z, y);
								}else{*/
									vec3Set(&currentState->position, x, y, z);
								//}

								//The Source Engine uses Z as its up axis, so we need to fix that with the root bone.
								/*if(boneID == 0){
									vec3InitSet(&currentState->pos, x, z, y);
									x = strtod(tokPos, &tokPos) - 1.5707963267948966192313216916398f;
								}else{
									vec3InitSet(&currentState->pos, x, y, z);
									x = strtod(tokPos, &tokPos);
								}*/

								//Load the bone's rotation!
								x = strtod(tokPos, &tokPos);
								y = strtod(tokPos, &tokPos);
								z = strtod(tokPos, NULL);
								/*Same idea here.
								if(boneID == 0){
									quatInitEulerRad(&currentState->rot, x - 1.5707963267948966192313216916398f, -z, y);
								}else{*/
									quatSetEuler(&currentState->orientation, x, y, z);
								//}

								//Set the bone's scale!
								vec3Set(&currentState->scale, 1.f, 1.f, 1.f);

								//bone thing = skl->bones[boneID].defaultState;
								//boneStateInvert(&thing, &thing);
								//boneTransformAppendPosition(currentState, &thing, currentState);

							}else{
								printf("Error loading skeletal animtion!\n"
									   "Path: %s\n"
									   "Line: %s\n"
									   "Error: Found skeletal data for bone %u, which doesn't exist!\n",
									   fullPath, line, boneID);
								sklaDelete(skla);
								free(tempBones);
								free(fullPath);
								return 0;
							}
						}
					}
				}
			}
		}


		//We don't delete them properly because we store the bone names elsewhere.
		if(tempBones != NULL){
			free(tempBones);
		}


		fclose(skeleAnimFile);
		free(fullPath);


		size_t fileLen = strlen(filePath);
		skla->name = malloc((fileLen+1)*sizeof(char));
		if(skla->name == NULL){
			/** Memory allocation failure. **/
			sklaDelete(skla);
			return -1;
		}
		memcpy(skla->name, filePath, fileLen);
		skla->name[fileLen] = '\0';
	}else{
		printf("Unable to open skeletal animation file!\n"
		       "Path: %s\n", fullPath);
		free(fullPath);
		return 0;
	}


	return 1;
}
boneIndex_t sklaFindBone(const sklAnim *skla, const boneIndex_t id, const char *name){
	//skli->animations[i].animFrags[j].animBoneLookup[boneID] != (boneIndex_t)-1;
	/*boneIndex_t i;
	for(i = 0; i < skla->boneNum; ++i){
		if(strcmp(skla->bones[i], name) == 0){
			return i;
		}
	}
	return (boneIndex_t)-1;*/
	boneIndex_t i;
	if(id < skla->boneNum){
		i = id;
		if(strcmp(skla->bones[i], name) == 0){
			return i;
		}
		while(i > 0){
			--i;
			if(strcmp(skla->bones[i], name) == 0){
				return i;
			}
		}
		for(i = id; i < skla->boneNum; ++i){
			if(strcmp(skla->bones[i], name) == 0){
				return i;
			}
		}
	}else{
		for(i = 0; i < skla->boneNum; ++i){
			if(strcmp(skla->bones[i], name) == 0){
				return i;
			}
		}
	}
	return (boneIndex_t)-1;
}
void sklaDelete(sklAnim *skla){
	if(skla->name != NULL){
		memFree(skla->name);
	}
	if(skla->bones != NULL){
		boneIndex_t i;
		for(i = 0; i < skla->boneNum; ++i){
			if(skla->bones[i] != NULL){
				memFree(skla->bones[i]);
			}
		}
		memFree(skla->bones);
	}
	if(skla->frames != NULL){
		frameIndex_t i;
		for(i = 0; i < skla->animData.frameNum; ++i){
			if(skla->frames[i] != NULL){
				memFree(skla->frames[i]);
			}
		}
		memFree(skla->frames);
	}
	animDataDelete(&skla->animData);
}

static return_t sklafInit(sklAnimFragment *sklaf, sklAnim *anim, const skeleton *skl, const frameIndex_t frame){

	// Initialize animBoneLookup.
	/*uint_least8_t i;
	for(i = 0; i < anim->boneNum; ++i){
		** Create a proper lookup below. **
		sklaf->animBoneLookup[i] = i;
	}*/

	sklaf->animStartFrame = 0;
	sklaf->animEndFrame = 0;
	sklaf->animInterpT = 0.f;
	sklaf->animBlendTime = -1.f;
	sklaf->animBlendProgress = -1.f;
	sklaf->currentAnim = anim;
	animInstInit(&sklaf->animator);

	// Set the fragment to the current frame.
	if(frame < anim->animData.frameNum){
		sklaf->animator.currentFrame = frame;
		if(frame > 0){
			sklaf->animator.totalElapsedTime = anim->animData.frameDelays[frame-1];
		}
		if(frame == anim->animData.frameNum-1){
			sklaf->animator.nextFrame = 0;
		}else{
			sklaf->animator.nextFrame = frame+1;
		}
	}

	return 1;

}
static void sklafDelete(sklAnimFragment *sklaf){
	/*if(sklaf->animBoneLookup != NULL){
		free(sklaf->animBoneLookup);
	}*/
}

static void sklaiInit(sklAnimInstance *sklai){
	sklai->timeMod = 1.f;
	sklai->additive = 0;
	sklai->animFragNum = 0;
	sklai->animFragCapacity = 0;
	sklai->animFrags = NULL;
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
static __FORCE_INLINE__ return_t sklaiStateCopy(sklAnimInstance *osklai, sklAnimInstance *csklai){

	animIndex_t i;
	animIndex_t fragmentID = 0;

	while(csklai->animFragNum > osklai->animFragNum){
		--csklai->animFragNum;
		sklafDelete(&csklai->animFrags[csklai->animFragNum]);
	}

	if(csklai->animFragCapacity != osklai->animFragCapacity){
		sklAnimFragment *tempBuffer = malloc(osklai->animFragCapacity * sizeof(sklAnimFragment));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		for(i = 0; i < csklai->animFragCapacity; ++i){
			tempBuffer[i] = csklai->animFrags[i];
		}
		csklai->animFragCapacity = osklai->animFragCapacity;
		csklai->animFrags = tempBuffer;
	}

	/* Loop through each sklAnimFragment, copying everything. */
	while(fragmentID < osklai->animFragNum){
		if(fragmentID >= csklai->animFragNum){
			// New animation fragment.
			/*csklai->animFrags[fragmentID].animBoneLookup = malloc(osklai->animFrags[fragmentID].currentAnim->boneNum * sizeof(size_t));
			if(csklai->animFrags[fragmentID].animBoneLookup == NULL){
				** Memory allocation failure. **
				break;
			}*/
		}else if(csklai->animFrags[fragmentID].currentAnim->boneNum != osklai->animFrags[fragmentID].currentAnim->boneNum){
			// Bone lookup size has changed.
			/*free(csklai->animFrags[fragmentID].animBoneLookup);
			csklai->animFrags[fragmentID].animBoneLookup = malloc(osklai->animFrags[fragmentID].currentAnim->boneNum * sizeof(size_t));
			if(csklai->animFrags[fragmentID].animBoneLookup == NULL){
				** Memory allocation failure. **
				break;
			}*/
		}
		//memcpy(csklai->animFrags[fragmentID].animBoneLookup, osklai->animFrags[fragmentID].animBoneLookup, osklai->animFrags[fragmentID].currentAnim->boneNum * sizeof(size_t));
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
		return -1;
	}
	csklai->animFragNum = osklai->animFragNum;

	csklai->timeMod = osklai->timeMod;
	csklai->additive = osklai->additive;
	return 1;

}
static __FORCE_INLINE__ void sklaiUpdateFragments(sklAnimInstance *sklai, const float elapsedTime, const float interpT){

	const float elapsedTimeMod = elapsedTime * sklai->timeMod;

	/* Loop through each sklAnimFragment, updating them. */
	animIndex_t fragmentID = 0;
	/**sklAnimFragment *previous = NULL;**/
	while(fragmentID < sklai->animFragNum){

		// Check if the fragment is blending into another.
		if(fragmentID+1 < sklai->animFragNum){
			/** **/
			// Check if the animation has finished blending on its oldest state.
			if(sklai->animFrags[fragmentID].animBlendProgress >= sklai->animFrags[fragmentID].animBlendTime){
				// Since it's no longer being used in any state, it can be safely freed.
				sklafDelete(&sklai->animFrags[fragmentID]);
				// Shift all the following fragments over.
				animIndex_t tempID = fragmentID+1;
				while(tempID < sklai->animFragNum){
					sklai->animFrags[tempID-1] = sklai->animFrags[tempID];
					++tempID;
				}
				--sklai->animFragNum;
				--fragmentID;
			}else{
				// Advance animator, update the blend and go to the next fragment.
				animAdvance(&sklai->animFrags[fragmentID].animator, &sklai->animFrags[fragmentID].currentAnim->animData, elapsedTimeMod);
				animGetRenderData(&sklai->animFrags[fragmentID].animator, &sklai->animFrags[fragmentID].currentAnim->animData, interpT,
				                  &sklai->animFrags[fragmentID].animStartFrame,
				                  &sklai->animFrags[fragmentID].animEndFrame,
				                  &sklai->animFrags[fragmentID].animInterpT);
				sklai->animFrags[fragmentID].animBlendProgress += elapsedTimeMod;
				++fragmentID;
			}
		}else{
			// Advance animator and exit.
			animAdvance(&sklai->animFrags[fragmentID].animator, &sklai->animFrags[fragmentID].currentAnim->animData, elapsedTimeMod);
			animGetRenderData(&sklai->animFrags[fragmentID].animator, &sklai->animFrags[fragmentID].currentAnim->animData, interpT,
			                  &sklai->animFrags[fragmentID].animStartFrame,
			                  &sklai->animFrags[fragmentID].animEndFrame,
			                  &sklai->animFrags[fragmentID].animInterpT);
			break;
		}

		/**previous = &sklai->animFrags[fragmentID];**/

	}

}
/*void sklaiAnimate(sklAnimInstance *sklai, const float elapsedTime){

	const float elapsedTimeMod = elapsedTime * sklai->timeMod;

	* Loop through each sklAnimFragment, updating them. *
	size_t fragmentID = 0;
	while(fragmentID < sklai->animFragNum){

		// Check if the fragment is blending into another.
		if(fragmentID+1 < sklai->animFragNum){
			** **
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
				--sklai->animFragNum;
				--fragmentID;
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
	bone lastState[SKL_MAX_BONE_NUM];  // Temporarily holds the states of bones from previous animation fragments.

	* Loop through each animation fragment in order, blending between them and generating bone states. *
	while(fragmentID < sklai->animFragNum){

		size_t startFrame;
		size_t endFrame;
		float animInterpT;
		animGetRenderData(&sklai->animFrags[fragmentID].animator, &sklai->animFrags[fragmentID].currentAnim->animData, interpT,
		                  &startFrame, &endFrame, &animInterpT);

		* Loop through each bone in the current animation fragment, running interpolation and writing to skeletonState. *
		for(i = 0; i < sklai->animFrags[fragmentID].currentAnim->boneNum; ++i){
			// Only continue if the current bone exists in the skeleton.
			if(sklai->animFrags[fragmentID].animBoneLookup[i] != (size_t)-1){

				// Interpolate between startFrame and endFrame, storing the result in interpBoneEnd.
				bone interpBoneEnd;
				boneInterpolate(&sklai->animFrags[fragmentID].currentAnim->frames[startFrame][i],
				                &sklai->animFrags[fragmentID].currentAnim->frames[endFrame][i],
				                animInterpT, &interpBoneEnd);

				if(fragmentID > 0){
					// Blend from the previous animation fragment. animInterpTBlend is always 1.f for the first animation fragment.
					boneInterpolate(&lastState[sklai->animFrags[fragmentID-1].animBoneLookup[i]], &interpBoneEnd, animInterpTBlend,
					                &lastState[sklai->animFrags[fragmentID].animBoneLookup[i]]);
					//printf("%f %f %f %f\n", interpBoneEnd.orientation.w, interpBoneEnd.orientation.v.x, interpBoneEnd.orientation.v.y, interpBoneEnd.orientation.v.z);
				}else{
					lastState[sklai->animFrags[fragmentID].animBoneLookup[i]] = interpBoneEnd;
				}

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

		++fragmentID;

	}

}*/
static __FORCE_INLINE__ return_t sklaiChangeAnimation(sklAnimInstance *sklai, const skeleton *skl, sklAnim *anim, const frameIndex_t frame, const float blendTime){

	// Resize the animation fragment array if needed.
	if(blendTime > 0.f || sklai->animFragCapacity == 0){
		if(sklai->animFragNum == sklai->animFragCapacity){
			animIndex_t tempCapacity;
			if(sklai->animFragCapacity == 0){
				tempCapacity = SKL_ANIM_FRAGMENT_START_CAPACITY;
			}else{
				tempCapacity = sklai->animFragCapacity*2;
			}
			sklAnimFragment *tempBuffer = realloc(sklai->animFrags, tempCapacity*sizeof(sklAnimFragment));
			if(tempBuffer == NULL){
				/** Memory allocation failure. **/
				return -1;
			}
			sklai->animFragCapacity = tempCapacity;
			sklai->animFrags = tempBuffer;
		}
	}

	// If the animation isn't blending, replace the last fragment.
	if(sklai->animFragNum > 0){
		if(blendTime <= 0.f){
			--sklai->animFragNum;
			sklafDelete(&sklai->animFrags[sklai->animFragNum]);
		}else{
			sklai->animFrags[sklai->animFragNum-1].animBlendTime = blendTime;
			sklai->animFrags[sklai->animFragNum-1].animBlendProgress = 0.f;
		}
	}

	// Initialize the new animation fragment.
	if(sklafInit(&sklai->animFrags[sklai->animFragNum], anim, skl, frame) == -1){
		/** Memory allocation failure. **/
		return -1;
	}
	++sklai->animFragNum;
	return 1;

}

return_t skliInit(sklInstance *skli, skeleton *skl, const animIndex_t animationCapacity){
	if(animationCapacity > 0){
		animIndex_t i;
		skli->animations = malloc(animationCapacity*sizeof(sklAnimInstance));
		if(skli->animations == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		for(i = 0; i < animationCapacity; ++i){
			sklaiInit(&skli->animations[i]);
		}
	}else{
		skli->animations = NULL;
	}
	skli->skl = skl;
	skli->timeMod = 1.f;
	skli->animationNum = 0;
	skli->animationCapacity = animationCapacity;
	return 1;
}
return_t skliLoad(sklInstance *skli, const char *prgPath, const char *filePath, cVector *allSklAnimations){

	/** stateNum is temporary. **/

	//skliInit(skli, 1);

	/*sklAnim *skla = malloc(sizeof(sklAnim));
	skla->name = malloc(5*sizeof(char));
	memcpy(skla->name, "test\0", 5);
	//skla->additive = 1;
	skla->animData.desiredLoops = -1;
	skla->boneNum = 3;
	skla->bones = malloc(skla->boneNum*sizeof(char*));
	skla->bones[0] = malloc(11*sizeof(char));
	memcpy(skla->bones[0], "bip_pelvis\0", 5);
	skla->bones[1] = malloc(12*sizeof(char));
	memcpy(skla->bones[1], "bip_spine_0\0", 4);
	skla->bones[2] = malloc(9*sizeof(char));
	memcpy(skla->bones[2], "bip_neck\0", 4);
	skla->animData.frameNum = 3;
	skla->frames = malloc(skla->animData.frameNum*sizeof(bone*));
	skla->animData.frameDelays = malloc(skla->animData.frameNum*sizeof(float));

	bone tempBoneRoot, tempBoneTop, tempBoneHead;
	boneInit(&tempBoneRoot); boneInit(&tempBoneTop); boneInit(&tempBoneHead);
	//tempBoneRoot.position.y = -1.f;
	//tempBoneTop.position.y = -1.f;

	skla->frames[0] = malloc(skla->boneNum*sizeof(bone));
	skla->frames[0][0] = tempBoneRoot;
	skla->frames[0][1] = tempBoneTop;
	skla->frames[0][2] = tempBoneHead;
	skla->animData.frameDelays[0] = 1000.f;

	skla->frames[1] = malloc(skla->boneNum*sizeof(bone));
	tempBoneRoot.orientation = quatNewEuler(-90.f*RADIAN_RATIO, 0.f, 0.f);
	skla->frames[1][0] = tempBoneRoot;
	//tempBoneTop.position.y += 6.f;
	skla->frames[1][1] = tempBoneTop;
	skla->frames[1][2] = tempBoneHead;
	skla->animData.frameDelays[1] = 2000.f;

	skla->frames[2] = malloc(skla->boneNum*sizeof(bone));
	//tempBoneRoot.position.y += 0.5f;
	skla->frames[2][0] = tempBoneRoot;
	//tempBoneTop.position.y -= 3.f;
	tempBoneTop.orientation = quatNewEuler(0.f, -90.f*RADIAN_RATIO, 0.f);
	skla->frames[2][1] = tempBoneTop;
	tempBoneHead.orientation = quatNewEuler(0.f, 90.f*RADIAN_RATIO, 0.f);
	skla->frames[2][2] = tempBoneHead;
	skla->animData.frameDelays[2] = 3000.f;

	sklaiCreate(&skli->animations[0], skla);
	skli->animations[0].animFrags[0].animBoneLookup[0] = 0;
	skli->animations[0].animFrags[0].animBoneLookup[1] = 1;
	skli->animations[0].animFrags[0].animBoneLookup[2] = 5;
	++skli->animationNum;*/







	sklAnim *skla = moduleSkeletonAnimationAllocate();
	skla->name = memAllocate(5*sizeof(char));
	memcpy(skla->name, "test\0", 5);
	//skla->additive = 1;
	skla->animData.desiredLoops = -1;
	skla->boneNum = 2;
	skla->bones = memAllocate(skla->boneNum*sizeof(char*));
	skla->bones[0] = memAllocate(5*sizeof(char));
	memcpy(skla->bones[0], "root\0", 5);
	skla->bones[1] = memAllocate(4*sizeof(char));
	memcpy(skla->bones[1], "top\0", 4);
	skla->animData.frameNum = 3;
	skla->frames = memAllocate(skla->animData.frameNum*sizeof(bone*));
	skla->animData.frameDelays = memAllocate(skla->animData.frameNum*sizeof(float));

	bone tempBoneRoot, tempBoneTop;
	boneInit(&tempBoneRoot); boneInit(&tempBoneTop);
	//tempBoneRoot.position.y = -1.f;
	//tempBoneTop.position.y = -1.f;

	skla->frames[0] = memAllocate(skla->boneNum*sizeof(bone));
	skla->frames[0][0] = tempBoneRoot;
	skla->frames[0][1] = tempBoneTop;
	skla->animData.frameDelays[0] = 1000.f;

	skla->frames[1] = memAllocate(skla->boneNum*sizeof(bone));
	tempBoneRoot.orientation = quatNewEuler(-90.f*RADIAN_RATIO, 0.f, 0.f);
	skla->frames[1][0] = tempBoneRoot;
	tempBoneTop.position.y += 0.5f;
	skla->frames[1][1] = tempBoneTop;
	skla->animData.frameDelays[1] = 2000.f;

	skla->frames[2] = memAllocate(skla->boneNum*sizeof(bone));
	tempBoneRoot.position.y += 0.5f;
	skla->frames[2][0] = tempBoneRoot;
	tempBoneTop.position.y -= 0.5f;
	tempBoneTop.orientation = quatNewEuler(0.f, -90.f*RADIAN_RATIO, 0.f);
	skla->frames[2][1] = tempBoneTop;
	skla->animData.frameDelays[2] = 3000.f;

	skliChangeAnimation(skli, 0, skla, 0, 0.f);
	//skli->animations[0].animFrags[0].animBoneLookup = malloc(sizeof(size_t)*2);
	//skli->animations[0].animFrags[0].animBoneLookup[0] = 0;
	//skli->animations[0].animFrags[0].animBoneLookup[1] = 1;

	return 1;

}
void skliUpdateAnimations(sklInstance *skli, const float elapsedTime, const float interpT){
	const float elapsedTimeMod = elapsedTime * skli->timeMod;
	animIndex_t i;
	for(i = 0; i < skli->animationNum; ++i){
		sklaiUpdateFragments(&skli->animations[i], elapsedTimeMod, interpT);
	}
}
//void skliGenerateBoneState(sklInstance *skli, bone *boneState, const boneIndex_t boneID){
void skliGenerateBoneState(const sklInstance *skli, const boneIndex_t id, const char *name, bone *state){

	animIndex_t i, j;
	float animInterpTBlend = 1.f;
	bone baseState = *state;
	bone animationState;
	bone fragmentState;

	// Loop through each animation.
	for(i = 0; i < skli->animationNum; ++i){

		boneInit(&animationState);

		// Loop through each animation fragment, blending between them and generating an animation state.
		for(j = 0; j < skli->animations[i].animFragNum; ++j){

			// If the bone exists in the current animation fragment, generate a fragment state and add it to the animation state.
			//if(skli->animations[i].animFrags[j].animBoneLookup[boneID] != (boneIndex_t)-1){
			const boneIndex_t animBoneID = sklaFindBone(skli->animations[i].animFrags[j].currentAnim, id, name);
			if(animBoneID < skli->animations[i].animFrags[j].currentAnim->boneNum){

				// Interpolate between startFrame and endFrame, storing the result in fragmentState.
				boneInterpolate(&skli->animations[i].animFrags[j].currentAnim->frames[skli->animations[i].animFrags[j].animStartFrame][animBoneID],
				                &skli->animations[i].animFrags[j].currentAnim->frames[skli->animations[i].animFrags[j].animEndFrame][animBoneID],
				                skli->animations[i].animFrags[j].animInterpT, &fragmentState);

				// Blend from the previous animation fragment. animInterpTBlend is always 1.f for the first animation fragment.
				boneInterpolate(&animationState, &fragmentState, animInterpTBlend, &animationState);

			}else{
				boneInit(&fragmentState);
			}

			if(j+1 < skli->animations[i].animFragNum){
				// If this fragment marks the beginning of a valid blend, set animInterpTBlend for later.
				animInterpTBlend = skli->animations[i].animFrags[j].animBlendProgress / skli->animations[i].animFrags[j].animBlendTime;
			}else{
				break;
			}

		}

		if(skli->animations[i].additive){
			// Add the changes in lastState to skeletonState if the animation is additive.
			boneTransformCombine(state, &animationState, state);
		}else{
			// Set if the animation is not additive. Start from the
			// base state so custom transformations aren't lost.
			boneTransformCombine(&baseState, &animationState, state);
		}

	}

}
void skliSetAnimationType(sklInstance *skli, const animIndex_t slot, const flags_t additive){
	if(slot < skli->animationCapacity){
		skli->animations[slot].additive = additive;
	}
}
return_t skliChangeAnimation(sklInstance *skli, const animIndex_t slot, sklAnim *anim, const frameIndex_t frame, const float blendTime){
	return_t r;
	animIndex_t newAnimationNum = skli->animationNum;
	if(slot >= skli->animationCapacity){
		/** Should you be able to change animationCapacity? **/
		sklAnimInstance *tempBuffer = realloc(skli->animations, (slot+1)*sizeof(sklAnimInstance));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		skli->animations = tempBuffer;
		while(skli->animationCapacity <= slot){
			sklaiInit(&skli->animations[skli->animationCapacity]);
			++skli->animationCapacity;
		}
	}
	// Check whether a new animation or just a new fragment is being added.
	if(skli->animations[slot].animFragNum == 0){
		// If it's a new animation, increase animationNum.
		++newAnimationNum;
	}
	r = sklaiChangeAnimation(&skli->animations[slot], skli->skl, anim, frame, blendTime);
	if(r > 0){
		// Only increase animationNum if a new animation was added.
		skli->animationNum = newAnimationNum;
	}
	return r;
}
void skliClearAnimation(sklInstance *skli, const animIndex_t slot){
	if(slot < skli->animationCapacity){
		if(skli->animations[slot].animFrags != NULL && skli->animations[slot].animFragNum > 0){
			/* Loop through each sklAnimFragment, deleting them. */
			do {
				--skli->animations[slot].animFragNum;
				sklafDelete(&skli->animations[slot].animFrags[skli->animations[slot].animFragNum]);
			} while(skli->animations[slot].animFragNum > 0);
			--skli->animationNum;
		}
		sklaiInit(&skli->animations[slot]);
	}
}
/*return_t skliSetAnimation(sklInstance *skli, const size_t slot, sklAnim *anim){
	return_t r = -1;
	if(slot >= skli->animationCapacity){
		** Should you be able to change animationCapacity? **
		sklAnimInstance *tempBuffer = realloc(skli->animations, (slot+1)*sizeof(sklAnimInstance));
		if(tempBuffer == NULL){
			** Memory allocation failure. **
			return r;
		}
		skli->animations = tempBuffer;
		while(skli->animationCapacity < slot){
			sklaiInit(&skli->animations[skli->animationCapacity]);
			++skli->animationCapacity;
		}
	}
	r = sklaiCreate(&skli->animations[slot], anim);
	if(r > 0){
		++skli->animationNum;
	}
	return r;
}
return_t skliClearAnimation(sklInstance *skli, const size_t slot){
	if(slot >= skli->animationCapacity){
		return 0;
	}
	sklaiDelete(&skli->animations[slot]);
	sklaiInit(&skli->animations[slot]);
	return 1;
}*/
return_t skliStateCopy(sklInstance *o, sklInstance *c){

	animIndex_t i;
	if(c->animationCapacity != o->animationCapacity || c->animations == NULL){

		/*
		** We need to allocate more or less memory so that
		** the memory allocated for both animation arrays match.
		*/
		sklAnimInstance *tempBuffer = malloc(o->animationCapacity*sizeof(sklAnimInstance));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
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
		if(sklaiStateCopy(&o->animations[i], &c->animations[i]) == -1){
			/** Memory allocation failure. **/
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
		return -1;
	}
	c->animationNum = o->animationNum;

	c->skl = o->skl;
	c->timeMod = o->timeMod;
	return 1;

}
void skliAddAnimation(sklInstance *skli, const sklAnim *skla, const frameIndex_t frame){
	//
}
void skliChangeSkeleton(sklInstance *skli, const skeleton *skl){
	/** Re-calculate bone lookups for all animation fragments. **/
}
void skliGenerateDefaultState(const skeleton *skl, mat4 *state, const boneIndex_t boneID){

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
void skliGenerateBoneStateFromLocal(const bone *skeletonState, const skeleton *oskl, const skeleton *mskl, mat4 *state, const boneIndex_t boneID){

	const boneIndex_t animBone = sklFindBone(oskl, boneID, mskl->bones[boneID].name);

	// Apply parent transformations first if possible.
	// This optimization is only relevant if the bones are in local space.
	if(boneID != mskl->bones[boneID].parent){
		state[boneID] = state[mskl->bones[boneID].parent];
	}else{
		mat4Identity(&state[boneID]);
	}

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
void skliGenerateBoneStateFromGlobal(const bone *skeletonState, const skeleton *oskl, const skeleton *mskl, mat4 *state, const boneIndex_t boneID){

	const boneIndex_t animBone = sklFindBone(oskl, boneID, mskl->bones[boneID].name);
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
		animIndex_t i;
		for(i = 0; i < skli->animationNum; ++i){
			sklaiDelete(&skli->animations[i]);
		}
		free(skli->animations);
	}
}
