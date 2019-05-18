#ifndef PHYSICSJOINTFIXED_H
#define PHYSICSJOINTFIXED_H

/*
** Keeps two bodies rigidly connected, preventing
** relative translational or rotational changes
** from occurring between them.
*/

typedef struct physJoint physJoint;
typedef struct {

	//

} physJointFixed;

void physJointSolveVelocityConstraintsFixed(const physJoint *const restrict joint);

#endif