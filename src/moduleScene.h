#ifndef MODULESCENE_H
#define MODULESCENE_H

#include "scene.h"
#include "memoryList.h"

extern memoryList __SceneResourceArray;  // Contains scenes.

/** Support locals? Merge all module containers? **/

return_t moduleSceneResourcesInit();
void moduleSceneResourcesReset();
void moduleSceneResourcesDelete();

scene *moduleSceneAllocateStatic();
scene *moduleSceneAllocate();
void moduleSceneFree(scene *resource);
void moduleSceneClear();

void moduleSceneUpdate(const float elapsedTime, const float dt);

#endif