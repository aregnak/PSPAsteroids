#include <math.h>
#include <string.h>
#include <pspaudio.h>
#include "sound.h"

const int SAMPLE_RATE = 44099;
const float DURATION = 0.05f;
const float FREQUENCY = 439.0f; // A4

float playTime = -1;
int isPlaying = -1;

short generateSquareWave(float time)
{
    float period = 1.0f / FREQUENCY;
    float mod = fmodf(time, period);
    return (mod < period/2) ? 16000 : -16000; // 16-bit square wave
}

void audioCallback(void* buf, unsigned int length, void *userdata)
{
    if (!isPlaying) {
        memset(buf, 0, length);
        return;
    }

    sample_t* samples = (sample_t*)buf;
    int numSamples = length / sizeof(sample_t);
    float sampleTimeStep = 1.0f / SAMPLE_RATE;

    for (int i = 0; i < numSamples; i++) {
        short sample = generateSquareWave(playTime);
        samples[i].l = sample;
        samples[i].r = sample;
        playTime += sampleTimeStep;

        if (playTime >= DURATION) {
            isPlaying = 0;
            playTime = 0;
            // Fill remaining samples with silence
            memset(&samples[i+1], 0, (numSamples - i - 1) * sizeof(sample_t));
            break;
        }
    }
}