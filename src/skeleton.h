#ifndef SKELETON_H
#define SKELETON_H

#include "animation.h"
#include "mat4.h"

#define MAX_BLEND_INSTANCES 1

// Skeleton bone data
typedef struct {
	vec3 position;
	quat orientation;
	vec3 scale;
} sklBone;

// Skeleton node, containing a bone and the index of its parent
typedef struct {
	char *name;
	sklBone defaultState;
	size_t parent;
} sklNode;

// Combines the above structures
typedef struct {
	char *name;
	size_t boneNum;
	sklNode *bones;  // Depth-first vector of each bone.
} skeleton;

// A full animation, containing a vector of keyframes
typedef struct {
	/**
	*** Redo this with a proper system that finds next bone transforms and
	*** works on independent bone sequences. Should be easy with the
	*** new animation system.
	**/
	char *name;
	animationData animData;
	size_t boneNum;  // The total number of unique bones in the animation.
	char **bones;    // Array of names for each bone.
	sklBone **frames;  // An array of keyframes, where each keyframe is an array of
	                   // bone delta transforms (offsets from their default states).
} sklAnim;

// Skeletal animation instance
typedef struct {
	animationInstance *animInst;  // An array of animations. Size is MAX_BLEND_INSTANCES + 1.
	                              // Animations after index 0 are used for blending, where smaller indices are more recent.
	                              // When an animation blend finishes, all of the animations that follow are shifted over to the left.
	float **animInstBlendProgress;   // The current progress through a blend. Size is MAX_BLEND_INSTANCES * stateNum.
	float **animInstBlendTime;       // How long to blend between animations for. Size is MAX_BLEND_INSTANCES * stateNum.
	sklBone **animBoneLookup;  // Which bone in animState each bone in skli->skl corresponds to.
	                           // If the root (first) bone in skli->skl is named "blah" and the
	                           // second bone in animState is named "blah", the array will start
	                           // with animBoneLookup[0] == &animState[1]. If the bone does not
	                           // exist, its entry points to NULL.
	sklBone *animState;  // Delta transformations for each bone.
} sklAnimInstance;

// Skeleton instance
/** Restructure for proper element attachments (?? No idea what I was talking about here) **/
typedef struct {
	skeleton *skl;  // Should never be changed manually.
	float timeMod;
	/** Can we use pushDynamicArray()? **/
	size_t animationNum;
	sklAnim **animations;
	size_t animInstNum;
	size_t animInstCapacity;
	sklAnimInstance *animInstances;
	sklBone **customState;  // Array of custom bone transformation arrays, one for each state.
} sklInstance;

void boneInit(sklBone *bone);
void boneInterpolate(const sklBone *b1, const sklBone *b2, const float t, sklBone *r);

void sklInit(skeleton *skl);
unsigned char sklLoad(skeleton *skl, const char *prgPath, const char *filePath);
void sklDelete(skeleton *skl);

void sklaInit(sklAnim *skla);
unsigned char sklaLoad(sklAnim *skla, const char *prgPath, const char *filePath);
void sklaDelete(sklAnim *skla);

unsigned char sklaiInit(sklAnimInstance *sklai, skeleton *skl, const size_t stateNum);
void sklaiChangeAnim(sklAnimInstance *sklai, const size_t anim, const size_t frame, const float blendTime);
void sklaiDelete(sklAnimInstance *sklai, const size_t boneNum);

unsigned char skliInit(sklInstance *skli, skeleton *skl, const size_t stateNum);
unsigned char skliLoad(sklInstance *skli, const size_t stateNum, const char *prgPath, const char *filePath);
void skliAnimate(sklInstance *skli, const size_t stateNum, const float elapsedTime);
void skliGenerateAnimStates(sklInstance *skli, const size_t state, const float interpT);
void skliGenerateBoneState(const sklInstance *skli, const skeleton *skl, mat4 *state, const size_t bone);
void skliGenerateState(const sklInstance *skli, const skeleton *skl, mat4 *state);
void skliDelete(sklInstance *skli, const size_t stateNum);

#endif
