#ifndef PHYSICSJOINTREVOLUTE_H
#define PHYSICSJOINTREVOLUTE_H

typedef struct physJoint physJoint;
typedef struct {

} physJointRevolute;

void physJointSolveVelocityConstraintsRevolute(const physJoint *const restrict joint);

#endif