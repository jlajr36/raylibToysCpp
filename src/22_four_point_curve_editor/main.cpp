#include "raylib.h"
#include <iostream>
#include <vector>

std::vector<Vector2> CubicBezier(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, int steps) {
    if (steps <= 0) return { p0 };

    float u, t, w0, w1, w2, w3, x, y;
    std::vector<Vector2> curve_out;
    curve_out.reserve(steps + 1);

    for (int i = 0; i <= steps; i++) {
        // Even time step
        t = (float)i / (float)steps;

        // Step 1: compute helper value
        u = 1 - t;                  // (1 - t)

        // Step 2: compute Bézier weights
        w0 = u * u * u;             // (1 - t)^3
        w1 = 3 * u * u * t;         // 3(1 - t)^2 t
        w2 = 3 * u * t * t;         // 3(1 - t) t^2
        w3 = t * t * t;             // t^3

        // Step 3: Weighted sum of control points (core Bézier equation)
        x = w0 * p0.x + w1 * p1.x + w2 * p2.x + w3 * p3.x;
        y = w0 * p0.y + w1 * p1.y + w2 * p2.y + w3 * p3.y;

        // Populate Vector
        curve_out.push_back({x, y});
    }

    return curve_out;
}

int main()
{
    const int screenWidth = 1200;
    const int screenHeight = 800;
    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Bezier Curve");
    SetTargetFPS(60);

    // Curve start points
    Vector2 p[4] = {
        { 400, 600 }, // p0
        { 500, 200 }, // p1
        { 800, 200 }, // p2
        { 900, 600 }  // p3
    };

    while(!WindowShouldClose()) {

        float thickness = 1.0f;
        int points = 256;
        std::vector<Vector2> curve = CubicBezier(p[0], p[1], p[2], p[3], points);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw curve line
        for (int i = 0; i < curve.size() - 1; i++) {
            DrawLineEx(curve[i], curve[i + 1], thickness, BLUE);
            DrawCircleV(curve[i], thickness * 0.5f, BLUE);
        }

        // Draw control points
        for (int i = 0; i < 4; i++)
            DrawCircleV(p[i], 6, BLUE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
