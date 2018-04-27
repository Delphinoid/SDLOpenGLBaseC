#define GLEW_STATIC
#include <GL/glew.h>
#include "texture.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

/** Maybe remove printf()s? **/

void tInit(texture *tex){
	tex->name = NULL;
	tex->id = 0;
	tex->width = 0;
	tex->height = 0;
	tex->translucent = 0;
}

signed char tLoad(texture *tex, const char *prgPath, const char *filePath){

	tInit(tex);

	const size_t pathLen = strlen(prgPath);
	const size_t fileLen = strlen(filePath);
	char *fullPath = malloc((pathLen+fileLen+1)*sizeof(char));
	if(fullPath == NULL){
		/** Remove printf()s **/
		/** Memory allocation failure. **/
		return -1;
	}
	memcpy(fullPath, prgPath, pathLen);
	memcpy(fullPath+pathLen, filePath, fileLen);
	fullPath[pathLen+fileLen] = '\0';

	/* Load image with SDL_Image */
	SDL_Surface *SDLimage = IMG_Load(fullPath);
	if(SDLimage == NULL){
		printf("Error generating SDL_Surface for texture \"%s\": %s\n", fullPath, SDL_GetError());
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TEXTURE_DEFAULT_FILTER_MODE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TEXTURE_DEFAULT_FILTER_MODE);

	GLenum glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error generating OpenGL texture \"%s\": %u\n", fullPath, glError);
		tDelete(tex);
		free(fullPath);
		return 0;
	}


	/* Check if the texture contains translucent (not just transparent) pixels and then free the SDL surface */
	if(pixelFormat == GL_RGBA){
		const unsigned char *pixelData = (unsigned char *)SDLimage->pixels;
		const size_t textureSize = tex->width * tex->height;
		size_t i;
		for(i = 0; i < textureSize; ++i){
			unsigned char alpha = pixelData[i*4+3];
			if(alpha > 0 && alpha < 255){
				tex->translucent = 1;
				i = tex->width * tex->height;
			}
		}
	}
	SDL_FreeSurface(SDLimage);


	free(fullPath);


	tex->name = malloc((fileLen+1)*sizeof(char));
	if(tex->name == NULL){
		/** Memory allocation failure. **/
		tDelete(tex);
		return -1;
	}
	memcpy(tex->name, filePath, fileLen);
	tex->name[fileLen] = '\0';

	return 1;

}

signed char tDefault(texture *tex){

	GLenum glError;
	unsigned int pixels[32*32];
	size_t h, w;

	tInit(tex);
	tex->name = malloc(8*sizeof(char));
	if(tex->name == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	tex->name[0] = 'd';
	tex->name[1] = 'e';
	tex->name[2] = 'f';
	tex->name[3] = 'a';
	tex->name[4] = 'u';
	tex->name[5] = 'l';
	tex->name[6] = 't';
	tex->name[7] = '\0';
	tex->width = 32;
	tex->height = 32;

	for(h = 0; h < tex->height; ++h){
		for(w = 0; w < tex->width; ++w){
			if((w / 4) % 2){
				if((h / 4) % 2){
					pixels[(h * tex->width) + w] = 0xFFFF00DC;
				}else{
					pixels[(h * tex->width) + w] = 0xFF000000;
				}
			}else{
				if((h / 4) % 2){
					pixels[(h * tex->width) + w] = 0xFF000000;
				}else{
					pixels[(h * tex->width) + w] = 0xFFFF00DC;
				}
			}
		}
	}

	glGenTextures(1, &tex->id);
	glBindTexture(GL_TEXTURE_2D, tex->id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TEXTURE_DEFAULT_FILTER_MODE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TEXTURE_DEFAULT_FILTER_MODE);

	glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error generating default texture.\n");
		tDelete(tex);
		return 0;
	}

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
