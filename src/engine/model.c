#include "model.h"
#include "sprite.h"
#include "vertex.h"
#include "skeleton.h"
#include "moduleSkeleton.h"
#include "memoryManager.h"
#include "helpersFileIO.h"
#include <string.h>

#define MODEL_RESOURCE_DIRECTORY_STRING FILE_PATH_RESOURCE_DIRECTORY_SHARED"Resources"FILE_PATH_DELIMITER_STRING"Models"FILE_PATH_DELIMITER_STRING
#define MODEL_RESOURCE_DIRECTORY_LENGTH 19

return_t mdlWavefrontObjLoad(const char *const __RESTRICT__ filePath, vertexIndex_t *const vertexNum, vertex **const vertices, vertexIndex_t *const __RESTRICT__ indexNum, vertexIndex_t **const indices, size_t *const __RESTRICT__ lodNum, mdlLOD **const lods, int *const __RESTRICT__ sprite, char *const __RESTRICT__ sklPath, size_t *const __RESTRICT__ sklPathLength);
return_t mdlSMDLoad(const char *filePath, vertexIndex_t *vertexNum, vertex **vertices, vertexIndex_t *indexNum, vertexIndex_t **indices, skeleton *const skl);

// Default models.
model g_mdlDefault = {
	.skl = &g_sklDefault,
	.lodNum = 0,
	.lods = NULL,
	.buffers.vertexNum = 0,
	.buffers.indexNum = 0,
	.buffers.vaoID = 0,
	.buffers.vboID = 0,
	.buffers.iboID = 0,
	.name = "default"
};
model g_mdlSprite = {
	.skl = NULL,
	.lodNum = 0,
	.lods = NULL,
	.buffers.vertexNum = 0,
	.buffers.indexNum = 0,
	.buffers.vaoID = 0,
	.buffers.vboID = 0,
	.buffers.iboID = 0,
	.name = "sprite"
};
model g_mdlBillboard = {
	.skl = &g_sklDefault,
	.lodNum = 0,
	.lods = NULL,
	.buffers.vertexNum = 0,
	.buffers.indexNum = 0,
	.buffers.vaoID = 0,
	.buffers.vboID = 0,
	.buffers.iboID = 0,
	.name = "billboard"
};

void mdlInit(model *const __RESTRICT__ mdl){
	mdl->name = NULL;
	mdl->buffers.vertexNum = 0;
	mdl->buffers.indexNum = 0;
	mdl->buffers.vaoID = 0;
	mdl->buffers.vboID = 0;
	mdl->buffers.iboID = 0;
	mdl->lodNum = 0;
	mdl->lods = NULL;
	mdl->skl = NULL;
}

return_t mdlLoad(model *const __RESTRICT__ mdl, const char *const __RESTRICT__ filePath, const size_t filePathLength){

	/** Create a proper model file that loads a specified mesh, a name and a skeleton. **/
	return_t r;

	vertexIndex_t vertexNum;
	vertex *vertices;
	vertexIndex_t indexNum;
	vertexIndex_t *indices;
	size_t lodNum;
	mdlLOD *lods;
	int sprite;

	char fullPath[FILE_MAX_PATH_LENGTH];
	char sklPath[FILE_MAX_PATH_LENGTH];
	size_t sklPathLength = 0;

	fileGenerateFullPath(fullPath, MODEL_RESOURCE_DIRECTORY_STRING, MODEL_RESOURCE_DIRECTORY_LENGTH, filePath, filePathLength);

	mdlInit(mdl);

	if(filePath[filePathLength-1] != 'd'){
		r = mdlWavefrontObjLoad(fullPath, &vertexNum, &vertices, &indexNum, &indices, &lodNum, &lods, &sprite, sklPath, &sklPathLength);
	}else{
		skeleton *const tempSkl = moduleSkeletonAllocate();
		r = mdlSMDLoad(fullPath, &vertexNum, &vertices, &indexNum, &indices, tempSkl);
		tempSkl->name = memAllocate(8*sizeof(char));
		memcpy(tempSkl->name, "SMDTest", 7);
		tempSkl->name[7] = '\0';
		mdl->skl = tempSkl;
		sprite = 0;
	}
	/** Replace and move the loading function here. **/
	if(r <= 0){
		return r;
	}

	/** Should mdlGenerateBuffers() be here? **/
	//if(sprite){
		//sprPackVertexBuffer(vertexNum, vertices);
		//r = sprGenerateBuffers(&mdl->buffers, vertexNum, vertices, indexNum, indices);
	//}else{
		r = meshGenerateBuffers(&mdl->buffers, vertexNum, vertices, indexNum, indices);
	//}
	mdl->lodNum = lodNum;
	mdl->lods = lods;
	memFree(vertices);
	memFree(indices);

	if(mdl->skl == NULL){if(sklPathLength == 0){
		// Use the default skeleton.
		mdl->skl = &g_sklDefault;
	}else{
		/** Check if the skeleton already exists. If not, load it. **/
		skeleton *const tempSkl = moduleSkeletonAllocate();
		if(tempSkl != NULL){
			const return_t r2 = sklLoad(tempSkl, sklPath, sklPathLength);
			if(r2 < 1){
				// The load failed. Clean up.
				moduleSkeletonFree(tempSkl);
				if(r2 < 0){
					/** Memory allocation failure. **/
					mdlDelete(mdl);
					return -1;
				}
				mdl->skl = &g_sklDefault;
			}else{
				mdl->skl = tempSkl;
			}
		}else{
			/** Memory allocation failure. **/
			memFree(lods);
			mdlDelete(mdl);
			return -1;
		}
	}}

	if(r > 0){

		if(lods != NULL){
			// Reallocate the LODs.
			mdl->lods = memReallocate(lods, mdl->lodNum*sizeof(mdlLOD));
			if(mdl->lods == NULL){
				/** Memory allocation failure. **/
				memFree(lods);
				mdlDelete(mdl);
			}
		}

		// Generate a name based off the file path.
		mdl->name = fileGenerateResourceName(filePath, filePathLength);
		if(mdl->name == NULL){
			/** Memory allocation failure. **/
			mdlDelete(mdl);
			return -1;
		}

	}else{
		mdlDelete(mdl);
	}

	return r;

}

void mdlDefaultInit(){
	g_mdlDefault.buffers = g_meshDefault;
}
void mdlSpriteInit(){
	g_mdlSprite.buffers = g_meshSprite;
}
void mdlBillboardInit(){
	g_mdlBillboard.buffers = g_meshBillboard;
}

__FORCE_INLINE__ void mdlFindCurrentLOD(const model *const __RESTRICT__ mdl, vertexIndex_t *const __RESTRICT__ indexNum, const void **const __RESTRICT__ offset, const float distance, size_t bias){

	if(mdl->lods == NULL){
		*indexNum = mdl->buffers.indexNum;
		*offset = 0;
	}else{

		// Find the current LOD based off the distance.
		lodNum_t i = 1;
		const mdlLOD *lod = mdl->lods;

		// Loop through each LOD until one within
		// the specified distance is found.
		MDL_FIND_CURRENT_LOD_LOOP:
		if((++lod)->distance <= distance){
			if(++i < mdl->lodNum){
				goto MDL_FIND_CURRENT_LOD_LOOP;
			}
		}else{
			--lod;
		}

		// Apply the specified LOD bias.
		if(bias != 0){
			if(bias < 0){
				// If the bias is negative, get some
				// higher-detail LODs.
				while(i < mdl->lodNum && bias != 0){
					--lod;
					--bias;
					++i;
				}
			}else{
				// If the bias is positive, get some
				// lower-detail LODs.
				while(lod->distance != 0.f && bias != 0){
					--lod;
					--bias;
				}
			}
		}

		*indexNum = lod->indexNum;
		*offset = lod->offset;

	}

}

void mdlDelete(model *const __RESTRICT__ mdl){
	if(mdl->name != NULL && mdl->name != g_mdlDefault.name && mdl->name != g_mdlSprite.name && mdl->name != g_mdlBillboard.name){
		memFree(mdl->name);
	}
	if(mdl->lods != NULL){
		memFree(mdl->lods);
	}
	meshDelete(&mdl->buffers);
}
