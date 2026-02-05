#include "Perlin2D.h"
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cmath>

namespace {
    // Helpers hidden from public interface
    float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
    float lerp(float a, float b, float t) { return a + t * (b - a); }
    float grad(int hash, float x, float y) {
        switch (hash & 3) {
            case 0: return x + y;
            case 1: return -x + y;
            case 2: return x - y;
            case 3: return -x - y;
        }
        return 0;
    }

    std::vector<int> generatePermutation(unsigned int seed) {
        std::vector<int> p(256);
        for (int i = 0; i < 256; i++) p[i] = i;

        srand(seed);
        for (int i = 255; i > 0; --i) {
            int j = rand() % (i + 1);
            std::swap(p[i], p[j]);
        }

        p.insert(p.end(), p.begin(), p.end());
        return p;
    }
}

float perlin2D(float x, float y, unsigned int seed) {
    // Static cache: keeps the last permutation table per seed
    static std::vector<int> perm;
    static unsigned int lastSeed = 0;

    if (perm.empty() || seed != lastSeed) {
        perm = generatePermutation(seed);
        lastSeed = seed;
    }

    int X = static_cast<int>(floor(x)) & 255;
    int Y = static_cast<int>(floor(y)) & 255;
    float xf = x - floor(x);
    float yf = y - floor(y);
    float u = fade(xf);
    float v = fade(yf);

    int aa = perm[perm[X] + Y];
    int ab = perm[perm[X] + Y + 1];
    int ba = perm[perm[X + 1] + Y];
    int bb = perm[perm[X + 1] + Y + 1];

    float x1 = lerp(grad(aa, xf, yf), grad(ba, xf - 1, yf), u);
    float x2 = lerp(grad(ab, xf, yf - 1), grad(bb, xf - 1, yf - 1), u);
    return lerp(x1, x2, v);
}
