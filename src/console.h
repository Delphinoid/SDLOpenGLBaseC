#ifndef CONSOLE_H
#define CONSOLE_H

typedef struct {
	// Contains each console variable.

} cmdVariables;

/*
** A command is effectively a pointer to a function that fits
** the following prototype:
**
** signed char func(const cmdVariables *cmdv, const cmdInput *cmdi);
*/
typedef struct {
	signed char parsed;  // Whether or not argv is parsed in the expected format or as a string
	unsigned int argc;
	void *argv;
} cmdInput;
typedef signed char (*command)(const cmdVariables *cmdv, const cmdInput *cmdi);

typedef struct cmdTrieNode cmdTrieNode;
typedef struct cmdTrieNode {
	char value;
	unsigned char childNum;
	cmdTrieNode *children;
	command cmd;
} cmdTrieNode;

typedef struct {
	cmdTrieNode cmdLookup;
	//char *displayData[512];       // An array of lines.
	//unsigned int displayDataEnd;  // The element displayData ends on.
} console;

void conInit(console *con);
signed char conAddCommand(console *con, char *name, signed char (*func)(const cmdVariables *cmdv, const cmdInput *cmdi));
signed char conRemoveCommand(console *con, char *name);
signed char conFindCommand(console *con, char *name, command *cmd);

#endif
