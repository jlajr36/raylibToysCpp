#include "raylib.h"

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "4-Point Curve Editor");
    SetTargetFPS(60);

    Vector2 mousePos = {0, 0};

    while (!WindowShouldClose())
    {
        mousePos = GetMousePosition();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText(
            TextFormat("Mouse: (%.0f, %.0f)", mousePos.x, mousePos.y),
            10, 10, 20,
            DARKGRAY
        );

        EndDrawing();
    }

    CloseWindow();
    return 0;
}