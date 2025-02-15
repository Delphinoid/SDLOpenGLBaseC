#include "scene.h"
#include "object.h"
#include "physicsRigidBody.h"
#include "memoryManager.h"

void scnInit(scene *const __RESTRICT__ scn){
	// Allocate the object pointer list.
	scn->objects = NULL;
	scn->objectNum = 0;
	physIslandInit(&scn->island);
}

void scnInsertJoint(scene *const __RESTRICT__ scn, physJoint *const joint){
	physIslandInsertJoint(&scn->island, joint);
}
void scnRemoveJoint(scene *const __RESTRICT__ scn, physJoint *const joint){
	physIslandRemoveJoint(&scn->island, joint);
}

void scnInsertRigidBody(scene *const __RESTRICT__ scn, physRigidBody *const body){
	physIslandInsertRigidBody(&scn->island, body);
}
void scnRemoveRigidBody(scene *const __RESTRICT__ scn, physRigidBody *const body){
	physIslandRemoveRigidBody(&scn->island, body);
}

void scnInsertObject(scene *const __RESTRICT__ scn, object *const __RESTRICT__ obj){

	///physRigidBody *body;

	// Prepend the object to the linked list.
	if(scn->objects != NULL){
		memDLinkPrev(scn->objects) = (byte_t *)obj;
		memDLinkNext(obj) = (byte_t *)scn->objects;
	}
	memDLinkPrev(obj) = NULL;
	scn->objects = obj;
	++scn->objectNum;

	// Insert the rigid bodies into the physics system,
	// maintaining the doubly-linked list pointers.
	physIslandInsertRigidBodies(&scn->island, obj->skeletonBodies, obj->skeletonBodyNum);

	/// Insert the rigid bodies into the physics system.
	/**body = obj->skeletonBodies;
	if(body != NULL){
		do {
			physIslandInsertRigidBody(&scn->island, body);
			body = (physRigidBody *)memDLinkNext(body);
		} while(body != NULL && !physRigidBodyIsRoot(body));
	}**/

}
void scnRemoveObject(scene *const __RESTRICT__ scn, object *const __RESTRICT__ obj){

	///physRigidBody *body;

	// Set the next element's previous pointer.
	if(memDLinkNext(obj) != NULL){
		memDLinkPrev(memDLinkNext(obj)) = memDLinkPrev(obj);
	}
	// Set the previous element's next pointer.
	if(memDLinkPrev(obj) != NULL && scn->objects != obj){
		memDLinkNext(memDLinkPrev(obj)) = memDLinkNext(obj);
	}else{
		scn->objects = (object *)memDLinkNext(obj);
	}
	// Completely remove the element from the list.
	memDLinkPrev(obj) = NULL;
	memDLinkNext(obj) = NULL;
	--scn->objectNum;

	// Remove the rigid bodies from the physics system,
	// maintaining the doubly-linked list pointers.
	physIslandRemoveRigidBodies(&scn->island, obj->skeletonBodies, obj->skeletonBodyNum);

	/// Remove the rigid bodies from the physics system.
	/**body = obj->skeletonBodies;
	if(body != NULL){
		do {
			physIslandRemoveRigidBody(&scn->island, body);
			body = (physRigidBody *)memDLinkNext(body);
		} while(body != NULL && !physRigidBodyIsRoot(body));
	}**/

}

#ifdef PHYSICS_CONTACT_STABILIZER_BAUMGARTE
return_t scnTick(scene *const __RESTRICT__ scn, const float dt_ms, const float dt_s, const float frequency){
#else
return_t scnTick(scene *const __RESTRICT__ scn, const float dt_ms, const float dt_s){
#endif

	// Update each object in the scene.
	object *i = scn->objects;
	while(i != NULL){
		// Update each object in the scene.
		if(objTick(i, dt_ms) < 0){
			/** Memory allocation failure. **/
			return -1;
		}
		i = (object *)memDLinkNext(i);
	}

	// Update the physics system.
	#ifdef PHYSICS_CONTACT_STABILIZER_BAUMGARTE
	return physIslandTick(&scn->island, dt_s, frequency);
	#else
	return physIslandTick(&scn->island, dt_s);
	#endif

}

void scnReset(scene *const __RESTRICT__ scn){
	physIslandDelete(&scn->island);
	scn->objects = NULL;
	scn->objectNum = 0;
}

void scnDelete(scene *const __RESTRICT__ scn){
	physIslandDelete(&scn->island);
}
