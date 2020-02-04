#include "graphicsRenderer.h"
#include "graphicsRendererSettings.h"
#include "memoryManager.h"
#include "camera.h"
#include "scene.h"
/** Shouldn't need to include object.h. Use gfxRenderStructure. **/
#include "object.h"
/** Shouldn't need to include object.h. Use gfxRenderStructure. **/

typedef struct {

	gfxRenderElement *elements;
	size_t elementNum;

} gfxRenderQueue;

typedef struct {

	gfxRenderQueue qOpaque;
	gfxRenderQueue qTranslucent;

	float interpT;

} gfxRenderer;


static __FORCE_INLINE__ void gfxRenderQueueInit(gfxRenderQueue *const __RESTRICT__ queue){
	queue->elementNum = 0;
	queue->elements = 0;
}

/** This should be done by the scene using a spatial partitioning system. **/
static __FORCE_INLINE__ void gfxRenderQueueDepthSort(gfxRenderQueue *const __RESTRICT__ queue){

	//

}
static __FORCE_INLINE__ void gfxRenderQueueDepthSortReverse(gfxRenderQueue *const __RESTRICT__ queue){

	//

}

static __FORCE_INLINE__ void gfxRendererInit(gfxRenderer *const __RESTRICT__ renderer, const float interpT){
	gfxRenderQueueInit(&renderer->qOpaque);
	gfxRenderQueueInit(&renderer->qTranslucent);
	renderer->interpT = interpT;
}

static __FORCE_INLINE__ return_t gfxRendererInitQueues(gfxRenderer *const __RESTRICT__ renderer, const size_t elementNum){

	gfxRenderElement *const array = memAllocate(elementNum * sizeof(gfxRenderElement));

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

/** UNUSED **/
static __FORCE_INLINE__ void gfxRendererGenerateQueuesArray(gfxRenderer *const __RESTRICT__ renderer, const size_t objectNum, const object **const __RESTRICT__ objects){

	// Generates opaque and translucent render
	// queues from a regular array of objects.

	gfxRenderElement *array = &renderer->qOpaque.elements[renderer->qOpaque.elementNum];

	const object **o;
	const object **const oLast = &objects[objectNum];

	for(o = objects; o < oLast; ++o){

		const gfxRenderGroup_t group = objRenderGroup(*o, renderer->interpT);

		if(group == GFX_RNDR_GROUP_OPAQUE){

			// The object is fully opaque.
			array->structure = (const void *)(*o);
			array->type = GFX_RNDR_ELEMENT_TYPE_OBJECT;
			array->distance = 0.f;
			++array;
			++renderer->qOpaque.elementNum;

		}else if(group == GFX_RNDR_GROUP_TRANSLUCENT){

			// The object contains translucency.
			// It will have to be depth-sorted and
			// rendered after the opaque objects.
			--renderer->qTranslucent.elements;
			renderer->qTranslucent.elements->structure = (const void *)(*o);
			renderer->qTranslucent.elements->type = GFX_RNDR_ELEMENT_TYPE_OBJECT;
			renderer->qTranslucent.elements->distance = 0.f;
			++renderer->qTranslucent.elementNum;

		}

	}

}

static __FORCE_INLINE__ void gfxRendererGenerateQueuesList(gfxRenderer *const __RESTRICT__ renderer, const size_t objectNum, memoryPool objects){

	// Generates opaque and translucent render
	// queues from a memoryPool.

	gfxRenderElement *array = &renderer->qOpaque.elements[renderer->qOpaque.elementNum];

	const memoryRegion *region = objects.region;
	const object **i;

	do {
		i = memPoolFirst(region);
		while(i < (const object **)memAllocatorEnd(region)){
			const byte_t flag = memPoolBlockStatus(i);
			if(flag == MEMORY_POOL_BLOCK_ACTIVE){

				const object *const obj = *i;
				const gfxRenderGroup_t group = objRenderGroup(obj, renderer->interpT);

				if(group == GFX_RNDR_GROUP_OPAQUE){

					// The object is fully opaque.
					array->structure = (const void *)obj;
					array->type = GFX_RNDR_ELEMENT_TYPE_OBJECT;
					array->distance = 0.f;
					++array;
					++renderer->qOpaque.elementNum;

				}else if(group == GFX_RNDR_GROUP_TRANSLUCENT){

					// The object contains translucency.
					// It will have to be depth-sorted and
					// rendered after the opaque objects.
					--renderer->qTranslucent.elements;
					renderer->qTranslucent.elements->structure = (const void *)obj;
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

static __FORCE_INLINE__ void gfxRendererDelete(const gfxRenderer *const __RESTRICT__ renderer){
	memFree(renderer->qOpaque.elements);
}

static __FORCE_INLINE__ void gfxRendererDrawElement(gfxRenderElement *const __RESTRICT__ element, graphicsManager *const __RESTRICT__ gfxMngr, const camera *const __RESTRICT__ cam, const float interpT){
	switch(element->type){
		case GFX_RNDR_ELEMENT_TYPE_OBJECT:
			/** REMOVE THIS LINE EVENTUALLY **/
			element->distance = camDistance(cam, ((const object *const)element->structure)->state.configuration[0].position);
			objRender((const object *const)element->structure, gfxMngr, cam, element->distance, interpT);
		break;
	}
}

return_t gfxRendererDrawScene(graphicsManager *const __RESTRICT__ gfxMngr, camera *const __RESTRICT__ cam, const scene *const __RESTRICT__ scn, const float interpT){

	size_t i;
	gfxRenderElement *queue;

	gfxRenderer renderer;
	gfxRendererInit(&renderer, interpT);
	if(gfxRendererInitQueues(&renderer, scn->objectNum) < 0){
		return -1;
	}

	// Update the camera's VP matrix.
	camUpdateViewProjectionMatrix(
		cam,
		gfxMngr->windowModified,
		gfxMngr->viewport.width,
		gfxMngr->viewport.height,
		interpT
	);

	// Switch to the camera's view.
	gfxMngrSwitchView(gfxMngr, &cam->view);

	// Feed the camera's view-projection matrix into the shader.
	glUniformMatrix4fv(gfxMngr->shdrPrgObj.vpMatrixID, 1, GL_FALSE, &cam->viewProjectionMatrix.m[0][0]);

	// Find which zones should be rendered.
	/** For each zone: **/
	gfxRendererGenerateQueuesList(&renderer, scn->objectNum, scn->objects);

	// Depth-sort the scene's objects.
	/** This should be done by the scene using a spatial partitioning system. **/
	#ifdef GFX_DEPTH_SORT_OPAQUE
	gfxRenderQueueDepthSort(&renderer.qOpaque);
	#endif
	gfxRenderQueueDepthSortReverse(&renderer.qTranslucent);

	// Render the scene's objects.
	queue = renderer.qOpaque.elements;
	for(i = renderer.qOpaque.elementNum; i > 0; --i){
		gfxRendererDrawElement(queue, gfxMngr, cam, interpT);
		++queue;
	}
	queue = renderer.qTranslucent.elements;
	for(i = renderer.qTranslucent.elementNum; i > 0; --i){
		gfxRendererDrawElement(queue, gfxMngr, cam, interpT);
		++queue;
	}

	//glClear(GL_DEPTH_BUFFER_BIT);

	gfxRendererDelete(&renderer);
	return 1;

}