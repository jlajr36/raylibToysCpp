#include "raylib.h"
#include "Perlin2D.h"
#include <cmath>
#include <vector>

// ===================== Helper Math =====================
float Clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float Lerp(float a, float b, float t) {
    return a + t * (b - a);
}

// ===================== Terrain =====================
struct TerrainType {
    float minHeight;
    float maxHeight;
    Color minColor;
    Color maxColor;
    float lerpAdjust;
};

float normalize(float value, float min, float max) {
    if (value < min) return 0.0f;
    if (value > max) return 1.0f;
    return (value - min) / (max - min);
}

Color lerpColor(Color a, Color b, float t) {
    t = Clamp(t, 0.0f, 1.0f);
    return Color{
        (unsigned char)Lerp(a.r, b.r, t),
        (unsigned char)Lerp(a.g, b.g, t),
        (unsigned char)Lerp(a.b, b.b, t),
        255
    };
}

Color getTerrainColor(float noise, const TerrainType& t) {
    float n = normalize(noise, t.minHeight, t.maxHeight);
    return lerpColor(t.minColor, t.maxColor, n + t.lerpAdjust);
}

// ===================== Fractal Perlin (multi-octave) =====================
float fractalPerlin(float x, float y, unsigned int seed, int octaves = 4, float persistence = 0.5f) {
    float total = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float maxValue = 0.0f;

    for (int i = 0; i < octaves; i++) {
        total += perlin2D(x * frequency, y * frequency, seed) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0f;
    }

    return total / maxValue; // may be in [-1,1]
}

// ===================== Main =====================
int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Smooth 2D Perlin Terrain");
    RenderTexture2D noiseTexture = LoadRenderTexture(screenWidth, screenHeight);

    float zoom = 150.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    const unsigned int seed = 1234;
    bool needsRedraw = true;

    // Terrain definitions (mapped to [0,1])
    TerrainType water  { 0.0f, 0.4f, Color{30,176,251,255}, Color{40,255,255,255},  0.0f };
    TerrainType sand   { 0.4f, 0.5f, Color{215,192,158,255}, Color{255,246,193,255}, 0.3f };
    TerrainType grass  { 0.5f, 0.7f, Color{  2,166,155,255}, Color{118,239,124,255}, 0.0f };
    TerrainType trees  { 0.7f, 1.0f, Color{ 22,181,141,255}, Color{ 10,145,113,255}, -0.3f };

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // ---- Input ----
        float panSpeed = 200.0f / zoom;

        if (IsKeyDown(KEY_LEFT))  offsetX -= panSpeed, needsRedraw = true;
        if (IsKeyDown(KEY_RIGHT)) offsetX += panSpeed, needsRedraw = true;
        if (IsKeyDown(KEY_UP))    offsetY -= panSpeed, needsRedraw = true;
        if (IsKeyDown(KEY_DOWN))  offsetY += panSpeed, needsRedraw = true;

        if (IsKeyDown(KEY_W)) { zoom *= 1.02f; needsRedraw = true; }
        if (IsKeyDown(KEY_S)) { zoom /= 1.02f; needsRedraw = true; }

        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            zoom *= (wheel > 0) ? 1.1f : 0.9f;
            zoom = Clamp(zoom, 50.0f, 800.0f);
            needsRedraw = true;
        }

        // ---- Redraw noise ----
        if (needsRedraw) {
            Image img = GenImageColor(screenWidth, screenHeight, BLANK);
            Color* pixels = (Color*)img.data;

            for (int y = 0; y < screenHeight; y++) {
                for (int x = 0; x < screenWidth; x++) {
                    float nx = (x - screenWidth * 0.5f + offsetX) / zoom;
                    float ny = (y - screenHeight * 0.5f + offsetY) / zoom;

                    // Fractal Perlin
                    float n = fractalPerlin(nx, ny, seed, 5, 0.5f);

                    // map [-1,1] → [0,1]
                    n = (n + 1.0f) * 0.5f;
                    n = Clamp(n, 0.0f, 1.0f);

                    // Terrain color
                    Color pixel;
                    if (n < water.maxHeight)
                        pixel = getTerrainColor(n, water);
                    else if (n < sand.maxHeight)
                        pixel = getTerrainColor(n, sand);
                    else if (n < grass.maxHeight)
                        pixel = getTerrainColor(n, grass);
                    else
                        pixel = getTerrainColor(n, trees);

                    pixels[y * screenWidth + x] = pixel;
                }
            }

            // Update texture (no negative height)
            UpdateTextureRec(noiseTexture.texture, (Rectangle){0,0,(float)screenWidth,(float)screenHeight}, pixels);
            UnloadImage(img);

            needsRedraw = false;
        }

        // ---- Draw to screen ----
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTextureRec(noiseTexture.texture, (Rectangle){0,0,(float)screenWidth,(float)screenHeight}, {0,0}, WHITE);
        DrawText("W/S = Zoom | Arrows = Pan | Mouse Wheel = Zoom", 10, 10, 20, RED);

        EndDrawing();
    }

    UnloadRenderTexture(noiseTexture);
    CloseWindow();
    return 0;
}

