#ifndef HEART_H
#define HEART_H

#include "vertex.h"
#include "triangle.h"

typedef struct Vertex Vertex;

typedef struct Heart
{
    float x, y;
    float w, h;

} Heart;

void drawHeart(Heart* h, int i);
void checkHearts(Triangle* player, Heart* heart);
void initHearts(Heart* heart, short int maxHP);

#endif // HEART.H
