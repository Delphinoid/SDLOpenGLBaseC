#ifndef MODULEOBJECT_H
#define MODULEOBJECT_H

#include "object.h"
#include "memoryPool.h"

#define RESOURCE_DEFAULT_OBJECT_SIZE sizeof(object)
#define RESOURCE_DEFAULT_OBJECT_NUM 1024

#define RESOURCE_DEFAULT_OBJECT_INSTANCE_SIZE sizeof(objInstance)
#define RESOURCE_DEFAULT_OBJECT_INSTANCE_NUM 4096

extern memoryPool __ObjectResourceArray;          // Contains objects.
extern memoryPool __ObjectInstanceResourceArray;  // Contains object instances.

/** Support locals? Merge all module containers? **/

/** Should scenes just be used for object instances? **/

return_t moduleObjectResourcesInit();
void moduleObjectResourcesReset();
void moduleObjectResourcesDelete();

object *moduleObjectAllocateStatic();
object *moduleObjectAllocate();
void moduleObjectFree(object *const restrict resource);
object *moduleObjectFind(const char *const restrict name);
void moduleObjectClear();

objInstance *moduleObjectInstanceAllocateStatic();
objInstance *moduleObjectInstanceAllocate();
objInstance *moduleObjectInstanceIndex(const size_t i);
void moduleObjectInstanceFree(objInstance *const restrict resource);
void moduleObjectInstanceClear();

#endif
