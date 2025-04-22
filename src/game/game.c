#include "game.h"

// Loop around the screen if at the edges
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
