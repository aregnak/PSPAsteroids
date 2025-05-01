#include <pspctrl.h>
#include <math.h>
#include "input.h"

void handlePlayerInput(Triangle* player, Bullet* pew, short int pewTimer, unsigned int button)
{
    if (button & PSP_CTRL_LTRIGGER)
    {
        player->angle -= 0.06f;
    }

    if (button & PSP_CTRL_RTRIGGER)
    {
        player->angle += 0.06f;
    }

    //make rotation a little nicer
    if (player->angle < 0) player->angle += 2 * M_PI;
    if (player->angle >= 2 * M_PI) player->angle -= 2 * M_PI;

    if (button & PSP_CTRL_CROSS)
    {
        if (!pewTimer)
        {
            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (!pew[i].active)
                {
                    float peakx, peaky;
                    getTriPeak(player, &peakx, &peaky);

                    // Spawn a new bullet
                    pew[i].x = peakx;
                    pew[i].y = peaky;
                    pew[i].angle = player->angle + (90.f * M_PI / 180.f);
                    pew[i].speed = 8.0f; // Set bullet speed
                    pew[i].active = 1;   // Mark as active
                    pewTimer = 15;
                    break;
                }
            }
        }
    }

}