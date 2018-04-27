#include "console.h"
#include <stdlib.h>
#include <string.h>

void trieInit(cmdTrieNode *node, char c){
	node->value = c;
	node->childNum = 0;
	node->children = NULL;
	node->cmd = NULL;
}

signed char trieNext(cmdTrieNode **node, char c){
	/* Gets the next node when finding a command. */
	// Loop through node's children until we find a child
	// that matches c.
	size_t index = 0;
	while(index < (*node)->childNum){
		if(c == (*node)->children[index].value){
			*node = &(*node)->children[index];
			return 1;
		}else if(c > (*node)->children[index].value){
			break;
		}
		++index;
	}
	return 0;
}

signed char trieAddNode(cmdTrieNode **node, char c){
	/*
	** Same as trieNext(), but creates a new node when necessary.
	*/
	size_t index = 0;
	while(index < (*node)->childNum){
		if(c == (*node)->children[index].value){
			// Node with value c already exists.
			*node = &(*node)->children[index];
			return 1;
		}else if(c < (*node)->children[index].value){
			// Insert node before index.
			++(*node)->childNum;
			cmdTrieNode *tempBuffer = realloc((*node)->children, (*node)->childNum * sizeof(cmdTrieNode));
			if(tempBuffer != NULL){
				(*node)->children = tempBuffer;
				memmove(&((*node)->children[index])+1,
				        &((*node)->children[index]),
				        ((*node)->childNum - 1 - index) * sizeof(cmdTrieNode));
				trieInit(&((*node)->children[index]), c);
				*node = &(*node)->children[index];
				return 1;
			}
			return 0;
		}
	}
	// Append node to the end.
	++(*node)->childNum;
	cmdTrieNode *tempBuffer = realloc((*node)->children, (*node)->childNum * sizeof(cmdTrieNode));
	if(tempBuffer == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	(*node)->children = tempBuffer;
	trieInit(&((*node)->children[index]), c);
	*node = &(*node)->children[index];
	return 1;
}

signed char cmdValid(char *name, command cmd){
	size_t index = 0;
	// Make sure the name and function pointer are not NULL.
	if(name != NULL && cmd != NULL){
		// Check for any invalid characters in the command's name.
		while(name[index] != '\0'){
            if((name[index] >= 'A' && name[index] <= 'Z') ||
			   (name[index] >= 'a' && name[index] <= 'z') ||
			   (name[index] == '_')){
				++index;
			}else{
				// Name contains an invalid character.
				return 0;
			}
		}
	}
	return index > 0;
}
signed char cmdParse(char *str){
	return 0;
}

void conInit(console *con){
	/**
	*** Value is set to DEL instead of NUL so that passing an empty
	*** string into conFindCommand() doesn't have unexpected results.
	**/
	con->cmdLookup.value = 0x7F;
	con->cmdLookup.childNum = 0;
	con->cmdLookup.children = NULL;
	con->cmdLookup.cmd = NULL;
}
signed char conAddCommand(console *con, char *name, signed char (*func)(const cmdVariables *cmdv, const cmdInput *cmdi)){
	// Check if the command is valid before adding it.
	if(cmdValid(name, func)){
		cmdTrieNode *node = &con->cmdLookup;
		size_t index = 0;
		// Go through each character in name, adding
		// a node when we don't have a match.
		while(name[index] != '\0'){
			if(trieAddNode(&node, name[index]) == -1){
				/** Memory allocation failure. **/
				return -1;
			}
			++index;
		}
		// We've reached the final node at the end of the command's name.
		if(node->cmd == NULL){
			// If a function is not already linked to
			// this node, link func and return success.
			node->cmd = func;
			return 1;
		}
	}
	return 0;
}
signed char conRemoveCommand(console *con, char *name){
	/*cmdTrieNode *node = &con->cmdLookup;
	cmdTrieNode *last = NULL;
	size_t index = 0;
	while(name[index] != '\0' && trieNext(&node, name[index])){
		if(node->childNum == 1 && last == NULL){
			last = node;
		}else{
			last = NULL;
		}
		++index;
	}
	// If we reached the end of the command's name, we've found it.
	if(index > 0 && name[index] == '\0'){
		if(node->childNum > 0){

		}
		return 1;
	}*/
	return 0;
}
signed char conFindCommand(console *con, char *name, command *cmd){
	/** Any non-null-terminated input name has unexpected results. **/
	cmdTrieNode *node = &con->cmdLookup;
	size_t index = 0;
	while(trieNext(&node, name[index])){
		++index;
	}
	// If we reached the end of the command's name, we've found it.
	if(name[index] == '\0'){
		*cmd = node->cmd;
		return 1;
	}
	return 0;
}
