#include "skeleton.h"
#include "memoryManager.h"
#include "moduleSkeleton.h"
#include "constantsMath.h"
#include "helpersFileIO.h"
/** TEMPORARY **/
#include "helpersMisc.h"
/** TEMPORARY **/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SKELETON_RESOURCE_DIRECTORY_STRING FILE_PATH_RESOURCE_DIRECTORY_SHARED"Resources"FILE_PATH_DELIMITER_STRING"Skeletons"FILE_PATH_DELIMITER_STRING
#define SKELETON_RESOURCE_DIRECTORY_LENGTH 22

#define SKELETON_ANIMATION_RESOURCE_DIRECTORY_STRING FILE_PATH_RESOURCE_DIRECTORY_SHARED"Resources"FILE_PATH_DELIMITER_STRING"Skeletons"FILE_PATH_DELIMITER_STRING"Animations"FILE_PATH_DELIMITER_STRING
#define SKELETON_ANIMATION_RESOURCE_DIRECTORY_LENGTH 33

#define SKELETON_ANIM_BONE_START_CAPACITY 1
#define SKELETON_ANIM_FRAME_START_CAPACITY 1

#define SKELETON_ANIM_FRAGMENT_START_CAPACITY 1

// Default skeleton.
static sklNode g_sklNodeDefault = {
	.name = "root",
	.parent = 0,
	.defaultState = {
		.position = {.x = 0.f, .y = 0.f, .z = 0.f},
		.orientation = {.w = 1.f, .v = {.x = 0.f, .y = 0.f, .z = 0.f}},
		.scale = {.x = 1.f, .y = 1.f, .z = 1.f}
	}
};
skeleton g_sklDefault = {
	.name = "default",
	.boneNum = 1,
	.bones = &g_sklNodeDefault
};

static void sklDefragment(skeleton *const __RESTRICT__ skl, const char *const __RESTRICT__ resource, const size_t length){

	frameIndex_t i;
	size_t bytes = 0;
	char *namePtr;

	// Count name string lengths.
	for(i = 0; i < skl->boneNum; ++i){
		bytes += strlen(skl->bones[i].name)+1;
	}
	skl->bones = memReallocate(skl->bones, skl->boneNum*sizeof(sklNode) + bytes + length + 1);

	namePtr = (char *)&skl->bones[skl->boneNum];
	for(i = 0; i < skl->boneNum; ++i){
		bytes = strlen(skl->bones[i].name)+1;
		memcpy(namePtr, skl->bones[i].name, bytes);
		memFree(skl->bones[i].name);
		skl->bones[i].name = namePtr;
		namePtr += bytes;
	}

	memcpy(namePtr, resource, length);
	namePtr[length] = '\0';
	skl->name = namePtr;

}

void sklInit(skeleton *const __RESTRICT__ skl){
	skl->name = NULL;
	skl->boneNum = 0;
	skl->bones = NULL;
}
return_t sklLoad(skeleton *const __RESTRICT__ skl, const char *const __RESTRICT__ filePath, const size_t filePathLength){

	char fullPath[FILE_MAX_PATH_LENGTH];

	FILE *sklInfo;

	sklInit(skl);

	fileGenerateFullPath(fullPath, SKELETON_RESOURCE_DIRECTORY_STRING, SKELETON_RESOURCE_DIRECTORY_LENGTH, filePath, filePathLength);
	sklInfo = fopen(fullPath, "r");

	if(sklInfo != NULL){

		char lineFeed[1024];
		char *line;
		size_t lineLength;

		size_t parent = 0;
		int currentCommand = -1;     // The current multiline command type (-1 = none, >-1 = bone).
		fileLine_t currentLine = 0;  // Current file line being read.

		skl->bones = memAllocate(SKELETON_MAX_BONE_NUM*sizeof(sklNode));
		if(skl->bones == NULL){
			/** Memory allocation failure. **/
			return -1;
		}

		while(fileParseNextLine(sklInfo, lineFeed, sizeof(lineFeed), &line, &lineLength) && skl->boneNum < SKELETON_MAX_BONE_NUM){

			++currentLine;

			// Close current multiline command
			if(lineLength > 0 && line[0] == '}'){
				if(currentCommand > -1){
					parent = skl->bones[parent].parent;
					if(skl->boneNum == parent){
						// Last bone was reached (equal number of opening and closing braces)
						break;
					}
					--currentCommand;
				}else{
					printf("Error loading skeleton \"%s\": Invalid closing brace at line %u.\n", fullPath, currentLine);
				}

			// New bone
			}else if(lineLength >= 24 && strncmp(line, "bone ", 5) == 0){

				const char *token = strtok(line+5, " ");

				if(token != NULL){

					size_t i, j;
					float data[3][3];  // Position, orientation (in Eulers) and scale

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

					for(i = 0; i < 3; ++i){
						for(j = 0; j < 3; ++j){
							token = strtok(NULL, "/");
							data[i][j] = strtod(token, NULL);
						}
					}

					skl->bones[skl->boneNum].defaultState.position = vec3New(data[0][0], data[0][1], data[0][2]);
					skl->bones[skl->boneNum].defaultState.orientation = quatNewEuler(data[1][0]*RADIAN_RATIO, data[1][1]*RADIAN_RATIO, data[1][2]*RADIAN_RATIO);
					skl->bones[skl->boneNum].defaultState.scale = vec3New(data[2][0], data[2][1], data[2][2]);

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
					printf("Error loading skeleton \"%s\": Bone command at line %u does not specify a name for the bone.\n", fullPath, currentLine);
				}

			}

		}

		fclose(sklInfo);

	}else{
		printf("Error loading skeleton \"%s\": Could not open file.\n", fullPath);
		return 0;
	}

	/**
	***
	***
	***
	***
	***
	**/
	sklDefragment(skl, filePath, filePathLength);

	// Generate a name based off the file path.
	/**skl->name = fileGenerateResourceName(filePath, filePathLength, skl->bones, totalBytes);
	if(skl->name == NULL){
		** Memory allocation failure. **
		sklDelete(skl);
		return -1;
	}**/

	return 1;

}
boneIndex_t sklFindBone(const skeleton *const __RESTRICT__ skl, const boneIndex_t id, const char *const __RESTRICT__ name){
	/** boneIndex_t i;
	for(i = 0; i < skl->boneNum; ++i){
		if(skl->bones[i].name != NULL && strcmp(skl->bones[i].name, name) == 0){
			return i;
		}
	}
	return (boneIndex_t)-1; **/
	boneIndex_t i;
	const sklNode *n;
	if(id < skl->boneNum){
		i = id;
		n = &skl->bones[id];
		if(strcmp(n->name, name) == 0){
			return i;
		}
		while(i > 0){
			--i;
			--n;
			if(strcmp(n->name, name) == 0){
				return i;
			}
		}
		for(i = id, n = &skl->bones[id]; i < skl->boneNum; ++i, ++n){
			if(strcmp(n->name, name) == 0){
				return i;
			}
		}
	}else{
		for(i = 0, n = skl->bones; i < skl->boneNum; ++i, ++n){
			if(strcmp(n->name, name) == 0){
				return i;
			}
		}
	}
	return (boneIndex_t)-1;
}
void sklDelete(skeleton *const __RESTRICT__ skl){
	if(skl->bones != NULL && skl->bones != &g_sklNodeDefault){
		// This also frees the name.
		memFree(skl->bones);
	}
}

void sklaInit(sklAnim *const __RESTRICT__ skla){
	skla->name = NULL;
	//skla->additive = 0;
	animDataInit(&skla->animData);
	skla->boneNum = 0;
	skla->bones = NULL;
	skla->frames = NULL;
}
static void sklaDefragment(sklAnim *const __RESTRICT__ skla, const char *const __RESTRICT__ resource, const size_t length){

	frameIndex_t i;
	size_t bytes = 0;
	char *namePtr;

	for(i = 0; i < skla->boneNum; ++i){
		bytes += strlen(skla->bones[i])+1;
	}

	// Allocate bones array, bone names arrays, frames array, frame transform arrays and frameDelays array.
	skla->bones = memReallocate(skla->bones,
		skla->boneNum*sizeof(char *) + bytes +
		skla->animData.frameNum*(sizeof(transform *) + skla->boneNum*sizeof(transform) + sizeof(float)) +
		length + 1
	);

	namePtr = (char *)&skla->bones[skla->boneNum];
	for(i = 0; i < skla->boneNum; ++i){
		bytes = strlen(skla->bones[i])+1;
		memcpy(namePtr, skla->bones[i], bytes);
		memFree(skla->bones[i]);
		skla->bones[i] = namePtr;
		namePtr += bytes;
	}

	memcpy(namePtr, skla->frames, skla->animData.frameNum*sizeof(transform *));
	memFree(skla->frames);
	skla->frames = (transform **)namePtr;
	namePtr += skla->animData.frameNum*sizeof(transform *);
	bytes = skla->boneNum*sizeof(transform);
	for(i = 0; i < skla->animData.frameNum; ++i){
		memcpy(namePtr, skla->frames[i], bytes);
		memFree(skla->frames[i]);
		skla->frames[i] = (transform *)namePtr;
		namePtr += bytes;
	}

	memcpy(namePtr, skla->animData.frameDelays, skla->animData.frameNum*sizeof(float));
	memFree(skla->animData.frameDelays);
	skla->animData.frameDelays = (float *)namePtr;
	namePtr += skla->animData.frameNum*sizeof(float);

	memcpy(namePtr, resource, length);
	namePtr[length] = '\0';
	skla->name = namePtr;

}
return_t sklaLoad(sklAnim *const __RESTRICT__ skla, const char *const __RESTRICT__ filePath, const size_t filePathLength){

	boneIndex_t boneCapacity = SKELETON_ANIM_BONE_START_CAPACITY;
	frameIndex_t frameCapacity = SKELETON_ANIM_FRAME_START_CAPACITY;

	char fullPath[FILE_MAX_PATH_LENGTH];

	FILE *sklaInfo;

	sklaInit(skla);

	fileGenerateFullPath(fullPath, SKELETON_ANIMATION_RESOURCE_DIRECTORY_STRING, SKELETON_ANIMATION_RESOURCE_DIRECTORY_LENGTH, filePath, filePathLength);
	sklaInfo = fopen(fullPath, "r");

	if(sklaInfo != NULL){

		char lineFeed[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;

		int currentCommand = -1;     // The current multiline command type (-1 = none, 0 = bones, 1 = frame).
		fileLine_t currentLine = 0;  // Current file line being read.

		skla->bones = memAllocate(SKELETON_ANIM_BONE_START_CAPACITY*sizeof(char *));
		if(skla->bones == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		skla->frames = memAllocate(SKELETON_ANIM_FRAME_START_CAPACITY*sizeof(transform *));
		if(skla->frames == NULL){
			/** Memory allocation failure. **/
			memFree(skla->bones);
			return -1;
		}
		skla->animData.frameDelays = memAllocate(SKELETON_ANIM_FRAME_START_CAPACITY*sizeof(float));
		if(skla->animData.frameDelays == NULL){
			/** Memory allocation failure. **/
			memFree(skla->animData.frameDelays);
			memFree(skla->bones);
			return -1;
		}

		while(fileParseNextLine(sklaInfo, lineFeed, sizeof(lineFeed), &line, &lineLength)){

			++currentLine;

			// Bone
			if(lineLength >= 6 && strncmp(line, "bone ", 5) == 0){
				if(currentCommand != 1){
					// Resize the bone name array if necessary.
					if(skla->boneNum == boneCapacity){
						char **tempBuffer;
						boneCapacity *= 2;
						tempBuffer = memReallocate(skla->bones, boneCapacity*sizeof(char *));
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
					memcpy(skla->bones[skla->boneNum], line+5, lineLength-5);
					skla->bones[skla->boneNum][lineLength-5] = '\0';
					++skla->boneNum;
					currentCommand = 0;
				}else{
					printf("Error loading skeletal animation \"%s\": Bone at line %u must be specified before any frames.\n", fullPath, currentLine);
				}


			// Make the current animation loop.
			}else if(lineLength >= 6 && strncmp(line, "loop ", 5) == 0){
				if(currentCommand != 1){
					skla->animData.desiredLoops = strtol(line+5, NULL, 0);
				}else{
					printf("Error loading skeletal animation \"%s\": Loop command at line %u does not belong inside a multiline command.\n", fullPath, currentLine);
				}


			// New frame
			}else if(lineLength >= 6 && strncmp(line, "frame", 5) == 0){
				if(strchr(line+5, '{')){
					boneIndex_t i;
					transform *tempBuffer;
					if(currentCommand == 0){
						// Make sure at least one bone has been loaded.
						if(skla->boneNum == 0){
							printf("Error loading skeletal animation \"%s\": Attempting to load frames when no bones have been specified. Aborting.\n", fullPath);
							sklaDelete(skla);
							fclose(sklaInfo);
							return 0;
						}
					}else if(currentCommand == 1){
						printf("Error loading skeletal animation \"%s\": Trying to start a multiline command at line %u while another is already in progress. "
						       "Closing the current command.\n", fullPath, currentLine);
					}
					// Resize the bone name array if necessary.
					if(skla->animData.frameNum == frameCapacity){
						transform **tempBuffer1;
						float *tempBuffer2;
						frameCapacity *= 2;
						tempBuffer1 = memReallocate(skla->frames, frameCapacity*sizeof(transform *));
						if(tempBuffer1 == NULL){
							/** Memory allocation failure. **/
							sklaDelete(skla);
							fclose(sklaInfo);
							return -1;
						}
						skla->frames = tempBuffer1;
						tempBuffer2 = memReallocate(skla->animData.frameDelays, frameCapacity*sizeof(float));
						if(tempBuffer2 == NULL){
							/** Memory allocation failure. **/
							sklaDelete(skla);
							fclose(sklaInfo);
							return -1;
						}
						skla->animData.frameDelays = tempBuffer2;
					}
					// Allocate bones for the new frame.
					tempBuffer = memAllocate(skla->boneNum*sizeof(transform));
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
							tfInit(&skla->frames[skla->animData.frameNum][i]);
						}
					}else{
						for(i = 0; i < skla->boneNum; ++i){
							skla->frames[skla->animData.frameNum][i] = skla->frames[skla->animData.frameNum-1][i];
						}
					}
					++skla->animData.frameNum;
					currentCommand = 1;
				}else{
					/** Worth it? **/
					printf("Error loading skeletal animation \"%s\": Frame command at line %u does not contain a brace.\n", fullPath, currentLine);
				}


			// Frame duration
			}else if(lineLength >= 8 && strncmp(line, "length ", 7) == 0){
				if(currentCommand == 1){
					skla->animData.frameDelays[skla->animData.frameNum-1] = strtof(line+7, NULL);
					if(skla->animData.frameNum > 1){
						skla->animData.frameDelays[skla->animData.frameNum-1] += skla->animData.frameDelays[skla->animData.frameNum-2];
					}
				}else{
					printf("Error loading skeletal animation \"%s\": Frame sub-command \"length\" invoked on line %u without specifying a frame.\n", fullPath, currentLine);
				}


			// Bone transform
			}else if(lineLength >= 29 && strncmp(line, "transform ", 10) == 0){
				if(currentCommand == 1){

					const char *token = strtok(line+10, " ");

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

						skla->frames[skla->animData.frameNum-1][boneID].position = vec3New(data[0][0], data[0][1], data[0][2]);
						skla->frames[skla->animData.frameNum-1][boneID].orientation = quatNewEuler(data[1][0]*RADIAN_RATIO, data[1][1]*RADIAN_RATIO, data[1][2]*RADIAN_RATIO);
						skla->frames[skla->animData.frameNum-1][boneID].scale = vec3New(data[2][0], data[2][1], data[2][2]);

					}else{
						printf("Error loading skeletal animation \"%s\": Frame sub-command \"transform\" at line %u does not specify a bone ID.\n", fullPath, currentLine);
					}

				}else{
					printf("Error loading skeletal animation \"%s\": Frame sub-command \"transform\" invoked on line %u without specifying a frame.\n", fullPath, currentLine);
				}


			}

			if(lineLength > 0 && line[lineLength-1] == '}'){
				if(currentCommand == 1){
					currentCommand = -1;
				}else{
					printf("Error loading skeletal animation \"%s\": Stray brace on line %u.\n", fullPath, currentLine);
				}
			}

		}

		fclose(sklaInfo);

	}else{
		printf("Error loading skeletal animation \"%s\": Could not open file.\n", fullPath);
		return 0;
	}

	/**
	***
	***
	***
	***
	***
	**/
	sklaDefragment(skla, filePath, filePathLength);

	// Generate a name based off the file path.
	/**skla->name = fileGenerateResourceName(filePath, filePathLength);
	if(skla->name == NULL){
		** Memory allocation failure. **
		sklaDelete(skla);
		return -1;
	}**/

	return 1;

}
/** TEMPORARY **/
return_t sklaLoadSMD(sklAnim *skla, const skeleton *skl, const char *const __RESTRICT__ filePath, const size_t filePathLength, const int invert){
	// Temporary function by 8426THMY.
	//Create and initialize the animation!
	sklaInit(skla);

	//Find the full path for the model!
	char *fullPath = memAllocate((filePathLength+1)*sizeof(char));
	if(fullPath == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	memcpy(fullPath, filePath, filePathLength);
	fullPath[filePathLength] = '\0';

	//Load the textureGroup!
	FILE *skeleAnimFile = fopen(fullPath, "r");
	if(skeleAnimFile != NULL){
		size_t tempCapacity = 1;
		//Temporarily stores bones.
		size_t tempBonesSize = 0;
		sklNode *tempBones = memAllocate(tempCapacity * sizeof(*tempBones));
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
					memFree(tempBones);
					memFree(fullPath);
					return 0;
				}
			}else{
				if(strcmp(line, "end") == 0){
					//If we've finished identifying the skeleton's bones, shrink the vector!
					if(dataType == 1){
						tempBones = memReallocate(tempBones, tempBonesSize * sizeof(*tempBones));
						skla->bones = memAllocate(tempBonesSize * sizeof(*skla->bones));
						skla->boneNum = tempBonesSize;
						skla->animData.frameDelays = memAllocate(sizeof(*skla->animData.frameDelays));
						skla->frames = memAllocate(sizeof(*skla->frames));

						size_t i;
						for(i = 0; i < tempBonesSize; ++i){
							skla->bones[i] = tempBones[i].name;
						}

						tempCapacity = 1;

					//If we've finished loading the animation, shrink the vectors!
					}else if(dataType == 2){
						skla->animData.frameDelays = memReallocate(skla->animData.frameDelays, skla->animData.frameNum * sizeof(*skla->animData.frameDelays));
						skla->frames = memReallocate(skla->frames, skla->animData.frameNum * sizeof(*skla->frames));
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
							tempBone.name = memAllocate(boneNameLength + 1);
							memcpy(tempBone.name, tokPos, boneNameLength);
							tempBone.name[boneNameLength] = '\0';

							//Get the ID of this bone's parent.
							tempBone.parent = strtoul(tokPos + boneNameLength + 1, NULL, 10);
							if(tempBone.parent == 255){
								tempBone.parent = boneID;
							}


							//If we're out of space, allocate some more!
							if(tempBonesSize >= tempCapacity){
								tempCapacity = tempBonesSize * 2;
								tempBones = memReallocate(tempBones, tempCapacity * sizeof(*tempBones));
							}
							//Add the bone to our vector!
							tempBones[tempBonesSize] = tempBone;
							++tempBonesSize;
						}else{
							printf("Error loading skeletal animtion!\n"
							       "Path: %s\n"
							       "Line: %s\n"
							       "Error: Found node %lu when expecting node %lu!\n",
							       fullPath, line, (unsigned long)boneID, (unsigned long)tempBonesSize);
							sklaDelete(skla);
							memFree(tempBones);
							memFree(fullPath);
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
									skla->animData.frameDelays = memReallocate(skla->animData.frameDelays, tempCapacity * sizeof(*skla->animData.frameDelays));

									//Resize the frame arrays!
									skla->frames = memReallocate(skla->frames, tempCapacity * sizeof(*skla->frames));
								}

								skla->frames[skla->animData.frameNum] = memAllocate(skla->boneNum * sizeof(**skla->frames));
								skla->animData.frameDelays[skla->animData.frameNum] = (data + 1) * (1000.f / 24.f);
								++skla->animData.frameNum;
							}else{
								printf("Error loading skeletal animtion!\n"
								       "Path: %s\n"
								       "Line: %s\n"
								       "Error: Frame timestamps do not increment sequentially!\n",
								       fullPath, line);
								sklaDelete(skla);
								memFree(tempBones);
								memFree(fullPath);
								return 0;
							}

						//Otherwise, we're setting the bone's state!
						}else{
							char *tokPos = line;

							//Get this bone's ID.
							size_t boneID = strtoul(tokPos, &tokPos, 10);
							if(boneID < tempBonesSize){
								transform *currentState = &skla->frames[skla->animData.frameNum - 1][boneID];

								// Load the bone's position!
								float x = strtod(tokPos, &tokPos) * 0.05f;
								float y = strtod(tokPos, &tokPos) * 0.05f;
								float z = strtod(tokPos, &tokPos) * 0.05f;
								currentState->position = vec3New(x, y, z);

								// Load the bone's rotation!
								x = strtod(tokPos, &tokPos);
								y = strtod(tokPos, &tokPos);
								z = strtod(tokPos, NULL);
								currentState->orientation = quatNewEuler(x, y, z);

								//The Source Engine uses Z as its up axis, so we need to fix that with the root bone.
								if(boneID == 0 && invert){
									transform rotateUp = {
										.position.x = 0.f, .position.y = 0.f, .position.z = 0.f,
										.orientation.w = 0.70710678118654752440084436210485f, .orientation.v.x = -0.70710678118654752440084436210485f, .orientation.v.y = 0.f, .orientation.v.z = 0.f,
										.scale.x = 1.f, .scale.y = 1.f, .scale.z = 1.f
									};
									*currentState = tfAppend(rotateUp, *currentState);
								}

								//Set the bone's scale!
								currentState->scale = vec3New(1.f, 1.f, 1.f);

								/** Some SMD animations are different to how I do animations. **/
								/** See skliGenerateBoneState for more information. **/
								if(invert){
									/// Originally used the old tfInverse function.
									transform thing = tfInverse(skl->bones[boneID].defaultState);
									thing.position = quatRotateVec3FastApproximate(thing.orientation, thing.position);
									*currentState = tfAppend(thing, *currentState);
								}

							}else{
								printf("Error loading skeletal animtion!\n"
									   "Path: %s\n"
									   "Line: %s\n"
									   "Error: Found skeletal data for bone %lu, which doesn't exist!\n",
									   fullPath, line, (unsigned long)boneID);
								sklaDelete(skla);
								memFree(tempBones);
								memFree(fullPath);
								return 0;
							}
						}
					}
				}
			}
		}


		//We don't delete them properly because we store the bone names elsewhere.
		if(tempBones != NULL){
			memFree(tempBones);
		}


		fclose(skeleAnimFile);
		memFree(fullPath);

		sklaDefragment(skla, filePath, filePathLength);
		/**skla->name = fileGenerateResourceName(filePath, filePathLength, skla->boneNum);
		if(skla->name == NULL){
			** Memory allocation failure. **
			sklaDelete(skla);
			return -1;
		}**/
	}else{
		printf("Unable to open skeletal animation file!\n"
		       "Path: %s\n", fullPath);
		memFree(fullPath);
		return 0;
	}

	return 1;
}
boneIndex_t sklaFindBone(const sklAnim *const __RESTRICT__ skla, const boneIndex_t id, const char *const __RESTRICT__ name){
	//skli->animations[i].animFrags[j].animBoneLookup[boneID] != (boneIndex_t)-1;
	/*boneIndex_t i;
	for(i = 0; i < skla->boneNum; ++i){
		if(strcmp(skla->bones[i], name) == 0){
			return i;
		}
	}
	return (boneIndex_t)-1;*/
	boneIndex_t i;
	char **n;
	if(id < skla->boneNum){
		i = id;
		n = &skla->bones[id];
		if(strcmp(*n, name) == 0){
			return i;
		}
		while(i > 0){
			--i;
			--n;
			if(strcmp(*n, name) == 0){
				return i;
			}
		}
		for(i = id, n = &skla->bones[id]; i < skla->boneNum; ++i, ++n){
			if(strcmp(*n, name) == 0){
				return i;
			}
		}
	}else{
		for(i = 0, n = skla->bones; i < skla->boneNum; ++i, ++n){
			if(strcmp(*n, name) == 0){
				return i;
			}
		}
	}
	return (boneIndex_t)-1;
}
void sklaDelete(sklAnim *const __RESTRICT__ skla){
	if(skla->bones != NULL){
		// This also frees frames, frameDelays and the name.
		memFree(skla->bones);
	}
}

void sklaiInit(sklAnimInstance *const __RESTRICT__ sklai, const sklAnim *const __RESTRICT__ animation, const float intensity, const flags_t flags){
	sklai->animation = animation;
	animInstInit(&sklai->animator);
	sklai->animStartFrame = 0;
	sklai->animEndFrame = 0;
	sklai->animInterpT = 0.f;
	sklai->intensity = intensity;
	sklai->decay = 0.f;
	sklai->timeMod = 1.f;
	sklai->flags = flags;
}
static __FORCE_INLINE__ void sklaiTick(sklAnimInstance *const __RESTRICT__ sklai, const float dt_ms, const float interpT){

	const float elapsedTimeMod = dt_ms * sklai->timeMod;

	// Invoke the animator.
	animTick(&sklai->animator, &sklai->animation->animData, elapsedTimeMod);
	animState(
		&sklai->animator, &sklai->animation->animData, interpT,
		&sklai->animStartFrame, &sklai->animEndFrame, &sklai->animInterpT
	);

	// Decay intensity for blending.
	sklai->intensity += sklai->decay;
	if(sklai->decay < 0.f && sklai->intensity <= sklai->intensityLimit){
		sklai->intensity = sklai->intensityLimit;
		sklai->decay = 0.f;
	}else if(sklai->decay > 0.f && sklai->intensity >= sklai->intensityLimit){
		sklai->intensity = sklai->intensityLimit;
		sklai->decay = 0.f;
	}

}
/**void sklaiAnimate(sklAnimInstance *sklai, const float dt_ms){

	const float elapsedTimeMod = dt_ms * sklai->timeMod;

	* Loop through each sklAnimFragment, updating them. *
	size_t fragmentID = 0;
	while(fragmentID < sklai->animFragNum){

		// Check if the fragment is blending into another.
		if(fragmentID+1 < sklai->animFragNum){
			// **
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
	bone lastState[SKELETON_MAX_BONE_NUM];  // Temporarily holds the states of bones from previous animation fragments.

	* Loop through each animation fragment in order, blending between them and generating bone states. *
	while(fragmentID < sklai->animFragNum){

		size_t startFrame;
		size_t endFrame;
		float animInterpT;
		animRenderState(&sklai->animFrags[fragmentID].animator, &sklai->animFrags[fragmentID].currentAnim->animData, interpT,
		                &startFrame, &endFrame, &animInterpT);

		* Loop through each bone in the current animation fragment, running interpolation and writing to skeletonState. *
		for(i = 0; i < sklai->animFrags[fragmentID].currentAnim->boneNum; ++i){
			// Only continue if the current bone exists in the skeleton.
			if(sklai->animFrags[fragmentID].animBoneLookup[i] != (size_t)-1){

				// Interpolate between startFrame and endFrame, storing the result in interpBoneEnd.
				bone interpBoneEnd;
				tfInterpolate(&sklai->animFrags[fragmentID].currentAnim->frames[startFrame][i],
				                &sklai->animFrags[fragmentID].currentAnim->frames[endFrame][i],
				                animInterpT, &interpBoneEnd);

				if(fragmentID > 0){
					// Blend from the previous animation fragment. animInterpTBlend is always 1.f for the first animation fragment.
					tfInterpolate(&lastState[sklai->animFrags[fragmentID-1].animBoneLookup[i]], &interpBoneEnd, animInterpTBlend,
					                &lastState[sklai->animFrags[fragmentID].animBoneLookup[i]]);
					//printf("%f %f %f %f\n", interpBoneEnd.orientation.w, interpBoneEnd.orientation.v.x, interpBoneEnd.orientation.v.y, interpBoneEnd.orientation.v.z);
				}else{
					lastState[sklai->animFrags[fragmentID].animBoneLookup[i]] = interpBoneEnd;
				}

				if(sklai->animFrags[fragmentID].currentAnim->additive){
					// Add the changes in lastState to skeletonState if the animation is additive.
					tfAppend(&skeletonState[sklai->animFrags[fragmentID].animBoneLookup[i]],
					                    &lastState[sklai->animFrags[fragmentID].animBoneLookup[i]],
					                    &skeletonState[sklai->animFrags[fragmentID].animBoneLookup[i]]);
				}else{
					// Set if the animation is not additive. Start from the
					// base state so custom transformations aren't lost.
					tfAppend(&baseState[sklai->animFrags[fragmentID].animBoneLookup[i]],
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

}**/
__FORCE_INLINE__ void sklaiSetType(sklAnimInstance *const __RESTRICT__ sklai, const flags_t additive){
	sklai->flags = additive;
}
void sklaiChange(sklAnimInstance *const __RESTRICT__ sklai, sklAnim *const __RESTRICT__ anim, const frameIndex_t frame){

	sklai->animation = anim;

	// Set the fragment to the current frame.
	if(frame < anim->animData.frameNum){
		sklai->animator.currentFrame = frame;
		if(frame > 0){
			sklai->animator.totalElapsedTime = anim->animData.frameDelays[frame-1];
		}
		if(frame == anim->animData.frameNum-1){
			sklai->animator.nextFrame = 0;
		}else{
			sklai->animator.nextFrame = frame+1;
		}
	}

}
__FORCE_INLINE__ void sklaiDecay(sklAnimInstance *const __RESTRICT__ sklai, const float intensity, const float decay){
	// Start decaying an animation.
	sklai->intensityLimit = intensity;
	sklai->decay = decay;
}

return_t skliInit(sklInstance *const __RESTRICT__ skli, const skeleton *const __RESTRICT__ skl, const animIndex_t animationCapacity){
	if(animationCapacity > 0){
		animIndex_t i = 0;
		while(i < animationCapacity){
			sklAnimInstance *newAnim = moduleSkeletonAnimationInstanceAppend(&skli->animations);
			if(newAnim == NULL){
				break;
			}
			sklaiInit(newAnim, NULL, 0.f, SKELETON_ANIM_INSTANCE_ADDITIVE);
			++i;
		}
		if(i != animationCapacity){
			/** Memory allocation failure. **/
			moduleSkeletonAnimationInstanceFreeArray(&skli->animations);
			return -1;
		}
	}else{
		skli->animations = NULL;
	}
	skli->skl = skl;
	skli->timeMod = 1.f;
	return 1;
}
return_t skliLoad(sklInstance *const __RESTRICT__ skli, const char *const __RESTRICT__ prgPath, const char *const __RESTRICT__ filePath){

	/** stateNum is temporary. **/

	//skliInit(skli, 1);

	/**sklAnim *skla = memAllocate(sizeof(sklAnim));
	skla->name = memAllocate(5*sizeof(char));
	memcpy(skla->name, "test\0", 5);
	//skla->additive = 1;
	skla->animData.desiredLoops = -1;
	skla->boneNum = 3;
	skla->bones = memAllocate(skla->boneNum*sizeof(char*));
	skla->bones[0] = memAllocate(11*sizeof(char));
	memcpy(skla->bones[0], "bip_pelvis\0", 5);
	skla->bones[1] = memAllocate(12*sizeof(char));
	memcpy(skla->bones[1], "bip_spine_0\0", 4);
	skla->bones[2] = memAllocate(9*sizeof(char));
	memcpy(skla->bones[2], "bip_neck\0", 4);
	skla->animData.frameNum = 3;
	skla->frames = memAllocate(skla->animData.frameNum*sizeof(bone*));
	skla->animData.frameDelays = memAllocate(skla->animData.frameNum*sizeof(float));

	bone tempBoneRoot, tempBoneTop, tempBoneHead;
	tfInit(&tempBoneRoot); tfInit(&tempBoneTop); tfInit(&tempBoneHead);
	//tempBoneRoot.position.y = -1.f;
	//tempBoneTop.position.y = -1.f;

	skla->frames[0] = memAllocate(skla->boneNum*sizeof(bone));
	skla->frames[0][0] = tempBoneRoot;
	skla->frames[0][1] = tempBoneTop;
	skla->frames[0][2] = tempBoneHead;
	skla->animData.frameDelays[0] = 1000.f;

	skla->frames[1] = memAllocate(skla->boneNum*sizeof(bone));
	tempBoneRoot.orientation = quatNewEuler(-90.f*RADIAN_RATIO, 0.f, 0.f);
	skla->frames[1][0] = tempBoneRoot;
	//tempBoneTop.position.y += 6.f;
	skla->frames[1][1] = tempBoneTop;
	skla->frames[1][2] = tempBoneHead;
	skla->animData.frameDelays[1] = 2000.f;

	skla->frames[2] = memAllocate(skla->boneNum*sizeof(bone));
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
	++skli->animationNum;**/







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
	skla->frames = memAllocate(skla->animData.frameNum*sizeof(transform *));
	skla->animData.frameDelays = memAllocate(skla->animData.frameNum*sizeof(float));

	transform tempBoneRoot, tempBoneTop;
	tfInit(&tempBoneRoot); tfInit(&tempBoneTop);
	//tempBoneRoot.position.y = -1.f;
	//tempBoneTop.position.y = -1.f;

	skla->frames[0] = memAllocate(skla->boneNum*sizeof(transform));
	skla->frames[0][0] = tempBoneRoot;
	skla->frames[0][1] = tempBoneTop;
	skla->animData.frameDelays[0] = 1000.f;

	skla->frames[1] = memAllocate(skla->boneNum*sizeof(transform));
	//tempBoneRoot.orientation = quatNewEuler(-90.f*RADIAN_RATIO, 0.f, 0.f);
	skla->frames[1][0] = tempBoneRoot;
	tempBoneTop.position.y += 0.5f;
	skla->frames[1][1] = tempBoneTop;
	skla->animData.frameDelays[1] = 2000.f;

	skla->frames[2] = memAllocate(skla->boneNum*sizeof(transform));
	tempBoneRoot.position.y += 0.5f;
	skla->frames[2][0] = tempBoneRoot;
	tempBoneTop.position.y -= 0.5f;
	tempBoneTop.orientation = quatNewEuler(0.f, -90.f*RADIAN_RATIO, 0.f);
	skla->frames[2][1] = tempBoneTop;
	skla->animData.frameDelays[2] = 3000.f;

	skliAnimationNew(skli, skla, 1.f, 0);



	return 1;

}
__FORCE_INLINE__ sklAnimInstance *skliAnimationNew(sklInstance *const __RESTRICT__ skli, sklAnim *const __RESTRICT__ anim, const float intensity, const flags_t flags){
	sklAnimInstance *const r = moduleSkeletonAnimationInstanceAppend(&skli->animations);
	if(r != NULL){
		sklaiInit(r, anim, intensity, flags);
	}
	return r;
}
__FORCE_INLINE__ void skliAnimationDelete(sklInstance *const __RESTRICT__ skli, sklAnimInstance *const __RESTRICT__ anim, sklAnimInstance *const __RESTRICT__ previous){
	moduleSkeletonAnimationInstanceFree(&skli->animations, anim, previous);
}
__FORCE_INLINE__ void skliTick(sklInstance *const __RESTRICT__ skli, const float dt_ms, const float interpT){
	const float elapsedTimeMod = dt_ms * skli->timeMod;
	sklAnimInstance *anim = skli->animations;
	while(anim != NULL){
		sklaiTick(anim, elapsedTimeMod, interpT);
		anim = moduleSkeletonAnimationInstanceNext(anim);
	}
}
transform skliGenerateBoneState(const sklInstance *const __RESTRICT__ skli, const boneIndex_t id, const char *const __RESTRICT__ name, transform state){

	const transform baseState = state;
	transform animationState;
	boneIndex_t animBoneID;

	// Loop through each animation.
	sklAnimInstance *anim = skli->animations;
	while(anim != NULL){

		if(anim->intensity > 0.f){

			// If the bone exists in the current animation, generate a fragment state.
			animBoneID = sklaFindBone(anim->animation, id, name);
			if(animBoneID < anim->animation->boneNum){

				// Interpolate between startFrame and endFrame, storing the result in animationState.
				animationState = tfInterpolate(
					anim->animation->frames[anim->animator.currentFrame][animBoneID],
					anim->animation->frames[anim->animator.nextFrame][animBoneID],
					anim->animInterpT
				);

				/** This is necessary if animations include bind information, such as with SMDs. **/
				///fragmentState = tfAppend(boneInvert(skli->skl->bones[id].defaultState), fragmentState);

				// Weight the animation by its intensity.
				if(anim->intensity != 1.f){
					animationState = tfInterpolate(tfIdentity(), animationState, anim->intensity);
				}

			}else{
				tfInit(&animationState);
			}

			if(anim->flags == SKELETON_ANIM_INSTANCE_OVERWRITE){
				// Set if the animation is not additive. Start from the
				// base state so custom transformations aren't lost.
				state = tfAppend(baseState, animationState);
			}else{
				// Add the changes in lastState to skeletonState if the animation is additive.
				state = tfAppend(state, animationState);
			}

		}

		anim = moduleSkeletonAnimationInstanceNext(anim);

	}

	return state;

}
void skliAddAnimation(sklInstance *const __RESTRICT__ skli, const sklAnim *const __RESTRICT__ skla, const frameIndex_t frame){
	//
}
void skliChangeSkeleton(sklInstance *const __RESTRICT__ skli, const skeleton *const __RESTRICT__ skl){
	/** Re-calculate bone lookups for all animation fragments. **/
}
void skliGenerateDefaultState(const skeleton *const __RESTRICT__ skl, mat4 *const __RESTRICT__ state, const boneIndex_t boneID){

	// NOTE: The generated matrix will be in row-major
	//       order, where as OpenGL accepts column-major
	//       matrices by default.

	// Apply parent transformations first if possible.
	if(boneID != skl->bones[boneID].parent){
		state[boneID] = state[skl->bones[boneID].parent];
	}else{
		mat4Identity(&state[boneID]);
	}

	// Apply default state transformations.
	state[boneID] = mat4Translate(state[boneID],
	                              skl->bones[boneID].defaultState.position.x,
	                              skl->bones[boneID].defaultState.position.y,
	                              skl->bones[boneID].defaultState.position.z);
	state[boneID] = mat4Rotate(state[boneID], skl->bones[boneID].defaultState.orientation);
	state[boneID] = mat4Scale(state[boneID],
	                          skl->bones[boneID].defaultState.scale.x,
	                          skl->bones[boneID].defaultState.scale.y,
	                          skl->bones[boneID].defaultState.scale.z);

}
void skliGenerateBoneStateFromLocal(const transform *const __RESTRICT__ skeletonState, const skeleton *const __RESTRICT__ oskl, const skeleton *const __RESTRICT__ mskl, mat4 *const __RESTRICT__ state, const boneIndex_t boneID){

	// NOTE: The generated matrix will be in row-major
	//       order, where as OpenGL accepts column-major
	//       matrices by default.

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
		state[boneID] = mat4Translate(state[boneID],
		                              skeletonState[animBone].position.x,
		                              skeletonState[animBone].position.y,
		                              skeletonState[animBone].position.z);
		state[boneID] = mat4Rotate(state[boneID], skeletonState[animBone].orientation);
		state[boneID] = mat4Scale(state[boneID],
		                          skeletonState[animBone].scale.x,
		                          skeletonState[animBone].scale.y,
		                          skeletonState[animBone].scale.z);

		// Apply model's negative default state transformations.
		state[boneID] = mat4Translate(state[boneID],
		                              -mskl->bones[boneID].defaultState.position.x,
		                              -mskl->bones[boneID].defaultState.position.y,
		                              -mskl->bones[boneID].defaultState.position.z);
		/** Probably won't keep what's below. **/
		/**quatInverseR(&mskl->bones[boneID].defaultState.orientation, &inverseOrientation);
		mat4Rotate(&state[boneID], &inverseOrientation);
		mat4Scale(&state[boneID], 1.f/mskl->bones[boneID].defaultState.scale.x,
		                          1.f/mskl->bones[boneID].defaultState.scale.y,
		                          1.f/mskl->bones[boneID].defaultState.scale.z);**/

		// Apply object's default state transformations.
		// This makes more sense when diagrammed.
		state[boneID] = mat4Translate(state[boneID],
		                              oskl->bones[animBone].defaultState.position.x,
		                              oskl->bones[animBone].defaultState.position.y,
		                              oskl->bones[animBone].defaultState.position.z);
		state[boneID] = mat4Rotate(state[boneID], oskl->bones[animBone].defaultState.orientation);
		/** Probably won't keep what's below. **/
		state[boneID] = mat4Scale(state[boneID],
		                          oskl->bones[animBone].defaultState.scale.x, ///*oskl->bones[animBone].defaultState.scale.x,
		                          oskl->bones[animBone].defaultState.scale.y, ///*oskl->bones[animBone].defaultState.scale.y,
		                          oskl->bones[animBone].defaultState.scale.z); ///*oskl->bones[animBone].defaultState.scale.z);

	}

}
void skliGenerateBoneStateFromGlobal(const transform *const __RESTRICT__ skeletonState, const skeleton *const __RESTRICT__ oskl, const skeleton *const __RESTRICT__ mskl, mat4 *const __RESTRICT__ state, const boneIndex_t boneID){

	// NOTE: The generated matrix will be in row-major
	//       order, where as OpenGL accepts column-major
	//       matrices by default.

	const boneIndex_t animBone = sklFindBone(oskl, boneID, mskl->bones[boneID].name);
	if(animBone < oskl->boneNum){

		// Apply animation transformations.
		state[boneID] = mat4TranslationMatrix(skeletonState[animBone].position.x,
		                                      skeletonState[animBone].position.y,
		                                      skeletonState[animBone].position.z);
		state[boneID] = mat4Rotate(state[boneID], skeletonState[animBone].orientation);
		state[boneID] = mat4Scale(state[boneID],
		                          skeletonState[animBone].scale.x,
		                          skeletonState[animBone].scale.y,
		                          skeletonState[animBone].scale.z);

	}


}
void skliDelete(sklInstance *const __RESTRICT__ skli){
	if(skli->animations != NULL){
		moduleSkeletonAnimationInstanceFreeArray(&skli->animations);
	}
}
