#ifndef MODULECOMMAND_H
#define MODULECOMMAND_H

#include "memoryDLink.h"
#include "return.h"

#define RESOURCE_DEFAULT_COMMAND_TOKENIZED_NUM 1024

// Forward declaration for inlining.
extern memoryDLink __g_CommandTokenizedResourceArray;  // Contains tokenized commands.

typedef struct cmdTokenized cmdTokenized;

/** Support locals? Merge all module containers? **/
/** Use compiler constants for loop offsets?     **/

return_t moduleCommandResourcesInit();
void moduleCommandResourcesReset();
void moduleCommandResourcesDelete();

cmdTokenized *moduleCommandTokenizedInsertAfterStatic(cmdTokenized **const __RESTRICT__ array, cmdTokenized *const __RESTRICT__ resource);
cmdTokenized *moduleCommandTokenizedInsertAfter(cmdTokenized **const __RESTRICT__ array, cmdTokenized *const __RESTRICT__ resource);
cmdTokenized *moduleCommandTokenizedInsertBeforeStatic(cmdTokenized **const __RESTRICT__ array, cmdTokenized *const __RESTRICT__ resource);
cmdTokenized *moduleCommandTokenizedInsertBefore(cmdTokenized **const __RESTRICT__ array, cmdTokenized *const __RESTRICT__ resource);
cmdTokenized *moduleCommandTokenizedNext(const cmdTokenized *const __RESTRICT__ i);
cmdTokenized *moduleCommandTokenizedPrev(const cmdTokenized *const __RESTRICT__ i);
void moduleCommandTokenizedFree(cmdTokenized **const __RESTRICT__ array, cmdTokenized *const __RESTRICT__ resource);
void moduleCommandTokenizedArray(cmdTokenized **const __RESTRICT__ array);
void moduleCommandTokenizedClear();

#endif
