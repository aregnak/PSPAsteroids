#ifndef GAME_H
#define GAME_H

typedef enum {
    GAME_RUNNING,
    GAME_OVER,
} GameState;

void handleArea(float *x, float *y, short int sHeight, short int sWidth);
void handleSpeed(float *accx, float *accy);

extern GameState gameState;

#endif // GAME_H