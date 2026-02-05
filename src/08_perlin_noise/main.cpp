#include "raylib.h"
#include "Perlin2D.h"
#include <iostream>

// Map Perlin noise [-1, 1] to grayscale [0, 255]
unsigned char noiseToColor(float n) {
    return static_cast<unsigned char>((n + 1.0f) * 0.5f * 255);
}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "2D Perlin Noise Viewer");

    RenderTexture2D noiseTexture = LoadRenderTexture(screenWidth, screenHeight);

    float zoom = 100.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;

    const unsigned int seed = 1234; // optional; use perlin2D(nx, ny) if you don't care

    SetTargetFPS(60);

    bool needsRedraw = true; // only redraw texture when zoom or offset changes

    while (!WindowShouldClose()) {

        // Handle input
        if (IsKeyDown(KEY_W)) { zoom *= 1.02f; needsRedraw = true; }
        if (IsKeyDown(KEY_S)) { zoom /= 1.02f; needsRedraw = true; }
        if (IsKeyDown(KEY_UP)) { offsetY -= 5; needsRedraw = true; }
        if (IsKeyDown(KEY_DOWN)) { offsetY += 5; needsRedraw = true; }
        if (IsKeyDown(KEY_LEFT)) { offsetX -= 5; needsRedraw = true; }
        if (IsKeyDown(KEY_RIGHT)) { offsetX += 5; needsRedraw = true; }

        // Redraw texture only if necessary
        if (needsRedraw) {
            BeginTextureMode(noiseTexture);
            ClearBackground(RAYWHITE);

            for (int y = 0; y < screenHeight; y++) {
                for (int x = 0; x < screenWidth; x++) {
                    float nx = (x + offsetX) / zoom;
                    float ny = (y + offsetY) / zoom;
                    float n = perlin2D(nx, ny, seed); // optional seed
                    unsigned char c = noiseToColor(n);
                    DrawPixel(x, y, Color{ c, c, c, 255 });
                }
            }

            EndTextureMode();
            needsRedraw = false;
        }

        // Draw to screen
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTextureRec(
            noiseTexture.texture,
            Rectangle{0, 0, (float)screenWidth, (float)-screenHeight}, // flip Y
            Vector2{0, 0},
            WHITE
        );

        DrawText("W/S = Zoom | Arrows = Pan", 10, 10, 20, RED);
        EndDrawing();
    }

    UnloadRenderTexture(noiseTexture);
    CloseWindow();
    return 0;
}
