#ifndef PHYSICSJOINTSPHERE_H
#define PHYSICSJOINTSPHERE_H

typedef struct physJoint physJoint;
typedef struct {

} physJointSphere;

void physJointSolveVelocityConstraintsSphere(const physJoint *const restrict joint);

#endif