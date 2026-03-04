#include "raylib.h"
#include <deque>

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Scrolling Horizontal Line with Deque");
    SetTargetFPS(60);

    std::deque<Vector2> points;

    float y = screenHeight / 2.0f;  // Center vertically
    const int lineLength = 300;      // Total length of the line
    const int spacing = 10;          // Distance between points
    int numPoints = lineLength / spacing;

    // Center the line horizontally
    int startX = (screenWidth - lineLength) / 2;

    for (int i = 0; i <= numPoints; i++)
    {
        points.push_back({ (float)(startX + i * spacing), y });
    }

    int direction = 1; // 1 = move right, -1 = move left
    float speed = 2.0f; // pixels per frame

    while (!WindowShouldClose())
    {
        // Move the line
        for (auto& p : points)
            p.x += direction * speed;

        // Remove/add points to simulate scrolling
        if (direction == 1 && points.back().x >= screenWidth) // scrolling right
        {
            points.pop_front(); // remove leftmost
            points.push_back({ points.back().x + spacing, y });
        }
        else if (direction == -1 && points.front().x <= 0) // scrolling left
        {
            points.pop_back(); // remove rightmost
            points.push_front({ points.front().x - spacing, y });
        }

        // Change direction when reaching edges
        if (points.front().x <= 0)
            direction = 1;
        if (points.back().x >= screenWidth)
            direction = -1;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw points
        for (const auto& p : points)
        {
            DrawCircleV(p, 3, BLUE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}