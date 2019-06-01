#include "modulePhysics.h"
#include "inline.h"

void physJointInit(physJoint *const restrict joint, const physJointType_t type){
	joint->type = type;
	joint->flags = 0;
}

void physJointCreate(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, const physJointType_t type){

    ///

}

/** The lines below should eventually be removed. **/
#define physJointSolveVelocityConstraintsFixed     NULL
#define physJointSolveVelocityConstraintsDistance  NULL
#define physJointSolveVelocityConstraintsPrismatic NULL
#define physJointSolveVelocityConstraintsRevolute  NULL
#define physJointSolveVelocityConstraintsSphere    NULL

void (* const physJointSolveVelocityConstraintsJumpTable[PHYSICS_JOINT_TYPE_NUM])(
	const void *const restrict joint
) = {
	physJointSolveVelocityConstraintsFixed,
	physJointSolveVelocityConstraintsDistance,
	physJointSolveVelocityConstraintsPrismatic,
	physJointSolveVelocityConstraintsRevolute,
	physJointSolveVelocityConstraintsSphere
};
__FORCE_INLINE__ void physJointSolveVelocityConstraints(const physJoint *const restrict joint){

	physJointSolveVelocityConstraintsJumpTable[joint->type](joint);

}

#ifdef PHYSICS_SOLVER_GAUSS_SEIDEL

/** The lines below should eventually be removed. **/
#define physJointSolveConfigurationConstraintsFixed     NULL
#define physJointSolveConfigurationConstraintsDistance  NULL
#define physJointSolveConfigurationConstraintsPrismatic NULL
#define physJointSolveConfigurationConstraintsRevolute  NULL
#define physJointSolveConfigurationConstraintsSphere    NULL

void (* const physJointSolveConfigurationConstraintsJumpTable[PHYSICS_JOINT_TYPE_NUM])(
	const void *const restrict joint
) = {
	physJointSolveConfigurationConstraintsFixed,
	physJointSolveConfigurationConstraintsDistance,
	physJointSolveConfigurationConstraintsPrismatic,
	physJointSolveConfigurationConstraintsRevolute,
	physJointSolveConfigurationConstraintsSphere
};
__FORCE_INLINE__ void physJointSolveConfigurationConstraints(const physJoint *const restrict joint){

	physJointSolveVelocityConstraintsJumpTable[joint->type](joint);

}

#endif

void physJointDelete(physJoint *const joint){

	/*
	** Removes a joint from its linked lists.
	*/

	physJoint *temp;

	// Remove references from the previous joints.
	temp = (physJoint *)memQLinkPrevA(joint);
	if(temp != NULL){
		memQLinkNextA(temp) = memQLinkNextA(joint);
	}else{
		joint->bodyA->joints = (physJoint *)memQLinkNextA(joint);
	}
	temp = (physJoint *)memQLinkPrevB(joint);
	if(temp != NULL){
		if(temp->bodyA == joint->bodyB){
			memQLinkNextA(temp) = memQLinkNextB(joint);
		}else{
			memQLinkNextB(temp) = memQLinkNextB(joint);
		}
	}else{
		joint->bodyB->joints = (physJoint *)memQLinkNextB(joint);
	}

	// Remove references from the next joints.
	temp = (physJoint *)memQLinkNextA(joint);
	if(temp != NULL){
		if(temp->bodyA == joint->bodyA){
			memQLinkPrevA(temp) = memQLinkPrevA(joint);
		}else{
			memQLinkPrevB(temp) = memQLinkPrevA(joint);
		}
	}
	temp = (physJoint *)memQLinkNextB(joint);
	if(temp != NULL){
		memQLinkPrevB(temp) = memQLinkPrevB(joint);
	}

}