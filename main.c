#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <pspkernel.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspdebug.h>

#include "callback.h"
#include "gu.h"
#include "config.h"

#include "vertex.h"
#include "heart.h"


PSP_MODULE_INFO("shape", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);

#define MAX_BULLETS 20
#define MAX_AST 20
#define MAX_HP 5

#define printf pspDebugScreenPrintf // don't need stdlib anyway

short int gameState = 1;
int score = 0;

// loop around the screen if at the edges
void handleArea(float *x, float *y, short int sHeight, short int sWidth)
{
    if (*x <= -22.f)
    {
        *x = 480.f;
    }
    else if (*x >= (float)sWidth)
    {
        *x = 0.f;
    }
    else if (*y <= -36.f)
    {
        *y = 272.f;
    }
    else if (*y >= (float)sHeight)
    {
        *y = 0.f;
    }
}

void handleSpeed(float *accx, float *accy)
{
    float maxAcc = 155.0f; // Maximum allowed acceleration
    if (*accx > maxAcc) *accx = maxAcc;
    if (*accx < maxAcc - 128) *accx = maxAcc - 128; //equalize positive and negative acceleration
    if (*accy > maxAcc) *accy = maxAcc;
    if (*accy < maxAcc - 128) *accy = maxAcc - 128;
}

// typedef struct Vertex
// {
//     unsigned short u, v;
//     short x, y, z;
// } Vertex;

typedef struct Triangle
{
    float x, y;
    float w, h;
    float angle;
    short int health;

} Triangle;

void drawTriangle(Triangle* t)
{
    // allocate enough memory for 4 verticles
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
        gameState = 0;
        player->health = 0;
    }
}


typedef struct Bullet
{
    float x, y;
    float angle;
    float speed;
    char active;

} Bullet;

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

typedef struct Asteroid
{
    float x, y;
    float w, h;
    float angle;
    float velx;
    float vely;
    // short int id;
    char active;
} Asteroid;

void drawAsteroid(Asteroid* r, int rIndex)
{
    Asteroid* rock = &r[rIndex];

    // allocate enough memory for 11 verticles for a nice looking asteroid
    Vertex* verts = (Vertex*)sceGuGetMemory(12 * sizeof(Vertex));

    // Vertex* verts = asteroidVerts[a->id];

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

void initGame(Asteroid* rock, short int sHeight, short int sWidth)
{
    for (int i = 0; i < 3; i++)
    {
        // spawnAsteroid(rock, 40, sHeight, sWidth);
        randomAsteroidSpawn(rock, sHeight, sWidth);
    }

    score = 0;
}

void initPlayer(Triangle* player, short int maxHP)
{
    player->x = 240;
    player->y = 136;
    player->w = 20;
    player->h = 34;
    player->angle = 0;
    player->health = maxHP;
}

// typedef struct Heart
// {
//     float x, y;
//     float w, h;

// } Heart;

// void drawHeart(Heart* h, int i)
// {
//     Heart* heart = &h[i];

//     Vertex* verts = (Vertex*)sceGuGetMemory(9 * sizeof(Vertex));

//     verts[0].x = heart->x + heart->w / 2;
//     verts[0].y = heart->y + heart->h;

//     verts[1].x = heart->x + heart->w * 0.75;
//     verts[1].y = heart->y + heart->h * 0.65;

//     verts[2].x = heart->x + heart->w;
//     verts[2].y = heart->y + heart->h * 0.25;

//     verts[3].x = heart->x + heart->w * 0.75;
//     verts[3].y = heart->y;

//     verts[4].x = heart->x + heart->w / 2;
//     verts[4].y = heart->y + heart->h * 0.25;

//     verts[5].x = heart->x + heart->w * 0.25;
//     verts[5].y = heart->y;

//     verts[6].x = heart->x;
//     verts[6].y = heart->y + heart->h * 0.25;

//     verts[7].x = heart->x + heart->w * 0.25;
//     verts[7].y = heart->y + heart->h * 0.65;

//     verts[8].x = heart->x + heart->w / 2;
//     verts[8].y = heart->y + heart->h;

//     sceGuColor(0xFFFFFFFF); // Red, colors are ABGR
//     sceGuDrawArray(GU_LINE_STRIP, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 9, 0, verts);
// }

// void checkHearts(Triangle* player, Heart* heart)
// {
//     short int hp = player->health;

//     for (int i = 0; i < hp; i++)
//     {
//         drawHeart(heart, i);
//     }
// }

// void initHearts(Heart* heart, short int maxHP)
// {
//     short int hX = 15;
//     short int hY = 244;

//     for (int i = 0; i < maxHP; i++)
//     {
//         heart[i].x = hX;
//         heart[i].y = hY;
//         heart[i].w = 16;
//         heart[i].h = 14;

//         // create a 10 pixel space between hearts
//         hX += heart[i].w + 10;
//     }
// }

int main()
{
    // Make exiting with the home button possible
    setupExitCallback();

    SceCtrlData pad;
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    pspDebugScreenInit();

    // spawn player at the center of the screen
    Triangle player = { 0 };
    Asteroid rock[MAX_AST] = { 0 };
    Bullet pew[MAX_BULLETS] = { 0 };
    Heart heart[MAX_HP] = { 0 };


    initAsteroid(rock, MAX_AST);
    initBullet(pew, MAX_BULLETS);
    initPlayer(&player, MAX_HP);
    initHearts(heart, MAX_HP);

    initGame(rock, SCREEN_HEIGHT, SCREEN_WIDTH);

    // Setup the library used for rendering
    initGu();

    srandom(time(NULL));

    // default, not moving
    float accx = 128.f;
    float accy = 128.f;
    float velx = 0.f, vely = 0.f;

    short int pewTimer = 0;
    short int asteroidTimer = 200;
    short int activeAsteroid = 0;

    gameState = 1; // just in case, also for clarity

    while (isRunning())
    {
        if (gameState)
        {
            startFrame();

            pspDebugScreenSetXY(0, 2);
            sceCtrlReadBufferPositive(&pad, 1);

            accx -= (pad.Lx  - 128) / 50;
            accy -= (pad.Ly  - 128) / 50;

            velx = (accx - pad.Lx) / 80; // immitate acceleration and no gravity
            vely = (accy - pad.Ly) / 80;

            player.x -= velx;
            player.y -= vely;

            handleSpeed(&accx, &accy);

            handleArea(&player.x, &player.y, SCREEN_HEIGHT, SCREEN_WIDTH);

            updateBullets(pew, MAX_BULLETS, SCREEN_HEIGHT, SCREEN_WIDTH);

            if (pad.Buttons != 0)
            {
                if (pad.Buttons & PSP_CTRL_LTRIGGER)
                {
                    player.angle -= 0.06f;
                }

                if (pad.Buttons & PSP_CTRL_RTRIGGER)
                {
                    player.angle += 0.06f;
                }

                //make rotation a little nicer
                if (player.angle < 0) player.angle += 2 * M_PI;
                if (player.angle >= 2 * M_PI) player.angle -= 2 * M_PI;

                if (pad.Buttons & PSP_CTRL_CROSS)
                {
                    if (!pewTimer)
                    {
                        for (int i = 0; i < MAX_BULLETS; i++)
                        {
                            if (!pew[i].active)
                            {
                                float peakx, peaky;
                                getTriPeak(&player, &peakx, &peaky);

                                // Spawn a new bullet
                                pew[i].x = peakx;
                                pew[i].y = peaky;
                                pew[i].angle = player.angle + (90.f * M_PI / 180.f);
                                pew[i].speed = 8.0f; // Set bullet speed
                                pew[i].active = 1;   // Mark as active
                                pewTimer = 15;
                                break;
                            }
                        }
                    }
                }
            }

            if (pewTimer > 0)
            {
                pewTimer--;
            }

            activeAsteroid = 0;
            for (int i = 0; i < MAX_AST; i++)
            {
                if (rock[i].active == 1 && rock[i].w == 40)
                {
                    activeAsteroid++;
                }
            }

            updateAsteroid(rock, pew, activeAsteroid, MAX_AST, MAX_BULLETS, SCREEN_HEIGHT, SCREEN_WIDTH);
            playerCollision(&player, rock, activeAsteroid, MAX_AST, SCREEN_HEIGHT, SCREEN_WIDTH);
            checkHearts(&player, heart);

            drawTriangle(&player);


            // periodically spawn asteroids
            // ! make this a better system ts sucks rn
            if (asteroidTimer > 0)
            {
                asteroidTimer--;
            }
            else
            {
                if (activeAsteroid < 10)
                {
                    // spawnAsteroid(rock, 40, SCREEN_HEIGHT, SCREEN_WIDTH);
                    randomAsteroidSpawn(rock, SCREEN_HEIGHT, SCREEN_WIDTH);
                    asteroidTimer = 500;
                }
            }

            endFrame();

            // printf("health = %hd\n", player.health);
            printf("score = %d\n", score);

            // for (int x = 0; x < MAX_AST; x++)
            // {
            //     if (rock[x].active)
            //     {
            //         printf("rock %d pos x = %.4f y = %.4f w = %.1f\n", x, rock[x].x, rock[x].y, rock[x].w);
            //     }
            // }

            // size_t freeMem = sceKernelTotalFreeMemSize();
            // printf("Free memory: %zu\n", freeMem);
        }
        else
        {
            startFrame();

            pspDebugScreenSetXY(0, 2);
            sceCtrlReadBufferPositive(&pad, 1);

            printf("Game Over! press circle to restart\n");
            if (pad.Buttons != 0)
            {
                if (pad.Buttons & PSP_CTRL_CIRCLE)
                {
                    gameState = 1;
                    initAsteroid(rock, MAX_AST);
                    initBullet(pew, MAX_BULLETS);
                    initPlayer(&player, MAX_HP);
                    initHearts(heart, MAX_HP);
                    initGame(rock, SCREEN_HEIGHT, SCREEN_WIDTH);
                }
            }

            endFrame();
        }
    }

    return 0;
}
