#include "raylib.h"
#include <cmath>
#include <iostream>

using namespace std;

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Orbit Trail");

    SetTargetFPS(60);

    const Vector2 center = { screenWidth/2.0f, screenHeight/2.0f };
    const float radius = 150.0f;
    const float speed = 1.0f;
    float angle = 0.0f;

    while (!WindowShouldClose())
    {
        angle += speed * GetFrameTime();
        if (angle > 2 * PI) angle -= 2 * PI;
        Vector2 pos = {
            center.x + radius * cos(angle),
            center.y + radius * sin(angle)
        };
        BeginDrawing();
            ClearBackground(BLACK);
            DrawCircleV(center, 5, RED);
            DrawCircleLinesV(center, radius, GREEN);
            DrawCircleV(pos, 20, MAROON);
        
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
