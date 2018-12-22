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

#define SKELETON_RESOURCE_DIRECTORY_STRING "Resources\\Skeletons\\"
#define SKELETON_RESOURCE_DIRECTORY_LENGTH 20

#define SKELETON_ANIMATION_RESOURCE_DIRECTORY_STRING "Resources\\Skeletons\\Animations\\"
#define SKELETON_ANIMATION_RESOURCE_DIRECTORY_LENGTH 31

#define SKELETON_ANIM_BONE_START_CAPACITY 1
#define SKELETON_ANIM_FRAME_START_CAPACITY 1

#define SKELETON_ANIM_FRAGMENT_START_CAPACITY 1

static void sklDefragment(skeleton *const restrict skl){
	frameIndex_t i;
	skl->bones = memReallocate(skl->bones, skl->boneNum*sizeof(sklNode));
	for(i = 0; i < skl->boneNum; ++i){
		skl->bones[i].name =
		memReallocate(
			skl->bones[i].name,
			strlen(skl->bones[i].name)*sizeof(char)
		);
	}
}

static return_t sklResizeToFit(skeleton *const restrict skl){
	/*bone *tempBuffer = memReallocate(skl->bones, skl->boneNum*sizeof(sklNode));
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
void sklInit(skeleton *const restrict skl){
	skl->name = NULL;
	skl->boneNum = 0;
	skl->bones = NULL;
}
return_t sklLoad(skeleton *const restrict skl, const char *const restrict prgPath, const char *const restrict filePath){

	char fullPath[FILE_MAX_PATH_LENGTH];
	size_t fileLength = strlen(filePath);

	FILE *sklInfo;

	sklInit(skl);

	fileGenerateFullPath(fullPath, prgPath, strlen(prgPath), SKELETON_RESOURCE_DIRECTORY_STRING, SKELETON_RESOURCE_DIRECTORY_LENGTH, filePath, fileLength);
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
	if(sklResizeToFit(skl) < 0){
		return -1;
	}

	// Generate a name based off the file path.
	skl->name = fileGenerateResourceName(filePath, fileLength);
	if(skl->name == NULL){
		/** Memory allocation failure. **/
		sklDelete(skl);
		return -1;
	}

	return 1;

}
return_t sklDefault(skeleton *const restrict skl){
	skl->name = memAllocate(8*sizeof(char));
	if(skl->name == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	skl->bones = memAllocate(sizeof(sklNode));
	if(skl->bones == NULL){
		/** Memory allocation failure. **/
		memFree(skl->name);
		return -1;
	}
	skl->bones[0].name = memAllocate(5*sizeof(char));
	if(skl->bones[0].name == NULL){
		/** Memory allocation failure. **/
		memFree(skl->bones);
		memFree(skl->name);
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
boneIndex_t sklFindBone(const skeleton *const restrict skl, const boneIndex_t id, const char *const restrict name){
	/*boneIndex_t i;
	for(i = 0; i < skl->boneNum; ++i){
		if(skl->bones[i].name != NULL && strcmp(skl->bones[i].name, name) == 0){
			return i;
		}
	}
	return (boneIndex_t)-1;*/
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
void sklDelete(skeleton *const restrict skl){
	if(skl->name != NULL){
		memFree(skl->name);
	}
	if(skl->bones != NULL){
		sklNode *n = skl->bones;
		sklNode *const nLast = &n[skl->boneNum];
		for(; n < nLast; ++n){
			if(n->name != NULL){
				memFree(n->name);
			}
		}
		memFree(skl->bones);
	}
}

void sklaInit(sklAnim *const restrict skla){
	skla->name = NULL;
	//skla->additive = 0;
	animDataInit(&skla->animData);
	skla->boneNum = 0;
	skla->bones = NULL;
	skla->frames = NULL;
}
static void sklaDefragment(sklAnim *const restrict skla){
	frameIndex_t i;
	skla->bones = memReallocate(skla->bones, skla->boneNum*sizeof(char *));
	skla->frames = memReallocate(skla->frames, skla->animData.frameNum*sizeof(bone *));
	skla->animData.frameDelays = memReallocate(skla->animData.frameDelays, skla->animData.frameNum*sizeof(float));
	for(i = 0; i < skla->boneNum; ++i){
		skla->bones[i] = memReallocate(skla->bones[i], strlen(skla->bones[i])*sizeof(char));
	}
	for(i = 0; i < skla->animData.frameNum; ++i){
		skla->frames[i] = memReallocate(skla->frames[i], skla->boneNum*sizeof(bone));
	}
}
static return_t sklaResizeToFit(sklAnim *const restrict skla, const size_t boneCapacity, const size_t frameCapacity){
	/*if(skla->boneNum != boneCapacity){
		skla->bones = memReallocate(skla->bones, skla->boneNum*sizeof(bone *));
		if(skla->bones == NULL){
			** Memory allocation failure. **
			sklaDelete(skla);
			return -1;
		}
	}
	if(skla->animData.frameNum != frameCapacity){
		skla->frames = memReallocate(skla->frames, skla->animData.frameNum*sizeof(char *));
		if(skla->frames == NULL){
			** Memory allocation failure. **
			sklaDelete(skla);
			return -1;
		}
		skla->animData.frameDelays = memReallocate(skla->animData.frameDelays, skla->animData.frameNum*sizeof(char *));
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
return_t sklaLoad(sklAnim *const restrict skla, const char *const restrict prgPath, const char *const restrict filePath){

	boneIndex_t boneCapacity = SKELETON_ANIM_BONE_START_CAPACITY;
	frameIndex_t frameCapacity = SKELETON_ANIM_FRAME_START_CAPACITY;

	char fullPath[FILE_MAX_PATH_LENGTH];
	const size_t fileLength = strlen(filePath);

	FILE *sklaInfo;

	sklaInit(skla);

	fileGenerateFullPath(fullPath, prgPath, strlen(prgPath), SKELETON_ANIMATION_RESOURCE_DIRECTORY_STRING, SKELETON_ANIMATION_RESOURCE_DIRECTORY_LENGTH, filePath, fileLength);
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
		skla->frames = memAllocate(SKELETON_ANIM_FRAME_START_CAPACITY*sizeof(bone *));
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
						boneCapacity *= 2;
						char **tempBuffer = memReallocate(skla->bones, boneCapacity*sizeof(char *));
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
					bone *tempBuffer;
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
						bone **tempBuffer1;
						float *tempBuffer2;
						frameCapacity *= 2;
						tempBuffer1 = memReallocate(skla->frames, frameCapacity*sizeof(bone *));
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
					tempBuffer = memAllocate(skla->boneNum*sizeof(bone));
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
					/// Worth it?
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

						vec3Set(&skla->frames[skla->animData.frameNum-1][boneID].position, data[0][0], data[0][1], data[0][2]);
						quatSetEuler(&skla->frames[skla->animData.frameNum-1][boneID].orientation, data[1][0]*RADIAN_RATIO, data[1][1]*RADIAN_RATIO, data[1][2]*RADIAN_RATIO);
						vec3Set(&skla->frames[skla->animData.frameNum-1][boneID].scale, data[2][0], data[2][1], data[2][2]);

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
	if(sklaResizeToFit(skla, boneCapacity, frameCapacity) < 0){
		return -1;
	}

	// Generate a name based off the file path.
	skla->name = fileGenerateResourceName(filePath, fileLength);
	if(skla->name == NULL){
		/** Memory allocation failure. **/
		sklaDelete(skla);
		return -1;
	}

	return 1;

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
	char *fullPath = memAllocate((pathLen+fileLen+1)*sizeof(char));
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
							       "Error: Found node %u when expecting node %u!\n",
							       fullPath, line, boneID, tempBonesSize);
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


		size_t fileLen = strlen(filePath);
		skla->name = memAllocate((fileLen+1)*sizeof(char));
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
		memFree(fullPath);
		return 0;
	}


	return 1;
}
boneIndex_t sklaFindBone(const sklAnim *const restrict skla, const boneIndex_t id, const char *const restrict name){
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
void sklaDelete(sklAnim *const restrict skla){
	if(skla->name != NULL){
		memFree(skla->name);
	}
	if(skla->bones != NULL){
		char **n = skla->bones;
		char **const nLast = &n[skla->boneNum];
		for(; n < nLast; ++n){
			if(*n != NULL){
				memFree(*n);
			}
		}
		memFree(skla->bones);
	}
	if(skla->frames != NULL){
		bone **b = skla->frames;
		bone **const bLast = &b[skla->animData.frameNum];
		for(; b < bLast; ++b){
			if(*b != NULL){
				memFree(*b);
			}
		}
		memFree(skla->frames);
	}
	animDataDelete(&skla->animData);
}

static return_t sklafInit(sklAnimFragment *const restrict sklaf, sklAnim *const restrict anim, const skeleton *const restrict skl, const frameIndex_t frame){

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
	sklaf->animation = anim;
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
/**static void sklafDelete(sklAnimFragment *sklaf){
	*if(sklaf->animBoneLookup != NULL){
		memFree(sklaf->animBoneLookup);
	}*
}**/

void sklaiInit(sklAnimInstance *const restrict sklai){
	sklai->flags = 0;
	sklai->timeMod = 1.f;
	sklai->fragments = NULL;
}
static __FORCE_INLINE__ void sklaiUpdateFragments(sklAnimInstance *const restrict sklai, const float elapsedTime, const float interpT){

	const float elapsedTimeMod = elapsedTime * sklai->timeMod;

	sklAnimFragment *frag = sklai->fragments;

	if(frag != NULL){

		sklAnimFragment *previous = NULL;
		sklAnimFragment *next = moduleSkeletonAnimationFragmentNext(frag);

		// Loop through each sklAnimFragment, updating them.
		while(next != NULL){

			/** **/
			// Check if the animation has finished blending on its oldest state.
			if(frag->animBlendProgress >= frag->animBlendTime){
				// Since it's no longer being used in any state, it can be safely freed.
				///sklafDelete(frag);
				moduleSkeletonAnimationFragmentFree(&sklai->fragments, frag, previous);
			}else{
				// Advance animator, update the blend and go to the next fragment.
				animAdvance(&frag->animator, &frag->animation->animData, elapsedTimeMod);
				animGetRenderData(&frag->animator, &frag->animation->animData, interpT,
				                  &frag->animStartFrame, &frag->animEndFrame, &frag->animInterpT);
				frag->animBlendProgress += elapsedTimeMod;
			}

			previous = frag;
			frag = next;
			next = moduleSkeletonAnimationFragmentNext(frag);

		}

		// Advance animator and exit.
		animAdvance(&frag->animator, &frag->animation->animData, elapsedTimeMod);
		animGetRenderData(&frag->animator, &frag->animation->animData, interpT,
		                  &frag->animStartFrame, &frag->animEndFrame, &frag->animInterpT);

	}

}
/**void sklaiAnimate(sklAnimInstance *sklai, const float elapsedTime){

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
	bone lastState[SKELETON_MAX_BONE_NUM];  // Temporarily holds the states of bones from previous animation fragments.

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

}**/
__FORCE_INLINE__ void sklaiSetType(sklAnimInstance *const restrict sklai, const flags_t additive){
	sklai->flags = additive;
}
return_t sklaiChange(sklAnimInstance *const restrict sklai, const skeleton *const restrict skl, sklAnim *const restrict anim, const frameIndex_t frame, const float blendTime){

	sklAnimFragment *newFragment;

	if(sklai->fragments == NULL){

		// Create a new fragment.
		newFragment = moduleSkeletonAnimationFragmentAppend(&sklai->fragments);

	}else{

		sklAnimFragment *lastFragment;
		sklAnimFragment *nextFragment = sklai->fragments;

		// Get the last fragment.
		do {
			lastFragment = nextFragment;
			nextFragment = moduleSkeletonAnimationFragmentNext(lastFragment);
		} while(nextFragment != NULL);

		// Set blending variables.
		lastFragment->animBlendTime = blendTime;
		lastFragment->animBlendProgress = 0.f;

		// Create a new fragment.
		newFragment = moduleSkeletonAnimationFragmentInsertAfter(&sklai->fragments, lastFragment);

	}

	if(newFragment == NULL){
		/** Memory allocation failure. **/
		return -1;
	}

	// Initialize the fragment.
	return sklafInit(newFragment, anim, skl, frame);

}
void sklaiClearAnimation(sklAnimInstance *const restrict sklai){
	if(sklai->fragments != NULL){
		moduleSkeletonAnimationFragmentFreeArray(&sklai->fragments);
	}
	sklaiInit(sklai);
}
void sklaiDelete(sklAnimInstance *const restrict sklai){
	if(sklai->fragments != NULL){
		moduleSkeletonAnimationFragmentFreeArray(&sklai->fragments);
	}
}

return_t skliInit(sklInstance *const restrict skli, const skeleton *const restrict skl, const animIndex_t animationCapacity){
	if(animationCapacity > 0){
		animIndex_t i = 0;
		while(i < animationCapacity){
			sklAnimInstance *newAnim = moduleSkeletonAnimationInstanceAppend(&skli->animations);
			if(newAnim == NULL){
				break;
			}
			sklaiInit(newAnim);
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
return_t skliLoad(sklInstance *const restrict skli, const char *const restrict prgPath, const char *const restrict filePath){

	/** stateNum is temporary. **/

	//skliInit(skli, 1);

	/*sklAnim *skla = memAllocate(sizeof(sklAnim));
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
	boneInit(&tempBoneRoot); boneInit(&tempBoneTop); boneInit(&tempBoneHead);
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
	++skli->animationNum;*/







	sklAnimInstance *sklai;
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

	sklai = skliAnimationNew(skli);
	sklaiChange(sklai, skli->skl, skla, 0, 0.f);

	return 1;

}
__FORCE_INLINE__ sklAnimInstance *skliAnimationNew(sklInstance *const restrict skli){
	sklAnimInstance *const r = moduleSkeletonAnimationInstanceAppend(&skli->animations);
	if(r != NULL){
		sklaiInit(r);
	}
	return r;
}
__FORCE_INLINE__ void skliAnimationDelete(sklInstance *const restrict skli, sklAnimInstance *const restrict anim, sklAnimInstance *const restrict previous){
	sklaiDelete(anim);
	moduleSkeletonAnimationInstanceFree(&skli->animations, anim, previous);
}
__FORCE_INLINE__ void skliUpdateAnimations(sklInstance *const restrict skli, const float elapsedTime, const float interpT){
	const float elapsedTimeMod = elapsedTime * skli->timeMod;
	sklAnimInstance *anim = skli->animations;
	while(anim != NULL){
		sklaiUpdateFragments(anim, elapsedTimeMod, interpT);
		anim = moduleSkeletonAnimationInstanceNext(anim);
	}
}
void skliGenerateBoneState(const sklInstance *const restrict skli, const boneIndex_t id, const char *const restrict name, bone *const restrict state){

	float animInterpTBlend = 1.f;
	bone baseState = *state;
	bone animationState;
	bone fragmentState;

	// Loop through each animation.
	sklAnimInstance *anim = skli->animations;
	while(anim != NULL){

		sklAnimFragment *frag = anim->fragments;

		boneInit(&animationState);

		// Loop through each animation fragment, blending between them and generating an animation state.
		if(frag != NULL){

			for(;;){

				sklAnimFragment *next;

				// If the bone exists in the current animation fragment, generate a fragment state and add it to the animation state.
				//if(skli->animations[i].animFrags[j].animBoneLookup[boneID] != (boneIndex_t)-1){
				boneIndex_t animBoneID = sklaFindBone(frag->animation, id, name);
				if(animBoneID < frag->animation->boneNum){

					// Interpolate between startFrame and endFrame, storing the result in fragmentState.
					boneInterpolateR(&frag->animation->frames[frag->animStartFrame][animBoneID],
					                 &frag->animation->frames[frag->animEndFrame][animBoneID],
					                 frag->animInterpT, &fragmentState);

					// Blend from the previous animation fragment. animInterpTBlend is always 1.f for the first animation fragment.
					boneInterpolate1(&animationState, &fragmentState, animInterpTBlend);

				}else{
					boneInit(&fragmentState);
				}

				next = moduleSkeletonAnimationFragmentNext(frag);
				if(next != NULL){
					// If this fragment marks the beginning of a valid blend, set animInterpTBlend for later.
					animInterpTBlend = frag->animBlendProgress / frag->animBlendTime;
					frag = next;
				}else{
					break;
				}

			}

		}

		if(anim->flags == SKELETON_ANIM_INSTANCE_OVERWRITE){
			// Set if the animation is not additive. Start from the
			// base state so custom transformations aren't lost.
			boneTransformCombineR(&baseState, &animationState, state);
		}else{
			// Add the changes in lastState to skeletonState if the animation is additive.
			boneTransformCombine1(state, &animationState);
		}

		anim = moduleSkeletonAnimationInstanceNext(anim);

	}

}
void skliAddAnimation(sklInstance *const restrict skli, const sklAnim *const restrict skla, const frameIndex_t frame){
	//
}
void skliChangeSkeleton(sklInstance *const restrict skli, const skeleton *const restrict skl){
	/** Re-calculate bone lookups for all animation fragments. **/
}
void skliGenerateDefaultState(const skeleton *const restrict skl, mat4 *const restrict state, const boneIndex_t boneID){

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
void skliGenerateBoneStateFromLocal(const bone *const restrict skeletonState, const skeleton *const restrict oskl, const skeleton *const restrict mskl, mat4 *const restrict state, const boneIndex_t boneID){

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
void skliGenerateBoneStateFromGlobal(const bone *const restrict skeletonState, const skeleton *const restrict oskl, const skeleton *const restrict mskl, mat4 *const restrict state, const boneIndex_t boneID){

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
void skliDelete(sklInstance *const restrict skli){
	if(skli->animations != NULL){
		moduleSkeletonAnimationInstanceFreeArray(&skli->animations);
	}
}
