#ifndef SOUND_H
#define SOUND_H

extern const int SAMPLE_RATE;
extern const float DURATION;
extern const float FREQUENCY; // A4

extern float playTime;
extern int isPlaying;

typedef struct {
    short l, r;
} sample_t;

short generateSquareWave(float time);
void audioCallback(void* buf, unsigned int length, void *userdata);

#endif // SOUND_H