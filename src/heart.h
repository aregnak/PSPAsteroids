#ifndef HEART_H
#define HEART_H

#include <pspgu.h>

#include "vertex.h"

typedef struct Vertex Vertex;

typedef struct Heart
{
    float x, y;
    float w, h;

} Heart;

void drawHeart(Heart* h, int i);
void checkHearts(void* Player, Heart* heart);
void initHearts(Heart* heart, short int maxHP);

#endif // HEART.H
