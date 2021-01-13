#ifndef MODULEINPUT_H
#define MODULEINPUT_H

#include "memorySLink.h"
#include "return.h"

#define RESOURCE_DEFAULT_INPUT_KEY_BINDING_NUM 128

// Forward declaration for inlining.
extern memorySLink __g_InputKeyBindingResourceArray;  // Contains key bindings.

typedef struct inputKeyBinding inputKeyBinding;

/** Support locals? Merge all module containers? **/
/** Use compiler constants for loop offsets?     **/

return_t moduleInputResourcesInit();
void moduleInputResourcesReset();
void moduleInputResourcesDelete();

inputKeyBinding *moduleInputKeyBindingPrependStatic(inputKeyBinding **const __RESTRICT__ array);
inputKeyBinding *moduleInputKeyBindingPrepend(inputKeyBinding **const __RESTRICT__ array);
inputKeyBinding *moduleInputKeyBindingNext(const inputKeyBinding *const __RESTRICT__ i);
void moduleInputKeyBindingFree(inputKeyBinding **const __RESTRICT__ array, inputKeyBinding *const __RESTRICT__ resource, const inputKeyBinding *const __RESTRICT__ previous);
void moduleInputKeyBindingFreeArray(inputKeyBinding **const __RESTRICT__ array);
void moduleInputKeyBindingClear();

#endif
