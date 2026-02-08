#include "raylib.h"
#include "Terrain.h"
#include "Camera.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "2D Terrain Modular");

    // Load custom font
    Font myFont = LoadFont("assets/Roboto.ttf");

    RenderTexture2D terrainTexture = LoadRenderTexture(screenWidth, screenHeight);
    Terrain terrain(screenWidth, screenHeight, 1234);
    CameraController camera;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        camera.Update(GetFrameTime());
        terrain.Generate(camera.offsetX, camera.offsetY, camera.zoom);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw terrain to render texture
        terrain.Draw(terrainTexture.texture);
        DrawTextureRec(terrainTexture.texture, (Rectangle){0,0,(float)screenWidth,(float)screenHeight}, (Vector2){0,0}, WHITE);

        // Draw UI text using custom font
        DrawTextEx(myFont,
                   "W/S = Zoom | Arrows = Pan | Mouse Wheel = Zoom",
                   (Vector2){10, 10},   // Position
                   20,                  // Font size
                   2,                   // Letter spacing
                   BLACK);              // Color

        EndDrawing();
    }

    // Unload resources
    UnloadFont(myFont);
    UnloadRenderTexture(terrainTexture);
    CloseWindow();

    return 0;
}
