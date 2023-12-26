#include "model.h"
#include "graphicsManager.h"
#include "texture.h"
#include "sprite.h"
#include "vertex.h"
#include "skeleton.h"
#include "camera.h"
#include "moduleSkeleton.h"
#include "moduleTextureWrapper.h"
#include "memoryManager.h"
#include "helpersFileIO.h"
#include <string.h>

return_t mdlWavefrontObjLoad(modelBase *const __RESTRICT__ base, const char *const __RESTRICT__ filePath, const size_t filePathLength);
return_t mdlSMDLoad(modelBase *const __RESTRICT__ base, const char *const __RESTRICT__ filePath, const size_t filePathLength);

// Default models.
modelBase g_mdlDefault = {
	.skl = &g_sklDefault,
	///.tw = &g_twDefault,
	///.buffers.indexNum = 0,
	///.buffers.vaoID = 0,
	///.buffers.vboID = 0,
	///.buffers.iboID = 0,
	.meshes = &g_meshDefault,
	.textures = (const textureWrapper **)&g_twDefaultP,
	.meshNum = 1,
	.lods = NULL,
	.lodNum = 0,
	.name = "default"
};
modelBase g_mdlSprite = {
	.skl = NULL,
	///.tw = &g_twDefault,
	///.buffers.indexNum = 0,
	///.buffers.vaoID = 0,
	///.buffers.vboID = 0,
	///.buffers.iboID = 0,
	.meshes = &g_meshSprite,
	.textures = (const textureWrapper **)&g_twDefaultP,
	.meshNum = 1,
	.lods = NULL,
	.lodNum = 0,
	.name = "sprite"
};
modelBase g_mdlBillboard = {
	.skl = &g_sklDefault,
	///.tw = &g_twDefault,
	///.buffers.indexNum = 0,
	///.buffers.vaoID = 0,
	///.buffers.vboID = 0,
	///.buffers.iboID = 0,
	.meshes = &g_meshBillboard,
	.textures = (const textureWrapper **)&g_twDefaultP,
	.meshNum = 1,
	.lods = NULL,
	.lodNum = 0,
	.name = "billboard"
};

void mdlBaseInit(modelBase *const __RESTRICT__ base){
	memset(base, 0, sizeof(modelBase));
}
return_t mdlBaseLoad(modelBase *const __RESTRICT__ base, const char *const __RESTRICT__ filePath, const size_t filePathLength){

	/** This sucks. **/
	switch(filePath[filePathLength-1]){
		case 'd':
			return mdlSMDLoad(base, filePath, filePathLength);
		case 'j':
			return mdlWavefrontObjLoad(base, filePath, filePathLength);
	}
	return 0;

}
/**return_t mdlBaseLoad(modelBase *const __RESTRICT__ base, const char *const __RESTRICT__ filePath, const size_t filePathLength){

	/** Create a proper model file that loads a specified mesh, a name and a skeleton. **
	return_t r;

	vertexIndex_t vertexNum;
	vertex *vertices;
	vertexIndex_t indexNum;
	vertexIndex_t *indices;
	size_t lodNum = 0;
	mdlLOD *lods = NULL;
	int sprite;

	char fullPath[FILE_MAX_PATH_LENGTH];
	char sklPath[FILE_MAX_PATH_LENGTH];
	char twPath[FILE_MAX_PATH_LENGTH];
	size_t sklPathLength = 0;
	size_t twPathLength = 0;

	fileGenerateFullPath(fullPath, MODEL_RESOURCE_DIRECTORY_STRING, MODEL_RESOURCE_DIRECTORY_LENGTH, filePath, filePathLength);

	mdlBaseInit(base);

	/** This sucks. **
	if(filePath[filePathLength-1] != 'd'){
		r = mdlWavefrontObjLoad(fullPath, &vertexNum, &vertices, &indexNum, &indices, &lodNum, &lods, &sprite, sklPath, &sklPathLength, twPath, &twPathLength);
	}else{
		skeleton *const tempSkl = moduleSkeletonAllocate();
		r = mdlSMDLoad(fullPath, &vertexNum, &vertices, &indexNum, &indices, tempSkl, twPath, &twPathLength);
		base->skl = tempSkl;
		sprite = 0;
	}
	/** Replace and move the loading function here. **
	if(r <= 0){
		return r;
	}

	/** Should mdlGenerateBuffers() be here? **
	//if(sprite){
		//sprPackVertexBuffer(vertexNum, vertices);
		//r = sprGenerateBuffers(&base->buffers, vertexNum, vertices, indexNum, indices);
	//}else{
		r = meshGenerateBuffers(&base->buffers, vertexNum, vertices, indexNum, indices);
	//}
	memFree(vertices);
	memFree(indices);

	if(base->skl == NULL){if(sklPathLength == 0){
		// Use the default skeleton.
		base->skl = &g_sklDefault;
	}else{
		/** Check if the skeleton already exists. If not, load it. **
		skeleton *const tempSkl = moduleSkeletonAllocate();
		if(tempSkl != NULL){
			const return_t r2 = sklLoad(tempSkl, sklPath, sklPathLength);
			if(r2 < 1){
				// The load failed. Clean up.
				moduleSkeletonFree(tempSkl);
				if(r2 < 0){
					/** Memory allocation failure. **
					mdlBaseDelete(base);
					return -1;
				}
				base->skl = &g_sklDefault;
			}else{
				base->skl = tempSkl;
			}
		}else{
			/** Memory allocation failure. **
			if(lods != NULL){
				memFree(lods);
			}
			mdlBaseDelete(base);
			return -1;
		}
	}}
	if(tw == NULL){if(twPathLength == 0){
		// Use the default texture wrapper.
		tw = &g_twDefault;
	}else{
		/** Check if the texture wrapper already exists. If not, load it. **
		textureWrapper *const tempTw = moduleTextureWrapperAllocate();
		if(tempTw != NULL){
			const return_t r2 = twLoad(tempTw, twPath, twPathLength);
			if(r2 < 1){
				// The load failed. Clean up.
				moduleTextureWrapperFree(tempTw);
				if(r2 < 0){
					/** Memory allocation failure. **
					mdlBaseDelete(base);
					return -1;
				}
				tw = &g_twDefault;
			}else{
				tw = tempTw;
			}
		}else{
			/** Memory allocation failure. **
			if(lods != NULL){
				memFree(lods);
			}
			mdlBaseDelete(base);
			return -1;
		}
	}}

	if(r > 0){

		// Generate a name based off the file path.
		base->name = memAllocate((filePathLength/**-extension**+1)*sizeof(char) + base->lodNum*sizeof(mdlLOD));
		if(base->name == NULL){
			/** Memory allocation failure. **
			mdlBaseDelete(base);
			return -1;
		}
		memcpy(base->name, filePath, filePathLength/**-extension**);
		base->name[filePathLength/**-extension**] = '\0';
		/**base->name = fileGenerateResourceName(filePath, filePathLength);
		if(base->name == NULL){
			** Memory allocation failure. **
			mdlBaseDelete(base);
			return -1;
		}**

		if(lods != NULL){
			memcpy(&base->name[filePathLength+1], lods, base->lodNum*sizeof(mdlLOD));
			memFree(lods);
		}

	}else{
		mdlBaseDelete(base);
	}

	return r;

}**/
void mdlBaseDelete(modelBase *const __RESTRICT__ base){
	if(base->meshes != NULL){
		mesh *m = base->meshes;
		if(m != &g_meshDefault && m != &g_meshSprite && m != &g_meshBillboard){
			const mesh *const mLast = &m[base->meshNum];
			while(m < mLast){
				// Set the LOD array pointer to NULL.
				// These are included in the same
				// allocation block as the mesh array.
				meshDelete(m);
				++m;
			}
			// Also frees textures, lods and the name.
			memFree(base->meshes);
		}
	}
	/**if(base->name != NULL && base->name != g_mdlDefault.name && base->name != g_mdlSprite.name && base->name != g_mdlBillboard.name){
		// Also frees the lods.
		memFree(base->name);
	}**/
}

/**void mdlBaseDefaultInit(){
	g_mdlDefault.meshes = &g_meshDefault;
}
void mdlBaseSpriteInit(){
	g_mdlSprite.meshes = &g_meshSprite;
}
void mdlBaseBillboardInit(){
	g_mdlBillboard.meshes = &g_meshBillboard;
}**/

__FORCE_INLINE__ void mdlFindCurrentLOD(const mdlLOD *lods, const size_t lodNum, vertexIndex_t *const __RESTRICT__ indexNum, const void **const __RESTRICT__ offset, const float distance, size_t bias){

	// Find the current LOD based off the distance.
	const mdlLOD *const lodFirst = lods;
	const mdlLOD *const lodLast = &lods[lodNum];

	// Loop through each LOD until one within
	// the specified distance is found.
	MDL_FIND_CURRENT_LOD_LOOP:
	if((++lods)->distance <= distance){
		if(lods < lodLast){
			goto MDL_FIND_CURRENT_LOD_LOOP;
		}
	}else{
		--lods;
	}

	// Apply the specified LOD bias.
	if(bias != 0){
		if(bias < 0){
			// If the bias is negative, get some
			// higher-detail LODs.
			while(lods > lodFirst && bias != 0){
				--lods; ++bias;
			}
		}else{
			// If the bias is positive, get some
			// lower-detail LODs.
			while(lods < lodLast && bias != 0){
				++lods; --bias;
			}
		}
	}

	*indexNum = lods->indexNum;
	*offset = lods->offset;

}

void mdlInit(model *const __RESTRICT__ mdl){
	///twiInit(&mdl->twi, &g_twDefault);
	mdl->twi = NULL;
	mdlStateInit(&mdl->state);
	billboardInit(&mdl->billboardData);
	mdl->base = NULL;
}
return_t mdlInstantiate(model *const __RESTRICT__ mdl, const modelBase *const base){
	mdl->twi = memAllocate(base->meshNum*sizeof(twInstance));
	if(mdl->twi == NULL){
		/** Memory allocation failure. **/
		return -1;
	}else{
		twInstance *twi = mdl->twi;
		const textureWrapper **tw = base->textures;
		const twInstance *const twiLast = &twi[base->meshNum];
		while(twi < twiLast){
			twiInit(twi, *tw);
			++twi; ++tw;
		}
		mdlStateInit(&mdl->state);
		billboardInit(&mdl->billboardData);
		mdl->base = base;
		return 1;
	}
	///twiInit(&mdl->twi, base->tw);
	///mdlStateInit(&mdl->state);
	///billboardInit(&mdl->billboardData);
	///mdl->base = base;
}
__FORCE_INLINE__ void mdlTick(model *const __RESTRICT__ mdl, const float dt_ms){
	twInstance *twi = mdl->twi;
	const twInstance *const twiLast = &twi[mdl->base->meshNum];
	while(twi < twiLast){
		twiTick(twi, dt_ms);
		++twi;
	}
	mdl->state.alphaPrevious = mdl->state.alphaCurrent;
	mdl->state.alphaCurrent = mdl->state.alpha;
}
void mdlRender(const model *const __RESTRICT__ mdl, const skeleton *const __RESTRICT__ skl, graphicsManager *const __RESTRICT__ gfxMngr, const camera *const __RESTRICT__ cam, const float distance, const vec3 centroid, const float interpT){

	// Interpolate the alpha.
	const float alpha = mdlStateAlphaRender(&mdl->state, interpT);

	if(alpha != 0.f && mdl->base->skl != NULL){

		const twInstance *twi = mdl->twi;
		const mesh *m = mdl->base->meshes;
		const mdlLOD *l = mdl->base->lods;
		const mesh *const mLast = &m[mdl->base->meshNum];

		vertexIndex_t indexNum;
		const void *offset;
		mat3x4 transformState;
		GLuint bArray = gfxMngr->shdrPrgObj.boneArrayID;
		boneIndex_t boneNum = mdl->base->skl->boneNum;
		sklNode *nLayout = mdl->base->skl->bones;
		boneIndex_t i;

		// If there is a valid animated skeleton,
		// apply animation transformations and feed
		// the bone configurations to the shader.
		for(i = 0; i < boneNum; ++i, ++bArray, ++nLayout){
			/** Use a lookup, same in object.c. **/
			boneIndex_t rndrBone = sklFindBone(skl, i, nLayout->name);
			if(rndrBone >= skl->boneNum){
				// Use the root bone's transformation if
				// the animated bone is not in the model.
				rndrBone = 0;
			}
			/** Not sure I like billboarding being per-bone. Can we use a model matrix? **/
			// Apply billboarding transformations if required.
			if(mdl->billboardData.flags != BILLBOARD_DISABLED){
				// Use the root bone's global position as the centroid for billboarding.
				transformState = billboardState(mdl->billboardData, cam, centroid, gfxMngr->shdrData.skeletonTransformState[rndrBone]);
			}else{
				transformState = gfxMngr->shdrData.skeletonTransformState[rndrBone];
			}
			// Feed the bone configuration to the shader.
			// OpenGL uses column-major order for matrix dimensions, unlike us!
			glUniformMatrix4x3fv(bArray, 1, GL_FALSE, &transformState.m[0][0]);
		}

		// Render each of the meshes associated with the model.
		while(m < mLast){

			// Get texture information for rendering and feed it to the shader.
			// Add an offset to the current animation for lenticular billboards.
			const twFrame *const frame = twiStateOffset(
				twi, billboardLenticular(mdl->billboardData, cam, *gfxMngr->shdrData.skeletonTransformState), interpT
			);

			// Bind the texture (if needed).
			gfxMngrBindTexture(gfxMngr, GL_TEXTURE0, frame->image->diffuseID);
			// Feed the texture coordinates to the shader.
			glUniform4fv(gfxMngr->shdrPrgObj.textureFragmentArrayID, 1, (const GLfloat *)&frame->subframe);
			// Feed the translucency multiplier to the shader.
			// Negative alpha values indicate dithering.
			glUniform1f(gfxMngr->shdrPrgObj.alphaID, alpha);

			glBindVertexArray(m->vaoID);
			// Find the right LOD.
			if(mdl->base->lods == NULL){
				indexNum = m->indexNum;
				offset = 0;
			}else{
				mdlFindCurrentLOD(l, mdl->base->lodNum, &indexNum, &offset, distance, gfxMngr->shdrData.biasLOD);
			}
			// Draw the mesh.
			if(indexNum > 0){
				glDrawElements(GL_TRIANGLES, indexNum, GL_UNSIGNED_INT, offset);
			}

			++twi; ++m;
			l += mdl->base->meshNum;

		}

	}

}
void mdlDelete(model *const __RESTRICT__ mdl){
	if(mdl->twi != NULL){
		memFree(mdl->twi);
	}
}
