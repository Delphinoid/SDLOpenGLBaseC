#ifndef COMMAND_H
#define COMMAND_H

#include "tick.h"
#include "flags.h"
#include "return.h"
#include "qualifiers.h"
#include <stddef.h>

// These settings mirror the Source engine.
#define COMMAND_MAX_LENGTH 256
#define COMMAND_MAX_ARGUMENT_BUFFER_SIZE 8192
#define COMMAND_MAX_ARGUMENTS 64

// Returns '1' for heap allocations and '0' for function pointers.
#define cmdType(cmd) ((uintptr_t)(cmd) & (uintptr_t)0x01)
#define cmdAddress(cmd) ((uintptr_t)(cmd) & (uintptr_t)~0x01)

typedef unsigned char cmdNodeIndex_t;

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
typedef uintptr_t command;

/// This is probably pretty slow due to poor caching.
/// Would it be better to just go with an array and
/// maybe a linked list for aliases?
// Command trie nodes store an array of characters
// that represent the following characters that
// appear for various strings.
typedef struct cmdTrieNode cmdTrieNode;
typedef struct cmdTrieNode {
	cmdTrieNode *children;
	// Either a pointer to a buffer or a cmdFunction.
	command cmd;
	cmdNodeIndex_t childNum;
	char value;
} cmdTrieNode, cmdSystem;

// Command function prototype.
// Takes in a tokenized array of arguments.
typedef void (*cmdFunction)(cmdSystem *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);

// Represents a tokenized command.
typedef struct cmdTokenized {
	// Argument array.
	// Points to a position in argBuffer.
	const char *argv[COMMAND_MAX_ARGUMENTS];
	size_t argc;
	// When the command was invoked. Used for ordering.
	tick_t timestamp;
	// The delay in executing the command, in ticks.
	tick_t delay;
} cmdTokenized;

typedef struct {

	// Argument buffer. Contains arguments
	// for all of the commands. The commands
	// store pointers to the arguments they
	// are interested in.
	char argBuffer[COMMAND_MAX_ARGUMENT_BUFFER_SIZE];
	size_t argBufferSize;

	// SLink of tokenized commands.
	cmdTokenized *cmdList;

} cmdBuffer;

void cmdSystemInit(cmdSystem *const __RESTRICT__ cmdsys);
return_t cmdSystemAdd(cmdSystem *node, const char *__RESTRICT__ name, const command cmd);
command cmdSystemFind(const cmdSystem *node, const char *__RESTRICT__ name);
void cmdSystemDelete(cmdSystem *const __RESTRICT__ cmdsys);

void cmdBufferInit(cmdBuffer *const __RESTRICT__ cmdbuf);
return_t cmdBufferParse(cmdBuffer *const __RESTRICT__ cmdbuf, const char *str, const size_t strLength, const tick_t timestamp, const tick_t delay);
return_t cmdBufferExecute(cmdBuffer *const __RESTRICT__ cmdbuf, cmdSystem *const __RESTRICT__ cmdsys);
void cmdBufferDelete(cmdBuffer *const __RESTRICT__ cmdbuf);

#endif
