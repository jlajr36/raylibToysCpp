#include "raylib.h"
#include <iostream>

using namespace std;

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "raylib");
    Vector2 ballPosition = { (float)screenWidth/2, (float)screenHeight/2};
    SetTargetFPS(60);
    while(!WindowShouldClose()) {
        if (IsKeyDown(KEY_RIGHT)) ballPosition.x += 2.0f;
        if (IsKeyDown(KEY_LEFT)) ballPosition.x -= 2.0f;
        if (IsKeyDown(KEY_UP)) ballPosition.y -= 2.0f;
        if (IsKeyDown(KEY_DOWN)) ballPosition.y += 2.0f;
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangleGradientV(0, 0, screenWidth, screenHeight, RAYWHITE, LIGHTGRAY);
        DrawText("Move the ball with arrow keys", 10, 10, 20, DARKGRAY);
        DrawCircleV(ballPosition, 55, Fade(MAROON, 0.3f));
        DrawCircleV(ballPosition, 50, MAROON);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
