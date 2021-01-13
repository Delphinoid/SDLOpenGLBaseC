#ifndef GRAPHICSRENDERGROUP_H
#define GRAPHICSRENDERGROUP_H

#include <stdint.h>

#define GFX_RNDR_GROUP_TOTAL 2

#define GFX_RNDR_GROUP_UNKNOWN    -1
#define GFX_RNDR_GROUP_OPAQUE      0
#define GFX_RNDR_GROUP_TRANSLUCENT 1

typedef int_fast8_t gfxRenderGroup_t;

#endif
