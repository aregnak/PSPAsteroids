#include <pspgu.h>
#include <math.h>
#include "bullet.h"

void resetBullet(Bullet* pew, int i)
{
    pew[i].active = 0;
    pew[i].angle = 0;
    pew[i].x = 0;
    pew[i].y = 0;
    pew[i].speed = 0;
}

void initBullet(Bullet* pew, short int maxPew)
{
    for (int i = 0; i < maxPew; i++)
    {
        resetBullet(pew, i);
    }
}

void drawBullet(Bullet* pewp, int pIndex)
{
    Bullet* pew = &pewp[pIndex];

    Vertex* p = (Vertex*)sceGuGetMemory(sizeof(Vertex));

    p[0].x = pew->x;
    p[0].y = pew->y;

    sceGuColor(0xFFFFFFFF); // colors are ABGR
    sceGuDrawArray(GU_POINTS, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 1, 0, p);
}

void moveBullet(Bullet* pewp, int i)
{
    Bullet* pew = &pewp[i];

    pew->x += cosf(pew->angle) * pew->speed;
    pew->y += sinf(pew->angle) * pew->speed;
}

void updateBullets(Bullet* pew, short int maxPew, short int sHeight, short int sWidth)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (pew[i].active)
        {
            // Move the bullet
            moveBullet(pew, i);

            // Draw the bullet
            drawBullet(pew, i);

            if (pew[i].x < 0 - 20 || pew[i].x > sWidth + 20 ||
                pew[i].y < 0 - 20 || pew[i].y > sHeight + 20)
            {
                // "remove" the bullet
                pew[i].active = 0;
            }
        }
    }
}