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
	if(joint->type < PHYSICS_JOINT_TYPE_NUM){
		physJointPresolveConstraintsJumpTable[joint->type](joint, joint->bodyA, joint->bodyB, dt_s);
	}
}

/** The lines below should eventually be removed. **/
#define physJointFixedSolveVelocityConstraints     NULL
#define physJointPrismaticSolveVelocityConstraints NULL
#define physJointRevoluteSolveVelocityConstraints  NULL

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
	if(joint->type < PHYSICS_JOINT_TYPE_NUM){
		physJointSolveVelocityConstraintsJumpTable[joint->type](joint, joint->bodyA, joint->bodyB);
	}
}

#ifdef PHYSICS_JOINT_STABILIZER_GAUSS_SEIDEL

/** The lines below should eventually be removed. **/
#define physJointFixedSolveConfigurationConstraints     NULL
#define physJointPrismaticSolveConfigurationConstraints NULL
#define physJointRevoluteSolveConfigurationConstraints  NULL

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
	if(joint->type >= PHYSICS_JOINT_TYPE_NUM){
		return 1;
	}
	return physJointSolveConfigurationConstraintsJumpTable[joint->type](joint, joint->bodyA, joint->bodyB);
}

#endif

void physJointAdd(physJoint *const joint, physRigidBody *bodyA, physRigidBody *bodyB){

	// Sort a new joint into its bodies.
	// A body's joint array first contains the joints
	// it owns, followed by joints that it does not own.
	// We assume that the owner of the joint is bodyA.

	joint->bodyA = bodyA;
	joint->bodyB = bodyB;


	#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
	memQLinkPrevA(joint) = (byte_t *)NULL;
	memQLinkNextA(joint) = (byte_t *)bodyA->joints;
	#else
	joint->prevA = NULL;
	joint->nextA = bodyA->joints;
	#endif
	// Since bodyA owns the joint, we can insert
	// it at the beginning of its joint array.
	if(bodyA->joints != NULL){
		#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
		memQLinkPrevA(bodyA->joints) = (byte_t *)joint;
		#else
		bodyA->joints->prevA = joint;
		#endif
    }
    bodyA->joints = joint;

	{
		// Find an insertion point for the joint in bodyB's joint array.
		physJoint *prev = NULL;
		physJoint *next = bodyB->joints;
		while(next != NULL && next->bodyA == bodyB){
			prev = next;
			#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
			next = (physJoint *)memQLinkNextA(next);
			#else
			next = next->nextA;
			#endif
		}
		// We've found the first joint not owned by bodyB.
		// Insert this joint before it.
		#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
		memQLinkPrevB(joint) = (byte_t *)prev;
		memQLinkNextB(joint) = (byte_t *)next;
		#else
		joint->prevB = prev;
		joint->nextB = next;
		#endif
		// If a joint exists before the insertion point, then
		// our bodyB is the same as this other joint's bodyA.
		if(prev != NULL){
			#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
			memQLinkNextA(prev) = (byte_t *)joint;
			#else
			prev->nextA = joint;
			#endif
		}else{
			bodyB->joints = joint;
		}
		// If a joint exists after the insertion point, then
		// our bodyB is the same as this other joint's bodyB.
		if(next != NULL){
			#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
			memQLinkPrevB(next) = (byte_t *)joint;
			#else
			next->prevB = joint;
			#endif
		}
	}

}

void physJointDelete(physJoint *const joint){

	// Removes a joint from its linked lists.

	physJoint *temp;

	// Remove references from the previous joints.
	#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
	temp = (physJoint *)memQLinkPrevA(joint);
	#else
	temp = joint->prevA;
	#endif
	if(temp != NULL){
		#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
		memQLinkNextA(temp) = memQLinkNextA(joint);
		#else
		temp->nextA = joint->nextA;
		#endif
	}else if(joint->bodyA != NULL){
		#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
		joint->bodyA->joints = (physJoint *)memQLinkNextA(joint);
		#else
		joint->bodyA->joints = joint->nextA;
		#endif
	}
	#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
	temp = (physJoint *)memQLinkPrevB(joint);
	#else
	temp = joint->prevB;
	#endif
	if(temp != NULL){
		if(temp->bodyA == joint->bodyB){
			#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
			memQLinkNextA(temp) = memQLinkNextB(joint);
			#else
			temp->nextA = joint->nextB;
			#endif
		}else{
			#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
			memQLinkNextB(temp) = memQLinkNextB(joint);
			#else
			temp->nextB = joint->nextB;
			#endif
		}
	}else if(joint->bodyB != NULL){
		#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
		joint->bodyB->joints = (physJoint *)memQLinkNextB(joint);
		#else
		joint->bodyB->joints = joint->nextB;
		#endif
	}

	// Remove references from the next joints.
	#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
	temp = (physJoint *)memQLinkNextA(joint);
	#else
	temp = joint->nextA;
	#endif
	if(temp != NULL){
		if(temp->bodyA == joint->bodyA){
			#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
			memQLinkPrevA(temp) = memQLinkPrevA(joint);
			#else
			temp->prevA = joint->prevA;
			#endif
		}else{
			#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
			memQLinkPrevB(temp) = memQLinkPrevA(joint);
			#else
			temp->prevB = joint->prevA;
			#endif
		}
	}
	#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
	temp = (physJoint *)memQLinkNextB(joint);
	#else
	temp = joint->nextB;
	#endif
	if(temp != NULL){
		#ifdef PHYSICS_CONTACT_USE_ALLOCATOR
		memQLinkPrevB(temp) = memQLinkPrevB(joint);
		#else
		temp->prevB = joint->prevB;
		#endif
	}

}
