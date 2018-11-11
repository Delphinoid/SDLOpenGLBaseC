#ifndef SKELETON_H
#define SKELETON_H

#include "bone.h"
#include "animation.h"
#include "flags.h"
#include "return.h"

#define SKL_MAX_BONE_NUM 128

typedef uint_least8_t boneIndex_t;

// Skeleton node, containing a bone and the index of its parent.
typedef struct {
	char *name;
	bone defaultState;
	boneIndex_t parent;  // If the node has no parent, this will be set to its own position in bones.
} sklNode;

// Combines the above structures.
typedef struct {
	char *name;
	boneIndex_t boneNum;
	sklNode *bones;  // Depth-first vector of each bone.
} skeleton;

/**
typedef struct {
	float length;
	bone state;
} sklaFrame;

// A full animation, containing a vector of keyframes.
typedef struct {
	char *name;
	animationData animData;
	boneIndex_t boneNum;  // The total number of unique bones in the animation.
	char *bones;         // Array of names for each bone.
	sklaFrame **frames;   // An array of keyframes, where each keyframe is an array of
	                      // bone delta transforms (offsets from their default states).
} sklAnim;
**/

// A full animation, containing a vector of keyframes.
typedef struct {
	char *name;
	animationData animData;
	boneIndex_t boneNum;  // The total number of unique bones in the animation.
	/** char* rather than char**? **/
	char **bones;         // Array of names for each bone.
	bone **frames;  // An array of keyframes, where each keyframe is an array of
	                // bone delta transforms (offsets from their default states).
} sklAnim;

/*
** Skeletal animation blending works using "animation fragments".
*/

// Animation fragment, containing an animation pointer, an animator, a bone lookup and a blend.
typedef struct {

	sklAnim *currentAnim;
	animationInstance animator;
	/** Really need to move animBoneLookup. **/
	//boneIndex_t animBoneLookup[SKL_MAX_BONE_NUM];  // Which bone in skeletonState each bone in currentAnim corresponds to.
	                                               // If the root (first) bone in currentAnim is named "blah" and the
	                                               // second bone in animState is named "blah", the array will start
	                                               // with animBoneLookup[0] == 1. If the bone does not exist, its entry
	                                               // will be (boneIndex_t)-1.

	frameIndex_t animStartFrame;  // The frame to interpolate from when updating.
	frameIndex_t animEndFrame;    // The frame to interpolate to when updating.
	float animInterpT;            // How much to interpolate between animStartFrame and animEndFrame.

	float animBlendTime;      // How long to blend between this and the next animation for. A value of -1 means there is currently no blend.
	float animBlendProgress;  // How long the animation has been blending for. A value of -1 means there is currently no blend.

} sklAnimFragment;

// Skeletal animation instance.
typedef struct {
	float timeMod;
	flags_t additive;
	animIndex_t animFragNum;
	animIndex_t animFragCapacity;
	sklAnimFragment *animFrags;
} sklAnimInstance;

// Skeleton instance.
/** Restructure for proper element attachments (and I've forgotten again, fantastic) **/
typedef struct {
	skeleton *skl;
	float timeMod;
	animIndex_t animationNum;
	animIndex_t animationCapacity;
	sklAnimInstance *animations;
} sklInstance;

void sklInit(skeleton *skl);
return_t sklLoad(skeleton *skl, const char *prgPath, const char *filePath);
return_t sklCopy(const skeleton *oskl, skeleton *cskl);
return_t sklDefault(skeleton *skl);
/** If we're sticking with fixed bone lookups, change the return value to void. **/
//return_t sklGenerateLookup(const skeleton *skl1, const skeleton *skl2, uint_least8_t *lookup);
boneIndex_t sklFindBone(const skeleton *skl, const boneIndex_t id, const char *name);
void sklDelete(skeleton *skl);

void sklaInit(sklAnim *skla);
return_t sklaLoad(sklAnim *skla, const char *prgPath, const char *filePath);
return_t sklaLoadSMD(sklAnim *skla, const skeleton *skl, const char *prgPath, const char *filePath);
boneIndex_t sklaFindBone(const sklAnim *skla, const boneIndex_t id, const char *name);
void sklaDelete(sklAnim *skla);

return_t skliInit(sklInstance *skli, skeleton *skl, const animIndex_t animationCapacity);
#include"cVector.h"
return_t skliLoad(sklInstance *skli, const char *prgPath, const char *filePath, cVector *allSklAnimations);
void skliUpdateAnimations(sklInstance *skli, const float elapsedTime, const float interpT);
//void skliGenerateBoneState(sklInstance *skli, bone *boneState, const boneIndex_t boneID);
void skliGenerateBoneState(const sklInstance *skli, const boneIndex_t id, const char *name, bone *state);
void skliSetAnimationType(sklInstance *skli, const animIndex_t slot, const flags_t additive);
return_t skliChangeAnimation(sklInstance *skli, const animIndex_t slot, sklAnim *anim, const frameIndex_t frame, const float blendTime);
void skliClearAnimation(sklInstance *skli, const animIndex_t slot);
return_t skliStateCopy(sklInstance *o, sklInstance *c);
void skliDelete(sklInstance *skli);

#endif
