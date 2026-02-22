#include "raylib.h"
#include <cmath>

using namespace std;

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "IK Robot Simulator");

    float cameraAngle = 45.0f;
    const float radius = 8.0f;
    const float cameraHeight = 4.0f;
    const float rotationSpeed = 60.0f;

    Camera camera = {0};
    camera.target = { 0.0f, 1.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 35.0f;

    SetTargetFPS(60);

    while(!WindowShouldClose()) {

        if (IsKeyDown(KEY_RIGHT)) cameraAngle += rotationSpeed * GetFrameTime();
        if (IsKeyDown(KEY_LEFT))  cameraAngle -= rotationSpeed * GetFrameTime();

        float rad = cameraAngle * (PI / 180.0f);

        camera.position.x = radius * sin(rad);
        camera.position.y = cameraHeight;
        camera.position.z = radius * cos(rad);

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(camera);

            DrawGrid(20, 1.0f);
        
        EndMode3D();

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
