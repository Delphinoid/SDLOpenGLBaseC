#include "textureWrapper.h"
#include "texture.h"
#include "moduleTexture.h"
#include "memoryManager.h"
#include "helpersFileIO.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define TEXTURE_WRAPPER_RESOURCE_DIRECTORY_STRING FILE_PATH_RESOURCE_DIRECTORY_SHARED"Resources"FILE_PATH_DELIMITER_STRING"Wrappers"FILE_PATH_DELIMITER_STRING
#define TEXTURE_WRAPPER_RESOURCE_DIRECTORY_LENGTH 21

#define TEXTURE_WRAPPER_ANIMATION_START_CAPACITY       1  // 128
#define TEXTURE_WRAPPER_ANIMATION_FRAME_START_CAPACITY 1  // 128

// Default texture wrapper.
static twFrame g_twfDefault = {
	.image = &g_tDefault,
	.subframe = {
		.x = 0.f,
		.y = 0.f,
		.w = 1.f,
		.h = 1.f,
	}
};
static twAnim g_twaDefault = {
	.animData = {
		.desiredLoops = -1,
		.frameNum = 1,
		.frameDelays = (float *)"\0\0\0\0"
	},
	.frames = &g_twfDefault
};
textureWrapper g_twDefault = {
	.name = "default",
	.animationNum = 1,
	.animations = &g_twaDefault
};

/** Remove printf()s **/

static void twaInit(twAnim *const __RESTRICT__ twa){
	twa->frames = NULL;
	animDataInit(&twa->animData);
}
static return_t twaNew(twAnim *const __RESTRICT__ twa, const frameIndex_t frameCapacity){
	twa->frames = memAllocate(frameCapacity*(sizeof(twFrame) + sizeof(float)));
	if(twa->frames == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	twa->animData.frameDelays = (float *)&twa->frames[frameCapacity];
	return 1;
}
static return_t twaAddFrame(twAnim *const __RESTRICT__ twa, frameIndex_t *const __RESTRICT__ frameCapacity, twFrame twf, const float d){
	if(twa->animData.frameNum == *frameCapacity){
		twFrame *tempBuffer;
		if(*frameCapacity > 0){
			///*frameCapacity *= 2;
			++(*frameCapacity);
		}else{
			*frameCapacity = 1;
		}
		tempBuffer = memReallocate(twa->frames, *frameCapacity*(sizeof(twFrame) + sizeof(float)));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		twa->frames               = tempBuffer;
		twa->animData.frameDelays = (float *)&tempBuffer[*frameCapacity];
		// Shift frame delays to account for the extra capacity.
		memmove(twa->animData.frameDelays, &tempBuffer[twa->animData.frameNum], twa->animData.frameNum*sizeof(float));
	}
	twf.subframe.x /= twf.image->width;
	twf.subframe.y /= twf.image->height;
	twf.subframe.w /= twf.image->width;
	twf.subframe.h /= twf.image->height;
	twa->frames[twa->animData.frameNum] = twf;
	// Convert frameDelay to frameEnd.
	twa->animData.frameDelays[twa->animData.frameNum] = twa->animData.frameNum > 0 ? d + twa->animData.frameDelays[twa->animData.frameNum-1] : d;
	++twa->animData.frameNum;
	return 1;
}
static return_t twaResizeToFit(twAnim *const __RESTRICT__ twa, const frameIndex_t frameCapacity){
	if(twa->animData.frameNum != frameCapacity){
		twFrame *tempBuffer;
		// Shift frame delays to account for the extra capacity.
		memmove(&twa->frames[twa->animData.frameNum], twa->animData.frameDelays, twa->animData.frameNum*sizeof(float));
		tempBuffer = memReallocate(twa->frames, twa->animData.frameNum*(sizeof(twFrame) + sizeof(float)));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		twa->frames               = tempBuffer;
		twa->animData.frameDelays = (float *)&tempBuffer[twa->animData.frameNum];
	}
	return 1;
}
static void twaDelete(twAnim *const __RESTRICT__ twa){
	if(twa->frames != NULL){
		memFree(twa->frames);
	}
}

static return_t twAddAnim(textureWrapper *const __RESTRICT__ tw, const twAnim *const __RESTRICT__ a, animIndex_t *const __RESTRICT__ animationCapacity){
	if(tw->animationNum == *animationCapacity){
		twAnim *tempBuffer;
		if(*animationCapacity > 0){
			///*animationCapacity *= 2;
			++(*animationCapacity);
		}else{
			*animationCapacity = 1;
		}
		tempBuffer = memReallocate(tw->animations, *animationCapacity*sizeof(twAnim));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		tw->animations = tempBuffer;
	}
	tw->animations[tw->animationNum++] = *a;
	return 1;
}
static void twDefragment(textureWrapper *const __RESTRICT__ tw){
	if(tw->animations != &g_twaDefault){
		twAnim *a; const twAnim *aLast;
		tw->animations = memReallocate(tw->animations, tw->animationNum*sizeof(twAnim ));
		a = tw->animations; aLast = &a[tw->animationNum];
		for(; a < aLast; ++a){
			// Shift frame delays to account for the extra capacity.
			memmove(&a->frames[a->animData.frameNum], a->animData.frameDelays, a->animData.frameNum*sizeof(float));
			a->frames = memReallocate(
				a->frames,
				a->animData.frameNum*(sizeof(twFrame) + sizeof(float))
			);
			a->animData.frameDelays = (float *)&a->frames[a->animData.frameNum];
		}
	}
}
static return_t twResizeToFit(textureWrapper *const __RESTRICT__ tw, const animIndex_t animationCapacity){
	/**
	*** Defrag until I create a new
	*** binary texture wrapper file
	*** format where the sizes are
	*** all known beforehand.
	**/
	twDefragment(tw);
	return 1;
}
void twInit(textureWrapper *const __RESTRICT__ tw){
	tw->name = NULL;
	tw->animationNum = 0;
	tw->animations = NULL;
}

return_t twLoad(textureWrapper *const __RESTRICT__ tw, const char *const __RESTRICT__ filePath, const size_t filePathLength){

	twAnim tempAnim;
	frameIndex_t frameCapacity = 0;
	animIndex_t animationCapacity = TEXTURE_WRAPPER_ANIMATION_START_CAPACITY;

	char fullPath[FILE_MAX_PATH_LENGTH];

	FILE *texInfo;

	twInit(tw);

	fileGenerateFullPath(fullPath, TEXTURE_WRAPPER_RESOURCE_DIRECTORY_STRING, TEXTURE_WRAPPER_RESOURCE_DIRECTORY_LENGTH, filePath, filePathLength);
	texInfo = fopen(fullPath, "r");

	if(texInfo != NULL){

		char lineFeed[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;

		int currentCommand = -1;     // The current multiline command type (-1 = none, 0 = animation).
		fileLine_t currentLine = 0;  // Current file line being read.

		tw->animations = memAllocate(animationCapacity*sizeof(twAnim));
		if(tw->animations == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		twaInit(&tempAnim);

		while(fileParseNextLine(texInfo, lineFeed, sizeof(lineFeed), &line, &lineLength)){

			++currentLine;

			// New texture frame
			if(lineLength >= 7 && strncmp(line, "frame ", 6) == 0){

				if(currentCommand == 0){

					twFrame tempFrame;
					texture *tempTex;
					float delay;
					char texPath[1024];
					size_t pathLength;
					char *token = line+6;

					// Create a new subframe and add it to the current texture frame.
					tempFrame.subframe.x = strtod(token, &token);
					tempFrame.subframe.y = strtod(++token, &token);
					tempFrame.subframe.w = strtod(++token, &token);
					tempFrame.subframe.h = strtod(++token, &token);

					// Load the frame delay.
					delay = strtod(++token, &token);

					// Create a new texture frame.
					tempFrame.image = NULL;
					pathLength = fileParseResourcePath(texPath, token+1, lineLength);

					// Check if the texture has already been loaded.
					tempTex = moduleTextureFind(texPath, pathLength);
					if(tempTex != NULL){
						tempFrame.image = tempTex;

					// If the texture path is surrounded by quotes, try and load it.
					}else{
						tempTex = moduleTextureAllocate();
						if(tempTex != NULL){
							const return_t r = tLoad(tempTex, texPath, pathLength);
							if(r < 1){
								// The load failed. Clean up.
								moduleTextureFree(tempTex);
								if(r < 0){
									/** Memory allocation failure. **/
									twaDelete(&tempAnim);
									twDelete(tw);
									fclose(texInfo);
									return -1;
								}
								printf("Error loading texture wrapper \"%s\": Image \"%s\" at line %u does not exist.\n", fullPath, texPath, currentLine);
								tempFrame.image = &g_tDefault;
							}else{
								tempFrame.image = tempTex;
							}
						}else{
							/** Memory allocation failure. **/
							twaDelete(&tempAnim);
							twDelete(tw);
							fclose(texInfo);
							return -1;
						}
					}

					if(twaAddFrame(&tempAnim, &frameCapacity, tempFrame, delay) < 0){
						/** Memory allocation failure. **/
						twaDelete(&tempAnim);
						twDelete(tw);
						fclose(texInfo);
						return -1;
					}

				}else{
					printf("Error loading texture wrapper \"%s\": Texture sub-command \"frame\" invoked on line %u without specifying an animation.\n", fullPath, currentLine);
				}


			// New entire texture frame
			}else if(lineLength >= 13 && strncmp(line, "frameEntire ", 12) == 0){

				if(currentCommand == 0){

					twFrame tempFrame;
					texture *tempTex;
					float delay;
					char texPath[1024];
					size_t pathLength;
					char *token = line+12;

					// Load the frame delay.
					delay = strtod(token, &token);

					// Create a new texture frame.
					tempFrame.image = NULL;
					pathLength = fileParseResourcePath(texPath, token+1, lineLength);

					// Check if the texture has already been loaded.
					tempTex = moduleTextureFind(texPath, pathLength);
					if(tempTex != NULL){
						tempFrame.image = tempTex;

					// If the texture path is surrounded by quotes, try and load it.
					}else{
						tempTex = moduleTextureAllocate();
						if(tempTex != NULL){
							const return_t r = tLoad(tempTex, texPath, pathLength);
							if(r < 1){
								// The load failed. Clean up.
								moduleTextureFree(tempTex);
								if(r < 0){
									/** Memory allocation failure. **/
									twaDelete(&tempAnim);
									twDelete(tw);
									fclose(texInfo);
									return -1;
								}
								printf("Error loading texture wrapper \"%s\": Image \"%s\" at line %u does not exist.\n", fullPath, texPath, currentLine);
								tempFrame.image = &g_tDefault;
							}else{
								tempFrame.image = tempTex;
							}
						}else{
							/** Memory allocation failure. **/
							twaDelete(&tempAnim);
							twDelete(tw);
							fclose(texInfo);
							return -1;
						}
					}

					tempFrame.subframe.x = 0.f;
					tempFrame.subframe.y = 0.f;
					tempFrame.subframe.w = tempFrame.image->width;
					tempFrame.subframe.h = tempFrame.image->height;
					if(twaAddFrame(&tempAnim, &frameCapacity, tempFrame, delay) < 0){
						/** Memory allocation failure. **/
						twaDelete(&tempAnim);
						twDelete(tw);
						fclose(texInfo);
						return -1;
					}

				}else{
					printf("Error loading texture wrapper \"%s\": Texture sub-command \"frame\" invoked on line %u without specifying an animation.\n", fullPath, currentLine);
				}


			// Frame macro
			}else if(lineLength >= 12 && strncmp(line, "frameMacro ", 11) == 0){

				if(currentCommand == 0){

					frameIndex_t i;
					frameIndex_t frameNum;
					char macroDirection = ' ';
					float u, v;
					twFrame tempFrame;
					texture *tempTex;
					float delay;
					GLsizei currentTexW;
					GLsizei currentTexH;
					char texPath[1024];
					size_t pathLength;
					char *token = line+11;

					// Load the number of frames.
					frameNum = strtoul(token, &token, 0);

					// Load the macro direction.
					macroDirection = token[1];
					++token;

					// Load the UV translations.
					u = strtod(++token, &token);
					v = strtod(++token, &token);

					// Load the base subframe boundaries.
					tempFrame.subframe.x = strtod(++token, &token);
					tempFrame.subframe.y = strtod(++token, &token);
					tempFrame.subframe.w = strtod(++token, &token);
					tempFrame.subframe.h = strtod(++token, &token);

					// Load the frame delay.
					delay = strtod(++token, &token);

					// Create a new texture frame.
					tempFrame.image = NULL;
					pathLength = fileParseResourcePath(texPath, token+1, lineLength);

					// Check if the texture has already been loaded.
					tempTex = moduleTextureFind(texPath, pathLength);
					if(tempTex != NULL){
						tempFrame.image = tempTex;

					// If the texture path is surrounded by quotes, try and load it.
					}else{
						tempTex = moduleTextureAllocate();
						if(tempTex != NULL){
							const return_t r = tLoad(tempTex, texPath, pathLength);
							if(r < 1){
								// The load failed. Clean up.
								moduleTextureFree(tempTex);
								if(r < 0){
									/** Memory allocation failure. **/
									twaDelete(&tempAnim);
									twDelete(tw);
									fclose(texInfo);
									return -1;
								}
								printf("Error loading texture wrapper \"%s\": Image \"%s\" at line %u does not exist.\n", fullPath, texPath, currentLine);
								tempFrame.image = &g_tDefault;
							}else{
								tempFrame.image = tempTex;
							}
						}else{
							/** Memory allocation failure. **/
							twaDelete(&tempAnim);
							twDelete(tw);
							fclose(texInfo);
							return -1;
						}
					}

					// Automatically generate subframes for a sprite sheet.
					currentTexW = tempFrame.image->width;
					currentTexH = tempFrame.image->height;
					for(i = 1; i <= frameNum; ++i){
						if(twaAddFrame(&tempAnim, &frameCapacity, tempFrame, delay) < 0){
							/** Memory allocation failure. **/
							twaDelete(&tempAnim);
							twDelete(tw);
							fclose(texInfo);
							return -1;
						}
						if(macroDirection == 'x'){  // Adds frames from left to right before resetting and moving down
							tempFrame.subframe.x = (GLsizei)(i * (tempFrame.subframe.w - u)) % currentTexW;
							tempFrame.subframe.y = (GLsizei)(i * tempFrame.subframe.w) / currentTexW * tempFrame.subframe.h + i * v;
						}else if(macroDirection == 'y'){  // Adds frames from top to bottom before resetting and moving right
							tempFrame.subframe.x = (GLsizei)(i * tempFrame.subframe.h) / currentTexH * tempFrame.subframe.w - i * u;
							tempFrame.subframe.y = (GLsizei)(i * (tempFrame.subframe.h + v)) % currentTexH;
						}else{
							printf("Error loading texture wrapper \"%s\": frameMacro command at line %u has an invalid direction. Only one frame could be loaded.\n", fullPath, currentLine);
							break;
						}
					}

				}else{
					printf("Error loading texture wrapper \"%s\": Texture sub-command \"frame\" invoked on line %u without specifying an animation.\n", fullPath, currentLine);
				}


			// New texture animation
			}else if(lineLength >= 9 && strncmp(line, "animation", 9) == 0){
				// Reset tempAnim.
				if(strrchr(line+9, '{')){
					twaInit(&tempAnim);
					frameCapacity = TEXTURE_WRAPPER_ANIMATION_FRAME_START_CAPACITY;
					if(twaNew(&tempAnim, frameCapacity) < 0){
						/** Memory allocation failure. **/
						twDelete(tw);
						fclose(texInfo);
						return -1;
					}
					currentCommand = 0;
				}else{
					// Worth it?
					printf("Error loading texture wrapper \"%s\": Animation command at line %u does not contain a brace.\n", fullPath, currentLine);
				}


			// Make the current animation loop.
			}else if(lineLength >= 6 && strncmp(line, "loop ", 5) == 0){
				if(currentCommand == 0){
					tempAnim.animData.desiredLoops = strtol(line+5, NULL, 0);
				}else{
					printf("Error loading texture wrapper \"%s\": Animation sub-command \"loop\" invoked on line %u without specifying an animation.\n", fullPath, currentLine);
				}
			}

			// Close the current multiline command
			if(lineLength > 0 && line[lineLength-1] == '}'){
				if(currentCommand == 0){
					// If a valid animation was being worked on, save it and continue.
					if(tempAnim.animData.frameNum > 0){
						if(twaResizeToFit(&tempAnim, frameCapacity) < 0){
							/** Memory allocation failure. **/
							twDelete(tw);
							fclose(texInfo);
							return -1;
						}
						if(twAddAnim(tw, &tempAnim, &animationCapacity) < 0){
							/** Memory allocation failure. **/
							twaDelete(&tempAnim);
							twDelete(tw);
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

	// Check if any animations were loaded.
	if(tw->animationNum == 0){
		if(tempAnim.animData.frameNum > 0){
			// If an animation was being worked on, just resize it before adding it.
			if(twaResizeToFit(&tempAnim, frameCapacity) < 0){
				/** Memory allocation failure. **/
				twaDelete(&tempAnim);
				twDelete(tw);
				return -1;
			}
			// Add the new tempAnim to tw->animations.
			if(twAddAnim(tw, &tempAnim, &animationCapacity) < 0){
				/** Memory allocation failure. **/
				twaDelete(&tempAnim);
				twDelete(tw);
				return -1;
			}
		}else{
			// Otherwise build the default animation.
			memFree(tw->animations);
			tw->animationNum = 1;
			tw->animations = &g_twaDefault;
		}
	}

	// Delete the old tempAnim.
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
	if(twResizeToFit(tw, animationCapacity) < 0){
		return -1;
	}

	// Generate a name based off the file path.
	tw->name = fileGenerateResourceName(filePath, filePathLength);
	if(tw->name == NULL){
		/** Memory allocation failure. **/
		twDelete(tw);
		return -1;
	}

	return 1;

}

void twDelete(textureWrapper *const __RESTRICT__ tw){
	if(tw->animations != NULL && tw->animations != &g_twaDefault){
		twAnim *a = tw->animations;
		const twAnim *const aLast = &a[tw->animationNum];
		for(; a < aLast; ++a){
			twaDelete(a);
		}
		memFree(tw->animations);
	}
	if(tw->name != NULL && tw->name != g_twDefault.name){
		memFree(tw->name);
	}
}

static __FORCE_INLINE__ twAnim *twAnimation(const textureWrapper *const __RESTRICT__ tw, const animIndex_t anim){
	return &tw->animations[anim];
}

static __FORCE_INLINE__ twFrame *twAnimationFrame(const textureWrapper *const __RESTRICT__ tw, const animIndex_t anim, const frameIndex_t frame){
	return &tw->animations[anim].frames[frame];
}

static __FORCE_INLINE__ rectangle *twAnimationSubframe(const textureWrapper *const __RESTRICT__ tw, const animIndex_t anim, const frameIndex_t frame){
	return &tw->animations[anim].frames[frame].subframe;
}

static __FORCE_INLINE__ float *twAnimationFrameDelay(const textureWrapper *const __RESTRICT__ tw, const animIndex_t anim, const frameIndex_t frame){
	return &tw->animations[anim].animData.frameDelays[frame];
}

void twiInit(twInstance *const __RESTRICT__ twi, const textureWrapper *const tw){
	twi->tw = tw;
	twi->timeMod = 1.f;
	twi->currentAnim = 0;
	animInstInit(&twi->animator);
}

__FORCE_INLINE__ void twiTick(twInstance *const __RESTRICT__ twi, const float elapsedTime){
	animTick(&twi->animator, &twi->tw->animations[twi->currentAnim].animData, elapsedTime*twi->timeMod);
}

__FORCE_INLINE__ GLuint twiTextureWidth(const twInstance *const __RESTRICT__ twi){
	// Make sure the current animation and frame are valid (within proper bounds)
	//if(twi->currentAnim < twi->tw->animationNum &&
	//   twi->animator.currentFrame < twGetAnim(twi->tw, twi->currentAnim)->animData.frameNum){
		return twAnimationFrame(twi->tw, twi->currentAnim, twi->animator.currentFrame)->image->width;
	//}
	//return 0;
}

__FORCE_INLINE__ GLuint twiTextureHeight(const twInstance *const __RESTRICT__ twi){
	// Make sure the current animation and frame are valid (within proper bounds)
	//if(twi->currentAnim < twi->tw->animationNum &&
	//   twi->animator.currentFrame < twGetAnim(twi->tw, twi->currentAnim)->animData.frameNum){
		return twAnimationFrame(twi->tw, twi->currentAnim, twi->animator.currentFrame)->image->height;
	//}
	//return 0;
}

__FORCE_INLINE__ const texture *twiTexture(const twInstance *const __RESTRICT__ twi){
	// Make sure the current animation and frame are valid (within proper bounds)
	//if(twi->currentAnim < twi->tw->animationNum &&
	//   twi->animator.currentFrame < twGetAnim(twi->tw, twi->currentAnim)->animData.frameNum){
		return twAnimationFrame(twi->tw, twi->currentAnim, twi->animator.currentFrame)->image;
	//}
	//return 0;
}

__FORCE_INLINE__ float twiFrameWidth(const twInstance *const __RESTRICT__ twi){
	return twAnimationSubframe(twi->tw, twi->currentAnim, twi->animator.currentFrame)->w;
}

__FORCE_INLINE__ float twiFrameHeight(const twInstance *const __RESTRICT__ twi){
	return twAnimationSubframe(twi->tw, twi->currentAnim, twi->animator.currentFrame)->h;
}

const twFrame *twiState(const twInstance *const __RESTRICT__ twi, const float interpT){

	// Make sure the current animation and frame are valid (within proper bounds)
	//if(
	//	twi->currentAnim < twi->tw->animationNum &&
	//	twi->animator.currentFrame < twGetAnim(twi->tw, twi->currentAnim)->animData.frameNum
	//){

		frameIndex_t frame;
		animState(&twi->animator, &twAnimation(twi->tw, twi->currentAnim)->animData, interpT, &frame, NULL, NULL);
		return &twi->tw->animations[twi->currentAnim].frames[frame];

	//}else{

	//	*x = 0.f;
	//	*y = 0.f;
	//	*w = 0.f;
	//	*h = 0.f;
	//	*frameTexID = 0;

	//}

}

const twFrame *twiStateOffset(const twInstance *const __RESTRICT__ twi, const unsigned int offset, const float interpT){

	// Make sure the current animation and frame are valid (within proper bounds)
	//if(
	//	twi->currentAnim < twi->tw->animationNum &&
	//	twi->animator.currentFrame < twGetAnim(twi->tw, twi->currentAnim)->animData.frameNum
	//){

		frameIndex_t frame;
		animState(&twi->animator, &twAnimation(twi->tw, twi->currentAnim+offset)->animData, interpT, &frame, NULL, NULL);
		return &twi->tw->animations[twi->currentAnim].frames[frame];

	//}else{

	//	*x = 0.f;
	//	*y = 0.f;
	//	*w = 0.f;
	//	*h = 0.f;
	//	*frameTexID = 0;

	//}

}

const twFrame *twState(const textureWrapper *const __RESTRICT__ tw, const animationInstance *const __RESTRICT__ animator, const animIndex_t currentAnim, const float interpT){
	frameIndex_t frame;
	animState(animator, &twAnimation(tw, currentAnim)->animData, interpT, &frame, NULL, NULL);
	return &tw->animations[currentAnim].frames[frame];
}

return_t twiTranslucent(const twInstance *const __RESTRICT__ twi){
	// Make sure the current animation and frame are valid (within proper bounds)
	if(twi->currentAnim < twi->tw->animationNum &&
	   twi->animator.currentFrame < twAnimation(twi->tw, twi->currentAnim)->animData.frameNum){
		return twAnimationFrame(twi->tw, twi->currentAnim, twi->animator.currentFrame)->image->translucent;
	}
	return 0;
}
