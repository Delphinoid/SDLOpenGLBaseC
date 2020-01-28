#ifndef MODULESCENE_H
#define MODULESCENE_H

#include "physicsSettings.h"
#include "memoryPool.h"
#include "memorySLink.h"
#include "return.h"

#define RESOURCE_DEFAULT_SCENE_NUM 2

// Forward declarations for inlining.
extern memoryPool  __g_SceneResourceArray;      // Contains scenes.
extern memorySLink __g_SceneZoneResourceArray;  // Contains zones.

typedef struct scene scene;
typedef struct scnZone scnZone;

/** Support locals? Merge all module containers? **/
/** Use compiler constants for loop offsets?     **/

return_t moduleSceneResourcesInit();
void moduleSceneResourcesReset();
void moduleSceneResourcesDelete();

scene *moduleSceneAllocateStatic();
scene *moduleSceneAllocate();
void moduleSceneFree(scene *const restrict resource);
void moduleSceneClear();

#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
void moduleSceneQueryIslands(const float frequency);
#else
void moduleSceneQueryIslands();
#endif
void moduleSceneTick(const float elapsedTime/**, const float dt**/);

#endif