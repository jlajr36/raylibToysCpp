#include "raylib.h"
#include <iostream>

using namespace std;

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "IK Robot Simulator");

    Camera camera = {
        { 8.0f, 4.0f, 8.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        35.0f,
        CAMERA_PERSPECTIVE
    };

    SetTargetFPS(60);

    while(!WindowShouldClose()) {

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            UpdateCamera(&camera, CAMERA_FREE);
        }

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
