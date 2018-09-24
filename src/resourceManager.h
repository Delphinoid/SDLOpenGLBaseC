#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "resourceSettings.h"
#include "memoryManager.h"

/** Replace some memory pools with pure free-lists? **/

typedef struct {

	#ifndef MEMORY_MANAGER_USE_GLOBAL_DEFINITION
	/* Memory manager. */
	memoryManager *memMngr;
	#endif


	/* Shared data. */
	//memoryTree resourceIDs;

	// Shared mesh data.
	//memoryTree meshes;

	// Shared hitbox data.
	memorySLink hitboxes;

	// Shared physics data.
	memorySLink colliders;


	/* Uninstantiated data. */
	// Uninstantiated object data.
	memoryList objects;

	// Uninstantiated texture data.
	memoryList textures;
	memoryList textureWrappers;
	//memoryTree textureWrapperData;

	// Uninstantiated model data.
	memoryList models;

	// Uninstantiated renderable data.
	memoryList renderables;

	// Uninstantiated skeleton data.
	memoryList skeletons;
	//memoryTree skeletonData;
	memoryList skeletalAnimations;
	//memoryTree skeletalAnimationData;

	// Uninstantiated physics data.
	memoryList rigidBodies;


	/* Instantiated data. */
	// Instantiated object data.
	memoryPool objectInstances;
	//memoryTree objectInstanceData;

	// Instantiated renderable data.
	memorySLink renderableInstances;

	// Instantiated skeleton data.
	memorySLink skeletalAnimationInstances;
	memorySLink skeletalAnimationFragments;

	// Instantiated hitbox data.
	memorySLink hitboxArrayInstances;

	// Instantiated physics data.
	memorySLink rigidBodyInstances;
	memorySLink constraintInstances;
	memorySLink collisionInstances;

} resourceManager;

#ifdef MEMORY_MANAGER_USE_LOCAL_DEFINITION

	#define resMngrAllocate(bytes)         memTreeAllocate(&resMngr->memMngr->allocator, bytes)
	#define resMngrReallocate(data, bytes) memTreeReallocate(&resMngr->memMngr->allocator, data, bytes)
	#define resMngrFree(data)              memTreeFree(&resMngr->memMngr->allocator, data)

	return_t resMngrInit(resourceManager *resMngr, memoryManager *memMngr);

#else

	#define resMngrAllocate(bytes)         memAllocate(bytes)
	#define resMngrReallocate(data, bytes) memReallocate(data, bytes)
	#define resMngrFree(data)              memFree(data)

	return_t resMngrInit(resourceManager *resMngr);

#endif

return_t resMngrCreateDefaultResources(resourceManager *resMngr);
void resMngrDelete(resourceManager *resMngr);

#endif
