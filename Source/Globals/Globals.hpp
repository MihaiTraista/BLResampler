#pragma once

#ifndef M_PI
#define M_PI (3.1415926535897932)
#endif

#define SAMPLE_RATE (44100)
#define TWOPI (2.0 * M_PI)

extern int WTSIZE;
extern float WTSIZE_OVER_SAMPLE_RATE;

#define N_WT_BANDS (10)
#define N_VOICES (8)
#define BASE_FREQ_FOR_BLW (25)
#define N_STACK_LAYERS (4)

// 10 times 25.0 * pow(2, i)
const float baseFrequencies[10] = {25.0, 50.0, 100.0, 200.0, 400.0, 800.0, 1600.0, 3200.0, 6400.0, 12800.0};
