#ifndef PARTICLESTATE_H
#define PARTICLESTATE_H

#include "mat4.h"
#include "rectangle.h"

typedef struct {
    mat4 transformation;
    ///float alpha;
    rectangle frame;
} particleState;

#endif
