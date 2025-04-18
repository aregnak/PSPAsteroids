#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>

#include <pspkernel.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <pspaudio.h>
#include <pspaudiolib.h>

#include "../common/callback.h"
#include "../common/config.h"
#include "../graphics/gu.h"

// even if the directories are in cmake,
// it is much clearer to have them listed in here
#include "audio/sound.h"
#include "entities/asteroid.h"
#include "entities/bullet.h"
#include "entities/heart.h"
#include "entities/triangle.h"
#include "game/game.h"

PSP_MODULE_INFO("shape", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);

#define printf pspDebugScreenPrintf // don't need stdlib anyway

int score = 0;
GameState gameState = GAME_RUNNING;

int main()
{
    // Make exiting with the home button possible
    setupExitCallback();

    SceCtrlData pad;
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    pspDebugScreenInit();

    pspAudioInit();
    pspAudioSetChannelCallback(0, audioCallback, NULL);


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

    while (isRunning())
    {
        if (gameState == GAME_RUNNING)
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
                        isPlaying = 1;
                        playTime = 0;   // audio testing

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
                    gameState = GAME_RUNNING;
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
