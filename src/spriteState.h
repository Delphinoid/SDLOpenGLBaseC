#ifndef SPRITESTATE_H
#define SPRITESTATE_H

#include "mat4.h"
#include "rectangle.h"

typedef struct {
    mat4 transformation;
    rectangle frame;
    ///float alpha;
} spriteState;

#endif
