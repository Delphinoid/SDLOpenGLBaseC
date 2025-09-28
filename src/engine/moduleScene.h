#ifndef MODULESCENE_H
#define MODULESCENE_H

#include "settingsPhysics.h"
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
void moduleSceneFree(scene *const __RESTRICT__ resource);
void moduleSceneClear();

#ifdef PHYSICS_CONTACT_STABILIZER_BAUMGARTE
void moduleSceneTick(const float dt_ms, const float dt_s, const float frequency);
#else
void moduleSceneTick(const float dt_ms, const float dt_s);
#endif

#endif
