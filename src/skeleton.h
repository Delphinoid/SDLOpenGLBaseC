#ifndef SKELETON_H
#define SKELETON_H

#include "bone.h"
#include "animation.h"
#include "flags.h"
#include "return.h"

#define SKELETON_MAX_BONE_NUM 128

#define SKELETON_ANIM_INSTANCE_ADDITIVE  0
#define SKELETON_ANIM_INSTANCE_OVERWRITE 1

typedef uint_least8_t boneIndex_t;

// Skeleton node, containing a bone and the index of its parent.
// Bone states are stored as offsets from their parents.
typedef struct {
	bone defaultState;
	/** Should this be set to (boneIndex_t)-1 instead? **/
	boneIndex_t parent;  // If the node has no parent, this will be set to its own position in bones.
	char *name;
} sklNode;

// Combines the above structures.
typedef struct {
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
typedef struct {
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
typedef struct {

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
typedef struct {
	sklAnimFragment *fragments;
	float timeMod;
	flags_t flags;
} sklAnimInstance;

// Skeleton instance.
/** Restructure for proper element attachments (and I've forgotten again, fantastic) **/
typedef struct {
	sklAnimInstance *animations;
	float timeMod;
	const skeleton *skl;
} sklInstance;

extern skeleton sklDefault;

void sklInit(skeleton *const restrict skl);
return_t sklLoad(skeleton *const restrict skl, const char *const restrict prgPath, const char *const restrict filePath);
/** If we're sticking with fixed bone lookups, change the return value to void. **/
//return_t sklGenerateLookup(const skeleton *skl1, const skeleton *skl2, uint_least8_t *lookup);
boneIndex_t sklFindBone(const skeleton *const restrict skl, const boneIndex_t id, const char *const restrict name);
void sklDelete(skeleton *const restrict skl);

void sklaInit(sklAnim *const restrict skla);
return_t sklaLoad(sklAnim *const restrict skla, const char *const restrict prgPath, const char *const restrict filePath);
return_t sklaLoadSMD(sklAnim *const restrict skla, const skeleton *const restrict skl, const char *const restrict prgPath, const char *const restrict filePath, const int invert);
boneIndex_t sklaFindBone(const sklAnim *const restrict skla, const boneIndex_t id, const char *const restrict name);
void sklaDelete(sklAnim *const restrict skla);

void sklaiInit(sklAnimInstance *const restrict sklai, const flags_t flags);
void sklaiSetType(sklAnimInstance *const restrict sklai, const flags_t additive);
return_t sklaiChange(sklAnimInstance *const restrict sklai, const skeleton *const restrict skl, sklAnim *const restrict anim, const float intensity, const frameIndex_t frame, const float blendTime);
void sklaiClear(sklAnimInstance *const restrict sklai);
void sklaiDelete(sklAnimInstance *const restrict sklai);

return_t skliInit(sklInstance *const restrict skli, const skeleton *const restrict skl, const animIndex_t animationCapacity);
return_t skliLoad(sklInstance *const restrict skli, const char *const restrict prgPath, const char *const restrict filePath);
sklAnimInstance *skliAnimationNew(sklInstance *const restrict skli, const flags_t flags);
void skliAnimationDelete(sklInstance *const restrict skli, sklAnimInstance *const restrict anim, sklAnimInstance *const restrict previous);
void skliUpdateAnimations(sklInstance *const restrict skli, const float elapsedTime, const float interpT);
//void skliGenerateBoneState(sklInstance *skli, bone *boneState, const boneIndex_t boneID);
void skliGenerateBoneState(const sklInstance *const restrict skli, const boneIndex_t id, const char *const restrict name, bone *const restrict state);
///void skliSetAnimationType(sklInstance *skli, const animIndex_t slot, const flags_t additive);
///return_t skliChangeAnimation(sklInstance *skli, const animIndex_t slot, sklAnim *anim, const frameIndex_t frame, const float blendTime);
///void skliClearAnimation(sklInstance *skli, const animIndex_t slot);
///return_t skliStateCopy(sklInstance *o, sklInstance *c);
void skliDelete(sklInstance *const restrict skli);

#endif
