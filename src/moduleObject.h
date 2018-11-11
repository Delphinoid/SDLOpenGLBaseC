#ifndef MODULEOBJECT_H
#define MODULEOBJECT_H

#include "object.h"
#include "memoryPool.h"

extern memoryPool __ObjectResourceArray;          // Contains objects.
extern memoryPool __ObjectInstanceResourceArray;  // Contains object instances.

/** Support locals? Merge all module containers? **/

/** Should scenes just be used for object instances? **/

return_t moduleObjectResourcesInit();
void moduleObjectResourcesReset();
void moduleObjectResourcesDelete();

object *moduleObjectAllocateStatic();
object *moduleObjectAllocate();
void moduleObjectFree(object *resource);
object *moduleObjectFind(const char *name);
void moduleObjectClear();

objInstance *moduleObjectInstanceAllocateStatic();
objInstance *moduleObjectInstanceAllocate();
objInstance *moduleObjectInstanceIndex(const size_t i);
void moduleObjectInstanceFree(objInstance *resource);
void moduleObjectInstanceClear();

#endif
