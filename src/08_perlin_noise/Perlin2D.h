#pragma once
#include <vector>

// ================================
// 2D Perlin Noise
// ================================

// Generate permutation table (seed optional)
std::vector<int> generatePermutation(unsigned int seed = 0);

// Compute 2D Perlin noise at (x, y) using permutation table
float perlin2D(float x, float y, const std::vector<int>& perm);

// Map noise value from [-1,1] to [0,255] for color
unsigned char noiseToColor(float n);