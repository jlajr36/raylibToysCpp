#pragma once

// Compute 2D Perlin noise at (x, y)
// Optional seed for reproducible patterns
float perlin2D(float x, float y, unsigned int seed = 0);