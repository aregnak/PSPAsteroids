#ifndef HEART_H
#define HEART_H

#define MAX_HP 5

#include "vertex.h"
#include "triangle.h"

typedef struct Vertex Vertex;

typedef struct Heart
{
    float x;
    float y;
    float w;
    float h;

} Heart;

void drawHeart(Heart* h, int i);
void checkHearts(Triangle* player, Heart* heart);
void initHearts(Heart* heart, short int maxHP);

#endif // HEART.H
