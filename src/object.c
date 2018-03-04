#include "object.h"

signed char objInit(void *obj){
	((object *)obj)->skl = NULL;
	((object *)obj)->customState = NULL;
	((object *)obj)->skeletonState[0] = NULL;
	((object *)obj)->skeletonState[1] = NULL;
	((object *)obj)->physicsSimulate = 0;
	((object *)obj)->physicsState = NULL;
	((object *)obj)->renderableNum = 0;
	((object *)obj)->renderables = NULL;
	return skliInit(&((object *)obj)->animationData, 0)/** &&
	       kcInit(  &((object *)obj)->animationData, 0) &&
	       hbInit(  &((object *)obj)->animationData, 0)**/;
}

signed char objNew(void *obj){
	((object *)obj)->name = NULL;
	rndrConfigInit(&((object *)obj)->configuration);
	return objInit(obj);
}

/** FIX FOR PHYSICS OBJECTS **/
signed char objStateCopy(void *o, void *c){

	/* Copy configuration. */
	rndrConfigStateCopy(&((object *)o)->configuration, &((object *)c)->configuration);

	/* Resize the skeleton state arrays, if necessary, and copy everything over. */
	if(((object *)o)->skl != NULL){
		// Check if the skeleton state arrays need to be resized.
		size_t arraySizeS = ((object *)o)->skl->boneNum*sizeof(bone);
		size_t arraySizeP = ((object *)o)->skl->boneNum*sizeof(physRigidBody);
		if(((object *)c)->skl == NULL || ((object *)c)->skl->boneNum != ((object *)o)->skl->boneNum){
			// We need to allocate more or less memory so that
			// the memory allocated for both custom states match.
			bone *tempBuffer1 = malloc(arraySizeS);
			if(tempBuffer1 == NULL){
				/** Memory allocation failure. **/
				return 0;
			}
			bone *tempBuffer2 = malloc(arraySizeS);
			if(tempBuffer2 == NULL){
				/** Memory allocation failure. **/
				free(tempBuffer1);
				return 0;
			}
			bone *tempBuffer3 = malloc(arraySizeS);
			if(tempBuffer3 == NULL){
				/** Memory allocation failure. **/
				free(tempBuffer2);
				free(tempBuffer1);
				return 0;
			}
			physRigidBody *tempBuffer4 = malloc(arraySizeP);
			if(tempBuffer4 == NULL){
				/** Memory allocation failure. **/
				free(tempBuffer3);
				free(tempBuffer2);
				free(tempBuffer1);
				return 0;
			}
			if(((object *)c)->customState != NULL){
				free(((object *)c)->customState);
			}
			if(((object *)c)->skeletonState[0] != NULL){
				free(((object *)c)->skeletonState[0]);
			}
			if(((object *)c)->skeletonState[1] != NULL){
				free(((object *)c)->skeletonState[1]);
			}
			if(((object *)c)->physicsState != NULL){
				free(((object *)c)->physicsState);
			}
			((object *)c)->customState      = tempBuffer1;
			((object *)c)->skeletonState[0] = tempBuffer2;
			((object *)c)->skeletonState[1] = tempBuffer3;
			((object *)c)->physicsState     = tempBuffer4;
		}
		memcpy(((object *)c)->customState,      ((object *)o)->customState,      arraySizeS);
		memcpy(((object *)c)->skeletonState[0], ((object *)o)->skeletonState[0], arraySizeS);
		memcpy(((object *)c)->skeletonState[1], ((object *)o)->skeletonState[1], arraySizeS);
		memcpy(((object *)c)->physicsState,     ((object *)o)->physicsState,     arraySizeP);
	}else{
		((object *)c)->customState      = NULL;
		((object *)c)->skeletonState[0] = NULL;
		((object *)c)->skeletonState[1] = NULL;
		((object *)c)->physicsState     = NULL;
	}
	((object *)c)->skl = ((object *)o)->skl;

	/* Resize the renderables array, if necessary, and copy everything over. */
	if(((object *)c)->renderableNum != ((object *)o)->renderableNum){
		renderable *tempBuffer = malloc(((object *)o)->renderableNum*sizeof(renderable));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return 0;
		}
		if(((object *)c)->renderables != NULL){
			free(((object *)c)->renderables);
		}
		((object *)c)->renderables = tempBuffer;
		((object *)c)->renderableNum = ((object *)o)->renderableNum;
	}
	memcpy(((object *)c)->renderables, ((object *)o)->renderables, ((object *)o)->renderableNum*sizeof(renderable));

	/* Copy the more complex data types. */
	return skliStateCopy(&((object *)o)->animationData, &((object *)c)->animationData)/** &&
	       kcStateCopy(  &((object *)o)->physicsData,   &((object *)c)->physicsData)   &&
	       hbStateCopy(  &((object *)o)->hitboxData,    &((object *)c)->hitboxData)**/;
}

void objResetInterpolation(void *obj){
	rndrConfigResetInterpolation(&((object *)obj)->configuration);
}

void objDelete(void *obj){
	if(((object *)obj)->name != NULL){
		free(((object *)obj)->name);
	}
	if(((object *)obj)->customState != NULL){
		free(((object *)obj)->customState);
	}
	if(((object *)obj)->skeletonState[0] != NULL){
		free(((object *)obj)->skeletonState[0]);
	}
	if(((object *)obj)->skeletonState[1] != NULL){
		free(((object *)obj)->skeletonState[1]);
	}
	if(((object *)obj)->physicsState != NULL){
		size_t i;
		for(i = 0; i < ((object *)obj)->skl->boneNum; ++i){
			physRigidBodyDelete(&((object *)obj)->physicsState[i]);
		}
	}
	if(((object *)obj)->renderables != NULL){
		free(((object *)obj)->renderables);
	}
	skliDelete(&((object *)obj)->animationData);
	/**kcDelete(&((object *)obj)->physicsData);
	hbDelete(&((object *)obj)->hitboxData);**/
}

signed char objNewRenderable(object *obj){
	/* Allocate room for a new renderable and initialize it. */
	renderable *tempBuffer = realloc(obj->renderables, (obj->renderableNum+1)*sizeof(renderable));
	if(tempBuffer == NULL){
		/** Memory allocation failure. **/
		return 0;
	}
	obj->renderables = tempBuffer;
	obj->renderables[obj->renderableNum].mdl = NULL;
	twiInit(&obj->renderables[obj->renderableNum].twi, NULL);
	++obj->renderableNum;
	return 1;
}

signed char objDeleteRenderable(object *obj, size_t id){
	/* Remove a specified renderable. */
	if(obj->renderableNum > 0){
		size_t i, write;
		size_t arraySize = (obj->renderableNum-1)*sizeof(renderable);
		renderable *tempBuffer = malloc(arraySize);
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return 0;
		}
		if(id >= obj->renderableNum-1){
			memcpy(tempBuffer, obj->renderables, arraySize);
		}else{
			for(i = 0; i < obj->renderableNum; ++i){
				write = i - (i > id);
				tempBuffer[write] = obj->renderables[i];
			}
		}
		free(obj->renderables);
		obj->renderables = tempBuffer;
		--obj->renderableNum;
	}
	return 1;
}

/** FIX FOR PHYSICS OBJECTS **/
signed char objInitSkeleton(object *obj, skeleton *skl){
	if(skl != NULL){
		if(obj->skl == NULL){
			size_t i;
			size_t arraySizeS = skl->boneNum*sizeof(bone);
			size_t arraySizeP = skl->boneNum*sizeof(physRigidBody);
			bone *tempBuffer1 = malloc(arraySizeS);
			if(tempBuffer1 == NULL){
				/** Memory allocation failure. **/
				return 0;
			}
			bone *tempBuffer2 = malloc(arraySizeS);
			if(tempBuffer2 == NULL){
				/** Memory allocation failure. **/
				free(tempBuffer1);
				return 0;
			}
			bone *tempBuffer3 = malloc(arraySizeS);
			if(tempBuffer3 == NULL){
				/** Memory allocation failure. **/
				free(tempBuffer2);
				free(tempBuffer1);
				return 0;
			}
			physRigidBody *tempBuffer4 = malloc(arraySizeP);
			if(tempBuffer3 == NULL){
				/** Memory allocation failure. **/
				free(tempBuffer3);
				free(tempBuffer2);
				free(tempBuffer1);
				return 0;
			}
			obj->skl = skl;
			obj->customState      = tempBuffer1;
			obj->skeletonState[0] = tempBuffer2;
			obj->skeletonState[1] = tempBuffer3;
			obj->physicsState     = tempBuffer4;
			for(i = 0; i < skl->boneNum; ++i){
				boneInit(&obj->customState[i]);
			}
		}else{
			/** Reallocate everything for the new skeleton. **/
		}
	}
	return 1;
}

void objUpdate(object *obj, const float elapsedTime){

	size_t i;

	/* Update the object's skeletal animations and skeleton. */
	const float elapsedTimeMod = elapsedTime * obj->animationData.timeMod;

	// Swap the state pointers, so the previous state is in skeletonState[1].
	// We can reuse the old skeletonState[1] for the current state.
	bone *lastState = obj->skeletonState[0];
	obj->skeletonState[0] = obj->skeletonState[1];
	obj->skeletonState[1] = lastState;

	// Instead of initializing the bone states to unit vectors / quaternions,
	// we can set them to their accompanied custom states to save doing it later.
	for(i = 0; i < obj->skl->boneNum; ++i){
		obj->skeletonState[0][i] = obj->customState[i];
	}

	// Using each animation, generate a new state for each bone.
	for(i = 0; i < obj->animationData.animationNum; ++i){
		sklaiAnimate(&obj->animationData.animations[i], elapsedTimeMod);
		sklaiGenerateAnimState(&obj->animationData.animations[i], obj->skeletonState[0], obj->skl->boneNum, 1.f);
	}


	/* Transform the bones to global space and update the object's physics skeleton. */
	for(i = 0; i < obj->skl->boneNum; ++i){

		/**
		*** This is pretty bad, but I'm not sure if I have a choice short of
		*** merging bones and physics bodies or something weird like that.
		**/
		// Transform bones from local to global space.
		boneTransformAppend(&obj->skl->bones[i].defaultState, &obj->skeletonState[0][i], &obj->skeletonState[0][i]);
		//vec3AddVToV(&obj->skeletonState[0][i].position, &obj->skl->bones[i].defaultState.position);
		vec3SubVFromV1(&obj->skeletonState[0][i].position, &obj->renderables[0].mdl->skl->bones[i].defaultState.position);
		if(i != obj->skl->bones[i].parent){
			boneTransformAppend(&obj->skeletonState[0][obj->skl->bones[i].parent], &obj->skeletonState[0][i], &obj->skeletonState[0][i]);
		}

		// Only bodies that are not being simulated are affected by skeletal animations.
		if(obj->physicsSimulate && (obj->physicsState[i].flags & PHYSICS_BODY_SIMULATE) == 0){
			// Apply the bone's change in position to the physics object.
			obj->physicsState[i].position = obj->skeletonState[0][i].position;
			// Apply the bone's change in orientation to the physics object.
			obj->physicsState[i].orientation = obj->skeletonState[0][i].orientation;
		}

	}

			/*// Apply the bone's change in position to the physics object.
			obj->physicsData.bodies[i].position.x += obj->skeletonState[0][i].position.x - obj->skeletonState[1][i].position.x;
			obj->physicsData.bodies[i].position.y += obj->skeletonState[0][i].position.y - obj->skeletonState[1][i].position.y;
			obj->physicsData.bodies[i].position.z += obj->skeletonState[0][i].position.z - obj->skeletonState[1][i].position.z;
			// Apply the bone's change in orientation to the physics object.
			quatDifference(&obj->skeletonState[1][i].orientation, &obj->skeletonState[0][i].orientation, &obj->physicsData.bodies[i].orientation);*/

			/*// Apply the bone's change in position to the physics object.
			vec3SubVFromVR(&obj->skeletonState[0][i].position, &obj->skeletonState[1][i].position, &tempVec3);
			physBodyAddLinearVelocity(&obj->physicsData.bodies[i], &tempVec3);
			// Apply the bone's change in orientation to the physics object.
			quatDifference(&obj->skeletonState[1][i].orientation, &obj->skeletonState[0][i].orientation, &tempQuat);
			quatAxisAngleFast(&tempQuat, &tempFloat, &tempVec3.x, &tempVec3.y, &tempVec3.z);
			physBodyAddAngularVelocity(&obj->physicsData.bodies[i], tempFloat, tempVec3.x, tempVec3.y, tempVec3.z);*/
		//}

	//}


	/* Update each of the object's texture wrappers. */
	for(i = 0; i < obj->renderableNum; ++i){
		twiAnimate(&obj->renderables[i].twi, elapsedTime);
	}

}

signed char objRenderMethod(object *obj, const float interpT){
	// Update alpha.
	iFloatUpdate(&obj->configuration.alpha, interpT);
	if(obj->configuration.alpha.render > 0.f){
		size_t i;
		for(i = 0; i < obj->renderableNum; ++i){
			if(obj->configuration.alpha.render < 1.f || twiContainsTranslucency(&obj->renderables[i].twi)){
				// The model contains translucency.
				return 1;
			}
		}
		// The model is fully opaque.
		return 0;

	}
	// The model is fully transparent.
	return 2;
}
