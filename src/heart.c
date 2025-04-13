#include <pspgu.h>
#include "heart.h"

void drawHeart(Heart* h, int i)
{
    Heart* heart = &h[i];

    Vertex* verts = (Vertex*)sceGuGetMemory(9 * sizeof(Vertex));

    verts[0].x = heart->x + heart->w / 2;
    verts[0].y = heart->y + heart->h;

    verts[1].x = heart->x + heart->w * 0.75;
    verts[1].y = heart->y + heart->h * 0.65;

    verts[2].x = heart->x + heart->w;
    verts[2].y = heart->y + heart->h * 0.25;

    verts[3].x = heart->x + heart->w * 0.75;
    verts[3].y = heart->y;

    verts[4].x = heart->x + heart->w / 2;
    verts[4].y = heart->y + heart->h * 0.25;

    verts[5].x = heart->x + heart->w * 0.25;
    verts[5].y = heart->y;

    verts[6].x = heart->x;
    verts[6].y = heart->y + heart->h * 0.25;

    verts[7].x = heart->x + heart->w * 0.25;
    verts[7].y = heart->y + heart->h * 0.65;

    verts[8].x = heart->x + heart->w / 2;
    verts[8].y = heart->y + heart->h;

    sceGuColor(0xFFFFFFFF); // Red, colors are ABGR
    sceGuDrawArray(GU_LINE_STRIP, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 9, 0, verts);
}

void checkHearts(Triangle* player, Heart* heart)
{
    short int hp = player->health;

    for (int i = 0; i < hp; i++)
    {
        drawHeart(heart, i);
    }
}

void initHearts(Heart* heart, short int maxHP)
{
    short int hX = 15;
    short int hY = 244;

    for (int i = 0; i < maxHP; i++)
    {
        heart[i].x = hX;
        heart[i].y = hY;
        heart[i].w = 16;
        heart[i].h = 14;

        // create a 10 pixel space between hearts
        hX += heart[i].w + 10;
    }
}