#include "raylib.h"
#include <vector>
#include <cmath>
#include <cstdlib>

// -----------------------------
// Helpers
// -----------------------------
float RandomRange(float min, float max)
{
    return min + (float)rand() / RAND_MAX * (max - min);
}

float Vec2Distance(Vector2 a, Vector2 b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return sqrtf(dx * dx + dy * dy);
}

// -----------------------------
// Lightning (Fractal)
// -----------------------------
std::vector<Vector2> GenerateLightning(Vector2 start, Vector2 end, int iterations, float displacement)
{
    std::vector<Vector2> points;
    points.push_back(start);
    points.push_back(end);

    for (int i = 0; i < iterations; i++)
    {
        std::vector<Vector2> newPoints;

        for (size_t j = 0; j < points.size() - 1; j++)
        {
            Vector2 a = points[j];
            Vector2 b = points[j + 1];

            // Midpoint
            Vector2 mid = {
                (a.x + b.x) * 0.5f,
                (a.y + b.y) * 0.5f
            };

            // Direction
            Vector2 dir = { b.x - a.x, b.y - a.y };
            float len = sqrtf(dir.x * dir.x + dir.y * dir.y);

            if (len != 0)
            {
                dir.x /= len;
                dir.y /= len;
            }

            // Perpendicular
            Vector2 normal = { -dir.y, dir.x };

            // Random offset
            float offset = RandomRange(-displacement, displacement);

            mid.x += normal.x * offset;
            mid.y += normal.y * offset;

            newPoints.push_back(a);
            newPoints.push_back(mid);
        }

        newPoints.push_back(points.back());
        points = newPoints;

        // Reduce chaos each iteration
        displacement *= 0.5f;
    }

    return points;
}

// -----------------------------
// Draw lightning
// -----------------------------
void DrawLightning(const std::vector<Vector2>& points, Color color, float thickness)
{
    for (size_t i = 1; i < points.size(); i++)
    {
        DrawLineEx(points[i - 1], points[i], thickness, color);
    }
}

// -----------------------------
// Main
// -----------------------------
int main()
{
    const int screenWidth = 1000;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Real Lightning - raylib");

    SetTargetFPS(60);

    Vector2 p1 = { screenWidth / 2.0f - 150, screenHeight / 2.0f };
    Vector2 p2 = { screenWidth / 2.0f + 150, screenHeight / 2.0f };

    bool dragging1 = false;
    bool dragging2 = false;

    while (!WindowShouldClose())
    {
        Vector2 mouse = GetMousePosition();

        // -----------------------------
        // Dragging
        // -----------------------------
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (CheckCollisionPointCircle(mouse, p1, 10)) dragging1 = true;
            else if (CheckCollisionPointCircle(mouse, p2, 10)) dragging2 = true;
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            dragging1 = false;
            dragging2 = false;
        }

        if (dragging1) p1 = mouse;
        if (dragging2) p2 = mouse;

        // -----------------------------
        // Lightning generation
        // -----------------------------
        float dist = Vec2Distance(p1, p2);

        int iterations = 6; // more = more detail
        float displacement = dist * (0.25f + 0.05f * sinf(GetTime() * 15));

        std::vector<Vector2> lightning = GenerateLightning(p1, p2, iterations, displacement);

        // -----------------------------
        // Drawing
        // -----------------------------
        BeginDrawing();
        ClearBackground({10, 20, 40, 255});

        // Glow layers
        for (int i = 0; i < 6; i++)
        {
            float thickness = 8.0f - i * 1.2f;
            Color glow = {255, 255, 255, (unsigned char)(50 - i * 8)};
            DrawLightning(lightning, glow, thickness);
        }

        // Core bolt
        DrawLightning(lightning, WHITE, 2.0f);

        // Endpoints
        DrawCircleV(p1, 8, RED);
        DrawCircleV(p2, 8, BLUE);

        DrawText("Drag the points!", 10, 10, 20, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}