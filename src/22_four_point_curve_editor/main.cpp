#include "raylib.h"

Vector2 CubicBezier(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, float t) {
    float u = 1.0f - t;
    float tt = t*t, uu = u*u, uuu = uu*u, ttt = tt*t;

    Vector2 p;
    p.x = uuu*p0.x + 3*uu*t*p1.x + 3*u*tt*p2.x + ttt*p3.x;
    p.y = uuu*p0.y + 3*uu*t*p1.y + 3*u*tt*p2.y + ttt*p3.y;
    return p;
}

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

        // Release mouse
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
        Vector2 prev = p0;
        for (int i = 1; i <= 100; i++) {
            float t = i / 100.0f;
            Vector2 curr = CubicBezier(p0, p1, p2, p3, t);
            DrawLineV(prev, curr, BLUE);
            prev = curr;
        }
        DrawLineV(p0, p1, LIGHTGRAY);
        DrawLineV(p2, p3, LIGHTGRAY);
        DrawCircleV(p0, radius, RED);
        DrawCircleV(p1, radius, GREEN);
        DrawCircleV(p2, radius, GREEN);
        DrawCircleV(p3, radius, RED);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
