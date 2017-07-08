#ifndef SKELETON_H
#define SKELETON_H

#include "quat.h"
#include "animationHandler.h"

// Skeleton bone data: position and rotation
typedef struct {
	vec3 position;
	quat orientation;
} sklBone;

// Skeleton node, containing a bone and links to the parent and its children
struct sklNode;
typedef struct {
	sklBone bone;
	struct sklNode *parent;
	size_t childNum;
	struct sklNode *children;
} sklNode;

// Keyframe used for animations
typedef struct {
	float   frameDelay;  // Time before the current frame
	cVector frameIDs;    // Holds size_ts; represents the sklBone each element in frameBones refers to
	cVector frameBones;  // Holds sklBones; represents the delta transforms for each modified bone
} sklKeyframe;

// A full animation, containing a vector of keyframes
typedef struct {
	int desiredLoops;  // How many times the animation will loop (with -1 being infinite times)
	cVector frames;    // Holds sklKeyframes
} sklAnim;

// Combines the above structures
typedef struct {
	char *name;
	sklNode *root;
	size_t boneNum;
} skeleton;

// Skeleton instance
typedef struct {
	skeleton *skl;
	sklBone *sklState;  // Current state of the skeleton, an array of transformed bones ready for GLSL
	unsigned char changed;  // Whether or not the skeleton state needs to be updated
	animationHandler animator;
} sklInstance;

void sklInit(skeleton *skl);
unsigned char sklLoad(skeleton *skl, const char *prgPath, const char *filePath);

void skliInit(sklInstance *skli, skeleton *skl);
void skliAnimate();
void skliChangeAnim();
unsigned char skliAnimFinished();
unsigned char skliGenerateState(sklInstance *skli);  // Generates an array of bones of size boneNum, ready for GLSL
void skliDelete(skeleton *skl);

#endif
