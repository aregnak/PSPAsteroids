// C lib includes
#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <limits.h>

// PSPSDK includes
#include <pspkernel.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <pspaudio.h>
#include <pspaudiolib.h>

#include "../common/callback.h"
#include "../common/config.h"
#include "../common/extra.h"
#include "../graphics/gu.h"

// #include "audio/sound.h"
#include "entities/asteroid.h"
#include "entities/bullet.h"
#include "entities/heart.h"
#include "entities/triangle.h"
#include "game/game.h"
#include "input/input.h"
#include "text/text.h"

PSP_MODULE_INFO("Asteroids", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);

#define printf pspDebugScreenPrintf // don't need stdlib anyway
#define WHITE 0xFFFFFFFF // For easy

int score = 0;
GameState gameState = MAIN_MENU;

int main()
{
    // Make exiting with the home button possible
    setupExitCallback();

    // Keeping these here instead of input.h for simplicity
    SceCtrlData pad;
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    pspDebugScreenInit();

    // pspAudioInit();
    // pspAudioSetChannelCallback(0, audioCallback, NULL);

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

            accx -= (pad.Lx  - 128.f) / 50;
            accy -= (pad.Ly  - 128.f) / 50;

            velx = (accx - pad.Lx) / 80; // immitate acceleration and no gravity
            vely = (accy - pad.Ly) / 80;

            player.x -= velx;
            player.y -= vely;

            handleSpeed(&accx, &accy);

            handleArea(&player.x, &player.y, SCREEN_HEIGHT, SCREEN_WIDTH);

            // Player inputs
            if (pad.Buttons != 0)
            {
                handlePlayerInput(&player, pew, pewTimer, pad.Buttons);
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

            disableBlend();
            updateBullets(pew, MAX_BULLETS, SCREEN_HEIGHT, SCREEN_WIDTH);
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

            // text rendering needs to be before endframe()
            enableBlend();

            char buffer[32];
            strcpy(buffer, "Score: ");
            appendIntToBuffer(buffer, sizeof(buffer), score);

            drawString(buffer, 10, 10, 0xFFFFFFFF, 0);

            endFrame();

            // ----------------------------
            // Old debug printf statements
            // ----------------------------
            // printf("health = %hd\n", player.health);
            //printf("score = %d\n", score);

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

        else if (gameState == GAME_OVER)
        {
            startFrame();

            pspDebugScreenSetXY(0, 2);
            sceCtrlReadBufferPositive(&pad, 1);

            // TODO: a better way of centering to not pass the same string twice
            drawString("Game Over!", centerText("Game Over!"), 120, WHITE, 0);
            drawString("Press O to restart.", centerText("Press O to restart."), 130, WHITE, 0);

            // Printing final score after death
            char buffer[32];
            strcpy(buffer, "Your score was only ");
            appendIntToBuffer(buffer, sizeof(buffer), score);
            drawString(buffer, centerText(buffer), 140, WHITE, 0);

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
