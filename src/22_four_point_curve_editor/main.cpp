#include "raylib.h"

bool IsMouseNearPoint(Vector2 p, float radius) {
    return CheckCollisionPointCircle(GetMousePosition(), p, radius);
}

int main()
{
    const int screenWidth = 1200;
    const int screenHeight = 800;

    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "4-Point Curve Editor");
    SetTargetFPS(60);

    const float radius = 8.0f;
    Vector2 mousePos = {0, 0};
    int selected = -1;

    // Point start positions
    Vector2 p0 = {400, 600};
    Vector2 p1 = {500, 200};
    Vector2 p2 = {800, 200};
    Vector2 p3 = {900, 600};

    while (!WindowShouldClose())
    {
        // ****** Logic ******
        mousePos = GetMousePosition();

        // Select point
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (IsMouseNearPoint(p0, radius)) selected = 0;
            else if (IsMouseNearPoint(p1, radius)) selected = 1;
            else if (IsMouseNearPoint(p2, radius)) selected = 2;
            else if (IsMouseNearPoint(p3, radius)) selected = 3;
        }

        // Drag point
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && selected != -1) {
            if (selected == 0) p0 = mousePos;
            if (selected == 1) p1 = mousePos;
            if (selected == 2) p2 = mousePos;
            if (selected == 3) p3 = mousePos;
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            selected = -1;
        }

        // ****** Render ******
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText(
            TextFormat("Mouse: (%.0f, %.0f)", mousePos.x, mousePos.y),
            10, 10, 20,
            DARKGRAY
        );
        DrawCircleV(p0, radius, RED);
        DrawCircleV(p1, radius, GREEN);
        DrawCircleV(p2, radius, GREEN);
        DrawCircleV(p3, radius, RED);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
