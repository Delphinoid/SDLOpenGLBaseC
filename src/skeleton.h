#ifndef SKELETON_H
#define SKELETON_H

#include "cVector.h"
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
	cVector bones;  // Holds sklBones; represents the delta transforms for each modified bone
	//cVector frameIDs;    // Holds size_ts; represents the sklBone each element in frameBones refers to
} sklKeyframe;

// A full animation, containing a vector of keyframes
typedef struct {
	int desiredLoops;     // How many times the animation will loop (with -1 being infinite times)
	cVector keyframes;    // Holds sklKeyframes
	cVector frameDelays;  // Holds floats; represents how long each frame should last
	size_t boneNum;
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
	unsigned char animMode;
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
typedef struct {
	skeleton *skl;  // Should never change
	cVector animations;    // Holds sklAnimInstances
	sklBone *customState;  // Custom bone transformations
	mat4 *skeletonState;
} sklInstance;

void sklInit(skeleton *skl);
unsigned char sklLoad(skeleton *skl, const char *prgPath, const char *filePath);
void sklDelete(skeleton *skl);

void sklaiInit(sklAnimInstance *sklai);
unsigned char sklaiLoad(sklAnimInstance *sklai, const char *prgPath, const char *filePath);
void sklaiDelete(sklAnimInstance *sklai);

void skliInit(sklInstance *skli, skeleton *skl);
unsigned char skliLoad(sklInstance *skli, const char *prgPath, const char *filePath);
void skliAnimate(sklInstance *skli, uint32_t currentTick, float globalDelayMod);
void skliDelete(sklInstance *skli);

#endif
