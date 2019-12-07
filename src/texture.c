#define GLEW_STATIC
#include <GL/glew.h>
#include "texture.h"
#include "memoryManager.h"
#include "helpersFileIO.h"
#include "inline.h"
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define IMAGE_RESOURCE_DIRECTORY_STRING "Resources"FILE_PATH_DELIMITER_STRING"Images"FILE_PATH_DELIMITER_STRING
#define IMAGE_RESOURCE_DIRECTORY_LENGTH 17

#define TEXTURE_RESOURCE_DIRECTORY_STRING "Resources"FILE_PATH_DELIMITER_STRING"Textures"FILE_PATH_DELIMITER_STRING
#define TEXTURE_RESOURCE_DIRECTORY_LENGTH 19

// Default texture.
texture tDefault = {
	.diffuseID = 0,
	.normalID = 0,
	.specularID = 0,
	.format = GL_RGBA8,
	.width = 32,
	.height = 32,
	.name = "default",
	.mips = 1,
	.translucent = 0
};

/** Maybe remove printf()s? **/

void tInit(texture *const restrict tex){
	tex->diffuseID = 0;
	tex->normalID = 0;
	tex->specularID = 0;
	tex->format = 0;
	tex->width = 0;
	tex->height = 0;
	tex->name = NULL;
	tex->filtering = TEXTURE_FILTER_MODE_ANY;
	tex->translucent = 0;
}

static void tInitFiltering(uint_least8_t filtering, const uint_least8_t mips){

	GLint filteringMin, filteringMag;

	if(filtering == TEXTURE_FILTER_MODE_ANY){
		filtering = TEXTURE_FILTER_MODE_DEFAULT;
	}

	switch(filtering){
		case TEXTURE_FILTER_MODE_NEAREST:
			if(mips > 1){
				filteringMin = GL_NEAREST_MIPMAP_NEAREST;
			}else{
				filteringMin = GL_NEAREST;
			}
			filteringMag = GL_NEAREST;
		break;
		case TEXTURE_FILTER_MODE_LINEAR:
			if(mips > 1){
				filteringMin = GL_NEAREST_MIPMAP_LINEAR;
			}else{
				filteringMin = GL_LINEAR;
			}
			filteringMag = GL_LINEAR;
		break;
		case TEXTURE_FILTER_MODE_BILINEAR:
			if(mips > 1){
				filteringMin = GL_LINEAR_MIPMAP_NEAREST;
			}else{
				filteringMin = GL_LINEAR;
			}
			filteringMag = GL_LINEAR;
		break;
		case TEXTURE_FILTER_MODE_TRILINEAR:
			if(mips > 1){
				filteringMin = GL_LINEAR_MIPMAP_LINEAR;
			}else{
				filteringMin = GL_LINEAR;
			}
			filteringMag = GL_LINEAR;
		break;
		default:
			return;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filteringMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filteringMag);

}

static SDL_Surface *tLoadImage(const char *const restrict path){
	return IMG_Load(path);
}

static GLuint tCreate(){
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	return id;
}

return_t tLoad(texture *const restrict tex, const char *const restrict prgPath, const char *const restrict filePath){

	GLenum glError;
	SDL_Surface *image = NULL;
	GLenum format;
	GLuint bytes;
	GLint mipNum = 0;
	GLsizei mips[32][4];

	char fullPath[FILE_MAX_PATH_LENGTH];
	const size_t fileLength = strlen(filePath);

	FILE *texInfo;

	tInit(tex);

	fileGenerateFullPath(fullPath, prgPath, strlen(prgPath), TEXTURE_RESOURCE_DIRECTORY_STRING, TEXTURE_RESOURCE_DIRECTORY_LENGTH, filePath, fileLength);
	texInfo = fopen(fullPath, "r");

	if(texInfo != NULL){

		char lineFeed[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;

		fileLine_t currentLine = 0;  // Current file line being read.

		while(fileParseNextLine(texInfo, lineFeed, sizeof(lineFeed), &line, &lineLength)){

			++currentLine;

			// Image
			if(lineLength >= 7 && strncmp(line, "image ", 6) == 0){
				if(image == NULL){
					char imgPath[FILE_MAX_PATH_LENGTH];
					char resPath[FILE_MAX_PATH_LENGTH];
					size_t resLength;
					fileParseResourcePath(&resPath[0], &resLength, line, lineLength, 6);
					fileGenerateFullPath(&imgPath[0], prgPath, strlen(prgPath), IMAGE_RESOURCE_DIRECTORY_STRING, IMAGE_RESOURCE_DIRECTORY_LENGTH, &resPath[0], resLength);
					image = tLoadImage(&imgPath[0]);
					if(image == NULL){
						printf("Error generating SDL_Surface for texture \"%s\": %s\n", imgPath, SDL_GetError());
					}
				}

			// Filtering
			}else if(lineLength >= 13 && strncmp(line, "filter ", 7) == 0){
				if(strncmp(line+7, "ANY", 3) == 0){
					tex->filtering = TEXTURE_FILTER_MODE_ANY;

				}else if(strncmp(line+7, "NEAREST", 7) == 0){
					tex->filtering = TEXTURE_FILTER_MODE_NEAREST;

				}else if(strncmp(line+7, "LINEAR", 6) == 0){
					tex->filtering = TEXTURE_FILTER_MODE_LINEAR;

				}else if(strncmp(line+7, "BILINEAR", 8) == 0){
					tex->filtering = TEXTURE_FILTER_MODE_BILINEAR;

				}else if(strncmp(line+7, "TRILINEAR", 9) == 0){
					tex->filtering = TEXTURE_FILTER_MODE_TRILINEAR;

				}

			// MIPs
			}else if(lineLength >= 11 && strncmp(line, "mip ", 4) == 0){
				if(mipNum < 32){
					// Loads start and end textures, start and end subframes and the frame delays.
					int i;
					const char *token = strtok(line+4, "/");
					for(i = 0; i < 4; ++i){
						mips[mipNum][i] = strtol(token, NULL, 0);
						token = strtok(NULL, "/");
					}
					if(
						(mipNum > 0 && mips[mipNum][2] == mips[mipNum-1][2] >> 1 && mips[mipNum][3] == mips[mipNum-1][3] >> 1) ||
						(mipNum == 0 && mips[0][0] == 0 && mips[0][1] == 0)
					){
						++mipNum;
					}else{
						printf("Error loading texture \"%s\": MIP at line %u is invalid.\n", fullPath, currentLine);
					}
				}

			}

		}

		fclose(texInfo);

	}else{
		printf("Error loading texture \"%s\": Could not open file.\n", fullPath);
		return 0;
	}

	if(image == NULL){
		printf("Error loading texture \"%s\": No image specified.\n", fullPath);
		return 0;
	}

	tex->diffuseID = tCreate();
	if(tex->diffuseID == 0){
		glError = glGetError();
		if(glError != GL_NO_ERROR){
			printf("Error generating mip levels for texture \"%s\": %u\nPlease make sure you specified valid mips.\n", fullPath, glError);
			return 0;
		}
		return 0;
	}

	// Determine the pixel format.
	switch(image->format->BytesPerPixel){
		case 4:
			SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA8888, 0);
			tex->format = GL_RGBA8;
			format = GL_RGBA;
			bytes = 4;
		break;
		default:
			SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGB888, 0);
			tex->format = GL_RGB8;
			format = GL_RGB;
			bytes = 3;
	}

	// Check if the texture contains translucent pixels.
	if(format == GL_RGBA){
		const byte_t *pixels = (byte_t *)image->pixels;
		const GLsizei textureSize = image->w * image->h;
		GLsizei i;
		for(i = 0; i < textureSize; ++i){
			byte_t alpha = pixels[i*4+3];
			if(alpha > 0 && alpha < 255){
				tex->translucent = 1;
				break;
			}
		}
	}

	// Generate MIPs if none were loaded.
	if(mipNum == 0){

		tex->width = image->w;
		tex->height = image->h;
		tex->mips = 1 + floor(log(fmax(tex->width, tex->height)));

		glTexImage2D(GL_TEXTURE_2D, 0, format, image->w, image->h, 0, format, GL_UNSIGNED_BYTE, image->pixels);
		glGenerateMipmap(GL_TEXTURE_2D);

	}else{

		GLint i = 0;
		const byte_t *pixels = (byte_t *)image->pixels;
		byte_t *mipmap = memAllocate(mips[0][2] * mips[0][3] * bytes);
		const GLsizei *mipCurrent = &mips[0][0];
		const GLsizei *const mipLast = &mips[mipNum][0];
		int y;

		tex->width = mips[0][2];
		tex->height = mips[0][3];
		tex->mips = mipNum;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipNum-1);
		for(; mipCurrent < mipLast; mipCurrent+=4, ++i){

			const GLsizei mipx = mipCurrent[0];
			const GLsizei mipy = mipCurrent[1];
			const GLsizei mipw = mipCurrent[2];
			const GLsizei miph = mipCurrent[3];

			// Copy each row of the MIP into a temporary buffer to feed to OpenGL.
			for(y = 0; y < miph; ++y){
				//printf("a %u\n", mipw * bytes);
				memcpy((void *)(mipmap + y * mipw * bytes), (void *)(pixels + (mipy + y) * image->pitch + mipx * bytes), mipw * bytes);
			}
			glTexImage2D(GL_TEXTURE_2D, i, format, mipw, miph, 0, format, GL_UNSIGNED_BYTE, mipmap);

		}

		memFree(mipmap);

	}

	SDL_FreeSurface(image);

	// Change the texture's filtering mode.
	tInitFiltering(tex->filtering, tex->mips);

	// Generate a name based off the file path.
	tex->name = fileGenerateResourceName(filePath, fileLength);
	if(tex->name == NULL){
		/** Memory allocation failure. **/
		tDelete(tex);
		return -1;
	}

	return 1;

}

return_t tDefaultInit(){

	GLenum glError;
	GLsizei pixels[1024];
	int x, y;

	for(y = 0; y < 32; ++y){
		for(x = 0; x < 32; ++x){
			if((x / 4) % 2){
				if((y / 4) % 2){
					pixels[(y * 32) + x] = 0xFFFF00DC;
				}else{
					pixels[(y * 32) + x] = 0xFF000000;
				}
			}else{
				if((y / 4) % 2){
					pixels[(y * 32) + x] = 0xFF000000;
				}else{
					pixels[(y * 32) + x] = 0xFFFF00DC;
				}
			}
		}
	}

	tDefault.diffuseID = tCreate();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	tInitFiltering(tDefault.filtering, tDefault.mips);

	glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error generating default texture: %u\n", glError);
		return 0;
	}

	return 1;

}

void tDelete(texture *const restrict tex){
	if(tex->diffuseID != 0){
		glDeleteTextures(1, &tex->diffuseID);
		tex->diffuseID = 0;
	}
	if(tex->normalID != 0){
		glDeleteTextures(1, &tex->normalID);
		tex->normalID = 0;
	}
	if(tex->specularID != 0){
		glDeleteTextures(1, &tex->specularID);
		tex->specularID = 0;
	}
	if(tex->name != NULL && tex->name != tDefault.name){
		memFree(tex->name);
	}
}
