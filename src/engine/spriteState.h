#ifndef SPRITESTATE_H
#define SPRITESTATE_H

#include "mat3x4.h"
#include "rectangle.h"

typedef struct {
    mat3x4 transformation;
    rectangle frame;
    ///float alpha;
} spriteState;

#endif
