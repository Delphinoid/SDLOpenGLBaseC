#ifndef SKELETON_H
#define SKELETON_H

#include "animation.h"
#include "mat4.h"

// Skeleton bone data.
typedef struct {
	vec3 position;
	quat orientation;
	vec3 scale;
} sklBone;

// Skeleton node, containing a bone and the index of its parent.
typedef struct {
	char *name;
	sklBone defaultState;
	size_t parent;
} sklNode;

// Combines the above structures.
typedef struct {
	char *name;
	size_t boneNum;
	sklNode *bones;  // Depth-first vector of each bone.
} skeleton;

// A full animation, containing a vector of keyframes.
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

/*
** Skeletal animation blending consists of the following components:
**
** An "animation fragment", containing:
**     A pointer to a skeletal animation (sklAnim).
**     An animator object, describing the animation's progress.
**     A bone look-up, translating bones in sklAnim to the sklAnimInstance's animState.
**     A pointer to an "animation blend".
**
** An "animation blend", containing:
**     A pointer to an "animation fragment", describing the animation that is being blended to.
**     The length of the blend, in milliseconds.
**     How long the animations have been blending for, in milliseconds.
**
** The skeleton animation instance contains a pointer to the oldest and newest animation
** fragments (the head and tail of the animation fragment "linked list). When blending to
** a new animation, a new animation fragment and animation blend are created, and added to
** the the last animation fragment's animNext. The tail is then adjusted accordingly.
**
** When an animation fragment has finished blending, it and its animation blend stay in the
** linked list, simply being ignored when calculating bone states. Animation fragments and
** blends are only removed when the oldest state has finished blending them, and they are
** therefore no longer necessary.
*/
/** ^ Last paragraph. **/

// Animation fragment, containing an animation pointer, an animator, a bone lookup and a blend.
typedef struct {
	sklAnim *currentAnim;
	animationInstance animator;
	/** Really need to move animBoneLookup. **/
	sklBone **animBoneLookup;  // Which bone in animState each bone in currentAnim corresponds to.
	                           // If the root (first) bone in currentAnim is named "blah" and the
	                           // second bone in animState is named "blah", the array will start
	                           // with animBoneLookup[0] == &animState[1]. If the bone does not
	                           // exist, its entry points to NULL.
	float animBlendTime;      // How long to blend between this and the next animation for. A value of -1 means there is currently no blend.
	float animBlendProgress;  // How long the animation has been blending for. A value of -1 means there is currently no blend.
} sklAnimFragment;

// Skeletal animation instance.
typedef struct {
	signed char additive;  // Whether the animation adds to previous animation instances or overwrites them.
	size_t animFragNum;
	size_t animFragCapacity;
	sklAnimFragment *animFrags;
	/** Move animState. **/
	sklBone *animState;  /** Is there any point storing this in previous states? **/  // Delta transformations for each bone in sklai->skl.
} sklAnimInstance;

// Skeleton instance.
/** Restructure for proper element attachments (and I've forgotten again, fantastic) **/
typedef struct {
	skeleton *skl;  // Should never be changed manually.
	float timeMod;
	/** Can we use pushDynamicArray()? **/
	size_t animationNum;
	size_t animationCapacity;
	sklAnimInstance *animations;
	/** Move sklAnimInstance's animState here if possible. **/
	sklBone *deltaState;
	sklBone *customState;  // Array of custom bone transformations.
} sklInstance;

void boneInit(sklBone *bone);
void boneInterpolate(const sklBone *b1, const sklBone *b2, const float t, sklBone *r);

void sklInit(skeleton *skl);
signed char sklLoad(skeleton *skl, const char *prgPath, const char *filePath);
void sklDelete(skeleton *skl);

void sklaInit(sklAnim *skla);
signed char sklaLoad(sklAnim *skla, const char *prgPath, const char *filePath);
void sklaDelete(sklAnim *skla);

signed char sklaiChangeAnim(sklAnimInstance *sklai, const skeleton *skl, sklAnim *anim, const size_t frame, const float blendTime);

signed char skliInit(sklInstance *skli, skeleton *skl, const size_t animationCapacity);
signed char skliLoad(sklInstance *skli, const char *prgPath, const char *filePath);
signed char skliStateCopy(sklInstance *o, sklInstance *c);
void skliAnimate(sklInstance *skli, const float elapsedTime);
void skliGenerateAnimStates(sklInstance *skli, const float interpT);
void skliGenerateBoneStates(const sklInstance *skli, mat4 *state);
void skliApplyBoneState(const sklInstance *skli, const mat4 *skeletonState, const skeleton *skl, mat4 *state, const size_t bone);
void skliDelete(sklInstance *skli);

#endif
