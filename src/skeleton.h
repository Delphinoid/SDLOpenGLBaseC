#ifndef SKELETON_H
#define SKELETON_H

#include "skeletonShared.h"
#include "bone.h"
#include "animation.h"
#include "flags.h"
#include "return.h"

#define SKELETON_ANIM_INSTANCE_ADDITIVE  0
#define SKELETON_ANIM_INSTANCE_OVERWRITE 1

// Skeleton node, containing a bone and the index of its parent.
// Bone states are stored as offsets from their parents.
typedef struct {
	bone defaultState;
	/** Should this be set to (boneIndex_t)-1 instead? **/
	boneIndex_t parent;  // If the node has no parent, this will be set to its own position in bones.
	char *name;
} sklNode;

// Combines the above structures.
typedef struct skeleton {
	sklNode *bones;  // Depth-first vector of each bone.
	boneIndex_t boneNum;
	char *name;
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
// Animation frames are independent and not relative to the previous frame.
// They are, however, relative to the skeleton's bind position. For example,
// translations are relative to the bind pose's axes, not the current frame's.
typedef struct sklAnim {
	animationData animData;
	boneIndex_t boneNum;  // The total number of unique bones in the animation.
	/** char* rather than char**? **/
	char **bones;         // Array of names for each bone.
	bone **frames;  // An array of keyframes, where each keyframe is an array of
	                // bone delta transforms (offsets from their default states).
	char *name;
} sklAnim;

// Skeletal animation blending works using "animation fragments".
// Animation fragment, containing an animation pointer, an animator, a bone lookup and a blend.
/** After implementing the intensity value, maybe remove animation fragments. **/
typedef struct sklAnimFragment {

	const sklAnim *animation;
	animationInstance animator;
	/** Really need to move animBoneLookup. **/
	//boneIndex_t animBoneLookup[SKELETON_MAX_BONE_NUM];  // Which bone in skeletonState each bone in currentAnim corresponds to.
	                                               // If the root (first) bone in currentAnim is named "blah" and the
	                                               // second bone in animState is named "blah", the array will start
	                                               // with animBoneLookup[0] == 1. If the bone does not exist, its entry
	                                               // will be (boneIndex_t)-1.

	frameIndex_t animStartFrame;  // The frame to interpolate from when updating.
	frameIndex_t animEndFrame;    // The frame to interpolate to when updating.
	float animInterpT;            // How much to interpolate between animStartFrame and animEndFrame.

	float animBlendTime;      // How long to blend between this and the next animation for. A value of -1 means there is currently no blend.
	float animBlendProgress;  // How long the animation has been blending for. A value of -1 means there is currently no blend.
	float animBlendInterpT;

	float intensity;

} sklAnimFragment;

// Skeletal animation instance.
/** Implement "intensity" value for blending. **/
typedef struct sklAnimInstance {
	sklAnimFragment *fragments;
	float timeMod;
	flags_t flags;
} sklAnimInstance;

// Skeleton instance.
/** Restructure for proper element attachments (and I've forgotten again, fantastic) **/
typedef struct sklInstance {
	sklAnimInstance *animations;
	float timeMod;
	const skeleton *skl;
} sklInstance;

extern skeleton g_sklDefault;

void sklInit(skeleton *const __RESTRICT__ skl);
return_t sklLoad(skeleton *const __RESTRICT__ skl, const char *const __RESTRICT__ filePath, const size_t filePathLength);
/** If we're sticking with fixed bone lookups, change the return value to void. **/
//return_t sklGenerateLookup(const skeleton *skl1, const skeleton *skl2, uint_least8_t *lookup);
boneIndex_t sklFindBone(const skeleton *const __RESTRICT__ skl, const boneIndex_t id, const char *const __RESTRICT__ name);
void sklDelete(skeleton *const __RESTRICT__ skl);

void sklaInit(sklAnim *const __RESTRICT__ skla);
return_t sklaLoad(sklAnim *const __RESTRICT__ skla, const char *const __RESTRICT__ filePath, const size_t filePathLength);
return_t sklaLoadSMD(sklAnim *const __RESTRICT__ skla, const skeleton *const __RESTRICT__ skl, const char *const __RESTRICT__ filePath, const size_t filePathLength, const int invert);
boneIndex_t sklaFindBone(const sklAnim *const __RESTRICT__ skla, const boneIndex_t id, const char *const __RESTRICT__ name);
void sklaDelete(sklAnim *const __RESTRICT__ skla);

void sklaiInit(sklAnimInstance *const __RESTRICT__ sklai, const flags_t flags);
void sklaiSetType(sklAnimInstance *const __RESTRICT__ sklai, const flags_t additive);
return_t sklaiChange(sklAnimInstance *const __RESTRICT__ sklai, const skeleton *const __RESTRICT__ skl, sklAnim *const __RESTRICT__ anim, const float intensity, const frameIndex_t frame, const float blendTime);
void sklaiClear(sklAnimInstance *const __RESTRICT__ sklai);
void sklaiDelete(sklAnimInstance *const __RESTRICT__ sklai);

return_t skliInit(sklInstance *const __RESTRICT__ skli, const skeleton *const __RESTRICT__ skl, const animIndex_t animationCapacity);
return_t skliLoad(sklInstance *const __RESTRICT__ skli, const char *const __RESTRICT__ prgPath, const char *const __RESTRICT__ filePath);
sklAnimInstance *skliAnimationNew(sklInstance *const __RESTRICT__ skli, const flags_t flags);
void skliAnimationDelete(sklInstance *const __RESTRICT__ skli, sklAnimInstance *const __RESTRICT__ anim, sklAnimInstance *const __RESTRICT__ previous);
void skliTick(sklInstance *const __RESTRICT__ skli, const float elapsedTime, const float interpT);
//void skliGenerateBoneState(sklInstance *skli, bone *boneState, const boneIndex_t boneID);
void skliGenerateBoneState(const sklInstance *const __RESTRICT__ skli, const boneIndex_t id, const char *const __RESTRICT__ name, bone *const __RESTRICT__ state);
///void skliSetAnimationType(sklInstance *skli, const animIndex_t slot, const flags_t additive);
///return_t skliChangeAnimation(sklInstance *skli, const animIndex_t slot, sklAnim *anim, const frameIndex_t frame, const float blendTime);
///void skliClearAnimation(sklInstance *skli, const animIndex_t slot);
///return_t skliStateCopy(sklInstance *o, sklInstance *c);
void skliDelete(sklInstance *const __RESTRICT__ skli);

#endif
