#ifndef MODULESCENE_H
#define MODULESCENE_H

#include "scene.h"
#include "memoryList.h"
#include "memorySLink.h"

#define RESOURCE_DEFAULT_SCENE_SIZE sizeof(scene)
#define RESOURCE_DEFAULT_SCENE_NUM 2

// Forward declarations for inlining.
extern memoryPool  __SceneResourceArray;      // Contains scenes.
extern memorySLink __SceneZoneResourceArray;  // Contains zones.

/** Support locals? Merge all module containers? **/
/** Use compiler constants for loop offsets?     **/

return_t moduleSceneResourcesInit();
void moduleSceneResourcesReset();
void moduleSceneResourcesDelete();

scene *moduleSceneAllocateStatic();
scene *moduleSceneAllocate();
void moduleSceneFree(scene *const restrict resource);
void moduleSceneClear();

#ifndef PHYSICS_SOLVER_GAUSS_SEIDEL
void moduleSceneQueryIslands(const float dt);
#else
void moduleSceneQueryIslands();
#endif
void moduleSceneUpdate(const float elapsedTime, const float dt);

#endif