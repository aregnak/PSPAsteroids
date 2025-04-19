#include <pspgu.h>
#include <math.h>
#include <stdlib.h>
#include "asteroid.h"
#include "../game/game.h"


void initGame(Asteroid* rock, short int sHeight, short int sWidth)
{
    for (int i = 0; i < 3; i++)
    {
        // spawnAsteroid(rock, 40, sHeight, sWidth);
        randomAsteroidSpawn(rock, sHeight, sWidth);
    }

    score = 0;
}

void drawAsteroid(Asteroid* r, int rIndex)
{
    Asteroid* rock = &r[rIndex];

    // Allocate enough memory for 11 vertices for a nice looking asteroid
    Vertex* verts = (Vertex*)sceGuGetMemory(12 * sizeof(Vertex));

    // Calculate the half-width and half-height for centering
    float halfW = rock->w / 2;
    float halfH = rock->h / 2;

    // Define the vertices relative to the center of the shape
    float vx[12] = {
        -halfW * 0.8,
        0,
        halfW * 0.5,
        halfW * 0.60,
        halfW,
        halfW * 0.25,
        halfW * 0.33,
        -halfW * 0.65,
        -halfW * 0.55,
        -halfW,
        -halfW * 0.6,
        -halfW * 0.80}; // x-coordinates

    float vy[12] = {
        -halfH * 0.95,
        -halfH * 0.5,
        -halfH * 0.75,
        0,
        halfH * 0.4,
        halfH * 0.5,
        halfH,
        halfH * 0.9,
        halfH * 0.4,
        0,
        -halfH * 0.45,
        -halfH * 0.95}; // y-coordinates

    // Apply rotation transformation to each vertex
    float cosA = cosf(rock->angle);
    float sinA = sinf(rock->angle);

    for (int i = 0; i < 12; i++)
    {
        // Rotate the vertex
        float xRot = vx[i] * cosA - vy[i] * sinA;
        float yRot = vx[i] * sinA + vy[i] * cosA;

        // Translate the vertex to the triangle's position
        verts[i].x = (short)(rock->x + xRot);
        verts[i].y = (short)(rock->y + yRot);
    }

    sceGuColor(0xFFFFFFFF); // colors are ABGR
    sceGuDrawArray(GU_LINE_STRIP, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 12, 0, verts);
}

// reset 1 asteroid
void resetAsteroid(Asteroid* rock, int i)
{
    rock[i].active = 0;
    rock[i].x = 0;
    rock[i].y = 0;
    rock[i].w = 0;
    rock[i].h = 0;
    rock[i].angle = 0;
    rock[i].velx = 0;
    rock[i].vely = 0;
}

// initialize asteroid array memory space
void initAsteroid(Asteroid* rock, short int maxAst)
{
    for (int i = 0; i < maxAst; i++)
    {
        resetAsteroid(rock, i);
    }
}

void spawnAsteroid(Asteroid* rock, short int aX, short int aY, short int size, short int sHeight, short int sWidth)
{
    for (int i = 0; i < MAX_AST; i++)
    {
        if (!rock[i].active)
        {
            // rock[i].id = i;
            rock[i].x = aX;
            rock[i].y = aY;
            rock[i].w = size;
            rock[i].h = size;
            rock[i].active = 1;
            rock[i].velx = (random() % 200) - 100;
            rock[i].vely = (random() % 200) - 100;
            break;
        }
    }
}

void randomAsteroidSpawn(Asteroid* rock, short int sHeight, short int sWidth)
{
    short int aX;
    short int aY;

    short int side = random() % 4;

    // spawn on random side
    switch (side)
    {
        case 0:
            aX = random() % sWidth;
            aY = -40;
            break; // top
        case 1:
            aX = random() % sWidth;
            aY = sHeight + 40;
            break; // bottom
        case 2:
            aX = -40;
            aY = random() % sHeight;
            break; // left
        case 3:
            aX = sWidth + 40;
            aY = random() % sHeight;
            break; // right
    }

    // spawn the asteroid at a random point
    spawnAsteroid(rock, aX, aY, 40, sHeight, sWidth);
}

void splitAsteroid(Asteroid* rock, int i, short int sHeight, short int sWidth)
{
    short int aX = rock[i].x;
    short int aY = rock[i].y;

    spawnAsteroid(rock, aX, aY, 20, sHeight, sWidth);
}

void updateAsteroid(Asteroid* rock, Bullet* pew, short int asteroidCount, short int maxAst, short int maxPew, short int sHeight, short int sWidth)
{
    for (int i = 0; i < maxAst; i++)
    {
        if (rock[i].active)
        {
            rock[i].x += rock[i].velx * 0.01;
            rock[i].y += rock[i].vely * 0.01;

            rock[i].angle += 0.01f;

            handleArea(&rock[i].x, &rock[i].y, sHeight, sWidth);

            drawAsteroid(rock, i);

            // collision checking with bullet
            for (int j = 0; j < maxPew; j++)
            {
                // giving the asteroid an 36x36 hitbox
                // TODO change to get the hitbox from rock width and height
                if (pew[j].active &&
                    pew[j].x >= rock[i].x - 18 && pew[j].x <= rock[i].x + 18 &&
                    pew[j].y >= rock[i].y - 18 && pew[j].y <= rock[i].y + 18)
                {
                    // delete bullet, hit asteroid
                    // spawn a new asteroid
                    // add score
                    // TODO: split asteroid into 2 smaller asteroids
                    resetBullet(pew, j);

                    // split asteroid, but if split respawn a new big asteroid
                    if (rock[i].w != 20)
                    {
                        // spawn 2 small asteroids where the big one was split
                        splitAsteroid(rock, i, sHeight, sWidth);
                        splitAsteroid(rock, i, sHeight, sWidth);
                    }
                    else
                    {
                        if (asteroidCount < 10)
                        {
                            randomAsteroidSpawn(rock, sHeight, sWidth);
                        }
                        // spawnAsteroid(rock, 40, sHeight, sWidth);
                    }

                    resetAsteroid(rock, i);

                    score++;

                    break;
                }
            }
        }
    }
}

void playerCollision(Triangle* tri, Asteroid* rock, short int asteroidCount, short int maxAst, short int sHeight, short int sWidth)
{
    // collision checking
    for (int i = 0; i < maxAst; i++)
    {
        // giving the asteroid an 36x36 hitbox
        // TODO change to get the hitbox from rock width and height
        if (rock[i].active &&
            tri->x >= rock[i].x - tri->w + 2 && tri->x <= rock[i].x + tri->w - 2 &&
            tri->y >= rock[i].y - tri->h + 2 && tri->y <= rock[i].y + tri->h - 2)
        {
            resetAsteroid(rock, i);
            playerHealthCheck(tri);
            // spawnAsteroid(rock, 40, sHeight, sWidth);
            // spawn a random asteroid if there are less than 10 big asteroids
            if (asteroidCount < 10)
            {
                randomAsteroidSpawn(rock, sHeight, sWidth);
            }

            break;
        }
    }
}