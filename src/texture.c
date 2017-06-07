#define GLEW_STATIC
#include <GL/glew.h>
#include "texture.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

//void generateNameFromPath(char **name, const char *path);
void copyString(char **destination, const char *source, const unsigned int length);

void tInit(texture *tex){
	tex->name = NULL;
	tex->id = 0;
	tex->width = 0;
	tex->height = 0;
	tex->translucent = 0;
}

unsigned char tLoad(texture *tex, const char *prgPath, const char *filePath){

	tInit(tex);

	char *fullPath = (char *)malloc((strlen(prgPath) + strlen(filePath) + 1) * sizeof(char));
	strcpy(fullPath, prgPath);
	strcat(fullPath, filePath);
	fullPath[strlen(prgPath)+strlen(filePath)] = '\0';

	/* Load image with SDL_Image */
	SDL_Surface *SDLimage = IMG_Load(fullPath);
	if(SDLimage == NULL){
		printf("Error generating SDL_Surface for texture at %s:\n%s\n", fullPath, SDL_GetError());
		free(fullPath);
		return 0;
	}

	GLint pixelFormat;
	if(SDLimage->format->BytesPerPixel == 3){
		pixelFormat = GL_RGB;  // 24 bits per pixel is most likely the RGB format
	}else if(SDLimage->format->BytesPerPixel == 4){
		pixelFormat = GL_RGBA; // 32 bits per pixel is most likely the RGBA format
	}else{
		pixelFormat = -1;
	}

	tex->width = SDLimage->w;
	tex->height = SDLimage->h;


	/* Convert it to an OpenGL texture and free the SDL Surface */
	glGenTextures(1, &tex->id);
	glBindTexture(GL_TEXTURE_2D, tex->id);

	glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, SDLimage->w, SDLimage->h, 0, pixelFormat, GL_UNSIGNED_BYTE, SDLimage->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error generating OpenGL texture at %s:\n%u\n", fullPath, glError);
		free(fullPath);
		return 0;
	}


	/* Check if the texture contains translucent (not just transparent) pixels and then free the SDL surface */
	if(pixelFormat == GL_RGBA){
		uint8_t *pixelData = (uint8_t*)SDLimage->pixels;
		for(uint32_t d = 0; d < tex->width * tex->height; d++){
			uint8_t alpha = pixelData[d*4+3];
			if(alpha > 0 && alpha < 255){
				tex->translucent = 1;
				d = tex->width * tex->height;
			}
		}
	}
	SDL_FreeSurface(SDLimage);


	//generateNameFromPath(tex->name, filePath);
	copyString(&tex->name, filePath, strlen(filePath));
	free(fullPath);
	return 1;

}

void tDelete(texture *tex){
	if(tex->id != 0){
		glDeleteTextures(1, &tex->id);
		tex->id = 0;
	}
	if(tex->name != NULL){
		free(tex->name);
	}
}
