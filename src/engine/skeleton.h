#ifndef SKELETON_H
#define SKELETON_H

#include "skeletonShared.h"
#include "transform.h"
#include "animation.h"
#include "flags.h"
#include "return.h"

#define SKELETON_ANIM_INSTANCE_ADDITIVE  0
#define SKELETON_ANIM_INSTANCE_OVERWRITE 1

// Skeleton node, containing a transform and the index of its parent.
// Bone states are stored as offsets from their parents.
typedef struct {
	transform defaultState;
	// If the node has no parent, this will be set to its own position in the array.
	boneIndex_t parent;
	char *name;
} sklNode;

// Combines the above structures.
typedef struct skeleton {
	// Depth-first vector of each bone.
	// Note that an array of bone names is stored after
	// the node array in bones, and this is followed by
	// the name of the skeleton. To free a skeleton, we
	// only need to free the bones array.
	sklNode *bones;
	boneIndex_t boneNum;
	char *name;
} skeleton;

// A full animation, containing a vector of keyframes.
// Animation frames are independent and not relative to the previous frame.
// They are, however, relative to the skeleton's bind position. For example,
// translations are relative to the bind pose's axes, not the current frame's.
typedef struct sklAnim {

	animationData animData;

	// The total number of unique bones in the animation.
	boneIndex_t boneNum;

	// Array of names for each bone.
	char **bones;
	// An array of keyframes, where each keyframe is an array of
	// bone delta transforms (offsets from their default states).
	transform **frames;

	char *name;

} sklAnim;

// Skeletal animation instance.
typedef struct sklAnimInstance {

	const sklAnim *animation;

	animationInstance animator;
	// The frame to interpolate from when updating.
	frameIndex_t animStartFrame;
	// The frame to interpolate to when updating.
	frameIndex_t animEndFrame;
	// How much to interpolate between animStartFrame and animEndFrame.
	float animInterpT;

	// Weight coefficient for the animation.
	// Used when overlapping animations.
	float intensity;
	// The following variables are generally used to
	// smoothly begin or end the playback of an animation,
	// by linearly interpolating its intensity value.
	// The intensity to decay to.
	float intensityLimit;
	// How much to decrease (or increase, if positive) the
	// intensity value by: each tick, intensity += decay.
	// If it sets intensity to R\(0, 1), decay is set to 0.f.
	float decay;

	float timeMod;
	flags_t flags;

} sklAnimInstance;

// Skeleton instance.
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

void sklaiInit(sklAnimInstance *const __RESTRICT__ sklai, const sklAnim *const __RESTRICT__ animation, const float intensity, const flags_t flags);
void sklaiSetType(sklAnimInstance *const __RESTRICT__ sklai, const flags_t additive);
void sklaiChange(sklAnimInstance *const __RESTRICT__ sklai, sklAnim *const __RESTRICT__ anim, const frameIndex_t frame);
void sklaiDecay(sklAnimInstance *const __RESTRICT__ sklai, const float intensity, const float decay);

return_t skliInit(sklInstance *const __RESTRICT__ skli, const skeleton *const __RESTRICT__ skl, const animIndex_t animationCapacity);
return_t skliLoad(sklInstance *const __RESTRICT__ skli, const char *const __RESTRICT__ prgPath, const char *const __RESTRICT__ filePath);
sklAnimInstance *skliAnimationNew(sklInstance *const __RESTRICT__ skli, sklAnim *const __RESTRICT__ anim, const float intensity, const flags_t flags);
void skliAnimationDelete(sklInstance *const __RESTRICT__ skli, sklAnimInstance *const __RESTRICT__ anim, sklAnimInstance *const __RESTRICT__ previous);
void skliTick(sklInstance *const __RESTRICT__ skli, const float dt_ms, const float interpT);
//void skliGenerateBoneState(sklInstance *skli, bone *boneState, const boneIndex_t boneID);
transform skliGenerateBoneState(const sklInstance *const __RESTRICT__ skli, const boneIndex_t id, const char *const __RESTRICT__ name, transform state);
///void skliSetAnimationType(sklInstance *skli, const animIndex_t slot, const flags_t additive);
///return_t skliChangeAnimation(sklInstance *skli, const animIndex_t slot, sklAnim *anim, const frameIndex_t frame, const float blendTime);
///void skliClearAnimation(sklInstance *skli, const animIndex_t slot);
///return_t skliStateCopy(sklInstance *o, sklInstance *c);
void skliDelete(sklInstance *const __RESTRICT__ skli);

#endif