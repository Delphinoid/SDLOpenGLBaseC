#include "object.h"

signed char objInit(void *obj){
	((object *)obj)->skl = NULL;
	((object *)obj)->configuration = NULL;
	((object *)obj)->skeletonState[0] = NULL;
	((object *)obj)->skeletonState[1] = NULL;
	((object *)obj)->renderableNum = 0;
	((object *)obj)->renderables = NULL;
	((object *)obj)->physicsSimulate = 0;
	((object *)obj)->physicsState = NULL;
	return skliInit(&((object *)obj)->animationData, 0)/** &&
	       kcInit(  &((object *)obj)->animationData, 0) &&
	       hbInit(  &((object *)obj)->animationData, 0)**/;
}

signed char objNew(void *obj){
	((object *)obj)->name = NULL;
	rndrConfigInit(&((object *)obj)->tempRndrConfig);
	return objInit(obj);
}

/** FIX FOR PHYSICS OBJECTS **/
signed char objStateCopy(void *o, void *c){

	/* Copy configuration. */
	rndrConfigStateCopy(&((object *)o)->tempRndrConfig, &((object *)c)->tempRndrConfig);

	/* Resize the skeleton state arrays, if necessary, and copy everything over. */
	if(((object *)o)->skl != NULL){
		// Check if the skeleton state arrays need to be resized.
		size_t arraySizeS = ((object *)o)->skl->boneNum*sizeof(bone);
		if(((object *)c)->skl == NULL || ((object *)c)->skl->boneNum != ((object *)o)->skl->boneNum){
			// We need to allocate more or less memory so that
			// the memory allocated for both custom states match.
			bone *tempBuffer3;
			bone *tempBuffer2;
			bone *tempBuffer1 = malloc(arraySizeS);
			if(tempBuffer1 == NULL){
				/** Memory allocation failure. **/
				return 0;
			}
			tempBuffer2 = malloc(arraySizeS);
			if(tempBuffer2 == NULL){
				/** Memory allocation failure. **/
				free(tempBuffer1);
				return 0;
			}
			tempBuffer3 = malloc(arraySizeS);
			if(tempBuffer3 == NULL){
				/** Memory allocation failure. **/
				free(tempBuffer2);
				free(tempBuffer1);
				return 0;
			}
			if(((object *)c)->configuration != NULL){
				free(((object *)c)->configuration);
			}
			if(((object *)c)->skeletonState[0] != NULL){
				free(((object *)c)->skeletonState[0]);
			}
			if(((object *)c)->skeletonState[1] != NULL){
				free(((object *)c)->skeletonState[1]);
			}
			((object *)c)->configuration    = tempBuffer1;
			((object *)c)->skeletonState[0] = tempBuffer2;
			((object *)c)->skeletonState[1] = tempBuffer3;
		}
		memcpy(((object *)c)->configuration,    ((object *)o)->configuration,    arraySizeS);
		memcpy(((object *)c)->skeletonState[0], ((object *)o)->skeletonState[0], arraySizeS);
		memcpy(((object *)c)->skeletonState[1], ((object *)o)->skeletonState[1], arraySizeS);
	}else{
		((object *)c)->configuration    = NULL;
		((object *)c)->skeletonState[0] = NULL;
		((object *)c)->skeletonState[1] = NULL;
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
	rndrConfigResetInterpolation(&((object *)obj)->tempRndrConfig);
}

void objDelete(void *obj){
	size_t i;
	if(((object *)obj)->name != NULL){
		free(((object *)obj)->name);
	}
	if(((object *)obj)->configuration != NULL){
		free(((object *)obj)->configuration);
	}
	if(((object *)obj)->skeletonState[0] != NULL){
		free(((object *)obj)->skeletonState[0]);
	}
	if(((object *)obj)->skeletonState[1] != NULL){
		free(((object *)obj)->skeletonState[1]);
	}
	if(((object *)obj)->renderables != NULL){
		for(i = 0; i < ((object *)obj)->renderableNum; ++i){
			rndrDelete(&((object *)obj)->renderables[i]);
		}
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
	rndrInit(&obj->renderables[obj->renderableNum]);
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
			bone *tempBuffer3;
			bone *tempBuffer2;
			bone *tempBuffer1 = malloc(arraySizeS);
			if(tempBuffer1 == NULL){
				/** Memory allocation failure. **/
				return 0;
			}
			tempBuffer2 = malloc(arraySizeS);
			if(tempBuffer2 == NULL){
				/** Memory allocation failure. **/
				free(tempBuffer1);
				return 0;
			}
			tempBuffer3 = malloc(arraySizeS);
			if(tempBuffer3 == NULL){
				/** Memory allocation failure. **/
				free(tempBuffer2);
				free(tempBuffer1);
				return 0;
			}
			obj->skl = skl;
			obj->configuration    = tempBuffer1;
			obj->skeletonState[0] = tempBuffer2;
			obj->skeletonState[1] = tempBuffer3;
			for(i = 0; i < skl->boneNum; ++i){
				boneInit(&obj->configuration[i]);
			}
		}else{
			/** Reallocate everything for the new skeleton. **/
		}
	}
	return 1;
}

void objUpdate(object *obj, const camera *cam, const float elapsedTime){

	size_t i;
	bone transformBone;

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
		obj->skeletonState[0][i] = obj->configuration[i];
	}

	// Using each animation, generate a new state for each bone.
	for(i = 0; i < obj->animationData.animationNum; ++i){
		sklaiAnimate(&obj->animationData.animations[i], elapsedTimeMod);
		sklaiGenerateAnimState(&obj->animationData.animations[i], obj->skeletonState[0], obj->configuration, obj->skl->boneNum, 1.f);
	}


	/* Transform the bones to global space and update the object's physics skeleton. */
	for(i = 0; i < obj->skl->boneNum; ++i){

		/**
		*** This is pretty bad, but I'm not sure if I have a choice short of
		*** merging bones and physics bodies or something weird like that.
		**/
		// Transform bones from local to global space.
		boneTransformAppendPosition(&obj->skl->bones[i].defaultState, &obj->skeletonState[0][i], &obj->skeletonState[0][i]);

		if(obj->skl->bones[i].parent < obj->skl->boneNum && i != obj->skl->bones[i].parent){
			// Transform parent into local space before applying transformations here.
			boneInvert(&obj->skl->bones[obj->skl->bones[i].parent].defaultState, &transformBone);
			boneTransformAppend(&obj->skeletonState[0][obj->skl->bones[i].parent], &transformBone, &transformBone);
			// Apply parent transformations.
			boneTransformAppend(&transformBone, &obj->skeletonState[0][i], &obj->skeletonState[0][i]);

		// Apply billboarding to root bones if necessary.
		}else if((obj->tempRndrConfig.flags & (RNDR_BILLBOARD_X | RNDR_BILLBOARD_Y | RNDR_BILLBOARD_Z)) > 0){
            //
		}

		// Update the positions of each rigid body connected to this bone.
		if(obj->physicsState != NULL && !obj->physicsSimulate &&
		   (obj->physicsState[i].flags & PHYSICS_BODY_SIMULATE) == 0){

			// Translate the bone into global space for the physics object.
			boneInvert(&obj->skl->bones[obj->skl->bones[i].parent].defaultState, &transformBone);
			boneTransformAppend(&obj->skeletonState[0][i], &transformBone,
			                    &obj->physicsState[i].configuration);

		}

		// Update the positions of each rigid body connected to this bone.
		//for(j = 0; j < obj->renderableNum; ++j){

			// Only bodies that are not being simulated are affected by skeletal animations.
			/** Use a lookup, same in renderScene.c. **/
			/*rndrBone = sklFindBone(obj->renderables[j].mdl->skl, obj->skl->bones[i].name);
			if(rndrBone < obj->renderables[j].mdl->skl->boneNum &&
			   obj->renderables[j].physicsState != NULL && !obj->renderables[j].physicsSimulate &&
			   (obj->renderables[j].physicsState[rndrBone].flags & PHYSICS_BODY_SIMULATE) == 0){

			   	// Translate the bone into global space for the physics object.
			   	boneInvert(&obj->skl->bones[obj->skl->bones[i].parent].defaultState, &transformBone);
			   	boneTransformAppend(&obj->skeletonState[0][i], &transformBone,
				                    &obj->renderables[j].physicsState[rndrBone].configuration);

			}*/

		//}

			// If any of the flags apart from RNDR_BILLBOARD_TARGET are set, continue.
			/**mat4 billboardRotation;
			if((rc->flags & RNDR_BILLBOARD_SPRITE) > 0){
				// Use a less accurate but faster method for billboarding.
				vec3 right, up, forward;
				// Use the camera's X, Y and Z axes for cheap sprite billboarding.
				vec3Set(&right,   cam->viewMatrix.m[0][0], cam->viewMatrix.m[0][1], cam->viewMatrix.m[0][2]);
				vec3Set(&up,      cam->viewMatrix.m[1][0], cam->viewMatrix.m[1][1], cam->viewMatrix.m[1][2]);
				vec3Set(&forward, cam->viewMatrix.m[2][0], cam->viewMatrix.m[2][1], cam->viewMatrix.m[2][2]);
				// Lock certain axes if needed.
				if((rc->flags & RNDR_BILLBOARD_X) == 0){
					right.y   = 0.f;
					up.y      = 1.f;
					forward.y = 0.f;
				}
				if((rc->flags & RNDR_BILLBOARD_Y) == 0){
					right.x   = 1.f;
					up.x      = 0.f;
					forward.x = 0.f;
				}
				if((rc->flags & RNDR_BILLBOARD_Z) == 0){
					right.z   = 0.f;
					up.z      = 0.f;
					forward.z = 1.f;
				}
				billboardRotation.m[0][0] = right.x; billboardRotation.m[0][1] = up.x; billboardRotation.m[0][2] = forward.x; billboardRotation.m[0][3] = 0.f;
				billboardRotation.m[1][0] = right.y; billboardRotation.m[1][1] = up.y; billboardRotation.m[1][2] = forward.y; billboardRotation.m[1][3] = 0.f;
				billboardRotation.m[2][0] = right.z; billboardRotation.m[2][1] = up.z; billboardRotation.m[2][2] = forward.z; billboardRotation.m[2][3] = 0.f;
				billboardRotation.m[3][0] = 0.f;     billboardRotation.m[3][1] = 0.f;  billboardRotation.m[3][2] = 0.f;       billboardRotation.m[3][3] = 1.f;
			}else{
				vec3 eye, target, up;
				if((rc->flags & RNDR_BILLBOARD_TARGET) > 0){
					eye = rc->targetPosition.render;
					target = rc->position.render;
					vec3Set(&up, 0.f, 1.f, 0.f);
					quatRotateVec3(&rc->targetOrientation.render, &up);
				}else if((rc->flags & RNDR_BILLBOARD_TARGET_CAMERA) > 0){
					eye = cam->position.render;
					target = rc->position.render;
					up = cam->up.render;
				}else{
					eye = cam->position.render;
					target = cam->targetPosition.render;
					up = cam->up.render;
				}
				// Lock certain axes if needed.
				if((rc->flags & RNDR_BILLBOARD_X) == 0){
					target.y = eye.y;
				}
				if((rc->flags & RNDR_BILLBOARD_Y) == 0){
					target.x = eye.x;
				}
				if((rc->flags & RNDR_BILLBOARD_Z) == 0){
					vec3Set(&up, 0.f, 1.f, 0.f);
				}
				mat4RotateToFace(&billboardRotation, &eye, &target, &up);
			}
			mat4MultMByM2(&billboardRotation, transformMatrix);  // Apply billboard rotation
		}**/

		// Only bodies that are not being simulated are affected by skeletal animations.
		/*if(obj->physicsSimulate && (obj->physicsState[i].flags & PHYSICS_BODY_SIMULATE) == 0){
			// Apply the bone's change in position to the physics object.
			obj->physicsState[i].position = obj->skeletonState[0][i].position;
			// Apply the bone's change in orientation to the physics object.
			obj->physicsState[i].orientation = obj->skeletonState[0][i].orientation;
		}*/

	}


	/* Update each of the object's texture wrappers. */
	for(i = 0; i < obj->renderableNum; ++i){
		twiAnimate(&obj->renderables[i].twi, elapsedTime);
	}

}

signed char objRenderMethod(object *obj, const float interpT){
	// Update alpha.
	iFloatUpdate(&obj->tempRndrConfig.alpha, interpT);
	if(obj->tempRndrConfig.alpha.render > 0.f){
		size_t i;
		for(i = 0; i < obj->renderableNum; ++i){
			if(obj->tempRndrConfig.alpha.render < 1.f || twiContainsTranslucency(&obj->renderables[i].twi)){
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

void objGenerateSprite(const object *obj, const size_t rndr, const float interpT, const float *texFrag, vertex *vertices){

	/* Generate the base sprite. */
	const float left   = -0.5f - obj->tempRndrConfig.pivot.render.x;
	const float top    = -0.5f - obj->tempRndrConfig.pivot.render.y;
	const float right  = 0.5f - obj->tempRndrConfig.pivot.render.x;
	const float bottom = 0.5f - obj->tempRndrConfig.pivot.render.y;
	const float z      = -obj->tempRndrConfig.pivot.render.z;
	bone transform;

	// Create the top left vertex.
	vertices[0].position.x = left;
	vertices[0].position.y = top;
	vertices[0].position.z = z;
	vertices[0].u = 0.f;
	vertices[0].v = 0.f;
	vertices[0].normal.x = 0.f;
	vertices[0].normal.y = 0.f;
	vertices[0].normal.z = 0.f;
	vertices[0].bIDs[0] = -1;
	vertices[0].bIDs[1] = -1;
	vertices[0].bIDs[2] = -1;
	vertices[0].bIDs[3] = -1;
	vertices[0].bWeights[0] = 0.f;
	vertices[0].bWeights[1] = 0.f;
	vertices[0].bWeights[2] = 0.f;
	vertices[0].bWeights[3] = 0.f;

	// Create the top right vertex.
	vertices[1].position.x = right;
	vertices[1].position.y = top;
	vertices[1].position.z = z;
	vertices[1].u = 1.f;
	vertices[1].v = 0.f;
	vertices[1].normal.x = 0.f;
	vertices[1].normal.y = 0.f;
	vertices[1].normal.z = 0.f;
	vertices[1].bIDs[0] = -1;
	vertices[1].bIDs[1] = -1;
	vertices[1].bIDs[2] = -1;
	vertices[1].bIDs[3] = -1;
	vertices[1].bWeights[0] = 0.f;
	vertices[1].bWeights[1] = 0.f;
	vertices[1].bWeights[2] = 0.f;
	vertices[1].bWeights[3] = 0.f;

	// Create the bottom left vertex.
	vertices[2].position.x = left;
	vertices[2].position.y = bottom;
	vertices[2].position.z = z;
	vertices[2].u = 0.f;
	vertices[2].v = -1.f;  // Flip the y dimension so the image isn't upside down.
	vertices[2].normal.x = 0.f;
	vertices[2].normal.y = 0.f;
	vertices[2].normal.z = 0.f;
	vertices[2].bIDs[0] = -1;
	vertices[2].bIDs[1] = -1;
	vertices[2].bIDs[2] = -1;
	vertices[2].bIDs[3] = -1;
	vertices[2].bWeights[0] = 0.f;
	vertices[2].bWeights[1] = 0.f;
	vertices[2].bWeights[2] = 0.f;
	vertices[2].bWeights[3] = 0.f;

	// Create the bottom right vertex.
	vertices[3].position.x = right;
	vertices[3].position.y = bottom;
	vertices[3].position.z = z;
	vertices[3].u = 1.f;
	vertices[3].v = -1.f;  // Flip the y dimension so the image isn't upside down.
	vertices[3].normal.x = 0.f;
	vertices[3].normal.y = 0.f;
	vertices[3].normal.z = 0.f;
	vertices[3].bIDs[0] = -1;
	vertices[3].bIDs[1] = -1;
	vertices[3].bIDs[2] = -1;
	vertices[3].bIDs[3] = -1;
	vertices[3].bWeights[0] = 0.f;
	vertices[3].bWeights[1] = 0.f;
	vertices[3].bWeights[2] = 0.f;
	vertices[3].bWeights[3] = 0.f;

	/* Generate a transformation for the sprite and transform each vertex. */
	/** Optimize? **/
	boneInterpolate(&obj->skeletonState[1][0], &obj->skeletonState[0][0], interpT, &transform);
	transform.scale.x *= twiGetFrameWidth(&obj->renderables[rndr].twi) * twiGetTexWidth(&obj->renderables[rndr].twi);
	transform.scale.y *= twiGetFrameHeight(&obj->renderables[rndr].twi) * twiGetTexHeight(&obj->renderables[rndr].twi);
	vertTransform(&vertices[0], &transform.position, &transform.orientation, &transform.scale);
	vertTransform(&vertices[1], &transform.position, &transform.orientation, &transform.scale);
	vertTransform(&vertices[2], &transform.position, &transform.orientation, &transform.scale);
	vertTransform(&vertices[3], &transform.position, &transform.orientation, &transform.scale);

	// We can't pass unique textureFragment values for each individual sprite when batching. Therefore,
	// we have to do the offset calculations for each vertex UV here instead of in the shader.
	vertices[0].u = vertices[0].u * texFrag[2] + texFrag[0];
	vertices[0].v = vertices[0].v * texFrag[3] + texFrag[1];
	vertices[1].u = vertices[1].u * texFrag[2] + texFrag[0];
	vertices[1].v = vertices[1].v * texFrag[3] + texFrag[1];
	vertices[2].u = vertices[2].u * texFrag[2] + texFrag[0];
	vertices[2].v = vertices[2].v * texFrag[3] + texFrag[1];
	vertices[3].u = vertices[3].u * texFrag[2] + texFrag[0];
	vertices[3].v = vertices[3].v * texFrag[3] + texFrag[1];

}
