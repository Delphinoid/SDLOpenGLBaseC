#ifndef SKELETON_H
#define SKELETON_H

#include "animation.h"
#include "mat4.h"

// Skeleton bone data
typedef struct {
	vec3 position;
	quat orientation;
	vec3 scale;
} sklBone;

// Skeleton node, containing a bone and links to the parent and its children
typedef struct sklNode sklNode;
typedef struct sklNode {
	char *name;
	sklBone defaultState;
	struct sklNode *parent;
	size_t childNum;
	struct sklNode **children;
} sklNode;

// Combines the above structures
typedef struct {
	char *name;
	sklNode *root;
	size_t boneNum;
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
	size_t boneNum;  // The total number of unique bones in the animation
	char **bones;    // Array of names for each bone
	sklBone **frames;  // An array of keyframes, where each keyframe is an array of
	                   // bone delta transforms (offsets from their default states).
} sklAnim;

// Skeletal animation instance
typedef struct {
	sklAnim *anim;
	animationInstance animInst;
	float animInterpT;
	sklBone *animInterpStart;
	sklBone *animInterpEnd;
	sklBone *animState;  // Delta transformations for each bone
} sklAnimInstance;

// Skeleton instance
/** Restructure for proper element attachments (?? No idea what I was talking about here) **/
typedef struct {
	skeleton *skl;  // Should never change
	float timeMod;
	size_t animationNum;
	size_t animationCapacity;
	sklAnimInstance *animations;
	sklBone *customState;  // Custom bone transformations
} sklInstance;

void boneInit(sklBone *bone);
void nodeInit(sklNode *node);

void sklInit(skeleton *skl);
unsigned char sklLoad(skeleton *skl, const char *prgPath, const char *filePath);
void sklDelete(skeleton *skl);

void sklaInit(sklAnim *skla);
unsigned char sklaLoad(sklAnim *skla, const char *prgPath, const char *filePath);
void sklaDelete(sklAnim *skla);

void sklaiInit(sklAnimInstance *sklai, const sklAnim *skla);
void sklaiChangeAnim(sklAnimInstance *sklai, const sklAnim *anim, const size_t frame, const float blendTime);
void sklaiDelete(sklAnimInstance *sklai);

void skliInit(sklInstance *skli, skeleton *skl);
unsigned char skliLoad(sklInstance *skli, const char *prgPath, const char *filePath);
void skliAnimate(sklInstance *skli, const float timeElapsed);
void skliGenerateState(sklInstance *skli, mat4 *state, const skeleton *skl);
void skliDelete(sklInstance *skli);

#endif
