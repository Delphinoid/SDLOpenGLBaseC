#ifndef TEXTURE_H
#define TEXTURE_H

#include "return.h"
#include <SDL2/SDL_opengl.h>

#define TEXTURE_DEFAULT_FILTER_MODE GL_LINEAR

typedef struct {

	char *name;
	GLuint id;
	GLsizei width;
	GLsizei height;
	int_least8_t translucent;  // If this is set, the texture contains translucency.
	                           // Used when depth-sorting before rendering.

} texture;

void tInit(texture *tex);
void tGenerate(texture *tex, const GLsizei width, const GLsizei height, const GLint format, const GLint filter, const GLvoid *data);
return_t tLoad(texture *tex, const char *prgPath, const char *filePath);
return_t tDefault(texture *tex);
void tDelete(texture *tex);

#endif
