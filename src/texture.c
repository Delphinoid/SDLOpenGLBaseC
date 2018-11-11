#define GLEW_STATIC
#include <GL/glew.h>
#include "texture.h"
#include "memoryManager.h"
#include "helpersFileIO.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

/** Maybe remove printf()s? **/

void tInit(texture *tex){
	tex->name = NULL;
	tex->id = 0;
	tex->width = 0;
	tex->height = 0;
	tex->translucent = 0;
}

void tGenerate(texture *tex, const GLsizei width, const GLsizei height, const GLint format, const GLint filter, const GLvoid *data){
	glGenTextures(1, &tex->id);
	glBindTexture(GL_TEXTURE_2D, tex->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	tex->width = width;
	tex->height = height;
}

return_t tLoad(texture *tex, const char *prgPath, const char *filePath){

	SDL_Surface *SDLimage;
	GLint pixelFormat;
	GLenum glError;

	char fullPath[FILE_MAX_PATH_LENGTH];
	const size_t fileLength = strlen(filePath);

	tInit(tex);

	fileGenerateFullPath(&fullPath[0], prgPath, strlen(prgPath), filePath, fileLength);

	/* Load image with SDL_Image */
	SDLimage = IMG_Load(&fullPath[0]);
	if(SDLimage == NULL){
		printf("Error generating SDL_Surface for texture \"%s\": %s\n", &fullPath[0], SDL_GetError());
		return 0;
	}

	if(SDLimage->format->BytesPerPixel == 3){
		pixelFormat = GL_RGB;  // 24 bits per pixel is most likely the RGB format
	}else if(SDLimage->format->BytesPerPixel == 4){
		pixelFormat = GL_RGBA; // 32 bits per pixel is most likely the RGBA format
	}else{
		pixelFormat = -1;
	}


	/* Convert it to an OpenGL texture and free the SDL Surface */
	tGenerate(tex, SDLimage->w, SDLimage->h, pixelFormat, TEXTURE_DEFAULT_FILTER_MODE, SDLimage->pixels);

	glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error generating OpenGL texture \"%s\": %u\n", &fullPath[0], glError);
		tDelete(tex);
		return 0;
	}


	/* Check if the texture contains translucent (not just transparent) pixels and then free the SDL surface */
	if(pixelFormat == GL_RGBA){
		const byte_t *pixelData = (byte_t *)SDLimage->pixels;
		const GLsizei textureSize = tex->width * tex->height;
		GLsizei i;
		for(i = 0; i < textureSize; ++i){
			byte_t alpha = pixelData[i*4+3];
			if(alpha > 0 && alpha < 255){
				tex->translucent = 1;
				i = tex->width * tex->height;
			}
		}
	}
	SDL_FreeSurface(SDLimage);


	tex->name = memAllocate((fileLength+1)*sizeof(char));
	if(tex->name == NULL){
		/** Memory allocation failure. **/
		tDelete(tex);
		return -1;
	}
	memcpy(tex->name, filePath, fileLength);
	tex->name[fileLength] = '\0';

	return 1;

}

return_t tDefault(texture *tex){

	GLenum glError;
	GLsizei pixels[1024];
	size_t h, w;

	tInit(tex);
	tex->name = memAllocate(8*sizeof(char));
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
		memFree(tex->name);
	}
}
