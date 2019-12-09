#include "modulePhysics.h"
#include "inline.h"

void physJointInit(physJoint *const restrict joint, const flags_t flags, const physJointType_t type){
	joint->type = type;
	joint->flags = flags;
	joint->bodyA = NULL;
	joint->bodyB = NULL;
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

#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL

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

void physJointAdd(physJoint *const joint, physRigidBody *bodyA, physRigidBody *bodyB){

	// Sort a new joint into its bodies.

	// Set the joint's miscellaneous variables.
	// bodyA must be the body with the greater address.
	if(bodyA < bodyB){
		joint->bodyA = bodyB;
		joint->bodyB = bodyA;
		bodyA = bodyB;
		bodyB = joint->bodyB;
	}else{
		joint->bodyA = bodyA;
		joint->bodyB = bodyB;
	}

	// Find an insertion point for the joint in bodyA's joint array.
	// Joints are sorted from smallest partner address to largest.
	physJoint *previous = NULL;
	physJoint *next = bodyA->joints;
	while(next != NULL && bodyB > next->bodyB){
		previous = next;
		next = (physJoint *)memQLinkNextA(next);
	}

	if(previous != NULL){
		// Insert between the previous joint and its next joint.
		memQLinkNextA(previous) = (byte_t *)joint;
	}else{
		// Insert directly before the first joint.
		bodyA->joints = joint;
	}
	if(next != NULL){
		if(next->bodyA == bodyA){
			memQLinkPrevA(next) = (byte_t *)joint;
		}else{
			memQLinkPrevB(next) = (byte_t *)joint;
		}
	}
	memQLinkPrevA(joint) = (byte_t *)previous;
	memQLinkNextA(joint) = (byte_t *)next;


	// Find an insertion point for the joint in bodyB's joint array.
	// Joints are sorted from smallest partner address to largest.
	previous = NULL;
	next = bodyB->joints;
	while(next != NULL && next->bodyA == bodyB){
		previous = next;
		next = (physJoint *)memQLinkNextA(next);
	}

	if(previous != NULL){
		// Insert between the previous joint and its next joint.
		memQLinkNextA(previous) = (byte_t *)joint;
	}else{
		// Insert directly before the first joint.
		bodyB->joints = joint;
	}
	if(next != NULL){
		memQLinkPrevB(next) = (byte_t *)joint;
	}
	memQLinkPrevB(joint) = (byte_t *)previous;
	memQLinkNextB(joint) = (byte_t *)next;

}

void physJointDelete(physJoint *const joint){

	// Removes a joint from its linked lists.

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