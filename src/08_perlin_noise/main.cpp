#include "raylib.h"
#include <cmath>
#include <vector>
#include <iostream>

// ================================
// Simple 2D Perlin Noise Function
// ================================
float fade(float t) {
    // Smoothstep function for interpolation
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float a, float b, float t) {
    // Linear interpolation
    return a + t * (b - a);
}

// Gradient function
float grad(int hash, float x, float y) {
    switch (hash & 3) {
        case 0: return  x + y;
        case 1: return -x + y;
        case 2: return  x - y;
        case 3: return -x - y;
    }
    return 0; // never happens
}

// 2D Perlin Noise function
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

// Generate permutation table for Perlin noise
std::vector<int> generatePermutation(unsigned int seed = 0) {
    std::vector<int> p(256);
    for (int i = 0; i < 256; i++) p[i] = i;

    // Shuffle permutation using seed
    srand(seed);
    for (int i = 255; i > 0; --i) {
        int j = rand() % (i + 1);
        std::swap(p[i], p[j]);
    }

    // Duplicate table for easy indexing
    p.insert(p.end(), p.begin(), p.end());
    return p;
}

// Map noise value from [-1,1] to [0,255] for color
unsigned char noiseToColor(float n) {
    return (unsigned char)((n + 1.0f) * 0.5f * 255);
}

// ===================================
// Main Program
// ===================================
int main() {
    // Window dimensions
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "2D Perlin Noise Viewer");

    // Generate permutation table for Perlin noise
    std::vector<int> perm = generatePermutation(1234); // seed = 1234

    // =========================
    // Off-screen texture to draw noise once
    // =========================
    RenderTexture2D noiseTexture = LoadRenderTexture(screenWidth, screenHeight);

    // Noise parameters
    float zoom = 100.0f;  // Controls the scale of noise
    float offsetX = 0.0f; // Panning X
    float offsetY = 0.0f; // Panning Y

    // Generate initial noise
    BeginTextureMode(noiseTexture);
    for (int y = 0; y < screenHeight; y++) {
        for (int x = 0; x < screenWidth; x++) {
            float nx = (x + offsetX) / zoom;
            float ny = (y + offsetY) / zoom;
            float n = perlin2D(nx, ny, perm);
            unsigned char c = noiseToColor(n);
            DrawPixel(x, y, Color{ c, c, c, 255 });
        }
    }
    EndTextureMode();

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // =========================
        // Input: Zooming
        // =========================
        if (IsKeyDown(KEY_W)) zoom *= 1.02f; // zoom in
        if (IsKeyDown(KEY_S)) zoom /= 1.02f; // zoom out

        // =========================
        // Input: Panning
        // =========================
        if (IsKeyDown(KEY_UP)) offsetY -= 5;
        if (IsKeyDown(KEY_DOWN)) offsetY += 5;
        if (IsKeyDown(KEY_LEFT)) offsetX -= 5;
        if (IsKeyDown(KEY_RIGHT)) offsetX += 5;

        // =========================
        // Redraw noise only if zoom/pan changed
        // =========================
        BeginTextureMode(noiseTexture);
        for (int y = 0; y < screenHeight; y++) {
            for (int x = 0; x < screenWidth; x++) {
                float nx = (x + offsetX) / zoom;
                float ny = (y + offsetY) / zoom;
                float n = perlin2D(nx, ny, perm);
                unsigned char c = noiseToColor(n);
                DrawPixel(x, y, Color{ c, c, c, 255 });
            }
        }
        EndTextureMode();

        // =========================
        // Draw the texture to screen
        // =========================
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTextureRec(noiseTexture.texture, Rectangle{ 0, 0, (float)screenWidth, (float)-screenHeight }, Vector2{0,0}, WHITE);
        DrawText("W/S = Zoom In/Out | Arrows = Pan", 10, 10, 20, RED);
        EndDrawing();
    }

    // Cleanup
    UnloadRenderTexture(noiseTexture);
    CloseWindow();

    return 0;
}
