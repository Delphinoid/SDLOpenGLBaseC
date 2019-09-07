#ifndef COMMAND_H
#define COMMAND_H

#include "flags.h"
#include "return.h"
#include <stddef.h>

typedef size_t cmdNodeIndex_t;

typedef struct {
	// Contains each console variable.

} cmdVariables;

/*
** A command is effectively a pointer to a function that fits
** the following prototype:
**
** int func(const cmdVariables *cmdv, const cmdInput *cmdi);
*/
typedef struct {
	size_t argc;
	void *argv;
	flags_t parsed;  // Whether argv is parsed in the expected format or as a string.
} cmdInput;
typedef const int (*command)(const cmdVariables *const restrict cmdv, const cmdInput *const restrict cmdi);

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
void cmdInit(cmdSystem *const restrict root);
return_t cmdAdd(cmdSystem *const node, const char *restrict name, const int (*func)(const cmdVariables *const restrict cmdv, const cmdInput *const restrict cmdi));
return_t cmdRemove(cmdSystem *const node, const char *restrict name);
return_t cmdFind(cmdSystem *const node, const char *restrict name, command *const cmd);

#endif
