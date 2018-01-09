#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL_opengl.h>

typedef struct {

	char *name;
	GLuint id;
	GLuint width;
	GLuint height;
	signed char translucent;  // If this is set, the texture contains translucency. Used when depth sorting before rendering

} texture;

void tInit(texture *tex);
signed char tLoad(texture *tex, const char *prgPath, const char *filePath);
void tDelete(texture *tex);

#endif
