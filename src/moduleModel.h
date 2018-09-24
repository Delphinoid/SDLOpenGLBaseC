#ifndef MODULEMODEL_H
#define MODULEMODEL_H

#include "moduleSettings.h"
#include "memoryManager.h"
#include "model.h"

return_t moduleModelInit();

model *moduleModelGetDefault();
model *moduleModelAllocate();
void moduleModelFree(model *resource);
model *moduleModelFind(const char *name);

#endif
