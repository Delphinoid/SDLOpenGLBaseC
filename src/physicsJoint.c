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
#define physJointFixedPresolveConstraints     NULL
#define physJointPrismaticPresolveConstraints NULL
#define physJointRevolutePresolveConstraints  NULL
#define physJointSpherePresolveConstraints    NULL

void (* const physJointPresolveConstraintsJumpTable[PHYSICS_JOINT_TYPE_NUM])(
	physJoint *const restrict joint,
	physRigidBody *const restrict bodyA,
	physRigidBody *const restrict bodyB,
	const float dt
) = {
	physJointFixedPresolveConstraints,
	physJointDistancePresolveConstraints,
	physJointPrismaticPresolveConstraints,
	physJointRevolutePresolveConstraints,
	physJointSpherePresolveConstraints
};
__FORCE_INLINE__ void physJointPresolveConstraints(physJoint *const restrict joint, const float dt){

	physJointPresolveConstraintsJumpTable[joint->type](joint, joint->bodyA, joint->bodyB, dt);

}

/** The lines below should eventually be removed. **/
#define physJointFixedSolveVelocityConstraints     NULL
#define physJointPrismaticSolveVelocityConstraints NULL
#define physJointRevoluteSolveVelocityConstraints  NULL
#define physJointSphereSolveVelocityConstraints    NULL

void (* const physJointSolveVelocityConstraintsJumpTable[PHYSICS_JOINT_TYPE_NUM])(
	physJoint *const restrict joint,
	physRigidBody *const restrict bodyA,
	physRigidBody *const restrict bodyB
) = {
	physJointFixedSolveVelocityConstraints,
	physJointDistanceSolveVelocityConstraints,
	physJointPrismaticSolveVelocityConstraints,
	physJointRevoluteSolveVelocityConstraints,
	physJointSphereSolveVelocityConstraints
};
__FORCE_INLINE__ void physJointSolveVelocityConstraints(physJoint *const restrict joint){

	physJointSolveVelocityConstraintsJumpTable[joint->type](joint, joint->bodyA, joint->bodyB);

}

#ifdef PHYSICS_SOLVER_GAUSS_SEIDEL

/** The lines below should eventually be removed. **/
#define physJointFixedSolveConfigurationConstraints     NULL
#define physJointPrismaticSolveConfigurationConstraints NULL
#define physJointRevoluteSolveConfigurationConstraints  NULL
#define physJointSphereSolveConfigurationConstraints    NULL

return_t (* const physJointSolveConfigurationConstraintsJumpTable[PHYSICS_JOINT_TYPE_NUM])(
	physJoint *const restrict joint,
	physRigidBody *const restrict bodyA,
	physRigidBody *const restrict bodyB
) = {
	physJointFixedSolveConfigurationConstraints,
	physJointDistanceSolveConfigurationConstraints,
	physJointPrismaticSolveConfigurationConstraints,
	physJointRevoluteSolveConfigurationConstraints,
	physJointSphereSolveConfigurationConstraints
};
__FORCE_INLINE__ return_t physJointSolveConfigurationConstraints(physJoint *const restrict joint){

	return physJointSolveConfigurationConstraintsJumpTable[joint->type](joint, joint->bodyA, joint->bodyB);

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