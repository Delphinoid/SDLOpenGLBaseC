#ifndef COMMAND_H
#define COMMAND_H

#include "tick.h"
#include "flags.h"
#include "return.h"
#include "qualifiers.h"
#include <stddef.h>

// These settings mirror the Source engine.
#ifndef COMMAND_MAX_LENGTH
	#define COMMAND_MAX_LENGTH 256
#endif
#ifndef COMMAND_MAX_ARGUMENT_BUFFER_SIZE
	#define COMMAND_MAX_ARGUMENT_BUFFER_SIZE 8192
#endif
#ifndef COMMAND_MAX_ARGUMENTS
	#define COMMAND_MAX_ARGUMENTS 64
#endif

#define COMMAND_TYPE_FUNCTION 0x00
#define COMMAND_TYPE_VARIABLE 0x01
#define COMMAND_TYPE_FREE     0xFF

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

typedef struct cmdTrieNode cmdTrieNode;

// Command function prototype.
// Takes in a tokenized array of arguments.
typedef void (*cmdFunction)(cmdTrieNode *const __RESTRICT__ cmdsys, const size_t argc, const char **const argv);
// User variables are stored as strings.
typedef char * cmdVariable;
// The C standard does not enforce any sort of rules on the size of
// of data pointers versus function pointers, so the safest way to
// define a command is to use a union rather than any single type.
typedef union command {
	cmdFunction f;
	cmdVariable v;
} command;

/// This is probably pretty slow due to poor caching.
/// Would it be better to just go with an array and
/// maybe a linked list for aliases?
// Command trie nodes store an array of characters
// that represent the following characters that
// appear for various strings.
typedef struct cmdTrieNode {
	cmdTrieNode *children;
	// Either a pointer to a function (cmdFunction)
	// or a pointer to a buffer (cmdVariable).
	command cmd;
	cmdNodeIndex_t childNum;
	char value;
	// Stores whether the command
	// is a function or a variable.
	flags_t type;
} cmdTrieNode, cmdSystem;

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

	// DLink of tokenized commands.
	cmdTokenized *cmdListStart;
	cmdTokenized *cmdListEnd;

} cmdBuffer;

void cmdSystemInit(cmdSystem *const __RESTRICT__ cmdsys);
return_t cmdSystemAddFunction(cmdSystem *node, const char *__RESTRICT__ name, const cmdFunction f);
return_t cmdSystemAddVariable(cmdSystem *node, const char *__RESTRICT__ name, const cmdVariable v);
const cmdTrieNode *const cmdSystemFind(const cmdSystem *node, const char *__RESTRICT__ name);
void cmdSystemDelete(cmdSystem *const __RESTRICT__ cmdsys);

void cmdBufferInit(cmdBuffer *const __RESTRICT__ cmdbuf);
return_t cmdBufferTokenize(cmdBuffer *const __RESTRICT__ cmdbuf, const char *str, const size_t strLength, const tick_t timestamp, const tick_t delay);
return_t cmdBufferExecute(cmdBuffer *const __RESTRICT__ cmdbuf, cmdSystem *const __RESTRICT__ cmdsys);
void cmdBufferDelete(cmdBuffer *const __RESTRICT__ cmdbuf);

#endif
