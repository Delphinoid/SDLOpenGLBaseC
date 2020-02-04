#ifndef TEXTURE_H
#define TEXTURE_H

#define GLEW_STATIC
#include <GL/glew.h>
#include "return.h"
#include "qualifiers.h"

#define TEXTURE_FILTER_MODE_ANY       0
#define TEXTURE_FILTER_MODE_NEAREST   1
#define TEXTURE_FILTER_MODE_LINEAR    2
#define TEXTURE_FILTER_MODE_BILINEAR  3
#define TEXTURE_FILTER_MODE_TRILINEAR 4

#ifndef TEXTURE_FILTER_MODE_DEFAULT
	#define TEXTURE_FILTER_MODE_DEFAULT TEXTURE_FILTER_MODE_BILINEAR
#endif

// The texture struct should be 31 bytes (minus padding).
// This, plus the extra byte overhead for the active flag
// in memory pools, makes the final block divisible by
// 8 bytes, reducing waste on 32-bit platforms.
typedef struct texture {

	// OpenGL texture IDs.
	GLuint diffuseID;
	GLuint normalID;
	GLuint specularID;

	// The internal pixel format of the texture.
	// Used for retrieving pixel data later on.
	GLenum format;

	// Dimensions of the texture's first MIP level.
	float width;
	float height;

	// Image name.
	char *name;

	// Number of MIP levels.
	uint_least8_t mips;

	// Forced filtering mode.
	uint_least8_t filtering;

	// If this is set, the texture contains translucency.
	// Used when depth-sorting before rendering.
	uint_least8_t translucent;

} texture;

extern texture g_tDefault;

void tInit(texture *const __RESTRICT__ tex);
return_t tLoad(texture *const __RESTRICT__ tex, const char *const __RESTRICT__ prgPath, const char *const __RESTRICT__ filePath);
return_t tDefaultInit();
void tDelete(texture *const __RESTRICT__ tex);

#endif
