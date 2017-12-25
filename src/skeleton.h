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

// Forward declaration for sklAnimBlend.
typedef struct sklAnimBlend sklAnimBlend;

// Animation fragment, containing an animation pointer, an animator, a bone lookup and a blend.
typedef struct {
	sklAnim *currentAnim;
	animationInstance animator;
	/** Move animBoneLookup. **/
	sklBone **animBoneLookup;  // Which bone in animState each bone in currentAnim corresponds to.
	                           // If the root (first) bone in currentAnim is named "blah" and the
	                           // second bone in animState is named "blah", the array will start
	                           // with animBoneLookup[0] == &animState[1]. If the bone does not
	                           // exist, its entry points to NULL.
	sklAnimBlend *animNext;
} sklAnimFragment;

// Animation blend.
typedef struct sklAnimBlend {
	sklAnimFragment *blendAnim;  // A pointer to the animation being blended to.
	float blendTime;      // How long to blend between animations for.
	float blendProgress;  // How long the animation has been blending for. Size is stateNum.
} sklAnimBlend;

// Skeletal animation instance.
typedef struct {
	sklAnimFragment *animListHead;  // A pointer to the head of a linked list of animation fragments.
	                                // Elements are only removed when their animNext is not NULL and
	                                // their animNext->blendProgress[stateNum] is greater than their
	                                // animNext->blendTime.
	sklAnimFragment *animListTail;  // A pointer to the tail of the above linked list of animation fragments.
	/** Move animState. **/
	sklBone *animState;  // Delta transformations for each bone.
} sklAnimInstance;

// Skeleton instance.
/** Restructure for proper element attachments (actually I do know now, but I'm not writing it down) **/
typedef struct {
	skeleton *skl;  // Should never be changed manually.
	float timeMod;
	/** Can we use pushDynamicArray()? **/
	size_t animationNum;
	size_t animationCapacity;
	sklAnimInstance *animations;
	/** Move sklAnimInstance's animState here if possible. **/
	sklBone *customState;  // Array of custom bone transformations.
} sklInstance;

void boneInit(sklBone *bone);
void boneInterpolate(const sklBone *b1, const sklBone *b2, const float t, sklBone *r);

void sklInit(skeleton *skl);
unsigned char sklLoad(skeleton *skl, const char *prgPath, const char *filePath);
void sklDelete(skeleton *skl);

void sklaInit(sklAnim *skla);
unsigned char sklaLoad(sklAnim *skla, const char *prgPath, const char *filePath);
void sklaDelete(sklAnim *skla);

unsigned char sklaiChangeAnim(sklAnimInstance *sklai, const skeleton *skl, sklAnim *anim, const size_t frame, const float blendTime);

unsigned char skliInit(sklInstance *skli, skeleton *skl, const size_t animationCapacity);
unsigned char skliLoad(sklInstance *skli, const char *prgPath, const char *filePath);
unsigned char skliStateCopy(const sklInstance *o, sklInstance *c);
void skliAnimate(sklInstance *skli, const float elapsedTime);
void skliGenerateAnimStates(sklInstance *skli, const float interpT);
void skliGenerateBoneState(const sklInstance *skli, const skeleton *skl, mat4 *state, const size_t bone);
void skliDelete(sklInstance *skli);

#endif
