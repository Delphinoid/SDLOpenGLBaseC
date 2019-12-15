#ifndef MODULEGUI_H
#define MODULEGUI_H

#include "memorySLink.h"
#include "return.h"

#define RESOURCE_DEFAULT_GUI_ELEMENT_NUM 100

// Forward declaration for inlining.
extern memorySLink __GUIElementResourceArray;  // Contains guiElements.

typedef struct guiElement guiElement;

/** Support locals? Merge all module containers? **/
/** Use compiler constants for loop offsets?     **/

return_t moduleGUIResourcesInit();
void moduleGUIResourcesReset();
void moduleGUIResourcesDelete();

guiElement *moduleGUIElementAppendStatic(guiElement **const restrict array);
guiElement *moduleGUIElementAppend(guiElement **const restrict array);
guiElement *moduleGUIElementInsertAfterStatic(guiElement **const restrict array, guiElement *const restrict resource);
guiElement *moduleGUIElementInsertAfter(guiElement **const restrict array, guiElement *const restrict resource);
guiElement *moduleGUIElementNext(const guiElement *const restrict i);
void moduleGUIElementFree(guiElement **const restrict array, guiElement *const restrict resource, const guiElement *const restrict previous);
void moduleGUIElementFreeArray(guiElement **const restrict array);
void moduleGUIElementClear();

#endif
