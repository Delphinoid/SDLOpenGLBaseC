#include "command.h"
#include "memoryManager.h"
#include "moduleCommand.h"
#include <stdlib.h>
#include <string.h>

#define cmdValidCharacter(c) (((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z') || ((c) == '_') || ((c) == '-') || ((c) == '+'))
#define cmdWhitespaceCharacter(c) ((c) == ' ' || (c) == '\t')

static __HINT_INLINE__ void cmdTrieInit(cmdTrieNode *const __RESTRICT__ node, const char c){
	node->childNum = 0;
	node->children = NULL;
	node->cmd = 0;
	node->value = c;
}
static __HINT_INLINE__ cmdTrieNode *cmdTrieNext(const cmdTrieNode *const node, const char c){
	// Gets the next node when finding a command.
	// Loop through node's children until we find a child
	// that matches c.
	cmdTrieNode *n = node->children;
	const cmdTrieNode *const nLast = &n[node->childNum];
	while(n < nLast){
		if(c == n->value){
			return n;
		}else if(c < n->value){
			break;
		}
		++n;
	}
	return NULL;
}
static __HINT_INLINE__ cmdTrieNode *cmdTrieAddNode(cmdTrieNode *const node, const char c){

	// Same as trieNext(), but creates a new node when necessary.
	cmdTrieNode *tempBuffer;
	cmdTrieNode *n = node->children;
	const cmdTrieNode *nLast = &n[node->childNum];

	while(n < nLast){

		if(c == n->value){
			// Node with value c already exists.
			return n;
		}else if(c < n->value){

			// Insert node before index.
			++node->childNum;
			tempBuffer = memReallocate(node->children, node->childNum*sizeof(cmdTrieNode));
			if(tempBuffer == NULL){
				/** Memory allocation failure. **/
				return NULL;
			}

			// Shift all the following children over.
			n = (cmdTrieNode *)((uintptr_t)tempBuffer + (uintptr_t)n - (uintptr_t)node->children);
			nLast = (cmdTrieNode *)((uintptr_t)tempBuffer + (uintptr_t)nLast - (uintptr_t)node->children);
			memmove(n+1, n, (uintptr_t)nLast - (uintptr_t)n);
			node->children = tempBuffer;

			cmdTrieInit(n, c);
			return n;

		}

		++n;

	}

	// Append node to the end.
	++node->childNum;
	tempBuffer = memReallocate(node->children, node->childNum*sizeof(cmdTrieNode));
	if(tempBuffer == NULL){
		/** Memory allocation failure. **/
		return NULL;
	}
	node->children = tempBuffer;

	cmdTrieInit(&node->children[node->childNum-1], c);
	return &node->children[node->childNum-1];

}
static __HINT_INLINE__ return_t cmdValid(const char *name, const command cmd){
	if(cmd == 0 || name == NULL || *name == '\0'){
		return 0;
	}else{
		char c = *name;
		do {
			if(cmdValidCharacter(c)){
				++name;
			}else{
				return 0;
			}
			c = *name;
		} while(c != '\0');
		return 1;
	}
}

void cmdSystemInit(cmdSystem *const __RESTRICT__ cmdsys){
	cmdsys->childNum = 0;
	cmdsys->children = NULL;
	cmdsys->cmd = 0;
	cmdsys->value = 0;
}
return_t cmdSystemAdd(cmdSystem *node, const char *__RESTRICT__ name, const command cmd){

	// Check if the command is valid before adding it.
	if(cmdValid(name, cmd)){

		// Go through each character in name, adding
		// a node when we don't have a match.
		while(*name != '\0'){
			if((node = cmdTrieAddNode(node, *name)) == NULL){
				/** Memory allocation failure. **/
				return -1;
			}
			++name;
		}

		// We've reached the final node at the end of the command's name.
		if(node->cmd == 0){
			// If a command is not already linked to
			// this node, link one and return success.
			node->cmd = cmd;
			return 1;
		}

	}

	return 0;

}
command cmdSystemFind(const cmdSystem *node, const char *__RESTRICT__ name){
	// Find the command in the trie.
	while(node != NULL){
		if(*name == '\0'){
			return node->cmd;
		}
		node = cmdTrieNext(node, *name);
		++name;
	}
	return 0;
}
void cmdSystemDelete(cmdSystem *const node){
	/// Yikes... I really don't like using recursive functions.
	/// It probably beats storing a parent pointer for trie nodes though.
	cmdTrieNode *c = node->children;
	const cmdTrieNode *const cLast = &node->children[node->childNum];
	while(c < cLast){
		cmdSystemDelete(c);
		++c;
	}
	if(node->children != NULL){
		memFree(node->children);
	}
	if(cmdType(node->cmd)){
		memFree((void *)cmdAddress(node->cmd));
	}
}

static __HINT_INLINE__ void cmdTokenizedInit(cmdTokenized *const __RESTRICT__ cmdtok){
	cmdtok->argc = 0;
	cmdtok->delay = 0;
}

void cmdBufferInit(cmdBuffer *const __RESTRICT__ buffer){
	memset(buffer, 0, sizeof(cmdBuffer));
}

static __FORCE_INLINE__ void cmdBufferAddArgument(
	char *const __RESTRICT__ argBuffer, size_t *argBufferSize, cmdTokenized *const __RESTRICT__ cmd, const char *const str
){

	// If we've just switched, copy the
	// last argument into the buffer.
	size_t argLength = str - cmd->argv[cmd->argc];
	if(*argBufferSize + argLength + 1 > COMMAND_MAX_ARGUMENT_BUFFER_SIZE){
		// The argument is too long to
		// fit in the buffer. Clamp it!
		argLength = COMMAND_MAX_ARGUMENT_BUFFER_SIZE - *argBufferSize - 1;
	}

	// Add the argument to the buffer and modify the pointers.
	memcpy(&argBuffer[*argBufferSize], cmd->argv[cmd->argc], argLength);
	cmd->argv[cmd->argc] = &argBuffer[*argBufferSize];
	*argBufferSize += argLength;
	++cmd->argc;

	// All arguments in the buffer get
	// separated by NULL terminators.
	argBuffer[*argBufferSize] = '\0';
	++(*argBufferSize);

}

static __FORCE_INLINE__ const char *cmdBufferParseCommand(
	char *const __RESTRICT__ argBuffer, size_t *argBufferSize,
	cmdTokenized *const __RESTRICT__ cmd,
	const char *str, const char *const strEnd
){

	// Parses a single command and all of its arguments.

	const size_t argBufferStartSize = *argBufferSize;
	unsigned int strQuoted = 0;
	unsigned int strCommented = 0;
	unsigned int strReading = 0;

	while(str != strEnd){

		const char c = *str;

		if(c == '\\'){
			// Skip this character and the next.
			++str;
		}else if(!strCommented){

			const unsigned int strWhitespace = cmdWhitespaceCharacter(c);

			if((!strQuoted && strWhitespace) || (strQuoted && c == '"')){

				// We've finished reading an argument.
				// Copy it into the argument buffer.
				cmdBufferAddArgument(argBuffer, argBufferSize, cmd, str);
				strReading = 0; strQuoted = 0;

			}else if(!strQuoted){

				if(c == '/'){
					// Check for C-style comments.
					++str;
					if(str == strEnd){
						break;
					}else if(*str == '/'){
						strCommented = 1;
					}
				}else if(c == ';'){
					// Check for the end of the command.
					break;

				}else if(!strReading && !strWhitespace){

					// Start parsing a new argument.
					if(c == '"'){
						cmd->argv[cmd->argc] = str+1;
						strQuoted = 1;
					}else{
						cmd->argv[cmd->argc] = str;
					}
					strReading = 1;

				}

			}

		}else if(c == '\n'){
			// End of commented line.
			break;
		}

		++str;

	}

	// Copy in the last argument.
	// This variable is only =/= 0 when cmd =/= NULL.
	if(strReading){
		cmdBufferAddArgument(argBuffer, argBufferSize, cmd, str);
	}

	// If the command was too long, just ignore it.
	// This helps prevent commands from taking over the argument buffer.
	if(*argBufferSize - argBufferStartSize > COMMAND_MAX_LENGTH){
		*argBufferSize = argBufferStartSize;
		cmd->argc = 0;
	}

	return str;

}

return_t cmdBufferParse(cmdBuffer *const __RESTRICT__ cmdbuf, const char *str, const size_t strLength, const tick_t timestamp, const tick_t delay){

	// Parse the string into a series of tokenized commands.
	const char *const strEnd = &str[strLength];
	cmdTokenized cmdtok;

	// Note that this delay is only modified when the
	// 'wait' command is invoked. This function takes
	// in a delay argument to work with aliases.
	cmdtok.argc = 0;
	cmdtok.timestamp = timestamp;
	cmdtok.delay = delay;

	while(str != strEnd){

		// Find where the current command ends and the next command begins.
		str = cmdBufferParseCommand(cmdbuf->argBuffer, &cmdbuf->argBufferSize, &cmdtok, str, strEnd);

		if(cmdtok.argc > 0){

			if(strncmp(cmdtok.argv[0], "wait", 4) == 0){

				// Handle the hardcoded 'wait' command.
				if(cmdtok.argc > 1){
					cmdtok.delay += strtoul(cmdtok.argv[1], NULL, 0);
				}else{
					++cmdtok.delay;
				}

			}else{

				// Add the command to the linked list.
				// We order the list using the command timestamps.
				cmdTokenized *cmdNew;
				cmdTokenized *cmdInsert = cmdbuf->cmdList;
				if(cmdInsert != NULL){
					while(cmdInsert->timestamp < timestamp){
						cmdTokenized *const cmdInsertNext = moduleCommandTokenizedNext(cmdInsert);
						if(cmdInsertNext == NULL){
							break;
						}
						cmdInsert = cmdInsertNext;
					}
				}

				cmdNew = moduleCommandTokenizedInsertAfter(&cmdbuf->cmdList, cmdInsert);
				if(cmdNew == NULL){
					/** Memory allocation failure. **/
					return -1;
				}
				*cmdNew = cmdtok;

				// Don't reset cmdtok.delay or cmdtok.timestamp.
				// The delay affects all commands here.
				cmdtok.argc = 0;

			}

		}

	}

	return 1;

}

return_t cmdBufferExecute(cmdBuffer *const __RESTRICT__ cmdbuf, cmdSystem *const __RESTRICT__ cmdsys){

	// Execute a series of tokenized commands.
	cmdTokenized *cmdtokPrev = NULL;
	cmdTokenized *cmdtokNext;
	cmdTokenized *cmdtok = cmdbuf->cmdList;
	size_t argBufferSize = 0;

	while(cmdtok != NULL){

		cmdtokNext = moduleCommandTokenizedNext(cmdtok);

		if(cmdtok->delay > 0){

			// This command is delayed. Move
			// it to the front of the buffer.
			const char *const argLast = cmdtok->argv[cmdtok->argc-1];
			const char **arg = &cmdtok->argv[0];
			const size_t cmdSize = argLast + strlen(argLast) - *arg;
			const size_t offset = *arg - &cmdbuf->argBuffer[argBufferSize];

			// Copy the arguments over.
			memmove(&cmdbuf->argBuffer[argBufferSize], *arg, cmdSize);

			// Adjust the pointers.
			while(*arg <= argLast){
				*arg -= offset;
			}

			--cmdtok->delay;
			argBufferSize += cmdSize;

			cmdtokPrev = cmdtok;

		}else{

			// Execute the command.
			command cmd = cmdSystemFind(cmdsys, cmdtok->argv[0]);
			if(cmd != 0){
				if(cmdType(cmd) == 0){

					// The command is valid! Execute it.
					((cmdFunction)cmd)(cmdsys, cmdtok->argc, cmdtok->argv);

				}else{

					// This is an alias.
					// Tokenize it and insert it into the command buffer.
					cmd = cmdAddress(cmd);
					if(cmdBufferParse(cmdbuf, (const char *)cmd, strlen((const char *)cmd), cmdtok->timestamp, cmdtok->delay) < 0){
						/** Memory allocation failure. **/
						return -1;
					}

				}
			}

			// Remove it from the list.
			moduleCommandTokenizedFree(&cmdbuf->cmdList, cmdtok, cmdtokPrev);


		}

		cmdtok = cmdtokNext;

	}

	// This is the size of all the delayed commands.
	cmdbuf->argBufferSize = argBufferSize;
	return 1;

}

void cmdBufferDelete(cmdBuffer *const __RESTRICT__ cmdbuf){
	cmdTokenized *cmdtokPrev = NULL;
	cmdTokenized *cmdtokNext;
	cmdTokenized *cmdtok = cmdbuf->cmdList;
	while(cmdtok != NULL){
		cmdtokNext = moduleCommandTokenizedNext(cmdtok);
		moduleCommandTokenizedFree(&cmdbuf->cmdList, cmdtok, cmdtokPrev);
		cmdtokPrev = cmdtok;
		cmdtok = cmdtokNext;
	}
}