#include "modulePhysics.h"

void physJointInit(physJoint *const restrict joint, const physJointType_t type){
	joint->type = type;
}

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
		joint->parent->joints = (physJoint *)memQLinkNextA(joint);
	}
	temp = (physJoint *)memQLinkPrevB(joint);
	if(temp != NULL){
		if(temp->parent == joint->child){
			memQLinkNextA(temp) = memQLinkNextB(joint);
		}else{
			memQLinkNextB(temp) = memQLinkNextB(joint);
		}
	}else{
		joint->child->joints = (physJoint *)memQLinkNextB(joint);
	}

	// Remove references from the next joints.
	temp = (physJoint *)memQLinkNextA(joint);
	if(temp != NULL){
		if(temp->parent == joint->parent){
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