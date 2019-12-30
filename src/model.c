#include "model.h"
#include "sprite.h"
#include "vertex.h"
#include "skeleton.h"
#include "moduleSkeleton.h"
#include "memoryManager.h"
#include "helpersFileIO.h"
#include "inline.h"
#include <string.h>

#define MODEL_RESOURCE_DIRECTORY_STRING "Resources"FILE_PATH_DELIMITER_STRING"Models"FILE_PATH_DELIMITER_STRING
#define MODEL_RESOURCE_DIRECTORY_LENGTH 17

return_t mdlWavefrontObjLoad(const char *const restrict filePath, vertexIndex_t *const vertexNum, vertex **const vertices, vertexIndexNum_t *const restrict indexNum, vertexIndex_t **const indices, size_t *const restrict lodNum, mdlLOD **const lods, char *const restrict sklPath);
return_t mdlSMDLoad(const char *filePath, vertexIndex_t *vertexNum, vertex **vertices, vertexIndexNum_t *indexNum, vertexIndex_t **indices, skeleton *const skl);

// Default models.
model mdlDefault = {
	.skl = &sklDefault,
	.lodNum = 0,
	.lods = NULL,
	.buffers.vertexNum = 0,
	.buffers.indexNum = 0,
	.buffers.vaoID = 0,
	.buffers.vboID = 0,
	.buffers.iboID = 0,
	.name = "default"
};
model mdlSprite = {
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
model mdlBillboard = {
	.skl = &sklDefault,
	.lodNum = 0,
	.lods = NULL,
	.buffers.vertexNum = 0,
	.buffers.indexNum = 0,
	.buffers.vaoID = 0,
	.buffers.vboID = 0,
	.buffers.iboID = 0,
	.name = "billboard"
};

void mdlInit(model *const restrict mdl){
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

return_t mdlLoad(model *const restrict mdl, const char *const restrict prgPath, const char *const restrict filePath){

	/** Create a proper model file that loads a specified mesh, a name and a skeleton. **/
	return_t r;

	vertexIndex_t vertexNum;
	vertex *vertices;
	vertexIndexNum_t indexNum;
	vertexIndex_t *indices;
	size_t lodNum;
	mdlLOD *lods;

	char fullPath[FILE_MAX_PATH_LENGTH];
	char sklPath[FILE_MAX_PATH_LENGTH];
	const size_t fileLength = strlen(filePath);

	sklPath[0] = '\0';
	fileGenerateFullPath(fullPath, prgPath, strlen(prgPath), MODEL_RESOURCE_DIRECTORY_STRING, MODEL_RESOURCE_DIRECTORY_LENGTH, filePath, fileLength);

	mdlInit(mdl);

	if(filePath[fileLength-1] != 'd'){
		r = mdlWavefrontObjLoad(fullPath, &vertexNum, &vertices, &indexNum, &indices, &lodNum, &lods, &sklPath[0]);
	}else{
		skeleton *const tempSkl = moduleSkeletonAllocate();
		r = mdlSMDLoad(fullPath, &vertexNum, &vertices, &indexNum, &indices, tempSkl);
		tempSkl->name = memAllocate(8*sizeof(char));
		strncpy(tempSkl->name, "SMDTest", 7);
		tempSkl->name[7] = '\0';
		mdl->skl = tempSkl;
	}
	/** Replace and move the loading function here. **/
	if(r <= 0){
		return r;
	}

	/** Should mdlGenerateBuffers() be here? **/
	r = meshGenerateBuffers(&mdl->buffers, vertexNum, vertices, indexNum, indices);
	mdl->lodNum = lodNum;
	mdl->lods = lods;
	memFree(vertices);
	memFree(indices);

	if(mdl->skl == NULL){if(sklPath[0] == '\0'){
		// Use the default skeleton.
        mdl->skl = &sklDefault;
	}else{
		/** Check if the skeleton already exists. If not, load it. **/
		skeleton *const tempSkl = moduleSkeletonAllocate();
		if(tempSkl != NULL){
			const return_t r2 = sklLoad(tempSkl, prgPath, &sklPath[0]);
			if(r2 < 1){
				// The load failed. Clean up.
				moduleSkeletonFree(tempSkl);
				if(r2 < 0){
					/** Memory allocation failure. **/
					mdlDelete(mdl);
					return -1;
				}
				mdl->skl = &sklDefault;
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
		mdl->name = fileGenerateResourceName(filePath, fileLength);
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
	mdlDefault.buffers = meshDefault;
}
void mdlSpriteInit(){
	mdlSprite.buffers = meshSprite;
}
void mdlBillboardInit(){
	mdlBillboard.buffers = meshBillboard;
}

__FORCE_INLINE__ void mdlFindCurrentLOD(const model *const restrict mdl, vertexIndexNum_t *const restrict indexNum, const void **const restrict offset, const float distance, size_t bias){

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

void mdlDelete(model *const restrict mdl){
	if(mdl->name != NULL && mdl->name != mdlDefault.name && mdl->name != mdlSprite.name){
		memFree(mdl->name);
	}
	if(mdl->lods != NULL){
		memFree(mdl->lods);
	}
	meshDelete(&mdl->buffers);
}
