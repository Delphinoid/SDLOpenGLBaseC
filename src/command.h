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

// A command is effectively a pointer to a function that fits
// the following prototype:
//
// int func(const cmdVariables *cmdv, const cmdInput *cmdi);
typedef struct {
	size_t argc;
	void *argv;
	flags_t parsed;  // Whether argv is parsed in the expected format or as a string.
} cmdInput;
typedef const int (*command)(const cmdVariables *const __RESTRICT__ cmdv, const cmdInput *const __RESTRICT__ cmdi);

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

/** Fixed-length names? **/
void cmdInit(cmdSystem *const __RESTRICT__ root);
return_t cmdAdd(cmdSystem *const node, const char *__RESTRICT__ name, const int (*func)(const cmdVariables *const __RESTRICT__ cmdv, const cmdInput *const __RESTRICT__ cmdi));
return_t cmdRemove(cmdSystem *const node, const char *__RESTRICT__ name);
return_t cmdFind(cmdSystem *const node, const char *__RESTRICT__ name, command *const cmd);

#endif
