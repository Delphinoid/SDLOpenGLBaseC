#include <stdlib.h>
#include <string.h>

/** Are these really necessary? Hint: no **/
void copyString(char **destination, const char *source, const unsigned int length){
	if(length > 0){
		*destination = malloc((length+1)*sizeof(char));
		if(destination != NULL){
			strcpy(*destination, source);
			(*destination)[length] = '\0';
		}
	}
}

void generateNameFromPath(char **name, const char *path){
	unsigned int pathLength = strlen(path);
	unsigned int pathLastSlash = strrchr(path, '\\') - path + 1;
	unsigned int pathLastPeriod = strrchr(path, '.') - path;
	if(pathLastPeriod > pathLastSlash && pathLastPeriod < pathLength){
		copyString(name, path+pathLastSlash, pathLastPeriod-pathLastSlash);
	}
}
