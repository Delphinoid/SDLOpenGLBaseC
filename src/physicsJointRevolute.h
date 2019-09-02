#ifndef PHYSICSJOINTREVOLUTE_H
#define PHYSICSJOINTREVOLUTE_H

typedef struct physJoint physJoint;
typedef struct {

} physJointRevolute;

void physJointRevoluteSolveVelocityConstraints(const physJoint *const restrict joint);

#endif