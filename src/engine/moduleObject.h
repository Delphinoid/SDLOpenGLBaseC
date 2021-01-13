#ifndef MODULEOBJECT_H
#define MODULEOBJECT_H

#include "memoryPool.h"
#include "memoryDLink.h"
#include "return.h"

#define RESOURCE_DEFAULT_OBJECT_BASE_NUM 1024
#define RESOURCE_DEFAULT_OBJECT_NUM 4096

// Forward declarations for inlining.
extern memoryPool __g_ObjectBaseResourceArray;  // Contains objectBases.
extern memoryDLink __g_ObjectResourceArray;     // Contains objects.

typedef struct objectBase objectBase;
typedef struct object object;

/** Support locals? Merge all module containers? **/
/** Use compiler constants for loop offsets?     **/

/** Should scenes just be used for object instances? **/

return_t moduleObjectResourcesInit();
void moduleObjectResourcesReset();
void moduleObjectResourcesDelete();

objectBase *moduleObjectBaseAllocateStatic();
objectBase *moduleObjectBaseAllocate();
void moduleObjectBaseFree(objectBase *const __RESTRICT__ resource);
objectBase *moduleObjectBaseFind(const char *const __RESTRICT__ name, const size_t nameLength);
void moduleObjectBaseClear();

object *moduleObjectAllocateStatic();
object *moduleObjectAllocate();
void moduleObjectFree(object *const __RESTRICT__ resource);
void moduleObjectClear();

#endif
