#ifndef MODULEGUI_H
#define MODULEGUI_H

#include "memorySLink.h"
#include "return.h"

#define RESOURCE_DEFAULT_GUI_ELEMENT_NUM 100

// Forward declaration for inlining.
extern memorySLink __g_GUIElementResourceArray;  // Contains guiElements.

typedef struct guiElement guiElement;

/** Support locals? Merge all module containers? **/
/** Use compiler constants for loop offsets?     **/

return_t moduleGUIResourcesInit();
void moduleGUIResourcesReset();
void moduleGUIResourcesDelete();

guiElement *moduleGUIElementAppendStatic(guiElement **const __RESTRICT__ array);
guiElement *moduleGUIElementAppend(guiElement **const __RESTRICT__ array);
guiElement *moduleGUIElementInsertAfterStatic(guiElement **const __RESTRICT__ array, guiElement *const __RESTRICT__ resource);
guiElement *moduleGUIElementInsertAfter(guiElement **const __RESTRICT__ array, guiElement *const __RESTRICT__ resource);
guiElement *moduleGUIElementNext(const guiElement *const __RESTRICT__ i);
void moduleGUIElementFree(guiElement **const __RESTRICT__ array, guiElement *const __RESTRICT__ resource, const guiElement *const __RESTRICT__ previous);
void moduleGUIElementFreeArray(guiElement **const __RESTRICT__ array);
void moduleGUIElementClear();

#endif
