#ifndef CONSOLE_H
#define CONSOLE_H

/*
** A command is effectively a pointer to a function that fits
** the following prototype:
**
** unsigned char func(unsigned int argc, char *argv[]);
*/
typedef unsigned char (*command)(unsigned int argc, char *argv[]);

typedef struct trieNode trieNode;
typedef struct trieNode {
	char value;
	unsigned char childNum;
	trieNode *children;
	command cmd;
} trieNode;

typedef struct {
	trieNode cmdLookup;
	//char *displayData[512];       // An array of lines
	//unsigned int displayDataEnd;  // The element displayData ends on.
} console;

void conInit(console *con);
unsigned char conAddCommand(console *con, char *name, unsigned char (*func)(unsigned int argc, char *argv[]));
unsigned char conRemoveCommand(console *con, char *name);
unsigned char conFindCommand(console *con, char *name, command *cmd);

#endif
