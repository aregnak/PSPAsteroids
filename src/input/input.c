#include <pspctrl.h>
#include <math.h>
#include "input.h"

void handlePlayerInput(Triangle* player, Bullet* pew, short int* pewTimer, unsigned int button)
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
            shootBullet(player, pew);
            *pewTimer = 15;
        }
    }

}