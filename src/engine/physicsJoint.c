#include "physicsJoint.h"
#include "physicsRigidBody.h"
#include "memoryQLink.h"
#include <stddef.h>

void physJointInit(physJoint *const __RESTRICT__ joint, const flags_t flags, const physJointType_t type){
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
	physJoint *const __RESTRICT__ joint,
	physRigidBody *const __RESTRICT__ bodyA,
	physRigidBody *const __RESTRICT__ bodyB,
	const float dt_s
) = {
	physJointFixedPresolveConstraints,
	physJointDistancePresolveConstraints,
	physJointPrismaticPresolveConstraints,
	physJointRevolutePresolveConstraints,
	physJointSpherePresolveConstraints
};
__FORCE_INLINE__ void physJointPresolveConstraints(physJoint *const __RESTRICT__ joint, const float dt_s){

	physJointPresolveConstraintsJumpTable[joint->type](joint, joint->bodyA, joint->bodyB, dt_s);

}

/** The lines below should eventually be removed. **/
#define physJointFixedSolveVelocityConstraints     NULL
#define physJointPrismaticSolveVelocityConstraints NULL
#define physJointRevoluteSolveVelocityConstraints  NULL
#define physJointSphereSolveVelocityConstraints    NULL

void (* const physJointSolveVelocityConstraintsJumpTable[PHYSICS_JOINT_TYPE_NUM])(
	physJoint *const __RESTRICT__ joint,
	physRigidBody *const __RESTRICT__ bodyA,
	physRigidBody *const __RESTRICT__ bodyB
) = {
	physJointFixedSolveVelocityConstraints,
	physJointDistanceSolveVelocityConstraints,
	physJointPrismaticSolveVelocityConstraints,
	physJointRevoluteSolveVelocityConstraints,
	physJointSphereSolveVelocityConstraints
};
__FORCE_INLINE__ void physJointSolveVelocityConstraints(physJoint *const __RESTRICT__ joint){

	physJointSolveVelocityConstraintsJumpTable[joint->type](joint, joint->bodyA, joint->bodyB);

}

#ifdef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL

/** The lines below should eventually be removed. **/
#define physJointFixedSolveConfigurationConstraints     NULL
#define physJointPrismaticSolveConfigurationConstraints NULL
#define physJointRevoluteSolveConfigurationConstraints  NULL
#define physJointSphereSolveConfigurationConstraints    NULL

return_t (* const physJointSolveConfigurationConstraintsJumpTable[PHYSICS_JOINT_TYPE_NUM])(
	physJoint *const __RESTRICT__ joint,
	physRigidBody *const __RESTRICT__ bodyA,
	physRigidBody *const __RESTRICT__ bodyB
) = {
	physJointFixedSolveConfigurationConstraints,
	physJointDistanceSolveConfigurationConstraints,
	physJointPrismaticSolveConfigurationConstraints,
	physJointRevoluteSolveConfigurationConstraints,
	physJointSphereSolveConfigurationConstraints
};
__FORCE_INLINE__ return_t physJointSolveConfigurationConstraints(physJoint *const __RESTRICT__ joint){

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
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		next = (physJoint *)memQLinkNextA(next);
		#else
		next = next->nextA;
		#endif
	}

	if(previous != NULL){
		// Insert between the previous joint and its next joint.
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		memQLinkNextA(previous) = (byte_t *)joint;
		#else
		previous->nextA = joint;
		#endif
	}else{
		// Insert directly before the first joint.
		bodyA->joints = joint;
	}
	if(next != NULL){
		if(next->bodyA == bodyA){
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			memQLinkPrevA(next) = (byte_t *)joint;
			#else
			next->prevA = joint;
			#endif
		}else{
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			memQLinkPrevB(next) = (byte_t *)joint;
			#else
			next->prevB = joint;
			#endif
		}
	}
	#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
	memQLinkPrevA(joint) = (byte_t *)previous;
	memQLinkNextA(joint) = (byte_t *)next;
	#else
	joint->prevA = previous;
	joint->nextA = next;
	#endif


	// Find an insertion point for the joint in bodyB's joint array.
	// Joints are sorted from smallest partner address to largest.
	previous = NULL;
	next = bodyB->joints;
	while(next != NULL && next->bodyA == bodyB){
		previous = next;
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		next = (physJoint *)memQLinkNextA(next);
		#else
		next = next->nextA;
		#endif
	}

	if(previous != NULL){
		// Insert between the previous joint and its next joint.
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		memQLinkNextA(previous) = (byte_t *)joint;
		#else
		previous->nextA = joint;
		#endif
	}else{
		// Insert directly before the first joint.
		bodyB->joints = joint;
	}
	if(next != NULL){
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		memQLinkPrevB(next) = (byte_t *)joint;
		#else
		next->prevB = joint;
		#endif
	}
	#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
	memQLinkPrevB(joint) = (byte_t *)previous;
	memQLinkNextB(joint) = (byte_t *)next;
	#else
	joint->prevB = previous;
	joint->nextB = next;
	#endif

}

void physJointDelete(physJoint *const joint){

	// Removes a joint from its linked lists.

	physJoint *temp;

	// Remove references from the previous joints.
	#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
	temp = (physJoint *)memQLinkPrevA(joint);
	#else
	temp = joint->prevA;
	#endif
	if(temp != NULL){
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		memQLinkNextA(temp) = memQLinkNextA(joint);
		#else
		temp->nextA = joint->nextA;
		#endif
	}else{
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		joint->bodyA->joints = (physJoint *)memQLinkNextA(joint);
		#else
		joint->bodyA->joints = joint->nextA;
		#endif
	}
	#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
	temp = (physJoint *)memQLinkPrevB(joint);
	#else
	temp = joint->prevB;
	#endif
	if(temp != NULL){
		if(temp->bodyA == joint->bodyB){
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			memQLinkNextA(temp) = memQLinkNextB(joint);
			#else
			temp->nextA = joint->nextB;
			#endif
		}else{
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			memQLinkNextB(temp) = memQLinkNextB(joint);
			#else
			temp->nextB = joint->nextB;
			#endif
		}
	}else{
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		joint->bodyB->joints = (physJoint *)memQLinkNextB(joint);
		#else
		joint->bodyB->joints = joint->nextB;
		#endif
	}

	// Remove references from the next joints.
	#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
	temp = (physJoint *)memQLinkNextA(joint);
	#else
	temp = joint->nextA;
	#endif
	if(temp != NULL){
		if(temp->bodyA == joint->bodyA){
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			memQLinkPrevA(temp) = memQLinkPrevA(joint);
			#else
			temp->prevA = joint->prevA;
			#endif
		}else{
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			memQLinkPrevB(temp) = memQLinkPrevA(joint);
			#else
			temp->prevB = joint->prevA;
			#endif
		}
	}
	#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
	temp = (physJoint *)memQLinkNextB(joint);
	#else
	temp = joint->nextB;
	#endif
	if(temp != NULL){
		#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
		memQLinkPrevB(temp) = memQLinkPrevB(joint);
		#else
		temp->prevB = joint->prevB;
		#endif
	}

}