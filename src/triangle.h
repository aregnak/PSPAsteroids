#ifndef TRI_H
#define TRI_H

#include "vertex.h"
// pspgu header in source file of course
// same for all /src files

typedef struct Vertex Vertex;

typedef struct Triangle
{
    float x, y;
    float w, h;
    float angle;
    short int health;

} Triangle;


void drawTriangle(Triangle* t);
void getTriPeak(Triangle* t, float *peakx, float *peaky);
void playerHealthCheck(Triangle* player);


#endif // TRI_H