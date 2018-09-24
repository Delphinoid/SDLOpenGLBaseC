#ifndef TEXTURE_H
#define TEXTURE_H

#include "typedefs.h"
#include <SDL2/SDL_opengl.h>

#define TEXTURE_DEFAULT_FILTER_MODE GL_LINEAR

typedef struct {

	char *name;
	GLuint id;
	GLsizei width;
	GLsizei height;
	int8_t translucent;  // If this is set, the texture contains translucency.
	                     // Used when depth-sorting before rendering.

} texture;

void tInit(texture *tex);
return_t tLoad(texture *tex, const char *prgPath, const char *filePath);
return_t tDefault(texture *tex);
void tDelete(texture *tex);

#endif
