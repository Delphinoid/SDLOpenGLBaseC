#include "textureWrapper.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void twInit(textureWrapper *tw){
	tw->name = NULL;
	cvInit(&tw->frames, 1);
	cvInit(&tw->animations, 1);
}

unsigned char twLoad(textureWrapper *tw, const char *prgPath, const char *filePath, cVector *allTextures){

	twInit(tw);

	size_t pathLen = strlen(prgPath);
	size_t fileLen = strlen(filePath);
	char *fullPath = malloc((pathLen+fileLen+1) * sizeof(char));
	memcpy(fullPath, prgPath, pathLen);
	memcpy(fullPath+pathLen, filePath, fileLen);
	fullPath[pathLen+fileLen] = '\0';
	FILE *texInfo = fopen(fullPath, "r");
	char lineFeed[1024];
	char *line;
	char compare[1024];
	size_t lineLength;

	twAnim tempAnim;               // Current texture animation being worked on
	tempAnim.frameIDs.size = 0; tempAnim.subframeIDs.size = 0; tempAnim.frameDelays.size = 0;
	int currentCommand = -1;       // The current multiline command type (-1 = none, 0 = texture, 1 = animation)
	unsigned int currentLine = 0;  // Current file line being read

	if(texInfo != NULL){
		while(!feof(texInfo)){

			fgets(lineFeed, sizeof(lineFeed), texInfo);
			line = lineFeed;
			currentLine++;
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
			size_t d;
			for(d = 0; (d < lineLength && !doneFront && !doneEnd); d++){
				if(!doneFront && line[d] != '\t' && line[d] != ' '){
					newOffset = d;
					doneFront = 1;
				}
				if(!doneEnd && d > 1 && d < lineLength && line[lineLength-d] != '\t' && line[lineLength-d] != ' '){
					lineLength -= d-1;
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
					if(((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->subframes.size == 0){
						twBounds baseSubframe = {.x = 0.f, .y = 0.f, .w = ((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->baseTexture->width,
						                                             .h = ((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->baseTexture->height};
						cvPush(&((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->subframes, (void *)&baseSubframe, sizeof(baseSubframe));
					}
					cvResize(&((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->subframes, ((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->subframes.size);

				}else if(currentCommand == 1){
					// If a valid animation was being worked on, save it and continue
					if(tempAnim.frameIDs.size > 0 &&
					   tempAnim.subframeIDs.size == tempAnim.frameIDs.size &&
					   tempAnim.frameDelays.size == tempAnim.frameIDs.size){

						cvResize(&tempAnim.frameIDs, tempAnim.frameIDs.size);
						cvResize(&tempAnim.subframeIDs, tempAnim.subframeIDs.size);
						cvResize(&tempAnim.frameDelays, tempAnim.frameDelays.size);
						cvPush(&tw->animations, (void *)&tempAnim, sizeof(tempAnim));
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
						if(((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->subframes.size == 0){
							twBounds baseSubframe = {.x = 0.f, .y = 0.f, .w = ((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->baseTexture->width,
							                                             .h = ((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->baseTexture->height};
							cvPush(&((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->subframes, (void *)&baseSubframe, sizeof(baseSubframe));
							cvResize(&((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->subframes, ((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->subframes.size);
						}

					}else if(currentCommand == 1){
						// If a valid animation is being worked on, save it and continue
						if(tempAnim.frameIDs.size > 0 &&
						   tempAnim.subframeIDs.size == tempAnim.frameIDs.size &&
						   tempAnim.frameDelays.size == tempAnim.frameIDs.size){

							cvResize(&tempAnim.frameIDs, tempAnim.frameIDs.size);
							cvResize(&tempAnim.subframeIDs, tempAnim.subframeIDs.size);
							cvResize(&tempAnim.frameDelays, tempAnim.frameDelays.size);
							cvPush(&tw->animations, (void *)&tempAnim, sizeof(tempAnim));
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
					cvPush(&tw->frames, (void *)&tempFrame, sizeof(tempFrame));
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

					unsigned int currentTexW = ((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->baseTexture->width;
					unsigned int currentTexH = ((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->baseTexture->height;

					// Automatically generate subframes for a sprite sheet
					for(d = 1; d <= numberOfFrames; d++){
						if(dimensions[0] + dimensions[2] <= currentTexW && dimensions[1] + dimensions[3] <= currentTexH){
							twBounds baseSubframe = {.x = dimensions[0], .y = dimensions[1], .w = dimensions[2], .h = dimensions[3]};
							cvPush(&((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->subframes, (void *)&baseSubframe, sizeof(baseSubframe));
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
					cvPush(&((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->subframes, (void *)&baseSubframe, sizeof(baseSubframe));

				}else{
					printf("Error loading texture wrapper:\nTexture sub-command \"subframe\" invoked on line %u without specifying a multiline texture.\n", currentLine);
				}


			// Load normal map
			}else if(lineLength >= 8 && strncpy(compare, line, 7) && (compare[7] = '\0') == 0 && strcmp(compare, "normal ") == 0){
				if(currentCommand == 0){
					/*char *texPath = malloc((lineLength-6) * sizeof(char));
					strcpy(texPath, line+7);
					tLoad(((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->normalTexture, prgPath, texPath);
					free(texPath);*/
					printf("Error loading texture wrapper:\nTexture sub-command \"normal\" is awaiting implementation.\n");
				}else{
					printf("Error loading texture wrapper:\nTexture sub-command \"normal\" invoked on line %u without specifying a multiline texture.\n", currentLine);
				}


			// New texture animation
			}else if(lineLength >= 9 && strncpy(compare, line, 9) && (compare[9] = '\0') == 0 && strcmp(compare, "animation") == 0){
				// Reset tempAnim
				tempAnim.desiredLoops = 0;
				cvInit(&tempAnim.frameIDs, 1);
				cvInit(&tempAnim.subframeIDs, 1);
				cvInit(&tempAnim.frameDelays, 1);
				currentCommand = 1;


			// Make the current animation loop
			}else if(lineLength >= 6 && strncpy(compare, line, 5) && (compare[5] = '\0') == 0 && strcmp(compare, "loop ") == 0){
				if(currentCommand == 1){
					tempAnim.desiredLoops = strtol(line+5, NULL, 0);
				}else{
					printf("Error loading texture wrapper:\nAnimation sub-command \"loop\" invoked on line %u without specifying an animation.\n", currentLine);
				}


			// Frame macro
			}else if(lineLength >= 16 && strncpy(compare, line, 7) && (compare[7] = '\0') == 0 && strcmp(compare, "fMacro ") == 0){

				if(currentCommand == 1){

					// Loads start and end textures, start and end subframes and the frame delays
					size_t textures[2] = {0, 0};
					size_t subframes[2] = {0, 0};
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
							if(d < tw->frames.size){
								size_t f;
								for(f = subframes[0]; f <= subframes[1]; f++){
									if(f < ((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->subframes.size){
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
					size_t frameID = 0, subframeID = 0;
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
					if(frameDelay > 0 && frameID < tw->frames.size &&
					   subframeID < ((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->subframes.size){
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
	if(tw->frames.size == 0){

		printf("Error loading texture wrapper:\nNo textures were loaded.\n");
		return 0;

	// If they were, check if the last texture added has any subframes. If it doesn't, add the default one
	}else if(((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->subframes.size == 0){

		twBounds baseSubframe = {.x = 0.f, .y = 0.f, .w = ((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->baseTexture->width,
		                                             .h = ((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->baseTexture->height};
		cvPush(&((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->subframes, (void *)&baseSubframe, sizeof(baseSubframe));
		cvResize(&((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->subframes, ((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->subframes.size);

	// If the last texture added has subframes, shrink the subframe vector to fit the amount of elements in it
	}else{
		cvResize(&((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->subframes, ((twFrame *)cvGet(&tw->frames, tw->frames.size-1))->subframes.size);
	}

	// Check if any animations were loaded
	if(tw->animations.size == 0){
		// If an animation was being worked on, add it
		if(tempAnim.frameIDs.size > 0 && tempAnim.subframeIDs.size == tempAnim.frameIDs.size &&
		   tempAnim.frameDelays.size == tempAnim.frameIDs.size){

			cvResize(&tempAnim.frameIDs, tempAnim.frameIDs.size);
			cvResize(&tempAnim.subframeIDs, tempAnim.subframeIDs.size);
			cvResize(&tempAnim.frameDelays, tempAnim.frameDelays.size);
			cvPush(&tw->animations, (void *)&tempAnim, sizeof(tempAnim));

		}else{  // Otherwise add the default animation

			twAnim defaultAnim = {.desiredLoops = 0};
			cvInit(&defaultAnim.frameIDs, 1);
			cvInit(&defaultAnim.subframeIDs, 1);
			cvInit(&defaultAnim.frameDelays, 1);
			size_t i = 0; float f = 0.f;
			cvPush(&defaultAnim.frameIDs, (void *)&i, sizeof(i));
			cvPush(&defaultAnim.subframeIDs, (void *)&i, sizeof(i));
			cvPush(&defaultAnim.frameDelays, (void *)&f, sizeof(f));
			cvPush(&tw->animations, (void *)&defaultAnim, sizeof(defaultAnim));

		}
	}

	// If no name was given, generate one based off the file name
	if(tw->name == NULL || tw->name[0] == '\0'){
		tw->name = malloc((fileLen+1)*sizeof(char));
		memcpy(tw->name, filePath, fileLen);
		tw->name[fileLen] = '\0';
	}
	cvResize(&tw->frames, tw->frames.size);
	cvResize(&tw->animations, tw->animations.size);
	return 1;

}

void twDelete(textureWrapper *tw){
	size_t d;
	for(d = 0; d < tw->frames.size; d++){
		cvClear(&((twFrame *)cvGet(&tw->frames, d))->subframes);
	}
	cvClear(&tw->frames);
	for(d = 0; d < tw->animations.size; d++){
		cvClear(&((twAnim *)cvGet(&tw->animations, d))->frameIDs);
		cvClear(&((twAnim *)cvGet(&tw->animations, d))->subframeIDs);
		cvClear(&((twAnim *)cvGet(&tw->animations, d))->frameDelays);
	}
	cvClear(&tw->animations);
	if(tw->name != NULL){
		free(tw->name);
	}
}

twAnim *twGetAnim(textureWrapper *tw, size_t anim){
	return (twAnim *)cvGet(&tw->animations, anim);
}

twFrame *twGetAnimFrame(textureWrapper *tw, size_t anim, size_t frame){
	/*size_t currentFrameID = *((size_t *)cvGet(&twGetAnim(tw, anim)->frameIDs, frame));*/
	return (twFrame *)cvGet(&tw->frames,
	                        *((size_t *)cvGet(&twGetAnim(tw, anim)->frameIDs, frame)));
}

twBounds *twGetAnimSubframe(textureWrapper *tw, size_t anim, size_t frame){
	/*size_t currentSubframeID = *((size_t *)cvGet(&twGetAnim(tw, anim)->subframeIDs, frame));*/
	return (twBounds *)cvGet(&twGetAnimFrame(tw, anim, frame)->subframes,
	                         *((size_t *)cvGet(&twGetAnim(tw, anim)->subframeIDs, frame)));
}

float *twGetAnimFrameDelay(textureWrapper *tw, size_t anim, size_t frame){
	return (float *)cvGet(&twGetAnim(tw, anim)->frameDelays, frame);
}

void twiInit(twInstance *twi, textureWrapper *tw){
	twi->texWrap = tw;
	animInit(&twi->animator);
}

void twiChangeAnim(twInstance *twi, size_t newAnim){
	if(newAnim < twi->texWrap->animations.size){
		animChange(&twi->animator, newAnim);
	}
}

/**unsigned char twiAnimFinished(twInstance *twi){
	// Return true if the animation has finished looping, is on the last frame
	// and the last frame has played for long enough
	return twi->animator.currentLoops >= twGetAnim(twi->texWrap, twi->animator.currentAnim)->desiredLoops &&
	       twi->animator.currentAnim == twGetAnim(twi->texWrap, twi->animator.currentAnim)->frameDelays.size-1 &&
	       twi->animator.lastUpdate-SDL_GetTicks() >= *twGetAnimFrameDelay(twi->texWrap,
	                                                                       twi->animator.currentAnim,
	                                                                       twi->animator.currentFrame);
}**/

void twiAnimate(twInstance *twi, float globalDelayMod){

	animAdvance(&twi->animator,
	            &twGetAnim(twi->texWrap, twi->animator.currentAnim)->frameDelays,
	            twGetAnim(twi->texWrap, twi->animator.currentAnim)->desiredLoops,
	            SDL_GetTicks(), globalDelayMod);

}

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
			twi->frame++;

			// Reset the animation if frame exceeds the number of frames in the animation
			if(twi->frame == twGetAnim(twi->texWrap, twi->animation)->frameIDs.size){
				twi->loops++;
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

GLuint twiGetTexWidth(twInstance *twi){
	// Make sure the current animation and frame are valid (within proper bounds)
	if(twi->animator.currentAnim < twi->texWrap->animations.size &&
	   twi->animator.currentFrame < twGetAnim(twi->texWrap, twi->animator.currentAnim)->frameIDs.size){
		return twGetAnimFrame(twi->texWrap, twi->animator.currentAnim, twi->animator.currentFrame)->baseTexture->width;
	}
	return 0;
}

GLuint twiGetTexHeight(twInstance *twi){
	// Make sure the current animation and frame are valid (within proper bounds)
	if(twi->animator.currentAnim < twi->texWrap->animations.size &&
	   twi->animator.currentFrame < twGetAnim(twi->texWrap, twi->animator.currentAnim)->frameIDs.size){
		return twGetAnimFrame(twi->texWrap, twi->animator.currentAnim, twi->animator.currentFrame)->baseTexture->height;
	}
	return 0;
}

GLuint twiGetTexID(twInstance *twi){
	// Make sure the current animation and frame are valid (within proper bounds)
	if(twi->animator.currentAnim < twi->texWrap->animations.size &&
	   twi->animator.currentFrame < twGetAnim(twi->texWrap, twi->animator.currentAnim)->frameIDs.size){
		return twGetAnimFrame(twi->texWrap, twi->animator.currentAnim, twi->animator.currentFrame)->baseTexture->id;
	}
	return 0;
}

void twiGetFrameInfo(twInstance *twi, float *x, float *y, float *w, float *h, GLuint *frameTexID){

	// Make sure the current animation and frame are valid (within proper bounds)
	if(twi->animator.currentAnim < twi->texWrap->animations.size &&
	   twi->animator.currentFrame < twGetAnim(twi->texWrap, twi->animator.currentAnim)->frameIDs.size){

		*x = twGetAnimSubframe(twi->texWrap, twi->animator.currentAnim, twi->animator.currentFrame)->x;
		*y = twGetAnimSubframe(twi->texWrap, twi->animator.currentAnim, twi->animator.currentFrame)->y;
		*w = twGetAnimSubframe(twi->texWrap, twi->animator.currentAnim, twi->animator.currentFrame)->w;
		*h = twGetAnimSubframe(twi->texWrap, twi->animator.currentAnim, twi->animator.currentFrame)->h;
		*frameTexID = twGetAnimFrame(twi->texWrap, twi->animator.currentAnim, twi->animator.currentFrame)->baseTexture->id;

	}else{

		*x = 0;
		*y = 0;
		*w = 0;
		*h = 0;
		*frameTexID = 0;

	}

}

unsigned char twiContainsTranslucency(twInstance *twi){
	// Make sure the current animation and frame are valid (within proper bounds)
	if(twi->animator.currentAnim < twi->texWrap->animations.size &&
	   twi->animator.currentFrame < twGetAnim(twi->texWrap, twi->animator.currentAnim)->frameIDs.size){
		return twGetAnimFrame(twi->texWrap, twi->animator.currentAnim, twi->animator.currentFrame)->baseTexture->translucent;
	}
	return 0;
}
