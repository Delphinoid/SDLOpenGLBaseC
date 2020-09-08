#include "command.h"
#include "memoryManager.h"
#include <string.h>

static void cmdTrieInit(cmdTrieNode *const __RESTRICT__ node, const char c){
	node->value = c;
	node->childNum = 0;
	node->children = NULL;
	node->cmd = NULL;
}
static size_t cmdTrieNext(cmdTrieNode **const __RESTRICT__ node, const char c){
	// Gets the next node when finding a command.
	// Loop through node's children until we find a child
	// that matches c.
	cmdNodeIndex_t index = 0;
	while(index < (*node)->childNum){
		if(c == (*node)->children[index].value){
			*node = &(*node)->children[index];
			return index;
		}else if(c > (*node)->children[index].value){
			break;
		}
		++index;
	}
	return (size_t)-1;
}

static return_t cmdTrieAddNode(cmdTrieNode **const __RESTRICT__ node, const char c){
	// Same as trieNext(), but creates a new node when necessary.
	cmdNodeIndex_t index = 0;
	while(index < (*node)->childNum){
		if(c == (*node)->children[index].value){
			// Node with value c already exists.
			*node = &(*node)->children[index];
			return 1;
		}else if(c < (*node)->children[index].value){
			// Insert node before index.
			cmdTrieNode *const tempBuffer = memReallocate((*node)->children, (*node)->childNum*sizeof(cmdTrieNode));
			if(tempBuffer == NULL){
				/** Memory allocation failure. **/
				return -1;
			}
			(*node)->children = tempBuffer;
			++(*node)->childNum;
			if(index < (*node)->childNum-1){
				memmove(
					&((*node)->children[index])+1 ,
					&((*node)->children[index]),
					((*node)->childNum-1-index)*sizeof(cmdTrieNode)
				);
			}
			cmdTrieInit(&((*node)->children[index]), c);
			*node = &(*node)->children[index];
			return 1;
		}
	}
	// Append node to the end.
	{
		cmdTrieNode *const tempBuffer = memReallocate((*node)->children, (*node)->childNum*sizeof(cmdTrieNode));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		(*node)->children = tempBuffer;
		++(*node)->childNum;
	}
	cmdTrieInit(&((*node)->children[index]), c);
	*node = &(*node)->children[index];
	return 1;
}
static return_t cmdTrieRemoveChild(cmdTrieNode *const __RESTRICT__ node, const cmdNodeIndex_t index){
	// Removes a child from a node.
	// Assumes the child is empty.
	if(node->childNum == 1){
		memFree(node->children);
		node->childNum = 0;
		node->value = 0x7F;
		node->cmd = NULL;
	}else{
		if(index < node->childNum-1){
			memmove(
				&(node->children[index])+1 ,
				&(node->children[index]),
				(node->childNum-1-index)*sizeof(cmdTrieNode)
			);
		}
		{
			cmdTrieNode *const tempBuffer = memReallocate(node->children, node->childNum*sizeof(cmdTrieNode));
			if(tempBuffer == NULL){
				/** Memory allocation failure. **/
				return -1;
			}
			node->children = tempBuffer;
			--node->childNum;
		}
	}
	return 1;
}

static return_t cmdValid(const char *const name, const cmdFunction cmd){
	const char *check = name;
	// Make sure the name and function pointer are not NULL.
	if(check != NULL && cmd != NULL){
		// Check for any invalid characters in the command's name.
		while(*check != '\0'){
            if((*check >= 'A' && *check <= 'Z') ||
			   (*check >= 'a' && *check <= 'z') ||
			   (*check == '_')){
				++check;
			}else{
				// Name contains an invalid character.
				return 0;
			}
		}
	}
	return check != name;
}
static return_t cmdParse(char *const __RESTRICT__ str){
	return 0;
}

void cmdInit(cmdSystem *const __RESTRICT__ root){
	/**
	*** Value is set to DEL instead of NUL so that passing an empty
	*** string into cmdFind() doesn't have unexpected results.
	**/
	root->value = 0x7F;
	root->childNum = 0;
	root->children = NULL;
	root->cmd = NULL;
}
return_t cmdAdd(cmdSystem *const node, const char *__RESTRICT__ name, cmdFunction cmd){
	// Check if the command is valid before adding it.
	if(cmdValid(name, cmd)){
		// Go through each character in name, adding
		// a node when we don't have a match.
		while(*name != '\0'){
			if(cmdTrieAddNode((cmdTrieNode **const __RESTRICT__)&node, *name) < 0){
				/** Memory allocation failure. **/
				return -1;
			}
			++name;
		}
		// We've reached the final node at the end of the command's name.
		if(node->cmd == NULL){
			// If a function is not already linked to
			// this node, link func and return success.
			node->cmd = cmd;
			return 1;
		}
	}
	return 0;
}
return_t cmdRemove(cmdSystem *const node, const char *__RESTRICT__ name){
	// Remove the command and all unused nodes.
	cmdTrieNode *last = node;
	cmdNodeIndex_t lastChildIndex = 0;
	cmdNodeIndex_t childIndex = 0;
	// Find the last node used by multiple commands.
	// We may remove every node after this.
	while(*name != '\0' && (childIndex = cmdTrieNext((cmdTrieNode **const __RESTRICT__)&node, *name)) != (size_t)-1){
		if(node->childNum != 1){
			last = node;
			lastChildIndex = childIndex;
		}
		++name;
	}
	// Remove all unused nodes.
	if(node->childNum == 0){
		cmdTrieNode *current = last->children[lastChildIndex].children;
		while(current != NULL){
			cmdTrieNode *const next = current->children;
			memFree(current);
			current = next;
		}
		// Remove the original last node separately,
		// as it may have multiple children.
		cmdTrieRemoveChild(last, lastChildIndex);
	}
	return 0;
}
return_t cmdFind(cmdSystem *const node, const char *__RESTRICT__ name, cmdFunction cmd){
	/** Any non-null-terminated input name has unexpected results. **/
	while(cmdTrieNext((cmdTrieNode **const __RESTRICT__)&node, *name) != (size_t)-1){
		++name;
	}
	// If we reached the end of the command's name, we've found it.
	if(*name == '\0'){
		cmd = node->cmd;
		return 1;
	}
	return 0;
}
