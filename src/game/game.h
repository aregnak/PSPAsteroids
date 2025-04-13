#ifndef GAME_H
#define GAME_H

typedef enum {
    GAME_RUNNING,
    GAME_OVER
} GameState;

extern GameState gameState;

#endif // GAME_H