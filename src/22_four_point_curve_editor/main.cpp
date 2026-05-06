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

// Simple slider
float Slider(float x, float y, float width, float min, float max, float value)
{
    Vector2 mouse = GetMousePosition();
    Rectangle bar = { x, y, width, 6 };

    bool hovering = CheckCollisionPointRec(mouse, bar);

    if (hovering && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        float t = (mouse.x - x) / width;
        if (t < 0) t = 0;
        if (t > 1) t = 1;
        value = min + t * (max - min);
    }

    DrawRectangleRec(bar, LIGHTGRAY);

    float t = (value - min) / (max - min);
    float hx = x + t * width;
    DrawCircle(hx, y + 3, 8, DARKGRAY);

    return value;
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

    float thickness = 2.0f;

    // Control points
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
        // Slider
        DrawText("Curve Thickness", 10, 40, 20, DARKGRAY);
        thickness = Slider(10, 70, 200, 1.0f, 20.0f, thickness);

        Vector2 points[4] = { p0, p1, p2, p3 };
        DrawSplineBezierCubic(points, 4, thickness, BLUE);

        // Control polygon
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
