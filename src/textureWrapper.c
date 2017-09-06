#include "textureWrapper.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define FRAME_START_CAPACITY 128
#define ANIM_START_CAPACITY 128
#define SUBFRAME_START_CAPACITY 128
#define ANIMFRAME_START_CAPACITY 128

/** Remove printf()s **/

static unsigned char twfInit(twFrame *twf, const size_t subframeCapacity){
	twf->subframes = malloc(subframeCapacity*sizeof(twBounds));
	if(twf->subframes == NULL){
		printf("Error loading texture wrapper: Memory allocation failure.\n");
		return 0;
	}
	return 1;
}

static unsigned char twfAddSubframe(twFrame *twf, const twBounds *sf, size_t *subframeCapacity){
	if(twf->subframeNum == *subframeCapacity){
		*subframeCapacity *= 2;
		twf->subframes = realloc(twf->subframes, *subframeCapacity*sizeof(twBounds));
		if(twf->subframes == NULL){
			printf("Error loading texture wrapper: Memory allocation failure.\n");
			return 0;
		}
	}
	twf->subframes[twf->subframeNum++] = *sf;
	return 1;
}

static unsigned char twfAddDefaultSubframe(twFrame *twf, const size_t subframeCapacity){
	if(subframeCapacity != twf->subframeNum+1){
		twf->subframes = realloc(twf->subframes, (twf->subframeNum+1)*sizeof(twBounds));
		if(twf->subframes == NULL){
			printf("Error loading texture wrapper: Memory allocation failure.\n");
			return 0;
		}
	}
	twf->subframes[twf->subframeNum].x = 0.f;
	twf->subframes[twf->subframeNum].y = 0.f;
	twf->subframes[twf->subframeNum].w = twf->baseTexture->width;
	twf->subframes[twf->subframeNum].h = twf->baseTexture->height;
	++twf->subframeNum;
	return 1;
}

static unsigned char twfResizeToFit(twFrame *twf, const size_t subframeCapacity){
	if(twf->subframeNum != subframeCapacity){
		twf->subframes = realloc(twf->subframes, twf->subframeNum*sizeof(twBounds));
		if(twf->subframes == NULL){
			printf("Error loading texture wrapper: Memory allocation failure.\n");
			return 0;
		}
	}
	return 1;
}

static void twfDelete(twFrame *twf){
	if(twf->subframes != NULL){
		free(twf->subframes);
	}
}

static unsigned char twaInit(twAnim *twa, const size_t animframeCapacity){
	twa->frameIDs = malloc(animframeCapacity*sizeof(size_t));
	if(twa->frameIDs == NULL){
		printf("Error loading texture wrapper: Memory allocation failure.\n");
		return 0;
	}
	twa->subframeIDs = malloc(animframeCapacity*sizeof(size_t));
	if(twa->subframeIDs == NULL){
		printf("Error loading texture wrapper: Memory allocation failure.\n");
		free(twa->frameIDs);
		return 0;
	}
	twa->animData.frameDelays = malloc(animframeCapacity*sizeof(float));
	if(twa->animData.frameDelays == NULL){
		printf("Error loading texture wrapper: Memory allocation failure.\n");
		free(twa->frameIDs);
		free(twa->subframeIDs);
		return 0;
	}
	twa->animData.desiredLoops = -1;
	twa->animData.frameNum = 0;  // Current texture animation being worked on
	return 1;
}

static unsigned char twaAddFrame(twAnim *twa, const size_t f, const size_t sf, const float d, size_t *animframeCapacity){
	if(twa->animData.frameNum == *animframeCapacity){
		*animframeCapacity *= 2;
		twa->frameIDs = realloc(twa->frameIDs, *animframeCapacity*sizeof(size_t));
		if(twa->frameIDs == NULL){
			printf("Error loading texture wrapper: Memory allocation failure.\n");
			return 0;
		}
		twa->subframeIDs = realloc(twa->subframeIDs, *animframeCapacity*sizeof(size_t));
		if(twa->subframeIDs == NULL){
			printf("Error loading texture wrapper: Memory allocation failure.\n");
			free(twa->frameIDs);
			return 0;
		}
		twa->animData.frameDelays = realloc(twa->animData.frameDelays, *animframeCapacity*sizeof(float));
		if(twa->animData.frameDelays == NULL){
			printf("Error loading texture wrapper: Memory allocation failure.\n");
			free(twa->frameIDs);
			free(twa->subframeIDs);
			return 0;
		}
	}
	twa->frameIDs[twa->animData.frameNum] = f;
	twa->subframeIDs[twa->animData.frameNum] = sf;
	twa->animData.frameDelays[twa->animData.frameNum] = d;
	++twa->animData.frameNum;
	return 1;
}

static unsigned char twaResizeToFit(twAnim *twa, const size_t animframeCapacity){
	if(twa->animData.frameNum != animframeCapacity){
		twa->frameIDs = realloc(twa->frameIDs, twa->animData.frameNum*sizeof(size_t));
		if(twa->frameIDs == NULL){
			printf("Error loading texture wrapper: Memory allocation failure.\n");
			return 0;
		}
		twa->subframeIDs = realloc(twa->subframeIDs, twa->animData.frameNum*sizeof(size_t));
		if(twa->subframeIDs == NULL){
			printf("Error loading texture wrapper: Memory allocation failure.\n");
			free(twa->frameIDs);
			return 0;
		}
		twa->animData.frameDelays = realloc(twa->animData.frameDelays, twa->animData.frameNum*sizeof(float));
		if(twa->animData.frameDelays == NULL){
			printf("Error loading texture wrapper: Memory allocation failure.\n");
			free(twa->frameIDs);
			free(twa->subframeIDs);
			return 0;
		}
	}
	return 1;
}

static void twaDelete(twAnim *twa){
	if(twa->frameIDs != NULL){
		free(twa->frameIDs);
	}
	if(twa->subframeIDs != NULL){
		free(twa->subframeIDs);
	}
	if(twa->animData.frameDelays != NULL){
		free(twa->animData.frameDelays);
	}
}

static unsigned char twAddFrame(textureWrapper *tw, const twFrame *f, size_t *frameCapacity){
	if(tw->frameNum == *frameCapacity){
		*frameCapacity *= 2;
		tw->frames = realloc(tw->frames, *frameCapacity*sizeof(twFrame));
		if(tw->frames == NULL){
			printf("Error loading texture wrapper: Memory allocation failure.\n");
			free(tw->animations);
			return 0;
		}
	}
	tw->frames[tw->frameNum++] = *f;
	return 1;
}

static unsigned char twAddAnim(textureWrapper *tw, const twAnim *a, size_t *animCapacity){
	if(tw->animationNum == *animCapacity){
		*animCapacity *= 2;
		tw->animations = realloc(tw->animations, *animCapacity*sizeof(twAnim));
		if(tw->animations == NULL){
			printf("Error loading texture wrapper: Memory allocation failure.\n");
			free(tw->frames);
			return 0;
		}
	}
	tw->animations[tw->animationNum++] = *a;
	return 1;
}

static unsigned char twResizeToFit(textureWrapper *tw, const size_t frameCapacity, const size_t animCapacity){
	if(tw->frameNum != frameCapacity){
		tw->frames = realloc(tw->frames, tw->frameNum*sizeof(twFrame));
		if(tw->frames == NULL){
			printf("Error loading texture wrapper: Memory allocation failure.\n");
			return 0;
		}
	}
	if(tw->animationNum != animCapacity){
		tw->animations = realloc(tw->animations, tw->animationNum*sizeof(twAnim));
		if(tw->animations == NULL){
			printf("Error loading texture wrapper: Memory allocation failure.\n");
			free(tw->frames);
			return 0;
		}
	}
	return 1;
}

void twInit(textureWrapper *tw){
	tw->name = NULL;
	tw->frameNum = 0;
	tw->animationNum = 0;
	tw->frames = NULL;
	tw->animations = NULL;
}

unsigned char twLoad(textureWrapper *tw, const char *prgPath, const char *filePath, cVector *allTextures){

	twInit(tw);

	size_t frameCapacity = FRAME_START_CAPACITY;
	tw->frames = malloc(frameCapacity*sizeof(twFrame));
	if(tw->frames == NULL){
		printf("Error loading texture wrapper: Memory allocation failure.\n");
		return 0;
	}

	size_t animCapacity = ANIM_START_CAPACITY;
	tw->animations = malloc(animCapacity*sizeof(twAnim));
	if(tw->animations == NULL){
		printf("Error loading texture wrapper: Memory allocation failure.\n");
		free(tw->frames);
		return 0;
	}

	twAnim tempAnim;
	tempAnim.animData.desiredLoops = -1;
	tempAnim.animData.frameNum = 0;

	size_t animframeCapacity = 0;
	size_t subframeCapacity = 0;

	int currentCommand = -1;       // The current multiline command type (-1 = none, 0 = texture, 1 = animation)
	unsigned int currentLine = 0;  // Current file line being read


	const size_t pathLen = strlen(prgPath);
	const size_t fileLen = strlen(filePath);
	char *fullPath = malloc((pathLen+fileLen+1) * sizeof(char));
	memcpy(fullPath, prgPath, pathLen);
	memcpy(fullPath+pathLen, filePath, fileLen);
	fullPath[pathLen+fileLen] = '\0';
	FILE *texInfo = fopen(fullPath, "r");
	char lineFeed[1024];
	char *line;
	char compare[1024];
	size_t lineLength;

	if(texInfo != NULL){
		while(fgets(lineFeed, sizeof(lineFeed), texInfo)){

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
			if(lineLength >= 6 && strncpy(compare, line, 5) && (compare[5] = '\0') == 0 && strcmp(compare, "name ") == 0){
				tw->name = malloc((lineLength-4) * sizeof(char));
				if(tw->name != NULL){
					strncpy(tw->name, line+5, lineLength-5);
					tw->name[lineLength-5] = '\0';
				}


			// Close current multiline command
			}else if(lineLength > 0 && line[0] == '}'){
				if(currentCommand == 0){
					// If a textureFrame was being worked on and has no subframes, add the default one
					if(tw->frames[tw->frameNum-1].subframeNum == 0){
						if(!twfAddDefaultSubframe(&tw->frames[tw->frameNum-1], subframeCapacity)){
							twaDelete(&tempAnim);
							twDelete(tw);
							return 0;
						}
					}else{
						// twfAddDefaultSubframe() automatically resizes to fit
						if(!twfResizeToFit(&tw->frames[tw->frameNum-1], subframeCapacity)){
							twaDelete(&tempAnim);
							twDelete(tw);
							return 0;
						}
					}

				}else if(currentCommand == 1){
					// If a valid animation was being worked on, save it and continue
					if(tempAnim.animData.frameNum > 0){
						if(!twaResizeToFit(&tempAnim, animframeCapacity)){
							twDelete(tw);
							return 0;
						}
						if(!twAddAnim(tw, &tempAnim, &animCapacity)){
							twaDelete(&tempAnim);
							return 0;
						}
					}
				}

				currentCommand = -1;


			// New texture frame
			}else if(lineLength >= 9 && strncpy(compare, line, 8) && (compare[8] = '\0') == 0 && strcmp(compare, "texture ") == 0){

				// A multiline command is already in progress; try to close it and continue
				if(currentCommand != -1){

					printf("Error loading texture wrapper: Trying to start a multiline command at line %u while another is already in progress; "
						   "I will attempt to close the current command.\n", currentLine);

					// If the multiline command is a texture...
					if(currentCommand == 0){
						// If a textureFrame was being worked on and has no subframes, add the default one
						if(tw->frames[tw->frameNum-1].subframeNum == 0){
							if(!twfAddDefaultSubframe(&tw->frames[tw->frameNum-1], subframeCapacity)){
								twaDelete(&tempAnim);
								twDelete(tw);
								return 0;
							}
						}else{
							// twfAddDefaultSubframe() automatically resizes to fit
							if(!twfResizeToFit(&tw->frames[tw->frameNum-1], subframeCapacity)){
								twaDelete(&tempAnim);
								twDelete(tw);
								return 0;
							}
						}

					}else if(currentCommand == 1){
						// If a valid animation is being worked on, save it and continue
						if(tempAnim.animData.frameNum > 0){
							if(!twaResizeToFit(&tempAnim, animframeCapacity)){
								twDelete(tw);
								return 0;
							}
							if(!twAddAnim(tw, &tempAnim, &animCapacity)){
								twaDelete(&tempAnim);
								return 0;
							}
						}
					}

					currentCommand = -1;

				}

				// Create a new texture frame
				twFrame tempFrame;
				tempFrame.baseTexture = NULL;
				tempFrame.normalTexture = NULL;
				tempFrame.subframeNum = 0;

				const char *firstQuote = strchr(line, '"');
				const char *lastQuote = strrchr(line, '"');
				size_t pathBegin;
				size_t pathLength;
				unsigned char skipLoad = 0;
				// If the texture identifier wasn't surrounded by quotes, don't give up:
				if(firstQuote != NULL && lastQuote > firstQuote){
					pathBegin = firstQuote-line+1;
					pathLength = lastQuote-line-pathBegin;
				}else{
					pathBegin = 8;
					pathLength = lineLength-8;
				}
				char *texPath = malloc((pathLength+1) * sizeof(char));
				strncpy(texPath, line+pathBegin, pathLength);
				texPath[pathLength] = '\0';

				/** Should the allTextures vector be used? It's not as nice or as modular
				but if the assetHandler system is decided on it'll probably be better **/
				// Look for texture name in allTextures
				for(i = 0; i < allTextures->size; ++i){
					texture *tempTex = (texture *)cvGet(allTextures, i);
					if(strcmp(texPath, tempTex->name) == 0){
						tempFrame.baseTexture = (texture *)cvGet(allTextures, i);
						skipLoad = 1;
						i = allTextures->size;
					}
				}
				// If the texture path is surrounded by quotes, try and load it
				if(!skipLoad){
					texture tempTex;
					if(tLoad(&tempTex, prgPath, texPath)){
						cvPush(allTextures, (void *)&tempTex, sizeof(tempTex));  // Add it to allTextures
						tempFrame.baseTexture = (texture *)cvGet(allTextures, allTextures->size-1);
						//tempFrame.baseTexture = &tempTex;
					}
				}

				// If the texture was loaded successfully:
				if(tempFrame.baseTexture != NULL){
					// Check if the command spans multiple lines (it contains an opening brace at the end)
					if(strrchr(line, '{') > line+pathBegin+1+pathLength){
						subframeCapacity = SUBFRAME_START_CAPACITY;
						twfInit(&tempFrame, subframeCapacity);
						currentCommand = 0;

					// If it doesn't, add the default subframe
					}else{
						subframeCapacity = 1;
						twfInit(&tempFrame, subframeCapacity);
						if(!twfAddDefaultSubframe(&tempFrame, subframeCapacity)){
							free(texPath);
							twfDelete(&tempFrame);
							twaDelete(&tempAnim);
							twDelete(tw);
							return 0;
						}

					}
					// Add the frame to tw->frames
					if(!twAddFrame(tw, &tempFrame, &frameCapacity)){
						free(texPath);
						twfDelete(&tempFrame);
						twaDelete(&tempAnim);
						return 0;
					}
				}

				free(texPath);


			// Subframe macro
			}else if(lineLength >= 14 && strncpy(compare, line, 7) && (compare[7] = '\0') == 0 && strcmp(compare, "sMacro ") == 0){

				if(currentCommand == 0){

					// Loads number of subframes to create, main dimension to loop in (x or y) and the subframe dimensions
					size_t numberOfFrames = 0;
					char macroDirection = ' ';
					float dimensions[4] = {0, 0, 0, 0};
					char *token = strtok(line+7, "/");
					for(i = 0; i < 6; ++i){
						if(i == 0){
							numberOfFrames = strtoul(token, NULL, 0);
						}else if(i == 1){
							macroDirection = token[0];
						}else{
							dimensions[i-2] = strtod(token, NULL);
						}
						token = strtok(NULL, "/");
					}

					const unsigned int currentTexW = tw->frames[tw->frameNum-1].baseTexture->width;
					const unsigned int currentTexH = tw->frames[tw->frameNum-1].baseTexture->height;

					// Automatically generate subframes for a sprite sheet
					for(i = 1; i <= numberOfFrames; ++i){
						if(dimensions[0] + dimensions[2] <= currentTexW && dimensions[1] + dimensions[3] <= currentTexH){
							twBounds baseSubframe = {.x = dimensions[0], .y = dimensions[1], .w = dimensions[2], .h = dimensions[3]};
							if(!twfAddSubframe(&tw->frames[tw->frameNum-1], &baseSubframe, &subframeCapacity)){
								twaDelete(&tempAnim);
								twDelete(tw);
								return 0;
							}
							if(macroDirection == 'x'){  // Adds frames from left to right before resetting and moving down
								dimensions[0] = (unsigned int)(i * dimensions[2]) % currentTexW;
								dimensions[1] = (unsigned int)(i * dimensions[2]) / currentTexW * dimensions[3];
							}else if(macroDirection == 'y'){  // Adds frames from top to bottom before resetting and moving right
								dimensions[0] = (unsigned int)(i * dimensions[3]) / currentTexH * dimensions[2];
								dimensions[1] = (unsigned int)(i * dimensions[3]) % currentTexH;
							}else{
								printf("Error loading texture wrapper: sMacro command at line %u has an invalid direction. Only one frame could be loaded.\n", currentLine);
								i = numberOfFrames+1;
							}
						}else{
							printf("Error loading texture wrapper: sMacro command at line %u could not load %u frame(s).\n", currentLine, numberOfFrames-i+1);
							i = numberOfFrames+1;
						}
					}

				}else{
					printf("Error loading texture wrapper: Texture sub-command \"sMacro\" invoked on line %u without specifying a multiline texture.\n", currentLine);
				}


			// New subframe
			}else if(lineLength >= 16 && strncpy(compare, line, 9) && (compare[9] = '\0') == 0 && strcmp(compare, "subframe ") == 0){

				if(currentCommand == 0){

					// Create a new subframe and add it to the current texture frame
					float dimensions[4] = {0, 0, 0, 0};
					char *token = strtok(line+9, "/");
					for(i = 0; i < 4; ++i){
						if(token != NULL){
							dimensions[i] = strtod(token, NULL);
							token = strtok(NULL, "/");
						}else{
							i = 4;
						}
					}
					twBounds baseSubframe = {.x = dimensions[0], .y = dimensions[1], .w = dimensions[2], .h = dimensions[3]};
					if(!twfAddSubframe(&tw->frames[tw->frameNum-1], &baseSubframe, &subframeCapacity)){
						twaDelete(&tempAnim);
						twDelete(tw);
						return 0;
					}

				}else{
					printf("Error loading texture wrapper: Texture sub-command \"subframe\" invoked on line %u without specifying a multiline texture.\n", currentLine);
				}


			// Load normal map
			}else if(lineLength >= 8 && strncpy(compare, line, 7) && (compare[7] = '\0') == 0 && strcmp(compare, "normal ") == 0){
				if(currentCommand == 0){
					/*char *texPath = malloc((lineLength-6) * sizeof(char));
					strcpy(texPath, line+7);
					tLoad(((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->normalTexture, prgPath, texPath);
					free(texPath);*/
					printf("Error loading texture wrapper: Texture sub-command \"normal\" is awaiting implementation.\n");
				}else{
					printf("Error loading texture wrapper: Texture sub-command \"normal\" invoked on line %u without specifying a multiline texture.\n", currentLine);
				}


			// New texture animation
			}else if(lineLength >= 9 && strncpy(compare, line, 9) && (compare[9] = '\0') == 0 && strcmp(compare, "animation") == 0){
				// Reset tempAnim
				animframeCapacity = ANIMFRAME_START_CAPACITY;
				if(!twaInit(&tempAnim, animframeCapacity)){
					twDelete(tw);
					return 0;
				}
				currentCommand = 1;


			// Make the current animation loop
			}else if(lineLength >= 6 && strncpy(compare, line, 5) && (compare[5] = '\0') == 0 && strcmp(compare, "loop ") == 0){
				if(currentCommand == 1){
					tempAnim.animData.desiredLoops = strtol(line+5, NULL, 0);
				}else{
					printf("Error loading texture wrapper: Animation sub-command \"loop\" invoked on line %u without specifying an animation.\n", currentLine);
				}


			// Frame macro
			}else if(lineLength >= 16 && strncpy(compare, line, 7) && (compare[7] = '\0') == 0 && strcmp(compare, "fMacro ") == 0){

				if(currentCommand == 1){

					// Loads start and end textures, start and end subframes and the frame delays
					size_t textures[2] = {0, 0};
					size_t subframes[2] = {0, 0};
					float frameDelay = 0.f;
					char *token = strtok(line+7, "/");
					for(i = 0; i < 6; ++i){
						if(token != NULL){
							if(i < 2){
								textures[i] = strtoul(token, NULL, 0);
							}else if(i < 4){
								subframes[i-2] = strtoul(token, NULL, 0);
							}else{
								frameDelay = strtod(token, NULL);
							}
							token = strtok(NULL, "/");
						}else{
							i = 6;
						}
					}

					if(frameDelay > 0){
						size_t j;
						for(i = textures[0]; i <= textures[1]; ++i){
							if(i < tw->frameNum){
								for(j = subframes[0]; j <= subframes[1]; ++j){
									if(j < tw->frames[tw->frameNum-1].subframeNum){
										if(!twaAddFrame(&tempAnim, i, j, frameDelay, &animframeCapacity)){
											twDelete(tw);
											return 0;
										}
									}else{
										j = subframes[1]+1;
									}
								}
							}else{
								i = textures[1]+1;
							}
						}
					}

				}else{
					printf("Error loading texture wrapper: Animation sub-command \"fMacro\" invoked on line %u without specifying an animation.\n", currentLine);
				}


			// New animation frame
			}else if(lineLength >= 11 && strncpy(compare, line, 6) && (compare[6] = '\0') == 0 && strcmp(compare, "frame ") == 0){

				if(currentCommand == 1){

					// Parse frame information
					size_t frameID = 0, subframeID = 0;
					float frameDelay = 0.f;
					char *token = strtok(line+6, "/");
					for(i = 0; i < 6; ++i){
						if(token != NULL){
							if(i == 0){
								frameID = strtoul(token, NULL, 0);
							}else if(i == 1){
								subframeID = strtoul(token, NULL, 0);
							}else{
								frameDelay = strtod(token, NULL);
							}
							token = strtok(NULL, "/");
						}else{
							i = 6;
						}
					}

					// Validate the frame information
					if(frameDelay > 0 && frameID < tw->frameNum && subframeID < tw->frames[tw->frameNum-1].subframeNum){
						if(!twaAddFrame(&tempAnim, frameID, subframeID, frameDelay, &animframeCapacity)){
							twDelete(tw);
							return 0;
						}
					}

				}else{
					printf("Error loading texture wrapper: Animation sub-command \"frame\" invoked on line %u without specifying an animation.\n", currentLine);
				}

			}

		}

		fclose(texInfo);
		free(fullPath);

	}else{
		printf("Error loading texture wrapper: Couldn't open %s\n", fullPath);
		free(fullPath);
		twaDelete(&tempAnim);
		free(tw->frames);
		free(tw->animations);
		return 0;
	}


	// Check if any animations were loaded
	if(tw->animationNum == 0){
		if(tempAnim.animData.frameNum > 0){
			// If an animation was being worked on, just resize it before adding it
			if(!twaResizeToFit(&tempAnim, animframeCapacity)){
				twDelete(tw);
				return 0;
			}
		}else{
			// Otherwise build the default animation
			animframeCapacity = 1;
			if(!twaInit(&tempAnim, animframeCapacity) || !twaAddFrame(&tempAnim, 0, 0, 0.f, &animframeCapacity)){
				twDelete(tw);
				return 0;
			}
		}
		// Add the new tempAnim to tw->animations
		if(!twAddAnim(tw, &tempAnim, &animCapacity)){
			twaDelete(&tempAnim);
			return 0;
		}
	}

	if(tempAnim.animData.frameNum == 0){
		twaDelete(&tempAnim);
	}

	// Check if any textures were loaded
	if(tw->frameNum == 0){
		printf("Error loading texture wrapper: No textures were loaded.\n");
		return 0;
	// If they were, check if the last texture added has any subframes. If it doesn't, add the default one
	}else if(tw->frames[tw->frameNum-1].subframeNum == 0){
		if(!twfAddDefaultSubframe(&tw->frames[tw->frameNum-1], subframeCapacity)){
			twDelete(tw);
			return 0;
		}
	// If the last texture added has subframes, shrink the subframe vector to fit the amount of elements in it
	}else{
		if(!twfResizeToFit(&tw->frames[tw->frameNum-1], subframeCapacity)){
			twDelete(tw);
			return 0;
		}
	}

	// If no name was given, generate one based off the file name
	if(tw->name == NULL || tw->name[0] == '\0'){
		tw->name = malloc((fileLen+1)*sizeof(char));
		memcpy(tw->name, filePath, fileLen);
		tw->name[fileLen] = '\0';
	}

	return twResizeToFit(tw, frameCapacity, animCapacity);

}

void twDelete(textureWrapper *tw){
	size_t i;
	for(i = 0; i < tw->frameNum; ++i){
		twfDelete(&tw->frames[i]);
	}
	free(tw->frames);
	for(i = 0; i < tw->animationNum; ++i){
		twaDelete(&tw->animations[i]);
	}
	free(tw->animations);
	if(tw->name != NULL){
		free(tw->name);
	}
}


static inline twAnim *twGetAnim(const textureWrapper *tw, const size_t anim){
	return &tw->animations[anim];
}

static inline twFrame *twGetAnimFrame(const textureWrapper *tw, const size_t anim, const size_t frame){
	/*size_t currentFrameID = *((size_t *)cvGet(&twGetAnim(tw, anim)->frameIDs, frame));*/
	return &tw->frames[tw->animations[anim].frameIDs[frame]];
}

static inline twBounds *twGetAnimSubframe(const textureWrapper *tw, const size_t anim, const size_t frame){
	/*size_t currentSubframeID = *((size_t *)cvGet(&twGetAnim(tw, anim)->subframeIDs, frame));*/
	return &tw->frames[tw->animations[anim].frameIDs[frame]].subframes[tw->animations[anim].subframeIDs[frame]];
}

static inline float *twGetAnimFrameDelay(const textureWrapper *tw, const size_t anim, const size_t frame){
	return &tw->animations[anim].animData.frameDelays[frame];
}

void twiInit(twInstance *twi, textureWrapper *tw){
	twi->tw = tw;
	twi->currentAnim = 0;
	twi->timeMod = 1.f;
	twi->animInst.currentLoops = 0;
	twi->animInst.currentFrame = 0;
	twi->animInst.currentFrameProgress = 0.f;
	twi->animInst.currentFrameLength = 0.f;
	twi->animInst.nextFrame = 0;
}

void twiAnimate(twInstance *twi, const float timeElapsed){
	/*if(twi->currentAnim >= twi->tw->animationNum){
		twi->currentAnim = 0;
	}
	if(twi->animInst.currentFrame >= twi->tw->animations[twi->currentAnim].animData.frameNum){
		twi->animInst.currentFrame = 0;
	}
	if(twi->animInst.nextFrame >= twi->tw->animations[twi->currentAnim].animData.frameNum){
		twi->animInst.nextFrame = 0;
	}*/
	animAdvance(&twi->animInst, &twi->tw->animations[twi->currentAnim].animData, timeElapsed*twi->timeMod);
}

/**void twiAnimate(twInstance *twi, const uint32_t currentTick, const float globalDelayMod){

	// Make sure lastUpdate has been set
	if(twi->lastUpdate == 0.f){
		twi->lastUpdate = currentTick;
	}

	const float totalDelayMod = twi->delayMod * globalDelayMod;

	// Only animate if the animation has more than one
	// frame and can still be animated
	if(totalDelayMod != 0.f && twGetAnim(twi->tw, twi->currentAnim)->frameNum > 1 &&
	   (twi->currentLoops < twGetAnim(twi->tw, twi->currentAnim)->desiredLoops ||
	    twGetAnim(twi->tw, twi->currentAnim)->desiredLoops < 0)){

		// Time passed since last update
		float deltaTime = currentTick - twi->lastUpdate;
		// Multiplier applied to the current frame's delay in order to slow down / speed up the animation
		float currentFrameDelay = *twGetAnimFrameDelay(twi->tw, twi->currentAnim, twi->currentFrame)*totalDelayMod;

		* While deltaTime exceeds the time that the current frame should last and the
		texture can still be animated, advance the animation *
		while(deltaTime >= currentFrameDelay &&
		      (twi->currentLoops < twGetAnim(twi->tw, twi->currentAnim)->desiredLoops ||
		       twGetAnim(twi->tw, twi->currentAnim)->desiredLoops < 0)){

			// Subtract the delay from deltaTime and add it to lastUpdate, for the next frame
			deltaTime -= currentFrameDelay;
			twi->lastUpdate += currentFrameDelay;

			// Increase currentFrame and check if it exceeds the number of frames
			if(++twi->currentFrame == twGetAnim(twi->tw, twi->currentAnim)->frameNum){
				// currentFrame has exceeded the number of frames, increase the loop counter
				++twi->currentLoops;
				if(twi->currentLoops < twGetAnim(twi->tw, twi->currentAnim)->desiredLoops ||
				   twGetAnim(twi->tw, twi->currentAnim)->desiredLoops < 0){
					// If the animation can continue to loop, reset it to the first frame
					twi->currentFrame = 0;
				}else{
					// Otherwise set it to the final frame
					twi->currentFrame = twGetAnim(twi->tw, twi->currentAnim)->frameNum-1;
					twi->lastUpdate = currentTick;
				}
			}

			// Update currentFrameDelay based on the new value of currentFrame
			currentFrameDelay = *twGetAnimFrameDelay(twi->tw, twi->currentAnim, twi->currentFrame)*totalDelayMod;

		}

	}

}**/

/*void twiAnimate(twInstance *twi, uint32_t currentTick, float globalDelayMod){
	animAdvance(&twi->animState,
	            &twGetAnim(twi->texWrap, twi->currentAnim)->frameDelays,
	            twGetAnim(twi->texWrap, twi->currentAnim)->desiredLoops,
	            currentTick, twi->delayMod*globalDelayMod);
}*/

/*void twiAnimate(twInstance *twi, float speedMod){
	// Make sure the animation is within the correct bounds
	if(twi->animation >= twi->texWrap->animations.size){
		twi->animation = 0;
		twi->frame = 0;
		twi->t = SDL_GetTicks();
	// Make sure the current frame is valid
	}else if(twi->frame >= twGetAnim(twi->texWrap, twi->animation)->frameIDs.size){
		twi->frame = 0;
		twi->t = SDL_GetTicks();
	// Make sure t has been set
	}else if(twi->t == 0.f){
		twi->t = SDL_GetTicks();
	}
	float totalSpeedMod = twi->speed * speedMod;
	// Only animate the texture if the animation has more than one
	// frame and can still be animated
	if(totalSpeedMod != 0.f &&
	   twGetAnim(twi->texWrap, twi->animation)->frameIDs.size > 1 &&
	   (twGetAnim(twi->texWrap, twi->animation)->loopNum < 0 || !twiAnimFinished(twi))){
		// Current tick
		float currentTick = SDL_GetTicks() * totalSpeedMod;
		* While deltaTime exceeds the time that the current frame should last and the
		texture can still be animated, advance the animation *
		while((currentTick-twi->t) >= *twGetAnimFrameDelay(twi->texWrap, twi->animation, twi->frame) &&
		      (twGetAnim(twi->texWrap, twi->animation)->loopNum < 0 || !twiAnimFinished(twi))){
			// Add the delay to frameProgress and advance the animation
			twi->t += *twGetAnimFrameDelay(twi->texWrap, twi->animation, twi->frame);
			++twi->frame;
			// Reset the animation if frame exceeds the number of frames in the animation
			if(twi->frame == twGetAnim(twi->texWrap, twi->animation)->frameIDs.size){
				++twi->loops;
				// If the animation can loop, set it to the first frame
				if(twGetAnim(twi->texWrap, twi->animation)->loopNum >= twi->loops ||
				   twGetAnim(twi->texWrap, twi->animation)->loopNum < 0){
					twi->frame = 0;
				// Otherwise set it to the final frame
				}else{
					twi->frame = twGetAnim(twi->texWrap, twi->animation)->frameIDs.size-1;
				}
			}
		}
	}
}*/

GLuint twiGetTexWidth(const twInstance *twi){
	// Make sure the current animation and frame are valid (within proper bounds)
	if(twi->currentAnim < twi->tw->animationNum &&
	   twi->animInst.currentFrame < twGetAnim(twi->tw, twi->currentAnim)->animData.frameNum){
		return twGetAnimFrame(twi->tw, twi->currentAnim, twi->animInst.currentFrame)->baseTexture->width;
	}
	return 0;
}

GLuint twiGetTexHeight(const twInstance *twi){
	// Make sure the current animation and frame are valid (within proper bounds)
	if(twi->currentAnim < twi->tw->animationNum &&
	   twi->animInst.currentFrame < twGetAnim(twi->tw, twi->currentAnim)->animData.frameNum){
		return twGetAnimFrame(twi->tw, twi->currentAnim, twi->animInst.currentFrame)->baseTexture->height;
	}
	return 0;
}

GLuint twiGetTexID(const twInstance *twi){
	// Make sure the current animation and frame are valid (within proper bounds)
	if(twi->currentAnim < twi->tw->animationNum &&
	   twi->animInst.currentFrame < twGetAnim(twi->tw, twi->currentAnim)->animData.frameNum){
		return twGetAnimFrame(twi->tw, twi->currentAnim, twi->animInst.currentFrame)->baseTexture->id;
	}
	return 0;
}

float twiGetFrameWidth(const twInstance *twi){
	return twGetAnimSubframe(twi->tw, twi->currentAnim, twi->animInst.currentFrame)->w;
}

float twiGetFrameHeight(const twInstance *twi){
	return twGetAnimSubframe(twi->tw, twi->currentAnim, twi->animInst.currentFrame)->h;
}

void twiGetFrameInfo(const twInstance *twi, float *x, float *y, float *w, float *h, GLuint *frameTexID){

	// Make sure the current animation and frame are valid (within proper bounds)
	if(twi->currentAnim < twi->tw->animationNum &&
	   twi->animInst.currentFrame < twGetAnim(twi->tw, twi->currentAnim)->animData.frameNum){

		*x = twGetAnimSubframe(twi->tw, twi->currentAnim, twi->animInst.currentFrame)->x;
		*y = twGetAnimSubframe(twi->tw, twi->currentAnim, twi->animInst.currentFrame)->y;
		*w = twGetAnimSubframe(twi->tw, twi->currentAnim, twi->animInst.currentFrame)->w;
		*h = twGetAnimSubframe(twi->tw, twi->currentAnim, twi->animInst.currentFrame)->h;
		*frameTexID = twGetAnimFrame(twi->tw, twi->currentAnim, twi->animInst.currentFrame)->baseTexture->id;

	}else{

		*x = 0;
		*y = 0;
		*w = 0;
		*h = 0;
		*frameTexID = 0;

	}

}

unsigned char twiContainsTranslucency(const twInstance *twi){
	// Make sure the current animation and frame are valid (within proper bounds)
	if(twi->currentAnim < twi->tw->animationNum &&
	   twi->animInst.currentFrame < twGetAnim(twi->tw, twi->currentAnim)->animData.frameNum){
		return twGetAnimFrame(twi->tw, twi->currentAnim, twi->animInst.currentFrame)->baseTexture->translucent;
	}
	return 0;
}
