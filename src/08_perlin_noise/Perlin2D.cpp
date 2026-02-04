#include "Perlin2D.h"
#include <cmath>
#include <cstdlib> // for srand, rand
#include <algorithm> // for std::swap

// Smoothstep function for interpolation
static float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

// Linear interpolation
static float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

// Gradient function
static float grad(int hash, float x, float y) {
    switch (hash & 3) {
        case 0: return  x + y;
        case 1: return -x + y;
        case 2: return  x - y;
        case 3: return -x - y;
    }
    return 0; // should never happen
}

// ================================
// Public Functions
// ================================

std::vector<int> generatePermutation(unsigned int seed) {
    std::vector<int> p(256);
    for (int i = 0; i < 256; i++) p[i] = i;

    srand(seed);
    for (int i = 255; i > 0; --i) {
        int j = rand() % (i + 1);
        std::swap(p[i], p[j]);
    }

    // Duplicate for overflow
    p.insert(p.end(), p.begin(), p.end());
    return p;
}

float perlin2D(float x, float y, const std::vector<int>& perm) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;

    float xf = x - floor(x);
    float yf = y - floor(y);

    float u = fade(xf);
    float v = fade(yf);

    int aa = perm[X + perm[Y]];
    int ab = perm[X + perm[Y + 1]];
    int ba = perm[X + 1 + perm[Y]];
    int bb = perm[X + 1 + perm[Y + 1]];

    float x1 = lerp(grad(aa, xf, yf), grad(ba, xf - 1, yf), u);
    float x2 = lerp(grad(ab, xf, yf - 1), grad(bb, xf - 1, yf - 1), u);

    return lerp(x1, x2, v);
}

unsigned char noiseToColor(float n) {
    return (unsigned char)((n + 1.0f) * 0.5f * 255);
}