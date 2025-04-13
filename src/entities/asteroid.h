#ifndef ASTEROID_H
#define ASTEROID_H

#define MAX_AST 20

#include "vertex.h"
#include "bullet.h"
#include "triangle.h"

typedef struct Vertex vertex;

typedef struct Asteroid
{
    float x;
    float y;
    float w;
    float h;
    float angle;
    float velx;
    float vely;
    char active;
} Asteroid;

void drawAsteroid(Asteroid* r, int rIndex);
void resetAsteroid(Asteroid* rock, int i);
void initAsteroid(Asteroid* rock, short int maxAst);
void spawnAsteroid(Asteroid* rock, short int aX, short int aY, short int size, short int sHeight, short int sWidth);
void randomAsteroidSpawn(Asteroid* rock, short int sHeight, short int sWidth);
void splitAsteroid(Asteroid* rock, int i, short int sHeight, short int sWidth);
void updateAsteroid(Asteroid* rock, Bullet* pew, short int asteroidCount, short int maxAst, short int maxPew, short int sHeight, short int sWidth);
void playerCollision(Triangle* tri, Asteroid* rock, short int asteroidCount, short int maxAst, short int sHeight, short int sWidth);
void initGame(Asteroid* rock, short int sHeight, short int sWidth);

extern int score;

#endif // ASTEROID_H