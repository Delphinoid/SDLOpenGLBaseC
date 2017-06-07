#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL_opengl.h>

typedef struct texture {

	char *name;
	GLuint id;
	GLuint width;
	GLuint height;
	unsigned char translucent;  // If this is set, the texture contains translucency. Used when depth sorting before rendering

} texture;

void tInit(texture *tex);
unsigned char tLoad(texture *tex, const char *prgPath, const char *filePath);
void tDelete(texture *tex);

#endif
