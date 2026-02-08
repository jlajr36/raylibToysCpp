#pragma once
#include "Perlin2D.h"

// Fractal Perlin wrapper
inline float fractalPerlin(float x, float y, unsigned int seed, int octaves=4, float persistence=0.5f) {
    float total = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float maxValue = 0.0f;

    for (int i=0; i<octaves; i++){
        total += perlin2D(x*frequency, y*frequency, seed)*amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0f;
    }

    return total / maxValue; // [-1,1]
}
