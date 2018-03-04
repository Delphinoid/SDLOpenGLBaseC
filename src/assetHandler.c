/*#include "assetHandler.hpp"

int assetHandler::loadTexture(const char *path){
	//
}

int assetHandler::loadTextureWrapper(const char *path){
	//
}

int assetHandler::loadModel(const char *path){
	//
}

int assetHandler::loadSprite(const char *path){
	//
}


texture *assetHandler::getTexture(unsigned int pos){
	if(pos < allTexWrappers.size()){
		return &allTexWrappers[pos];
	}
	return &allTexWrappers[0];
}
texture *assetHandler::getTexture(const char *name){
	for(unsigned int d = allTexWrappers.size() - 1; d >= 0; d--){
		if(strcmp(name, allTexWrappers[d].name)){
			return &allTexWrappers[d];
		}
	}
	return &allTexWrappers[0];
}

textureWrapper *assetHandler::getTextureWrapper(unsigned int pos){
	if(pos < allTextureWrappers.size()){
		return &allTextureWrappers[pos];
	}
	return &allTextureWrappers[0];
}
textureWrapper *assetHandler::getTextureWrapper(const char *name){
	for(unsigned int d = allTextureWrappers.size() - 1; d >= 0; d--){
		if(strcmp(name, allTextureWrappers[d].name)){
			return &allTextureWrappers[d];
		}
	}
	return &allTextureWrappers[0];
}

model *assetHandler::getModel(unsigned int pos){
	if(pos < allModels.size()){
		return &allModels[pos];
	}
	return &allModels[0];
}
model *assetHandler::getModel(const char *name){
	for(unsigned int d = allModels.size() - 1; d >= 0; d--){
		if(strcmp(name, allModels[d].name)){
			return &allModels[d];
		}
	}
	return &allModels[0];
}

sprite *assetHandler::getSprite(unsigned int pos){
	if(pos < allSprites.size()){
		return &allSprites[pos];
	}
	return &allSprites[0];
}
sprite *assetHandler::getSprite(const char *name){
	for(unsigned int d = allSprites.size() - 1; d >= 0; d--){
		if(strcmp(name, allSprites[d].name)){
			return &allSprites[d];
		}
	}
	return &allSprites[0];
}*/
