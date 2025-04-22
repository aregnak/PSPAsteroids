#include <pspgu.h>
#include <math.h>
#include "triangle.h"
#include "../game/game.h"

void initPlayer(Triangle* player, short int maxHP)
{
    player->x = 240;
    player->y = 136;
    player->w = 20;
    player->h = 34;
    player->angle = 0;
    player->health = maxHP;
}

void drawTriangle(Triangle* t)
{
    // allocate enough memory for 4 vertices
    Vertex* verts = (Vertex*)sceGuGetMemory(4 * sizeof(Vertex));

    // Calculate the half-width and half-height for centering
    float halfW = t->w / 2;
    float halfH = t->h / 2;

    // Define the vertices relative to the center of the triangle
    float vx[4] = {-halfW, halfW, 0, -halfW}; // x-coordinates
    float vy[4] = {-halfH, -halfH, halfH, -halfH}; // y-coordinates

    // Apply rotation transformation to each vertex
    float cosA = cosf(t->angle); // Precompute cosine
    float sinA = sinf(t->angle); // Precompute sine

    for (int i = 0; i < 4; i++)
    {
        // Rotate the vertex
        float xRot = vx[i] * cosA - vy[i] * sinA;
        float yRot = vx[i] * sinA + vy[i] * cosA;

        // Translate the vertex to the triangle's position
        verts[i].x = (short)(t->x + xRot);
        verts[i].y = (short)(t->y + yRot);
    }

    sceGuColor(0xFFFFFFFF); // colors are ABGR
    sceGuDrawArray(GU_LINE_STRIP, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 4, 0, verts);
}

void getTriPeak(Triangle* t, float *peakx, float *peaky)
{
    float localx = 0;
    float localy = t->h / 2;

    // Rotate the peak position based on the triangle's angle
    float cosA = cosf(t->angle);
    float sinA = sinf(t->angle);
    *peakx = t->x + (localx * cosA - localy * sinA);
    *peaky = t->y + (localx * sinA + localy * cosA);
}

void playerHealthCheck(Triangle* player)
{
    if (player->health > 1)
    {
       player->health--;
    }
    else
    {
        gameState = GAME_OVER;
        player->health = 0;
    }
}