#include "skeleton.h"

void sklInit(skeleton *skl){
	skl->name = NULL;
	skl->root = NULL;
	skl->boneNum = 0;
}

unsigned char sklLoad(skeleton *skl, const char *prgPath, const char *filePath){
	return 0;
}

void skliInit(sklInstance *skli, skeleton *skl){
	skli->skl = skl;
	if(skl != NULL){
		skli->sklState = malloc(skl->boneNum*sizeof(sklBone));
	}
	animInit(&skli->animator);
}

unsigned char skliGenerateState(sklInstance *skli){
	//*sklState = malloc(skl->boneNum*sizeof(sklBone));
	//(*sklState+i)->position =
	return 0;
}

void skliDelete(skeleton *skl){
	//
}
