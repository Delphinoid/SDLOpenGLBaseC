#ifndef PHYSICSJOINTREVOLUTE_H
#define PHYSICSJOINTREVOLUTE_H

/*
** Keeps the bodies at a fixed distance
** and allows rotation along only one axis.
** Acts as a hinge joint.
*/

typedef struct physJoint physJoint;
typedef struct {

	//

} physJointRevolute;

void physJointRevoluteSolveVelocityConstraints(const physJoint *const restrict joint);

#endif