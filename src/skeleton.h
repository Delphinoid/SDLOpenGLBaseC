#ifndef SKELETON_H
#define SKELETON_H

#include <stdlib.h>
#include <stdint.h>
#include "mat4.h"

// Skeleton bone data
typedef struct {
	char *name;
	vec3 position;
	quat orientation;
	vec3 scale;
} sklBone;

// Skeleton node, containing a bone and links to the parent and its children
typedef struct sklNode sklNode;
typedef struct sklNode {
	sklBone bone;
	struct sklNode *parent;
	size_t childNum;
	struct sklNode *children;
} sklNode;

// Keyframe used for animations
typedef struct {
	// NOTE: each sklKeyframe in an sklAnim will have the same number of frameBones and frameIDs.
	// Non-changing elements are marked by a NULL name.
	size_t boneNum;
	sklBone *bones;  // Represents the delta transforms for each *modified* bone
} sklKeyframe;

// A full animation, containing a vector of keyframes
typedef struct {
	/** Redo this with a proper system that finds next bone transforms **/
	char *name;
	int desiredLoops;  // How many times the animation will loop (with -1 being infinite times)
	size_t frameNum;
	size_t boneNum;  // The total number of unique bones in the animation
	sklKeyframe *keyframes;  // Each individual keyframe for the animation
	size_t *frameDelays;     // Represents how long each frame should last
} sklAnim;

// Combines the above structures
typedef struct {
	char *name;
	sklNode *root;
	size_t boneNum;
} skeleton;

// Skeletal animation instance
typedef struct {
	sklAnim *anim;
	float delayMod;
	size_t currentFrame;
	size_t nextFrame;
	int currentLoops;
	float lastUpdate;
	sklBone *animInterpStart;
	sklBone *animInterpEnd;
	float animInterpT;
	sklBone *animState;  // Delta transformations for each bone
} sklAnimInstance;

// Skeleton instance
/** Restructure for proper element attachments (?? No idea what I was talking about here) **/
typedef struct {
	skeleton *skl;  // Should never change
	size_t animationNum;
	size_t animationCapacity;
	sklAnimInstance *animations;
	sklBone *customState;  // Custom bone transformations
} sklInstance;

void sklInit(skeleton *skl);
unsigned char sklLoad(skeleton *skl, const char *prgPath, const char *filePath);
void sklDelete(skeleton *skl);

void sklaInit(sklAnim *skla);
unsigned char sklaLoad(sklAnim *skla, const char *prgPath, const char *filePath);
void sklaDelete(sklAnim *skla);

void sklaiInit(sklAnimInstance *sklai);
unsigned char sklaiLoad(sklAnimInstance *sklai, const char *prgPath, const char *filePath);
void sklaiChangeAnim(sklAnimInstance *sklai, const sklAnim *anim);
void sklaiDelete(sklAnimInstance *sklai);

void skliInit(sklInstance *skli, skeleton *skl);
unsigned char skliLoad(sklInstance *skli, const char *prgPath, const char *filePath);
void skliAnimate(sklInstance *skli, const uint32_t currentTick, const float globalDelayMod);
void skliGenerateState(sklInstance *skli, mat4 *state, const skeleton *skl);
void skliDelete(sklInstance *skli);

#endif
