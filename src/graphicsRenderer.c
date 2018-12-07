#include "graphicsRenderer.h"
#include "memoryManager.h"
/// Shouldn't need to include object.h. Use gfxRenderStructure.
#include "object.h"
/// Shouldn't need to include object.h. Use gfxRenderStructure.
#include "inline.h"

typedef struct {

	gfxRenderElement *elements;
	size_t elementNum;

} gfxRenderQueue;

typedef struct {

	gfxRenderQueue qOpaque;
	gfxRenderQueue qTranslucent;

	float interpT;

} gfxRenderer;


static __FORCE_INLINE__ void gfxRenderQueueInit(gfxRenderQueue *queue){
	queue->elementNum = 0;
	queue->elements = 0;
}

static __FORCE_INLINE__ void gfxRenderQueueDepthSort(gfxRenderQueue *queue){

	//

}

static __FORCE_INLINE__ void gfxRendererInit(gfxRenderer *renderer, const float interpT){
	gfxRenderQueueInit(&renderer->qOpaque);
	gfxRenderQueueInit(&renderer->qTranslucent);
	renderer->interpT = interpT;
}

static __FORCE_INLINE__ return_t gfxRendererInitQueues(gfxRenderer *renderer, const size_t elementNum){

	gfxRenderElement *array = memAllocate(elementNum * sizeof(gfxRenderElement));

	if(array == NULL){
		/** Memory allocation failure. **/
		return -1;
	}

	// Add opaque elements to the front of the array.
	renderer->qOpaque.elementNum = 0;
	renderer->qOpaque.elements = array;

	// Add translucent elements to the back of the array.
	renderer->qTranslucent.elementNum = 0;
	renderer->qTranslucent.elements = &array[elementNum];

	return 1;

}

static __FORCE_INLINE__ void gfxRendererGenerateQueuesArray(gfxRenderer *renderer, const size_t objectNum, objInstance **objects){

	/*
	** Generates opaque and translucent render
	** queues from a regular array of objects.
	*/

	gfxRenderElement *array = &renderer->qOpaque.elements[renderer->qOpaque.elementNum];

	objInstance **o;
	objInstance **oLast = &objects[objectNum];

	for(o = objects; o < oLast; ++o){

		const gfxRenderGroup_t group = objiRenderGroup(*o, renderer->interpT);

		if(group == GFX_RENDER_GROUP_OPAQUE){

			// The object is fully opaque.
			array->structure = (void *)(*o);
			array->type = GFX_RNDR_ELEMENT_TYPE_OBJECT;
			array->distance = 0.f;
			++array;
			++renderer->qOpaque.elementNum;

		}else if(group == GFX_RENDER_GROUP_TRANSLUCENT){

			// The object contains translucency.
			// It will have to be depth-sorted and
			// rendered after the opaque objects.
			--renderer->qTranslucent.elements;
			renderer->qTranslucent.elements->structure = (void *)(*o);
			renderer->qTranslucent.elements->type = GFX_RNDR_ELEMENT_TYPE_OBJECT;
			renderer->qTranslucent.elements->distance = 0.f;
			++renderer->qTranslucent.elementNum;

		}

	}

}

static __FORCE_INLINE__ void gfxRendererGenerateQueuesList(gfxRenderer *renderer, const size_t objectNum, memoryPool objects){

	/*
	** Generates opaque and translucent render
	** queues from a memoryPool.
	*/

	gfxRenderElement *array = &renderer->qOpaque.elements[renderer->qOpaque.elementNum];

	memoryRegion *region = objects.region;
	objInstance **i;

	do {
		i = memPoolFirst(region);
		while(i < (objInstance **)memAllocatorEnd(region)){
			const byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				objInstance *obji = *i;
				const gfxRenderGroup_t group = objiRenderGroup(obji, renderer->interpT);

				if(group == GFX_RENDER_GROUP_OPAQUE){

					// The object is fully opaque.
					array->structure = (void *)obji;
					array->type = GFX_RNDR_ELEMENT_TYPE_OBJECT;
					array->distance = 0.f;
					++array;
					++renderer->qOpaque.elementNum;

				}else if(group == GFX_RENDER_GROUP_TRANSLUCENT){

					// The object contains translucency.
					// It will have to be depth-sorted and
					// rendered after the opaque objects.
					--renderer->qTranslucent.elements;
					renderer->qTranslucent.elements->structure = (void *)obji;
					renderer->qTranslucent.elements->type = GFX_RNDR_ELEMENT_TYPE_OBJECT;
					renderer->qTranslucent.elements->distance = 0.f;
					++renderer->qTranslucent.elementNum;

				}

			}else if(flag == MEMORY_POOL_BLOCK_INVALID){
				return;
			}
			i = memPoolBlockNext(objects, i);
		}
		region = memAllocatorNext(region);
	} while(region != NULL);

}

static __FORCE_INLINE__ void gfxRendererDelete(const gfxRenderer *renderer){
	memFree(renderer->qOpaque.elements);
}

/** MOVE AND RENAME renderModel!!!! **/
void renderModel(const objInstance *obji, const float distance, const camera *cam, const float interpT, const graphicsManager *gfxMngr);
/** MOVE AND RENAME renderModel!!!! **/

static __FORCE_INLINE__ void gfxRendererDrawElement(gfxRenderElement *element, const camera *cam, const float interpT, const graphicsManager *gfxMngr){
	switch(element->type){
		case GFX_RNDR_ELEMENT_TYPE_OBJECT:
			/// REMOVE THIS LINE EVENTUALLY
			element->distance = camDistance(cam, &((objInstance *)element->structure)->state.skeleton[0].position);
			renderModel((objInstance *)element->structure, element->distance, cam, interpT, gfxMngr);
		break;
	}
}

return_t gfxRendererDrawScene(graphicsManager *gfxMngr, camera *cam, const scene *scn, const float interpT){

	size_t i;
	gfxRenderElement *queue;

	gfxRenderer renderer;
	gfxRendererInit(&renderer, interpT);
	if(gfxRendererInitQueues(&renderer, scn->objectNum) == -1){
		return -1;
	}

	/// Generate camera render data. Don't store it with the camera.

	/* Update the camera's VP matrix. */
	camUpdateViewProjectionMatrix(
		cam,
		gfxMngr->windowModified,
		gfxMngr->windowAspectRatioX,
		gfxMngr->windowAspectRatioY,
		interpT
	);

	/* Switch to the camera's view. */
	gfxMngrSwitchView(gfxMngr, &cam->view);

	/* Feed the camera's view-projection matrix into the shader. */
	glUniformMatrix4fv(gfxMngr->vpMatrixID, 1, GL_FALSE, &cam->viewProjectionMatrix.m[0][0]);

	/* Find which zones should be rendered. */
	/** For each zone: **/
	gfxRendererGenerateQueuesList(&renderer, scn->objectNum, scn->objects);

	/* Depth-sort the scene's translucent objects. */
	gfxRenderQueueDepthSort(&renderer.qTranslucent);

	/* Render the scene's objects. */
	queue = renderer.qOpaque.elements;
	for(i = renderer.qOpaque.elementNum; i > 0; --i){
		gfxRendererDrawElement(queue, cam, interpT, gfxMngr);
		++queue;
	}
	queue = renderer.qTranslucent.elements;
	for(i = renderer.qTranslucent.elementNum; i > 0; --i){
		gfxRendererDrawElement(queue, cam, interpT, gfxMngr);
		++queue;
	}

	//glClear(GL_DEPTH_BUFFER_BIT);

	gfxRendererDelete(&renderer);
	return 1;

}