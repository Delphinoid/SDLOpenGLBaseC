#include "graphicsRenderer.h"
/// Shouldn't need to include object.h. Use gfxRenderStructure.
#include "object.h"
/// Shouldn't need to include object.h. Use gfxRenderStructure.
#include "inline.h"

typedef struct {

	objInstance **objects;
	size_t objectNum;

} gfxRenderQueue;

typedef struct {

	gfxRenderQueue qOpaque;
	gfxRenderQueue qTranslucent;

	float interpT;

} gfxRenderer;


static __FORCE_INLINE__ void gfxRenderQueueInit(gfxRenderQueue *queue){
	queue->objectNum = 0;
	queue->objects = 0;
}

static __FORCE_INLINE__ void gfxRenderQueueDepthSort(gfxRenderQueue *queue){

	//

}

static __FORCE_INLINE__ void gfxRendererInit(gfxRenderer *renderer, const float interpT){
	gfxRenderQueueInit(&renderer->qOpaque);
	gfxRenderQueueInit(&renderer->qTranslucent);
	renderer->interpT = interpT;
}

static __FORCE_INLINE__ return_t gfxRendererInitQueues(gfxRenderer *renderer, const size_t objectNum){

	objInstance **array = memAllocate(objectNum * sizeof(objInstance *));

	if(array == NULL){
		/** Memory allocation failure. **/
		return -1;
	}

	// Add opaque objects to the front of the array.
	renderer->qOpaque.objectNum = 0;
	renderer->qOpaque.objects = array;

	// Add translucent objects to the back of the array.
	renderer->qTranslucent.objectNum = 0;
	renderer->qTranslucent.objects = &array[objectNum];

	return 1;

}

static __FORCE_INLINE__ void gfxRendererGenerateQueuesArray(gfxRenderer *renderer, const size_t objectNum, objInstance **objects){

	/*
	** Generates opaque and translucent render
	** queues from a regular array of objects.
	*/

	objInstance **array = &renderer->qOpaque.objects[renderer->qOpaque.objectNum];

	size_t i;
	for(i = 0; i < objectNum; ++i){

		if(objects[i] != NULL){

			const gfxRenderGroup_t group = objiRenderGroup(objects[i], renderer->interpT);

			if(group == GFX_RENDER_GROUP_OPAQUE){

				// The object is fully opaque.
				*array = objects[i];
				++array;
				++renderer->qOpaque.objectNum;

			}else if(group == GFX_RENDER_GROUP_TRANSLUCENT){

				// The object contains translucency.
				// It will have to be depth-sorted and
				// rendered after the opaque objects.
				--renderer->qTranslucent.objects;
				*renderer->qTranslucent.objects = objects[i];
				++renderer->qTranslucent.objectNum;

			}

		}

	}

}

static __FORCE_INLINE__ void gfxRendererGenerateQueuesList(gfxRenderer *renderer, const size_t objectNum, memoryList objects){

	/*
	** Generates opaque and translucent render
	** queues from a memoryList.
	*/

	if(objectNum > 0){

		objInstance **array = &renderer->qOpaque.objects[renderer->qOpaque.objectNum];

		size_t objectsLeft = objectNum;
		memoryRegion *region = objects.region;
		objInstance **i;

		do {
			i = memListFirst(region);
			while(i < (objInstance **)memAllocatorEnd(region)){

				if(objectsLeft > 0){

					objInstance *obji = *i;
					if(obji != NULL){

						const gfxRenderGroup_t group = objiRenderGroup(obji, renderer->interpT);

						if(group == GFX_RENDER_GROUP_OPAQUE){

							// The object is fully opaque.
							*array = obji;
							++array;
							++renderer->qOpaque.objectNum;

						}else if(group == GFX_RENDER_GROUP_TRANSLUCENT){

							// The object contains translucency.
							// It will have to be depth-sorted and
							// rendered after the opaque objects.
							--renderer->qTranslucent.objects;
							*renderer->qTranslucent.objects = obji;
							++renderer->qTranslucent.objectNum;

						}

						--objectsLeft;

					}
					memListBlockNext(objects, i);

				}else{
					return;
				}

			}
			region = memAllocatorNext(region);
		} while(region != NULL);

	}

}

static __FORCE_INLINE__ void gfxRendererDelete(gfxRenderer *renderer){
	memFree(renderer->qOpaque.objects);
}

/** MOVE AND RENAME renderModel!!!! **/
void renderModel(const objInstance *obji, const camera *cam, const float interpT, graphicsManager *gfxMngr);
/** MOVE AND RENAME renderModel!!!! **/

return_t gfxRendererDrawScene(graphicsManager *gfxMngr, camera *cam, scene *scn, const float interpT){

	size_t i;
	objInstance **queue;

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
	queue = renderer.qOpaque.objects;
	for(i = renderer.qOpaque.objectNum; i > 0; --i){
		renderModel(*queue, cam, interpT, gfxMngr);
		++queue;
	}
	queue = renderer.qTranslucent.objects;
	for(i = renderer.qTranslucent.objectNum; i > 0; --i){
		renderModel(*queue, cam, interpT, gfxMngr);
		++queue;
	}

	//glClear(GL_DEPTH_BUFFER_BIT);

	gfxRendererDelete(&renderer);
	return 1;

}