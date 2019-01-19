#ifndef MODULEOBJECT_H
#define MODULEOBJECT_H

#include "object.h"
#include "memoryPool.h"

#define RESOURCE_DEFAULT_OBJECT_LOCAL_SIZE sizeof(objectBase)
#define RESOURCE_DEFAULT_OBJECT_LOCAL_NUM 1024

#define RESOURCE_DEFAULT_OBJECT_SIZE sizeof(object)
#define RESOURCE_DEFAULT_OBJECT_NUM 4096

// Forward declarations for inlining.
extern memoryPool __ObjectBaseResourceArray;  // Contains objectBases.
extern memoryPool __ObjectResourceArray;      // Contains objects.

/** Support locals? Merge all module containers? **/
/** Use compiler constants for loop offsets?     **/

/** Should scenes just be used for object instances? **/

return_t moduleObjectResourcesInit();
void moduleObjectResourcesReset();
void moduleObjectResourcesDelete();

objectBase *moduleObjectBaseAllocateStatic();
objectBase *moduleObjectBaseAllocate();
void moduleObjectBaseFree(objectBase *const restrict resource);
objectBase *moduleObjectBaseFind(const char *const restrict name);
void moduleObjectBaseClear();

object *moduleObjectAllocateStatic();
object *moduleObjectAllocate();
object *moduleObjectIndex(const size_t i);
void moduleObjectFree(object *const restrict resource);
void moduleObjectClear();

#endif
