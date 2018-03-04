#ifndef ASSETHANDLER_H
#define ASSETHANDLER_H

#include "model.hpp"
#include "sprite.hpp"

struct assetHandler {

	private:
		std::vector<texture> allTexWrappers;                // All textures currently loaded. Texture 0 is always an error handler
		std::vector<textureWrapper> allTextureWrappers;  // All texture wrappers currently loaded. Texture wrapper 0 is always an error handler
		std::vector<model> allModels;                    // All models currently loaded. Model 0 is always an error handler
		std::vector<sprite> allSprites;                  // All sprites currently loaded. Sprite 0 is always an error handler


	public:
		int loadTexture(const char *path);      // Load a texture and add it to allTexWrappers
		texture *getTexture(unsigned int pos);  // Return a pointer to a texture at "pos" in the allTexWrappers vector
		texture *getTexture(const char *name);  // Return a pointer to a texture named "name"

		int loadTextureWrapper(const char *path);             // Load a texture wrapper and add it to allTextureWrappers
		textureWrapper *getTextureWrapper(unsigned int pos);  // Return a pointer to a texture wrapper at "pos" in the allTextureWrappers vector
		textureWrapper *getTextureWrapper(const char *name);  // Return a pointer to a texture wrapper named "name"

		int loadModel(const char *path);    // Load a model and add it to allModels
		model *getModel(unsigned int pos);  // Return a pointer to a model at "pos" in the allModels vector
		model *getModel(const char *name);  // Return a pointer to a model named "name"

		int loadSprite(const char *path);     // Load a sprite and add it to allSprites
		sprite *getSprite(unsigned int pos);  // Return a pointer to a sprite at "pos" in the allSprites vector
		sprite *getSprite(const char *name);  // Return a pointer to a sprite named "name"

};

#endif
