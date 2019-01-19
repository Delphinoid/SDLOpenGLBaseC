#ifndef TEXTURE_H
#define TEXTURE_H

#include "return.h"
#include <SDL2/SDL_opengl.h>

#define TEXTURE_FILTER_MODE_ANY       0
#define TEXTURE_FILTER_MODE_NEAREST   1
#define TEXTURE_FILTER_MODE_LINEAR    2
#define TEXTURE_FILTER_MODE_BILINEAR  3
#define TEXTURE_FILTER_MODE_TRILINEAR 4

#define TEXTURE_FILTER_MODE_DEFAULT TEXTURE_FILTER_MODE_BILINEAR

/*
** The texture struct should be 23 bytes (minus padding).
** This, plus the extra byte overhead for the active flag
** in memory pools, makes the final block divisible by
** 8 bytes, reducing waste on 32-bit platforms.
*/
typedef struct {

	// OpenGL texture ID.
	GLuint id;

	// The internal pixel format of the texture.
	// Used for retrieving pixel data later on.
	GLenum format;

	// Dimensions of the texture's first MIP level.
	float width;
	float height;

	// Image name.
	char *name;

	// Number of MIP levels.
	int_least8_t mips;

	// Forced filtering mode.
	int_least8_t filtering;

	// If this is set, the texture contains translucency.
	// Used when depth-sorting before rendering.
	int_least8_t translucent;

} texture;

void tInit(texture *const restrict tex);
return_t tLoad(texture *const restrict tex, const char *const restrict prgPath, const char *const restrict filePath);
return_t tDefault(texture *const restrict tex);
void tDelete(texture *const restrict tex);

#endif
