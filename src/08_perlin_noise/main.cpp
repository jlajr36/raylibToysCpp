#include "raylib.h"
#include "Terrain.h"
#include "Camera.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "2D Terrain Modular");

    RenderTexture2D terrainTexture = LoadRenderTexture(screenWidth, screenHeight);
    Terrain terrain(screenWidth, screenHeight, 1234);
    CameraController camera;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        camera.Update(GetFrameTime());
        terrain.Generate(camera.offsetX, camera.offsetY, camera.zoom);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        terrain.Draw(terrainTexture.texture); // ✅ Pass inner Texture2D
        DrawTextureRec(terrainTexture.texture, (Rectangle){0,0,(float)screenWidth,(float)screenHeight}, {0,0}, WHITE);
        DrawText("W/S = Zoom | Arrows = Pan | Mouse Wheel = Zoom", 10,10,20,RED);
        EndDrawing();
    }

    UnloadRenderTexture(terrainTexture);
    CloseWindow();
}
