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
	node->type = 0;
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

	n = &tempBuffer[node->childNum-1];
	cmdTrieInit(n, c);
	return n;

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
	cmdsys->type = 0;
}
return_t cmdSystemAdd(cmdSystem *node, const char *__RESTRICT__ name, const command cmd, const unsigned char type){

	// Check if the command is valid before adding it.
	if(cmdValid(name, cmd)){

		// Go through each character in name, adding
		// a node when we don't have a match.
		do {
			if((node = cmdTrieAddNode(node, *name)) == NULL){
				/** Memory allocation failure. **/
				return -1;
			}
			++name;
		} while(*name != '\0');

		// We've reached the final node at the end of the command's name.
		if(node->cmd == 0){
			// If a command is not already linked to
			// this node, link one and return success.
			node->cmd = cmd;
			node->type = type;
			return 1;
		}

	}

	return 0;

}
const cmdTrieNode *const cmdSystemFind(const cmdSystem *node, const char *__RESTRICT__ name){
	// Find the command in the trie.
	while(node != NULL){
		if(*name == '\0'){
			return node;
		}
		node = cmdTrieNext(node, *name);
		++name;
	}
	return NULL;
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
	if(node->type != COMMAND_TYPE_FUNCTION){
		memFree((void *)node->cmd);
	}
	/**if(cmdType(node->cmd)){
		memFree((void *)cmdAddress(node->cmd));
	}**/
}

static __HINT_INLINE__ void cmdTokenizedInit(cmdTokenized *const __RESTRICT__ cmdtok){
	cmdtok->argc = 0;
	cmdtok->delay = 0;
}

void cmdBufferInit(cmdBuffer *const __RESTRICT__ buffer){
	buffer->argBufferSize = 0;
	buffer->cmdListStart = NULL;
	buffer->cmdListEnd = NULL;
}

static __FORCE_INLINE__ void cmdBufferAddArgument(
	char *const __RESTRICT__ argBuffer, size_t *argBufferSize, cmdTokenized *const __RESTRICT__ cmd, const char *const str
){

	// Copy the last argument into the buffer.
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

static __FORCE_INLINE__ const char *cmdBufferTokenizeCommand(
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

			if(strReading && ((!strQuoted && strWhitespace) || (strQuoted && c == '"'))){

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
					}else{
						continue;
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

	// If we found a newline or semicolon, skip it.
	return str + (str != strEnd);

}

return_t cmdBufferTokenize(cmdBuffer *const __RESTRICT__ cmdbuf, const char *str, const size_t strLength, const tick_t timestamp, const tick_t delay){

	// Parse the string into a series of tokenized commands.
	const char *const strEnd = &str[strLength];
	cmdTokenized cmdtok;

	// Note that this delay is only modified when the
	// 'wait' command is invoked. This function takes
	// in a delay argument to work with aliases.
	cmdtok.timestamp = timestamp;
	cmdtok.delay = delay;

	while(str != strEnd){

		// Find where the current command ends and the next command begins.
		// Don't reset cmdtok.delay or cmdtok.timestamp.
		// The delay affects all commands here.
		cmdtok.argc = 0;
		str = cmdBufferTokenizeCommand(cmdbuf->argBuffer, &cmdbuf->argBufferSize, &cmdtok, str, strEnd);

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
				// We loop backwards through the list, since we
				// assume commands will roughly be added in order
				// of timestamp. We wish to maintain chronological
				// ordering for all elements in the list.
				cmdTokenized *cmdNew;
				cmdTokenized *cmdInsert = cmdbuf->cmdListEnd;
				while(cmdInsert != NULL && timestamp < cmdInsert->timestamp){
					cmdInsert = moduleCommandTokenizedPrev(cmdInsert);
				}

				// Insert the command.
				// Note that sometimes we might encounter a situation where
				// cmdInsert is NULL, but the list is not empty. This can
				// create a weird situation for the doubly-linked list.
				// Thus if cmdInsert is NULL, memDLinkInsertAfter() will
				// actually prepend the element to the list.
				cmdNew = moduleCommandTokenizedInsertAfter(&cmdbuf->cmdListStart, cmdInsert);
				if(cmdNew == NULL){
					/** Memory allocation failure. **/
					return -1;
				}
				// Update the end pointer.
				if(cmdInsert == cmdbuf->cmdListEnd){
					cmdbuf->cmdListEnd = cmdNew;
				}
				*cmdNew = cmdtok;

			}

		}

	}

	return 1;

}

return_t cmdBufferExecute(cmdBuffer *const __RESTRICT__ cmdbuf, cmdSystem *const __RESTRICT__ cmdsys){

	// Execute a series of tokenized commands.
	size_t argBufferSize = 0;
	cmdTokenized *cmdListEnd = NULL;

	cmdTokenized *cmdtokNext;
	cmdTokenized *cmdtok = cmdbuf->cmdListStart;

	while(cmdtok != NULL){

		cmdtokNext = moduleCommandTokenizedNext(cmdtok);

		if(cmdtok->delay > 0){

			// This command is delayed. Move
			// it to the front of the buffer.
			const char **arg = &cmdtok->argv[0];
			const char **const argLast = &cmdtok->argv[cmdtok->argc-1];
			const size_t cmdSize = *argLast + strlen(*argLast) - *arg;
			const size_t offset = *arg - &cmdbuf->argBuffer[argBufferSize];

			// Copy the arguments over.
			memmove(&cmdbuf->argBuffer[argBufferSize], *arg, cmdSize);
			// Adjust the pointers.
			while(arg <= argLast){
				*arg -= offset;
				++arg;
			}
			// Update the delay.
			--cmdtok->delay;

			// Track buffer variables.
			argBufferSize += cmdSize;
			cmdListEnd = cmdtok;

		}else{

			// Execute the command.
			const cmdTrieNode *const node = cmdSystemFind(cmdsys, cmdtok->argv[0]);
			if(node != NULL){
				if(node->type == 0){

					// The command is valid! Execute it.
					// Don't forget to skip the first argument!
					((cmdFunction)node->cmd)(cmdsys, cmdtok->argc-1, &cmdtok->argv[1]);

				}else{

					// This is an alias.
					// Tokenize it and insert it into the command buffer.
					// We temporarily set the end of the
					// linked list for faster insertions.
					cmdbuf->cmdListEnd = cmdtok;
					///cmd = cmdAddress(cmd);
					if(cmdBufferTokenize(cmdbuf, (const char *)node->cmd, strlen((const char *)node->cmd), cmdtok->timestamp, cmdtok->delay) < 0){
						/** Memory allocation failure. **/
						return -1;
					}

				}
			}

			// Remove it from the list.
			moduleCommandTokenizedFree(&cmdbuf->cmdListStart, cmdtok);


		}

		cmdtok = cmdtokNext;

	}

	// This is the size of all the delayed
	// commands, and a pointer to the last one.
	cmdbuf->argBufferSize = argBufferSize;
	cmdbuf->cmdListEnd = cmdListEnd;
	return 1;

}

void cmdBufferDelete(cmdBuffer *const __RESTRICT__ cmdbuf){
	cmdTokenized *cmdtok = cmdbuf->cmdListStart;
	while(cmdtok != NULL){
		cmdTokenized *const cmdtokNext = moduleCommandTokenizedNext(cmdtok);
		moduleCommandTokenizedFree(&cmdbuf->cmdListStart, cmdtok);
		cmdtok = cmdtokNext;
	}
}