#ifndef COMMAND_H
#define COMMAND_H

#include "flags.h"
#include "return.h"
#include "qualifiers.h"
#include <stddef.h>

typedef size_t cmdNodeIndex_t;

typedef struct {
	// Contains each console variable.

} cmdVariables;

typedef struct command command;
typedef struct {
	size_t argc;
	void *argv;
} cmdArgument;
// int func(const cmdVariables *cmdv, const cmdInput *cmdi);
typedef const int (*cmdFunction)(const command *const __RESTRICT__ cmd, const flags_t state, const cmdVariables *const __RESTRICT__ cmdv);

// A command instance contains a set of
// inputs and a function to run on them.
typedef struct command {
	cmdArgument arg;
	cmdFunction cmd;
} command;

// Command trie nodes store an array of characters
// that represent the following characters that
// appear for various strings.
/** Create a new cmdSystem containing cmdVariables and a root node. **/
typedef struct cmdTrieNode cmdTrieNode;
typedef struct cmdTrieNode {
	cmdNodeIndex_t childNum;
	cmdTrieNode *children;
	cmdFunction cmd;
	char value;
} cmdTrieNode, cmdSystem;

/** Fixed-length names? **/
void cmdInit(cmdSystem *const __RESTRICT__ root);
return_t cmdAdd(cmdSystem *const node, const char *__RESTRICT__ name, cmdFunction cmd);
return_t cmdRemove(cmdSystem *const node, const char *__RESTRICT__ name);
return_t cmdFind(cmdSystem *const node, const char *__RESTRICT__ name, cmdFunction cmd);

#endif
