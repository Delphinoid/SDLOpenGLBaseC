#ifndef COMMAND_H
#define COMMAND_H

#include "flags.h"
#include "return.h"
#include "qualifiers.h"
#include <stddef.h>

// Returns '1' for heap allocations and '0' for function pointers.
#define cmdType(cmd) ((uintptr_t)(cmd) & (uintptr_t)~0x01)

typedef size_t cmdNodeIndex_t;

// Commands are a little weird. They're not strictly
// commands, per se: they may refer to either function
// pointers or addresses on the heap.
//
// This allows the creation of regular commands, as
// well as aliases, registers and so on.
//
// To separate these two cases, a '1' is stored in the
// LSB of the address if it is to refer to a location
// on the heap.
typedef void *command;

// Command trie nodes store an array of characters
// that represent the following characters that
// appear for various strings.
typedef struct cmdTrieNode cmdTrieNode;
typedef struct cmdTrieNode {
	cmdNodeIndex_t childNum;
	cmdTrieNode *children;
	command cmd;
	char value;
} cmdTrieNode, cmdSystem;

// Command function prototype.
typedef const void (*cmdFunction)(cmdSystem *const __RESTRICT__ cmdsys);

void cmdSystemInit(cmdSystem *const __RESTRICT__ cmdsys);
return_t cmdSystemAdd(cmdSystem *const node, const char *__RESTRICT__ name, command cmd);
return_t cmdSystemRemove(cmdSystem *const node, const char *__RESTRICT__ name);
command cmdSystemFind(cmdSystem *const node, const char *__RESTRICT__ name);
void cmdSystemDelete(cmdSystem *const __RESTRICT__ cmdsys);

return_t cmdSystemParse(cmdSystem *const __RESTRICT__ cmdsys, char *str);

#endif
