#include "raylib.h"
#include "Perlin2D.h"
#include <vector>
#include <iostream>

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "2D Perlin Noise Viewer");

    std::vector<int> perm = generatePermutation(1234);

    RenderTexture2D noiseTexture = LoadRenderTexture(screenWidth, screenHeight);

    float zoom = 100.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;

    // Initial draw
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
        if (IsKeyDown(KEY_W)) zoom *= 1.02f;
        if (IsKeyDown(KEY_S)) zoom /= 1.02f;
        if (IsKeyDown(KEY_UP)) offsetY -= 5;
        if (IsKeyDown(KEY_DOWN)) offsetY += 5;
        if (IsKeyDown(KEY_LEFT)) offsetX -= 5;
        if (IsKeyDown(KEY_RIGHT)) offsetX += 5;

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

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTextureRec(noiseTexture.texture, Rectangle{0,0,(float)screenWidth,(float)-screenHeight}, Vector2{0,0}, WHITE);
        DrawText("W/S = Zoom | Arrows = Pan", 10, 10, 20, RED);
        EndDrawing();
    }

    UnloadRenderTexture(noiseTexture);
    CloseWindow();
    return 0;
}
