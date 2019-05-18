#ifndef PHYSICSJOINTPRISMATIC_H
#define PHYSICSJOINTPRISMATIC_H

typedef struct physJoint physJoint;
typedef struct {

} physJointPrismatic;

void physJointSolveVelocityConstraintsPrismatic(const physJoint *const restrict joint);

#endif