#include "textureWrapper.h"
#include "memoryManager.h"
#include "moduleTexture.h"
#include "helpersFileIO.h"
#include "inline.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define TEXTURE_WRAPPER_RESOURCE_DIRECTORY_STRING "Resources"FILE_PATH_DELIMITER_STRING"Wrappers"FILE_PATH_DELIMITER_STRING
#define TEXTURE_WRAPPER_RESOURCE_DIRECTORY_LENGTH 19

#define FRAME_START_CAPACITY     1  // 128
#define ANIM_START_CAPACITY      1  // 128
#define SUBFRAME_START_CAPACITY  1  // 128
#define ANIMFRAME_START_CAPACITY 1  // 128

/** Remove printf()s **/

static void twfInit(twFrame *const restrict twf){
	twf->subframes = NULL;
	twf->diffuse = NULL;
	twf->normals = NULL;
	twf->specular = NULL;
	twf->subframeNum = 0;
}

static return_t twfNew(twFrame *const restrict twf, const frameIndex_t subframeCapacity){
	twf->subframes = memAllocate(subframeCapacity*sizeof(rectangle));
	if(twf->subframes == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	return 1;
}

static return_t twfAddSubframe(twFrame *const restrict twf, const rectangle *const restrict sf, frameIndex_t *const restrict subframeCapacity){
	if(twf->subframeNum == *subframeCapacity){
		rectangle *tempBuffer;
		if(*subframeCapacity > 0){
			//*subframeCapacity *= 2;
			++(*subframeCapacity);
		}else{
			*subframeCapacity = 1;
		}
		tempBuffer = memReallocate(twf->subframes, *subframeCapacity*sizeof(rectangle));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		twf->subframes = tempBuffer;
	}
	// Divide each property by the texture width / height to save doing it later on.
	twf->subframes[twf->subframeNum].x = sf->x / twf->diffuse->width;
	twf->subframes[twf->subframeNum].y = sf->y / twf->diffuse->height;
	twf->subframes[twf->subframeNum].w = sf->w / twf->diffuse->width;
	twf->subframes[twf->subframeNum].h = sf->h / twf->diffuse->height;
	++twf->subframeNum;
	return 1;
}

static return_t twfAddDefaultSubframe(twFrame *const restrict twf, const frameIndex_t subframeCapacity){
	if(subframeCapacity != twf->subframeNum+1){
		rectangle *tempBuffer;
		tempBuffer = memReallocate(twf->subframes, (twf->subframeNum+1)*sizeof(rectangle));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		twf->subframes = tempBuffer;
	}
	twf->subframes[twf->subframeNum].x = 0.f;
	twf->subframes[twf->subframeNum].y = 0.f;
	twf->subframes[twf->subframeNum].w = 1.f;
	twf->subframes[twf->subframeNum].h = 1.f;
	++twf->subframeNum;
	return 1;
}

static return_t twfResizeToFit(twFrame *const restrict twf, const frameIndex_t subframeCapacity){
	if(twf->subframeNum != subframeCapacity){
		rectangle *tempBuffer;
		tempBuffer = memReallocate(twf->subframes, twf->subframeNum*sizeof(rectangle));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		twf->subframes = tempBuffer;
	}
	return 1;
}

static void twfDelete(twFrame *const restrict twf){
	if(twf->subframes != NULL){
		memFree(twf->subframes);
	}
}

static void twaInit(twAnim *const restrict twa){
	twa->frames = NULL;
	animDataInit(&twa->animData);
}

static return_t twaNew(twAnim *const restrict twa, const frameIndex_t animframeCapacity){
	twa->frames = memAllocate(animframeCapacity*sizeof(twFramePair));
	if(twa->frames == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	twa->animData.frameDelays = memAllocate(animframeCapacity*sizeof(float));
	if(twa->animData.frameDelays == NULL){
		/** Memory allocation failure. **/
		memFree(twa->frames);
		return -1;
	}
	return 1;
}

static return_t twaAddFrame(twAnim *const restrict twa, const frameIndex_t f, const frameIndex_t sf, const float d, frameIndex_t *animframeCapacity){
	if(twa->animData.frameNum == *animframeCapacity){
		twFramePair *tempBuffer1;
		float *tempBuffer2;
		if(*animframeCapacity > 0){
			//*animframeCapacity *= 2;
			++(*animframeCapacity);
		}else{
			*animframeCapacity = 1;
		}
		tempBuffer1 = memReallocate(twa->frames, *animframeCapacity*sizeof(twFramePair));
		if(tempBuffer1 == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		tempBuffer2 = memReallocate(twa->animData.frameDelays, *animframeCapacity*sizeof(float));
		if(tempBuffer2 == NULL){
			/** Memory allocation failure. **/
			memFree(tempBuffer1);
			return -1;
		}
		twa->frames               = tempBuffer1;
		twa->animData.frameDelays = tempBuffer2;
	}
	twa->frames[twa->animData.frameNum].frameID = f;
	twa->frames[twa->animData.frameNum].subframeID = sf;
	twa->animData.frameDelays[twa->animData.frameNum] = d;
	++twa->animData.frameNum;
	return 1;
}

static return_t twaResizeToFit(twAnim *const restrict twa, const frameIndex_t animframeCapacity){
	if(twa->animData.frameNum != animframeCapacity){
		twFramePair *tempBuffer1;
		float *tempBuffer2;
		tempBuffer1 = memReallocate(twa->frames, twa->animData.frameNum*sizeof(twFramePair));
		if(tempBuffer1 == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		tempBuffer2 = memReallocate(twa->animData.frameDelays, twa->animData.frameNum*sizeof(float));
		if(tempBuffer2 == NULL){
			/** Memory allocation failure. **/
			memFree(tempBuffer1);
			return -1;
		}
		twa->frames               = tempBuffer1;
		twa->animData.frameDelays = tempBuffer2;
	}
	return 1;
}

static void twaDelete(twAnim *const restrict twa){
	if(twa->frames != NULL){
		memFree(twa->frames);
	}
	animDataDelete(&twa->animData);
}

static return_t twAddFrame(textureWrapper *const restrict tw, const twFrame *const restrict f, frameIndex_t *const restrict frameCapacity){
	if(tw->frameNum == *frameCapacity){
		twFrame *tempBuffer;
		if(*frameCapacity > 0){
			//*frameCapacity *= 2;
			++(*frameCapacity);
		}else{
			*frameCapacity = 1;
		}
		tempBuffer = memReallocate(tw->frames, *frameCapacity*sizeof(twFrame));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			memFree(tw->animations);
			return -1;
		}
		tw->frames = tempBuffer;
	}
	tw->frames[tw->frameNum++] = *f;
	return 1;
}

static return_t twAddAnim(textureWrapper *const restrict tw, const twAnim *const restrict a, animIndex_t *const restrict animCapacity){
	if(tw->animationNum == *animCapacity){
		twAnim *tempBuffer;
		if(*animCapacity > 0){
			//*animCapacity *= 2;
			++(*animCapacity);
		}else{
			*animCapacity = 1;
		}
		tempBuffer = memReallocate(tw->animations, *animCapacity*sizeof(twAnim));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			memFree(tw->frames);
			return -1;
		}
		tw->animations = tempBuffer;
	}
	tw->animations[tw->animationNum++] = *a;
	return 1;
}

static void twDefragment(textureWrapper *const restrict tw){
	frameIndex_t i;
	tw->frames     = memReallocate(tw->frames,     tw->frameNum    *sizeof(twFrame));
	tw->animations = memReallocate(tw->animations, tw->animationNum*sizeof(twAnim ));
	for(i = 0; i < tw->frameNum; ++i){
		/**tw->frames[i].diffuse->name =
		memReallocate(
			tw->frames[i].diffuse->name,
			strlen(tw->frames[i].diffuse->name)*sizeof(char)
		);**/
		tw->frames[i].subframes =
		memReallocate(
			tw->frames[i].subframes,
			tw->frames[i].subframeNum*sizeof(rectangle)
		);
	}
	for(i = 0; i < tw->animationNum; ++i){
		tw->animations[i].frames =
		memReallocate(
			tw->animations[i].frames,
			tw->animations[i].animData.frameNum*sizeof(twFramePair)
		);
		tw->animations[i].animData.frameDelays =
		memReallocate(
			tw->animations[i].animData.frameDelays,
			tw->animations[i].animData.frameNum*sizeof(float)
		);
	}
}

static return_t twResizeToFit(textureWrapper *const restrict tw, const frameIndex_t frameCapacity, const animIndex_t animCapacity){
	/*if(tw->frameNum != frameCapacity){
		twFrame *tempBuffer1;
		tempBuffer1 = memReallocate(tw->frames, tw->frameNum*sizeof(twFrame));
		if(tempBuffer1 == NULL){
			** Memory allocation failure. **
			twDelete(tw);
			return -1;
		}
		tw->frames = tempBuffer1;
	}
	if(tw->animationNum != animCapacity){
		twAnim *tempBuffer2;
		tempBuffer2 = memReallocate(tw->animations, tw->animationNum*sizeof(twAnim));
		if(tempBuffer2 == NULL){
			** Memory allocation failure. **
			twDelete(tw);
			return -1;
		}
		tw->animations = tempBuffer2;
	}*/
	/**
	*** Defrag until I create a new
	*** binary texture wrapper file
	*** format where the sizes are
	*** all known beforehand.
	**/
	twDefragment(tw);
	return 1;
}

void twInit(textureWrapper *const restrict tw){
	tw->name = NULL;
	tw->frameNum = 0;
	tw->animationNum = 0;
	tw->frames = NULL;
	tw->animations = NULL;
}

return_t twLoad(textureWrapper *const restrict tw, const char *const restrict prgPath, const char *const restrict filePath){

	twAnim tempAnim;
	frameIndex_t animframeCapacity = 0;
	frameIndex_t subframeCapacity = 0;
	frameIndex_t frameCapacity = FRAME_START_CAPACITY;
	animIndex_t animCapacity = ANIM_START_CAPACITY;

	char fullPath[FILE_MAX_PATH_LENGTH];
	const size_t fileLength = strlen(filePath);

	FILE *texInfo;

	twInit(tw);

	fileGenerateFullPath(fullPath, prgPath, strlen(prgPath), TEXTURE_WRAPPER_RESOURCE_DIRECTORY_STRING, TEXTURE_WRAPPER_RESOURCE_DIRECTORY_LENGTH, filePath, fileLength);
	texInfo = fopen(fullPath, "r");

	if(texInfo != NULL){

		char lineFeed[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;

		int currentCommand = -1;     // The current multiline command type (-1 = none, 0 = texture, 1 = animation).
		fileLine_t currentLine = 0;  // Current file line being read.

		tw->frames = memAllocate(frameCapacity*sizeof(twFrame));
		if(tw->frames == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		tw->animations = memAllocate(animCapacity*sizeof(twAnim));
		if(tw->animations == NULL){
			/** Memory allocation failure. **/
			memFree(tw->frames);
			return -1;
		}
		twaInit(&tempAnim);

		while(fileParseNextLine(texInfo, lineFeed, sizeof(lineFeed), &line, &lineLength)){

			++currentLine;

			// New texture frame
			if(lineLength >= 9 && strncmp(line, "texture ", 8) == 0){

				texture *tempTex;
				char texPath[1024];
				size_t pathLength;

				// A multiline command is already in progress; try to close it and continue.
				if(currentCommand != -1){

					printf("Error loading texture wrapper \"%s\": Trying to start a multiline command at line %u while another is already in progress. "
						   "Closing the current command.\n", fullPath, currentLine);

					// If the multiline command is a texture...
					if(currentCommand == 0){
						// If a textureFrame was being worked on and has no subframes, add the default one.
						if(tw->frames[tw->frameNum-1].subframeNum == 0){
							if(twfAddDefaultSubframe(&tw->frames[tw->frameNum-1], subframeCapacity) < 0){
								/** Memory allocation failure. **/
								twaDelete(&tempAnim);
								twDelete(tw);
								fclose(texInfo);
								return -1;
							}
						}else{
							// twfAddDefaultSubframe() automatically resizes to fit.
							if(twfResizeToFit(&tw->frames[tw->frameNum-1], subframeCapacity) < 0){
								/** Memory allocation failure. **/
								twaDelete(&tempAnim);
								twDelete(tw);
								fclose(texInfo);
								return -1;
							}
						}

					}else if(currentCommand == 1){
						// If a valid animation is being worked on, save it and continue.
						if(tempAnim.animData.frameNum > 0){
							if(twaResizeToFit(&tempAnim, animframeCapacity) < 0){
								/** Memory allocation failure. **/
								twDelete(tw);
								fclose(texInfo);
								return -1;
							}
							if(twAddAnim(tw, &tempAnim, &animCapacity) < 0){
								/** Memory allocation failure. **/
								twaDelete(&tempAnim);
								fclose(texInfo);
								return -1;
							}
						}
					}

					currentCommand = -1;

				}

				// Create a new texture frame.
				twFrame tempFrame;
				twfInit(&tempFrame);

				fileParseResourcePath(&texPath[0], &pathLength, line, lineLength, 8);

				// Check if the texture has already been loaded.
				tempTex = moduleTextureFind(&texPath[0]);
				if(tempTex != NULL){
					tempFrame.diffuse = tempTex;

				// If the texture path is surrounded by quotes, try and load it.
				}else{
					tempTex = moduleTextureAllocate();
					if(tempTex != NULL){
						const return_t r = tLoad(tempTex, prgPath, &texPath[0]);
						if(r < 1){
							// The load failed. Clean up.
							moduleTextureFree(tempTex);
							if(r < 0){
								/** Memory allocation failure. **/
								twaDelete(&tempAnim);
								fclose(texInfo);
								return -1;
							}
							printf("Error loading texture wrapper \"%s\": Image \"%s\" at line %u does not exist.\n", fullPath, &texPath[0], currentLine);
							tempFrame.diffuse = moduleTextureGetDefault();
						}else{
							tempFrame.diffuse = tempTex;
						}
					}else{
						/** Memory allocation failure. **/
						twaDelete(&tempAnim);
						fclose(texInfo);
						return -1;
					}
				}

				// Check if the command spans multiple lines (it contains an opening brace at the end).
				if(strrchr(line, '{') > line+1+pathLength){
					subframeCapacity = SUBFRAME_START_CAPACITY;
					twfNew(&tempFrame, subframeCapacity);
					currentCommand = 0;

				// If it doesn't, add the default subframe.
				}else{
					subframeCapacity = 1;
					twfNew(&tempFrame, subframeCapacity);
					if(twfAddDefaultSubframe(&tempFrame, subframeCapacity) < 0){
						/** Memory allocation failure. **/
						twfDelete(&tempFrame);
						twaDelete(&tempAnim);
						twDelete(tw);
						fclose(texInfo);
						return -1;
					}

				}
				// Add the frame to tw->frames.
				if(twAddFrame(tw, &tempFrame, &frameCapacity) < 0){
					/** Memory allocation failure. **/
					twfDelete(&tempFrame);
					twaDelete(&tempAnim);
					fclose(texInfo);
					return -1;
				}


			// Subframe macro
			}else if(lineLength >= 14 && strncmp(line, "sMacro ", 7) == 0){

				if(currentCommand == 0){

					// Loads number of subframes to create, main dimension to loop in (x or y) and the subframe dimensions.
					frameIndex_t i;
					frameIndex_t numberOfFrames = 0;
					char macroDirection = ' ';
					float dimensions[4];
					const char *token = strtok(line+7, "/");
					for(i = 0; i < 6; ++i){
						switch(i){
							case 0:
								numberOfFrames = strtoul(token, NULL, 0);
							break;
							case 1:
								macroDirection = token[0];
							break;
							default:
								dimensions[i-2] = strtod(token, NULL);
						}
						token = strtok(NULL, "/");
					}

					const GLsizei currentTexW = tw->frames[tw->frameNum-1].diffuse->width;
					const GLsizei currentTexH = tw->frames[tw->frameNum-1].diffuse->height;

					// Automatically generate subframes for a sprite sheet.
					for(i = 1; i <= numberOfFrames; ++i){
						if(dimensions[0] + dimensions[2] <= currentTexW && dimensions[1] + dimensions[3] <= currentTexH){
							rectangle baseSubframe = {.x = dimensions[0], .y = dimensions[1], .w = dimensions[2], .h = dimensions[3]};
							if(twfAddSubframe(&tw->frames[tw->frameNum-1], &baseSubframe, &subframeCapacity) < 0){
								/** Memory allocation failure. **/
								twaDelete(&tempAnim);
								twDelete(tw);
								fclose(texInfo);
								return -1;
							}
							if(macroDirection == 'x'){  // Adds frames from left to right before resetting and moving down
								dimensions[0] = (GLsizei)(i * dimensions[2]) % currentTexW;
								dimensions[1] = (GLsizei)(i * dimensions[2]) / currentTexW * dimensions[3];
							}else if(macroDirection == 'y'){  // Adds frames from top to bottom before resetting and moving right
								dimensions[0] = (GLsizei)(i * dimensions[3]) / currentTexH * dimensions[2];
								dimensions[1] = (GLsizei)(i * dimensions[3]) % currentTexH;
							}else{
								printf("Error loading texture wrapper \"%s\": sMacro command at line %u has an invalid direction. Only one frame could be loaded.\n", fullPath, currentLine);
								break;
							}
						}else{
							printf("Error loading texture wrapper \"%s\": sMacro command at line %u could not load %u frame(s).\n", fullPath, currentLine, numberOfFrames-i+1);
							break;
						}
					}

				}else{
					printf("Error loading texture wrapper \"%s\": Texture sub-command \"sMacro\" invoked on line %u without specifying a multiline texture.\n", fullPath, currentLine);
				}


			// New subframe
			}else if(lineLength >= 16 && strncmp(line, "subframe ", 9) == 0){

				if(currentCommand == 0){

					// Create a new subframe and add it to the current texture frame.
					frameIndex_t i;
					float dimensions[4];
					const char *token = strtok(line+9, "/");
					for(i = 0; i < 4; ++i){
						dimensions[i] = strtod(token, NULL);
						token = strtok(NULL, "/");
					}
					rectangle baseSubframe = {.x = dimensions[0], .y = dimensions[1], .w = dimensions[2], .h = dimensions[3]};
					if(twfAddSubframe(&tw->frames[tw->frameNum-1], &baseSubframe, &subframeCapacity) < 0){
						/** Memory allocation failure. **/
						twaDelete(&tempAnim);
						twDelete(tw);
						fclose(texInfo);
						return -1;
					}

				}else{
					printf("Error loading texture wrapper \"%s\": Texture sub-command \"subframe\" invoked on line %u without specifying a multiline texture.\n", fullPath, currentLine);
				}


			// Load normal map
			}else if(lineLength >= 8 && strncmp(line, "normal ", 7) == 0){
				if(currentCommand == 0){
					/*char *texPath = memAllocate((lineLength-7) * sizeof(char));
					strcpy(texPath, line+7);
					tLoad(((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->normals, prgPath, texPath);
					memFree(texPath);*/
					printf("Error loading texture wrapper \"%s\": Texture sub-command \"normal\" is awaiting implementation.\n", fullPath);
				}else{
					printf("Error loading texture wrapper \"%s\": Texture sub-command \"normal\" invoked on line %u without specifying a multiline texture.\n", fullPath, currentLine);
				}


			// Load specular map
			}else if(lineLength >= 10 && strncmp(line, "specular ", 9) == 0){
				if(currentCommand == 0){
					/*char *texPath = memAllocate((lineLength-9) * sizeof(char));
					strcpy(texPath, line+9);
					tLoad(((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->specular, prgPath, texPath);
					memFree(texPath);*/
					printf("Error loading texture wrapper \"%s\": Texture sub-command \"specular\" is awaiting implementation.\n", fullPath);
				}else{
					printf("Error loading texture wrapper \"%s\": Texture sub-command \"specular\" invoked on line %u without specifying a multiline texture.\n", fullPath, currentLine);
				}


			// New texture animation
			}else if(lineLength >= 9 && strncmp(line, "animation", 9) == 0){
				// Reset tempAnim.
				if(strrchr(line+9, '{')){
					twaInit(&tempAnim);
					animframeCapacity = ANIMFRAME_START_CAPACITY;
					if(twaNew(&tempAnim, animframeCapacity) < 0){
						/** Memory allocation failure. **/
						twDelete(tw);
						fclose(texInfo);
						return -1;
					}
					currentCommand = 1;
				}else{
					// Worth it?
					printf("Error loading texture wrapper \"%s\": Animation command at line %u does not contain a brace.\n", fullPath, currentLine);
				}


			// Make the current animation loop.
			}else if(lineLength >= 6 && strncmp(line, "loop ", 5) == 0){
				if(currentCommand == 1){
					tempAnim.animData.desiredLoops = strtol(line+5, NULL, 0);
				}else{
					printf("Error loading texture wrapper \"%s\": Animation sub-command \"loop\" invoked on line %u without specifying an animation.\n", fullPath, currentLine);
				}


			// Frame macro
			}else if(lineLength >= 16 && strncmp(line, "fMacro ", 7) == 0){

				if(currentCommand == 1){

					// Loads start and end textures, start and end subframes and the frame delays.
					frameIndex_t i;
					frameIndex_t textures[2];
					frameIndex_t subframes[2];
					float frameDelay = 0.f;
					const char *token = strtok(line+7, "/");
					for(i = 0; i < 5; ++i){
						if(i < 2){
							textures[i] = strtoul(token, NULL, 0);
						}else if(i < 4){
							subframes[i-2] = strtoul(token, NULL, 0);
						}else{
							frameDelay = strtod(token, NULL);
						}
						token = strtok(NULL, "/");
					}

					if(frameDelay > 0){
						frameIndex_t j;
						for(i = textures[0]; i <= textures[1]; ++i){
							if(i < tw->frameNum){
								for(j = subframes[0]; j <= subframes[1]; ++j){
									if(j < tw->frames[tw->frameNum-1].subframeNum){
										// Convert frameDelay to frameEnd.
										float frameEnd = frameDelay;
										if(tempAnim.animData.frameNum > 0){
											frameEnd += tempAnim.animData.frameDelays[tempAnim.animData.frameNum-1];
										}
										if(twaAddFrame(&tempAnim, i, j, frameEnd, &animframeCapacity) < 0){
											/** Memory allocation failure. **/
											twDelete(tw);
											fclose(texInfo);
											return -1;
										}
									}else{
										break;
									}
								}
							}else{
								break;
							}
						}
					}

				}else{
					printf("Error loading texture wrapper \"%s\": Animation sub-command \"fMacro\" invoked on line %u without specifying an animation.\n", fullPath, currentLine);
				}


			// New animation frame
			}else if(lineLength >= 11 && strncmp(line, "frame ", 6) == 0){

				if(currentCommand == 1){

					// Parse frame information.
					frameIndex_t i;
					size_t frameID = 0, subframeID = 0;
					float frameDelay = 0.f;
					const char *token = strtok(line+6, "/");
					for(i = 0; i < 3; ++i){
						switch(i){
							case 0:
								frameID = strtoul(token, NULL, 0);
							break;
							case 1:
								subframeID = strtoul(token, NULL, 0);
							break;
							default:
								frameDelay = strtod(token, NULL);
						}
						token = strtok(NULL, "/");
					}

					// Validate the frame information.
					if(frameDelay > 0 && frameID < tw->frameNum && subframeID < tw->frames[tw->frameNum-1].subframeNum){
						// Convert frameDelay to frameEnd.
						if(tempAnim.animData.frameNum > 0){
							frameDelay += tempAnim.animData.frameDelays[tempAnim.animData.frameNum-1];
						}
						if(twaAddFrame(&tempAnim, frameID, subframeID, frameDelay, &animframeCapacity) < 0){
							/** Memory allocation failure. **/
							twDelete(tw);
							fclose(texInfo);
							return -1;
						}
					}

				}else{
					printf("Error loading texture wrapper \"%s\": Animation sub-command \"frame\" invoked on line %u without specifying an animation.\n", fullPath, currentLine);
				}

			}

			// Close the current multiline command
			if(lineLength > 0 && line[lineLength-1] == '}'){
				if(currentCommand == 0){
					// If a textureFrame was being worked on and has no subframes, add the default one.
					if(tw->frames[tw->frameNum-1].subframeNum == 0){
						if(twfAddDefaultSubframe(&tw->frames[tw->frameNum-1], subframeCapacity) < 0){
							/** Memory allocation failure. **/
							twaDelete(&tempAnim);
							twDelete(tw);
							fclose(texInfo);
							return -1;
						}
					}else{
						// twfAddDefaultSubframe() automatically resizes to fit.
						if(twfResizeToFit(&tw->frames[tw->frameNum-1], subframeCapacity) < 0){
							/** Memory allocation failure. **/
							twaDelete(&tempAnim);
							twDelete(tw);
							fclose(texInfo);
							return -1;
						}
					}
				}else if(currentCommand == 1){
					// If a valid animation was being worked on, save it and continue.
					if(tempAnim.animData.frameNum > 0){
						if(twaResizeToFit(&tempAnim, animframeCapacity) < 0){
							/** Memory allocation failure. **/
							twDelete(tw);
							fclose(texInfo);
							return -1;
						}
						if(twAddAnim(tw, &tempAnim, &animCapacity) < 0){
							/** Memory allocation failure. **/
							twaDelete(&tempAnim);
							fclose(texInfo);
							return -1;
						}
					}
				}else{
					printf("Error loading texture wrapper \"%s\": Stray brace on line %u.\n", fullPath, currentLine);
				}
				currentCommand = -1;
			}

		}

		fclose(texInfo);

	}else{
		printf("Error loading texture wrapper \"%s\": Could not open file.\n", fullPath);
		return 0;
	}

	// Check if any textures were loaded.
	if(tw->frameNum == 0){

		// Otherwise build the default frame.
		twFrame tempFrame;
		twfInit(&tempFrame);
		subframeCapacity = 1;
		printf("Error loading texture wrapper \"%s\": No textures were loaded.\n", fullPath);

		if(
			twfNew(&tempFrame, subframeCapacity) < 0 ||
			twfAddDefaultSubframe(&tempFrame, subframeCapacity) < 0 ||
			twAddFrame(tw, &tempFrame, &frameCapacity) < 0
		){
			/** Memory allocation failure. **/
			twfDelete(&tempFrame);
			twDelete(tw);
			return -1;
		}

		tw->frames[0].diffuse = moduleTextureGetDefault();

	// If they were, check if the last texture added has any subframes. If it doesn't, add the default one.
	}else if(tw->frames[tw->frameNum-1].subframeNum == 0){
		if(twfAddDefaultSubframe(&tw->frames[tw->frameNum-1], subframeCapacity) < 0){
			/** Memory allocation failure. **/
			twDelete(tw);
			return -1;
		}
	// If the last texture added has subframes, shrink the subframe vector to fit the amount of elements in it.
	}else{
		if(twfResizeToFit(&tw->frames[tw->frameNum-1], subframeCapacity) < 0){
			/** Memory allocation failure. **/
			twDelete(tw);
			return -1;
		}
	}

	// Check if any animations were loaded.
	if(tw->animationNum == 0){
		if(tempAnim.animData.frameNum > 0){
			// If an animation was being worked on, just resize it before adding it.
			if(twaResizeToFit(&tempAnim, animframeCapacity) < 0){
				/** Memory allocation failure. **/
				twaDelete(&tempAnim);
				twDelete(tw);
				return -1;
			}
		}else{
			// Otherwise build the default animation.
			animframeCapacity = 1;
			if(
				twaNew(&tempAnim, animframeCapacity) < 0 ||
				twaAddFrame(&tempAnim, 0, 0, 0.f, &animframeCapacity) < 0
			){
				/** Memory allocation failure. **/
				twaDelete(&tempAnim);
				twDelete(tw);
				return -1;
			}
		}
		// Add the new tempAnim to tw->animations.
		if(twAddAnim(tw, &tempAnim, &animCapacity) < 0){
			/** Memory allocation failure. **/
			twaDelete(&tempAnim);
			twDelete(tw);
			return -1;
		}
	}

	if(tempAnim.animData.frameNum == 0){
		twaDelete(&tempAnim);
	}

	/**
	***
	***
	***
	***
	***
	**/
	if(twResizeToFit(tw, frameCapacity, animCapacity) < 0){
		return -1;
	}

	// Generate a name based off the file path.
	tw->name = fileGenerateResourceName(filePath, fileLength);
	if(tw->name == NULL){
		/** Memory allocation failure. **/
		twDelete(tw);
		return -1;
	}

	return 1;

}

return_t twDefault(textureWrapper *const restrict tw){

	twFrame tempFrame;
	twAnim tempAnim;
	frameIndex_t frameCapacity = 1;
	animIndex_t animCapacity = 1;

	twInit(tw);

	tw->name = memAllocate(8*sizeof(char));
	if(tw->name == NULL){
		/** Memory allocation failure. **/
		return -1;
	}

	tw->frames = memAllocate(frameCapacity*sizeof(twFrame));
	if(tw->frames == NULL){
		/** Memory allocation failure. **/
		memFree(tw->name);
		return -1;
	}

	tw->animations = memAllocate(animCapacity*sizeof(twAnim));
	if(tw->animations == NULL){
		/** Memory allocation failure. **/
		memFree(tw->frames);
		memFree(tw->name);
		return -1;
	}

	twfInit(&tempFrame);
	twaInit(&tempAnim);

	if(
		twfNew(&tempFrame, 1) < 0 ||
		twfAddDefaultSubframe(&tempFrame, 1) < 0 ||
		twAddFrame(tw, &tempFrame, &frameCapacity) < 0
	){
		/** Memory allocation failure. **/
		twfDelete(&tempFrame);
		twDelete(tw);
		return -1;
	}

	if(
		twaNew(&tempAnim, 1) < 0 ||
		twaAddFrame(&tempAnim, 0, 0, 0.f, &frameCapacity) < 0 ||
		twAddAnim(tw, &tempAnim, &animCapacity) < 0
	){
		/** Memory allocation failure. **/
		twaDelete(&tempAnim);
		twDelete(tw);
		return -1;
	}

	tw->name[0] = 'd';
	tw->name[1] = 'e';
	tw->name[2] = 'f';
	tw->name[3] = 'a';
	tw->name[4] = 'u';
	tw->name[5] = 'l';
	tw->name[6] = 't';
	tw->name[7] = '\0';
	tw->frames[0].diffuse = moduleTextureGetDefault();
	return 1;

}

void twDelete(textureWrapper *const restrict tw){
	if(tw->frames != NULL){
		twFrame *f = tw->frames;
		const twFrame *const fLast = &f[tw->frameNum];
		for(; f < fLast; ++f){
			twfDelete(f);
		}
		memFree(tw->frames);
	}
	if(tw->animations != NULL){
		twAnim *a = tw->animations;
		const twAnim *const aLast = &a[tw->animationNum];
		for(; a < aLast; ++a){
			twaDelete(a);
		}
		memFree(tw->animations);
	}
	if(tw->name != NULL){
		memFree(tw->name);
	}
}


static __FORCE_INLINE__ twAnim *twGetAnim(const textureWrapper *const restrict tw, const animIndex_t anim){
	return &tw->animations[anim];
}

static __FORCE_INLINE__ twFrame *twGetAnimFrame(const textureWrapper *const restrict tw, const animIndex_t anim, const frameIndex_t frame){
	/*size_t currentFrameID = *((size_t *)cvGet(&twGetAnim(tw, anim)->frameIDs, frame));*/
	return &tw->frames[tw->animations[anim].frames[frame].frameID];
}

static __FORCE_INLINE__ rectangle *twGetAnimSubframe(const textureWrapper *const restrict tw, const animIndex_t anim, const frameIndex_t frame){
	/*size_t currentSubframeID = *((size_t *)cvGet(&twGetAnim(tw, anim)->subframeIDs, frame));*/
	twFramePair *f = &tw->animations[anim].frames[frame];
	return &tw->frames[f->frameID].subframes[f->subframeID];
}

static __FORCE_INLINE__ float *twGetAnimFrameDelay(const textureWrapper *const restrict tw, const animIndex_t anim, const frameIndex_t frame){
	return &tw->animations[anim].animData.frameDelays[frame];
}

void twiInit(twInstance *const restrict twi, const textureWrapper *const tw){
	twi->tw = tw;
	twi->timeMod = 1.f;
	twi->currentAnim = 0;
	animInstInit(&twi->animator);
}

__FORCE_INLINE__ void twiAnimate(twInstance *const restrict twi, const float elapsedTime){
	animAdvance(&twi->animator, &twi->tw->animations[twi->currentAnim].animData, elapsedTime*twi->timeMod);
}

GLuint twiGetTexWidth(const twInstance *const restrict twi){
	// Make sure the current animation and frame are valid (within proper bounds)
	//if(twi->currentAnim < twi->tw->animationNum &&
	//   twi->animator.currentFrame < twGetAnim(twi->tw, twi->currentAnim)->animData.frameNum){
		return twGetAnimFrame(twi->tw, twi->currentAnim, twi->animator.currentFrame)->diffuse->width;
	//}
	//return 0;
}

GLuint twiGetTexHeight(const twInstance *const restrict twi){
	// Make sure the current animation and frame are valid (within proper bounds)
	//if(twi->currentAnim < twi->tw->animationNum &&
	//   twi->animator.currentFrame < twGetAnim(twi->tw, twi->currentAnim)->animData.frameNum){
		return twGetAnimFrame(twi->tw, twi->currentAnim, twi->animator.currentFrame)->diffuse->height;
	//}
	//return 0;
}

GLuint twiGetTexID(const twInstance *const restrict twi){
	// Make sure the current animation and frame are valid (within proper bounds)
	//if(twi->currentAnim < twi->tw->animationNum &&
	//   twi->animator.currentFrame < twGetAnim(twi->tw, twi->currentAnim)->animData.frameNum){
		return twGetAnimFrame(twi->tw, twi->currentAnim, twi->animator.currentFrame)->diffuse->id;
	//}
	//return 0;
}

float twiGetFrameWidth(const twInstance *const restrict twi){
	return twGetAnimSubframe(twi->tw, twi->currentAnim, twi->animator.currentFrame)->w;
}

float twiGetFrameHeight(const twInstance *const restrict twi){
	return twGetAnimSubframe(twi->tw, twi->currentAnim, twi->animator.currentFrame)->h;
}

void twiGetFrameInfo(const twInstance *const restrict twi, float *const restrict x, float *const restrict y, float *const restrict w, float *const restrict h, GLuint *const restrict frameTexID, const float interpT){

	// Make sure the current animation and frame are valid (within proper bounds)
	//if(
	//	twi->currentAnim < twi->tw->animationNum &&
	//	twi->animator.currentFrame < twGetAnim(twi->tw, twi->currentAnim)->animData.frameNum
	//){

		frameIndex_t startFrame;
		twFramePair *framePair;
		rectangle *subframe;

		animGetRenderData(&twi->animator, &twGetAnim(twi->tw, twi->currentAnim)->animData, interpT,
		                  &startFrame, NULL, NULL);

		framePair = &twi->tw->animations[twi->currentAnim].frames[startFrame];
		subframe = &twi->tw->frames[framePair->frameID].subframes[framePair->subframeID];

		*x = subframe->x;
		*y = subframe->y;
		*w = subframe->w;
		*h = subframe->h;
		*frameTexID = twi->tw->frames[framePair->frameID].diffuse->id;

	//}else{

	//	*x = 0.f;
	//	*y = 0.f;
	//	*w = 0.f;
	//	*h = 0.f;
	//	*frameTexID = 0;

	//}

}

return_t twiContainsTranslucency(const twInstance *const restrict twi){
	// Make sure the current animation and frame are valid (within proper bounds)
	if(twi->currentAnim < twi->tw->animationNum &&
	   twi->animator.currentFrame < twGetAnim(twi->tw, twi->currentAnim)->animData.frameNum){
		return twGetAnimFrame(twi->tw, twi->currentAnim, twi->animator.currentFrame)->diffuse->translucent;
	}
	return 0;
}
