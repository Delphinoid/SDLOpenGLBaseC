#include "textureWrapper.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//void generateNameFromPath(char **name, const char *path);
void copyString(char **destination, const char *source, const unsigned int length);

void twInit(textureWrapper *texWrap){
	texWrap->name = NULL;
	cvInit(&texWrap->frames, 1);
	cvInit(&texWrap->animations, 1);
}

unsigned char twLoad(textureWrapper *texWrap, const char *prgPath, const char *filePath, cVector *allTextures){

	twInit(texWrap);

	char *fullPath = malloc((strlen(prgPath) + strlen(filePath) + 1) * sizeof(char));
	strcpy(fullPath, prgPath);
	strcat(fullPath, filePath);
	fullPath[strlen(prgPath)+strlen(filePath)] = '\0';
	FILE *texInfo = fopen(fullPath, "r");
	char lineFeed[1024];
	char *line;
	char compare[1024];
	unsigned int lineLength;

	twAnim tempAnim;               // Current texture animation being worked on
	tempAnim.frameIDs.size = 0; tempAnim.subframeIDs.size = 0; tempAnim.frameDelays.size = 0;
	int currentCommand = -1;       // The current multiline command type (-1 = none, 0 = texture, 1 = animation)
	unsigned int currentLine = 0;  // Current file line being read

	if(texInfo != NULL){
		while(!feof(texInfo)){

			fgets(lineFeed, sizeof(lineFeed), texInfo);
			lineFeed[strcspn(lineFeed, "\r\n")] = 0;
			line = lineFeed;
			currentLine++;
			lineLength = strlen(line);

			// Remove any comments from the line
			char *commentPos = strstr(line, "//");
			if(commentPos != NULL){
				lineLength -= commentPos-line;
				commentPos = '\0';
			}
			// Remove any indentations from the line, as well as any trailing spaces and tabs
			unsigned int d;
			unsigned char doneFront = 0, doneEnd = 0;
			for(d = 0; (d < lineLength && !doneFront && !doneEnd); d++){
				if(!doneFront && line[d] != '\t' && line[d] != ' '){
					line += d;
					lineLength -= d;
					doneFront = 1;
				}
				if(!doneEnd && d > 1 && d < lineLength && line[lineLength-d] != '\t' && line[lineLength-d] != ' '){
					line[lineLength-d-1] = '\0';
					lineLength -= d;
					doneEnd = 1;
				}
			}

			// Name
			if(lineLength >= 6 && strncpy(compare, line, 5) && (compare[5] = '\0') == 0 && strcmp(compare, "name ") == 0){
				texWrap->name = malloc((lineLength-4) * sizeof(char));
				if(texWrap->name != NULL){
					strncpy(texWrap->name, line+5, lineLength-5);
					texWrap->name[lineLength-5] = '\0';
				}


			// Close current multiline command
			}else if(lineLength > 0 && line[0] == '}'){
				if(currentCommand == 0){
					// If a textureFrame was being worked on and has no subframes, add the default one
					if(((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->subframes.size == 0){
						twBounds baseSubframe = {.x = 0.f, .y = 0.f, .w = ((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->baseTexture->width,
						                                             .h = ((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->baseTexture->height};
						cvPush(&((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->subframes, (void *)&baseSubframe, sizeof(baseSubframe));
					}
					cvResize(&((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->subframes, ((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->subframes.size);

				}else if(currentCommand == 1){
					// If a valid animation was being worked on, save it and continue
					if(tempAnim.frameIDs.size > 0 &&
					   tempAnim.subframeIDs.size == tempAnim.frameIDs.size &&
					   tempAnim.frameDelays.size == tempAnim.frameIDs.size){

						cvResize(&tempAnim.frameIDs, tempAnim.frameIDs.size);
						cvResize(&tempAnim.subframeIDs, tempAnim.subframeIDs.size);
						cvResize(&tempAnim.frameDelays, tempAnim.frameDelays.size);
						cvPush(&texWrap->animations, (void *)&tempAnim, sizeof(tempAnim));
					}
				}

				currentCommand = -1;


			// New texture frame
			}else if(lineLength >= 9 && strncpy(compare, line, 8) && (compare[8] = '\0') == 0 && strcmp(compare, "texture ") == 0){

				// A multiline command is already in progress; try to close it and continue
				if(currentCommand != -1){

					printf("Error loading texture wrapper:\nTrying to start a multiline command at line %u while another is already in progress; "
						   "I will attempt to close the current command.\n", currentLine);

					// If the multiline command is a texture...
					if(currentCommand == 0){
						// If a textureFrame was being worked on and has no subframes, add the default one
						if(((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->subframes.size == 0){
							twBounds baseSubframe = {.x = 0.f, .y = 0.f, .w = ((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->baseTexture->width,
							                                             .h = ((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->baseTexture->height};
							cvPush(&((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->subframes, (void *)&baseSubframe, sizeof(baseSubframe));
							cvResize(&((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->subframes, ((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->subframes.size);
						}

					}else if(currentCommand == 1){
						// If a valid animation is being worked on, save it and continue
						if(tempAnim.frameIDs.size > 0 &&
						   tempAnim.subframeIDs.size == tempAnim.frameIDs.size &&
						   tempAnim.frameDelays.size == tempAnim.frameIDs.size){

							cvResize(&tempAnim.frameIDs, tempAnim.frameIDs.size);
							cvResize(&tempAnim.subframeIDs, tempAnim.subframeIDs.size);
							cvResize(&tempAnim.frameDelays, tempAnim.frameDelays.size);
							cvPush(&texWrap->animations, (void *)&tempAnim, sizeof(tempAnim));
						}
					}

					currentCommand = -1;

				}

				// Create a new texture frame
				twFrame tempFrame; cvInit(&tempFrame.subframes, 1);
				tempFrame.baseTexture = NULL;
				tempFrame.normalTexture = NULL;

				char *firstQuote = strchr(line, '"');
				char *lastQuote = strrchr(line, '"');
				unsigned int pathBegin;
				unsigned int pathLength;
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
				unsigned int d;
				for(d = 0; d < allTextures->size; d++){
					texture *tempTex = (texture *)cvGet(allTextures, d);
					if(strcmp(texPath, tempTex->name) == 0){
						tempFrame.baseTexture = (texture *)cvGet(allTextures, d);
						skipLoad = 1;
						d = allTextures->size;
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
					if(strrchr(line, '{') > line+pathBegin+1+pathLength){  // Check if the command spans multiple lines (it contains an opening brace at the end)
						currentCommand = 0;
					}else{  // If it doesn't, add the default subframe
						twBounds baseSubframe = {.x = 0.f, .y = 0.f, .w = tempFrame.baseTexture->width,
						                                             .h = tempFrame.baseTexture->height};
						cvPush(&tempFrame.subframes, (void *)&baseSubframe, sizeof(baseSubframe));
						cvResize(&tempFrame.subframes, tempFrame.subframes.size);
					}
					cvPush(&texWrap->frames, (void *)&tempFrame, sizeof(tempFrame));
				}

				free(texPath);


			// Subframe macro
			}else if(lineLength >= 14 && strncpy(compare, line, 7) && (compare[7] = '\0') == 0 && strcmp(compare, "sMacro ") == 0){

				if(currentCommand == 0){

					// Loads number of subframes to create, main dimension to loop in (x or y) and the subframe dimensions
					unsigned int numberOfFrames = 0;
					char macroDirection = ' ';
					float dimensions[4] = {0, 0, 0, 0};
					char *token = strtok(line+7, "/");
					for(d = 0; d < 6; d++){
						if(d == 0){
							numberOfFrames = strtoul(token, NULL, 0);
						}else if(d == 1){
							macroDirection = line[7];
						}else{
							dimensions[d-2] = strtof(token, NULL);
						}
						token = strtok(NULL, "/");
					}

					unsigned int currentTexW = ((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->baseTexture->width;
					unsigned int currentTexH = ((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->baseTexture->height;

					// Automatically generate subframes for a sprite sheet
					for(d = 1; d <= numberOfFrames; d++){
						if(dimensions[0] + dimensions[2] <= currentTexW && dimensions[1] + dimensions[3] <= currentTexH){
							twBounds baseSubframe = {.x = dimensions[0], .y = dimensions[1], .w = dimensions[2], .h = dimensions[3]};
							cvPush(&((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->subframes, (void *)&baseSubframe, sizeof(baseSubframe));
							if(macroDirection == 'x'){  // Adds frames from left to right before resetting and moving down
								dimensions[0] = (unsigned int)(d * dimensions[2]) % currentTexW;
								dimensions[1] = (unsigned int)(d * dimensions[2]) / currentTexW * dimensions[3];
							}else if(macroDirection == 'y'){  // Adds frames from top to bottom before resetting and moving right
								dimensions[0] = (unsigned int)(d * dimensions[3]) / currentTexH * dimensions[2];
								dimensions[1] = (unsigned int)(d * dimensions[3]) % currentTexH;
							}else{
								printf("Error loading texture wrapper:\nsMacro command at line %u has an invalid \"direction\". Only one frame could be loaded.\n", currentLine);
								d = numberOfFrames++;
							}
						}else{
							printf("Error loading texture wrapper:\nsMacro command at line %u could not load %u frame(s).\n", currentLine, numberOfFrames-d+1);
							d = numberOfFrames++;
						}
					}

				}else{
					printf("Error loading texture wrapper:\nTexture sub-command \"sMacro\" invoked on line %u without specifying a multiline texture.\n", currentLine);
				}


			// New subframe
			}else if(lineLength >= 16 && strncpy(compare, line, 9) && (compare[9] = '\0') == 0 && strcmp(compare, "subframe ") == 0){

				if(currentCommand == 0){

					// Create a new subframe and add it to the current texture frame
					float dimensions[4] = {0, 0, 0, 0};
					char *token = strtok(line+9, "/");
					for(d = 0; d < 4; d++){
						if(token != NULL){
							dimensions[d] = strtof(token, NULL);
							token = strtok(NULL, "/");
						}else{
							d = 4;
						}
					}
					twBounds baseSubframe = {.x = dimensions[0], .y = dimensions[1], .w = dimensions[2], .h = dimensions[3]};
					cvPush(&((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->subframes, (void *)&baseSubframe, sizeof(baseSubframe));

				}else{
					printf("Error loading texture wrapper:\nTexture sub-command \"subframe\" invoked on line %u without specifying a multiline texture.\n", currentLine);
				}


			// Load normal map
			}else if(lineLength >= 8 && strncpy(compare, line, 7) && (compare[7] = '\0') == 0 && strcmp(compare, "normal ") == 0){
				if(currentCommand == 0){
					/*char *texPath = malloc((lineLength-6) * sizeof(char));
					strcpy(texPath, line+7);
					tLoad(((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->normalTexture, prgPath, texPath);
					free(texPath);*/
					printf("Error loading texture wrapper:\nTexture sub-command \"normal\" is awaiting implementation.\n");
				}else{
					printf("Error loading texture wrapper:\nTexture sub-command \"normal\" invoked on line %u without specifying a multiline texture.\n", currentLine);
				}


			// New texture animation
			}else if(lineLength >= 9 && strncpy(compare, line, 9) && (compare[9] = '\0') == 0 && strcmp(compare, "animation") == 0){
				// Reset tempAnim
				tempAnim.loop = 0;
				cvInit(&tempAnim.frameIDs, 1);
				cvInit(&tempAnim.subframeIDs, 1);
				cvInit(&tempAnim.frameDelays, 1);
				currentCommand = 1;


			// Make the current animation loop
			}else if(lineLength >= 6 && strncpy(compare, line, 5) && (compare[5] = '\0') == 0 && strcmp(compare, "loop ") == 0){
				if(currentCommand == 1){
					tempAnim.loop = strtol(line+5, NULL, 0);
				}else{
					printf("Error loading texture wrapper:\nAnimation sub-command \"loop\" invoked on line %u without specifying an animation.\n", currentLine);
				}


			// Frame macro
			}else if(lineLength >= 16 && strncpy(compare, line, 7) && (compare[7] = '\0') == 0 && strcmp(compare, "fMacro ") == 0){

				if(currentCommand == 1){

					// Loads start and end textures, start and end subframes and the frame delays
					unsigned int textures[2] = {0, 0};
					unsigned int subframes[2] = {0, 0};
					float frameDelay = 0.f;
					char *token = strtok(line+7, "/");
					for(d = 0; d < 6; d++){
						if(token != NULL){
							if(d < 2){
								textures[d] = strtoul(token, NULL, 0);
							}else if(d < 4){
								subframes[d-2] = strtoul(token, NULL, 0);
							}else{
								frameDelay = strtof(token, NULL);
							}
							token = strtok(NULL, "/");
						}else{
							d = 6;
						}
					}

					if(frameDelay > 0){
						for(d = textures[0]; d <= textures[1]; d++){
							if(d < texWrap->frames.size){
								unsigned int f;
								for(f = subframes[0]; f <= subframes[1]; f++){
									if(f < ((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->subframes.size){
										cvPush(&tempAnim.frameIDs, (void *)&d, sizeof(d));
										cvPush(&tempAnim.subframeIDs, (void *)&f, sizeof(f));
										cvPush(&tempAnim.frameDelays, (void *)&frameDelay, sizeof(frameDelay));
									}else{
										f = subframes[1] + 1;
									}
								}
							}else{
								d = textures[1] + 1;
							}
						}
					}

				}else{
					printf("Error loading texture wrapper:\nAnimation sub-command \"fMacro\" invoked on line %u without specifying an animation.\n", currentLine);
				}


			// New animation frame
			}else if(lineLength >= 11 && strncpy(compare, line, 6) && (compare[6] = '\0') == 0 && strcmp(compare, "frame ") == 0){

				if(currentCommand == 1){

					// Parse frame information
					unsigned int frameID = 0, subframeID = 0;
					float frameDelay = 0.f;
					char *token = strtok(line+6, "/");
					for(d = 0; d < 6; d++){
						if(token != NULL){
							if(d == 0){
								frameID = strtoul(token, NULL, 0);
							}else if(d == 1){
								subframeID = strtoul(token, NULL, 0);
							}else{
								frameDelay = strtof(token, NULL);
							}
							token = strtok(NULL, "/");
						}else{
							d = 6;
						}
					}

					// Validate the frame information
					if(frameDelay > 0 && frameID < texWrap->frames.size &&
					   subframeID < ((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->subframes.size){
						cvPush(&tempAnim.frameIDs, (void *)&frameID, sizeof(frameID));
						cvPush(&tempAnim.subframeIDs, (void *)&subframeID, sizeof(subframeID));
						cvPush(&tempAnim.frameDelays, (void *)&frameDelay, sizeof(frameDelay));
					}

				}else{
					printf("Error loading texture wrapper:\nAnimation sub-command \"frame\" invoked on line %u without specifying an animation.\n", currentLine);
				}

			}

		}

		fclose(texInfo);
		free(fullPath);

	}else{
		printf("Error loading texture wrapper:\nCouldn't open %s\n", fullPath);
		free(fullPath);
		return 0;
	}


	// Check if any textures were loaded
	if(texWrap->frames.size == 0){

		printf("Error loading texture wrapper:\nNo textures were loaded.\n");
		return 0;

	// If they were, check if the last texture added has any subframes. If it doesn't, add the default one
	}else if(((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->subframes.size == 0){

		twBounds baseSubframe = {.x = 0.f, .y = 0.f, .w = ((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->baseTexture->width,
		                                             .h = ((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->baseTexture->height};
		cvPush(&((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->subframes, (void *)&baseSubframe, sizeof(baseSubframe));
		cvResize(&((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->subframes, ((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->subframes.size);

	// If the last texture added has subframes, shrink the subframe vector to fit the amount of elements in it
	}else{
		cvResize(&((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->subframes, ((twFrame *)cvGet(&texWrap->frames, texWrap->frames.size-1))->subframes.size);
	}

	// Check if any animations were loaded
	if(texWrap->animations.size == 0){
		// If an animation was being worked on, add it
		if(tempAnim.frameIDs.size > 0 && tempAnim.subframeIDs.size == tempAnim.frameIDs.size &&
		   tempAnim.frameDelays.size == tempAnim.frameIDs.size){

			cvResize(&tempAnim.frameIDs, tempAnim.frameIDs.size);
			cvResize(&tempAnim.subframeIDs, tempAnim.subframeIDs.size);
			cvResize(&tempAnim.frameDelays, tempAnim.frameDelays.size);
			cvPush(&texWrap->animations, (void *)&tempAnim, sizeof(tempAnim));

		}else{  // Otherwise add the default animation

			twAnim defaultAnim = {.loop = 0};
			cvInit(&defaultAnim.frameIDs, 1);
			cvInit(&defaultAnim.subframeIDs, 1);
			cvInit(&defaultAnim.frameDelays, 1);
			unsigned int i = 0; float f = 0.f;
			cvPush(&defaultAnim.frameIDs, (void *)&i, sizeof(i));
			cvPush(&defaultAnim.subframeIDs, (void *)&i, sizeof(i));
			cvPush(&defaultAnim.frameDelays, (void *)&f, sizeof(f));
			cvPush(&texWrap->animations, (void *)&defaultAnim, sizeof(defaultAnim));

		}
	}

	// If no name was given, generate one based off the file name
	if(texWrap->name == NULL || strlen(texWrap->name) == 0){
		//generateNameFromPath(texWrap->name, filePath);
		copyString(&texWrap->name, filePath, strlen(filePath));
	}
	cvResize(&texWrap->frames, texWrap->frames.size);
	cvResize(&texWrap->animations, texWrap->animations.size);
	return 1;

}

void twAnimate(textureWrapper *texWrap, float speedMod, unsigned int *currentAnim, unsigned int *currentFrame, float *frameProgress, int *timesLooped){

	// Only animate the texture if the current animation is valid,
	// has more than one frame and can still be animated
	twAnim *curAnimRef = (twAnim *)cvGet(&texWrap->animations, *currentAnim);
	if(curAnimRef != NULL && curAnimRef->frameIDs.size > 1 &&
	   ((*timesLooped <= curAnimRef->loop || curAnimRef->loop < 0) ||
	     *currentFrame < curAnimRef->frameIDs.size - 1)){

		if(*currentFrame < curAnimRef->frameIDs.size){  // Make sure the current frame is valid

			// Get the current tick
			float currentTick = SDL_GetTicks() * speedMod;

			/* While the difference between currentTick and the frameProgress is greater than the
			desired delay period and the texture can still be animated, advance the animation */
			while((currentTick - *frameProgress) >= *((float *)cvGet(&curAnimRef->frameDelays, *currentFrame)) &&
			      (((*timesLooped <= curAnimRef->loop || curAnimRef->loop < 0) ||
			         *currentFrame < curAnimRef->frameIDs.size - 1))){

				// Add the delay to frameProgress and advance the animation
				*frameProgress += *((float *)cvGet(&curAnimRef->frameDelays, *currentFrame));
				(*currentFrame)++;

				// Reset currentFrame if it gets too high
				if(*currentFrame == curAnimRef->frameIDs.size){
					(*timesLooped)++;
					if(*timesLooped <= curAnimRef->loop || curAnimRef->loop < 0){
						*currentFrame = 0;
					}else{
						*currentFrame = curAnimRef->frameIDs.size - 1;
					}
				}

			}

		}else{  // If it is too high, set it back

			// Only reset the animation if it can loop
			if(!curAnimRef->loop){
				*currentFrame = curAnimRef->frameIDs.size - 1;
			}else{
				*currentFrame = 0;
			}
			*frameProgress = SDL_GetTicks();

		}

	}else{

		if(*currentAnim >= texWrap->animations.size){
			*currentAnim = 0;
		}
		if(*currentFrame >= ((twAnim *)cvGet(&texWrap->animations, *currentAnim))->frameIDs.size){
			if(!((twAnim *)cvGet(&texWrap->animations, *currentAnim))->loop){
				*currentFrame = ((twAnim *)cvGet(&texWrap->animations, *currentAnim))->frameIDs.size - 1;
			}else{
				*currentFrame = 0;
			}
			*frameProgress = SDL_GetTicks();
		}

	}

}

void twChangeAnim(textureWrapper *texWrap, unsigned int newAnim, unsigned int *currentAnim, unsigned int *currentFrame, float *frameProgress, int *timesLooped){
	if(newAnim < texWrap->animations.size){
		*currentAnim = newAnim;
		*currentFrame = 0;
		*frameProgress = 0.f;
		*timesLooped = 0;
	}
}

GLuint twGetTexWidth(textureWrapper *texWrap, unsigned int currentAnim, unsigned int currentFrame){
	if(currentAnim < texWrap->animations.size && currentFrame < ((twAnim *)cvGet(&texWrap->animations, currentAnim))->frameIDs.size){
		return ((twFrame *)cvGet(&texWrap->frames, *((unsigned int *)cvGet(&((twAnim *)cvGet(&texWrap->animations, currentAnim))->frameIDs, currentFrame))))->baseTexture->width;
	}
	return 0;
}

GLuint twGetTexHeight(textureWrapper *texWrap, unsigned int currentAnim, unsigned int currentFrame){
	if(currentAnim < texWrap->animations.size && currentFrame < ((twAnim *)cvGet(&texWrap->animations, currentAnim))->frameIDs.size){
		return ((twFrame *)cvGet(&texWrap->frames, *((unsigned int *)cvGet(&((twAnim *)cvGet(&texWrap->animations, currentAnim))->frameIDs, currentFrame))))->baseTexture->height;
	}
	return 0;
}

GLuint twGetTexID(textureWrapper *texWrap, unsigned int currentAnim, unsigned int currentFrame){
	if(currentAnim < texWrap->animations.size && currentFrame < ((twAnim *)cvGet(&texWrap->animations, currentAnim))->frameIDs.size){
		return ((twFrame *)cvGet(&texWrap->frames, *((unsigned int *)cvGet(&((twAnim *)cvGet(&texWrap->animations, currentAnim))->frameIDs, currentFrame))))->baseTexture->id;
	}
	return 0;
}

void twGetFrameInfo(textureWrapper *texWrap, unsigned int currentAnim, unsigned int currentFrame, float *x, float *y, float *w, float *h, GLuint *frameTexID){

	// Check if currentAnim and currentFrame are valid
	if(currentAnim < texWrap->animations.size && currentFrame < ((twAnim *)cvGet(&texWrap->animations, currentAnim))->frameIDs.size){

		unsigned int currentFrameID = *((unsigned int *)cvGet(&((twAnim *)cvGet(&texWrap->animations, currentAnim))->frameIDs, currentFrame));
		unsigned int currentSubframeID = *((unsigned int *)cvGet(&((twAnim *)cvGet(&texWrap->animations, currentAnim))->subframeIDs, currentFrame));

		*x = ((twBounds *)cvGet(&((twFrame *)cvGet(&texWrap->frames, currentFrameID))->subframes, currentSubframeID))->x;
		*y = ((twBounds *)cvGet(&((twFrame *)cvGet(&texWrap->frames, currentFrameID))->subframes, currentSubframeID))->y;
		*w = ((twBounds *)cvGet(&((twFrame *)cvGet(&texWrap->frames, currentFrameID))->subframes, currentSubframeID))->w;
		*h = ((twBounds *)cvGet(&((twFrame *)cvGet(&texWrap->frames, currentFrameID))->subframes, currentSubframeID))->h;
		*frameTexID = ((twFrame *)cvGet(&texWrap->frames, currentFrameID))->baseTexture->id;

	}else{

		*x = 0;
		*y = 0;
		*w = 0;
		*h = 0;
		*frameTexID = 0;

	}

}

unsigned char twContainsTranslucency(textureWrapper *texWrap, unsigned int currentAnim, unsigned int currentFrame){
	if(currentAnim < texWrap->animations.size && currentFrame < ((twAnim *)cvGet(&texWrap->animations, currentAnim))->frameIDs.size){
		unsigned int currentFrameID = *((unsigned int *)cvGet(&((twAnim *)cvGet(&texWrap->animations, currentAnim))->frameIDs, currentFrame));
		return ((twFrame *)cvGet(&texWrap->frames, currentFrameID))->baseTexture->translucent;
	}
	return 0;
}

void twDelete(textureWrapper *texWrap){
	unsigned int d;
	for(d = 0; d < texWrap->frames.size; d++){
		cvClear(&((twFrame *)cvGet(&texWrap->frames, d))->subframes);
	}
	cvClear(&texWrap->frames);
	for(d = 0; d < texWrap->animations.size; d++){
		cvClear(&((twAnim *)cvGet(&texWrap->animations, d))->frameIDs);
		cvClear(&((twAnim *)cvGet(&texWrap->animations, d))->subframeIDs);
		cvClear(&((twAnim *)cvGet(&texWrap->animations, d))->frameDelays);
	}
	cvClear(&texWrap->animations);
	if(texWrap->name != NULL){
		free(texWrap->name);
	}
}
