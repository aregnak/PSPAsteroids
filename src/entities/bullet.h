#ifndef BULLET_H
#define BULLET_H

#define MAX_BULLETS 20

#include "vertex.h"

typedef struct Vertex vertex;

typedef struct Bullet
{
    float x, y;
    float angle;
    float speed;
    char active;

} Bullet;

void resetBullet(Bullet* pew, int i);
void initBullet(Bullet* pew, short int maxPew);
void drawBullet(Bullet* pewp, int pIndex);
void moveBullet(Bullet* pewp, int i);
void updateBullets(Bullet* pew, short int maxPew, short int sHeight, short int sWidth);

#endif // BULLET_H