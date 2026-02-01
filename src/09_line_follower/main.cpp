#include "raylib.h"
#include <vector>

// Simple cubic Bezier sample function
static Vector2 CubicBezier(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, float t) {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    Vector2 p = {0, 0};
    p.x = uuu * p0.x;
    p.y = uuu * p0.y;

    p.x += 3 * uu * t * p1.x;
    p.y += 3 * uu * t * p1.y;

    p.x += 3 * u * tt * p2.x;
    p.y += 3 * u * tt * p2.y;

    p.x += ttt * p3.x;
    p.y += ttt * p3.y;

    return p;
}

int main() {
    const int screenW = 1024;
    const int screenH = 775;
    InitWindow(screenW, screenH, "Black Path on White Background");

    SetTargetFPS(60);

    std::vector<Vector2> pathPoints;

    // Start point
    pathPoints.push_back({100, 100});

    // Straight line to the right
    pathPoints.push_back({400, 100});

    // Sharp turn down
    pathPoints.push_back({400, 300});

    // Small sharp turn left
    pathPoints.push_back({350, 300});

    // Long straight left
    pathPoints.push_back({150, 300});

    // Curve: create cubic bezier from current point to new point
    Vector2 b0 = pathPoints.back();
    Vector2 b1 = {100, 300};
    Vector2 b2 = {50, 450};
    Vector2 b3 = {250, 500};

    const int BEZIER_STEPS = 40;
    for (int i = 1; i <= BEZIER_STEPS; ++i) {
        float t = i / (float)BEZIER_STEPS;
        pathPoints.push_back(CubicBezier(b0, b1, b2, b3, t));
    }

    // More straight and sharp turn sequence
    pathPoints.push_back({500, 520});
    pathPoints.push_back({500, 380});
    pathPoints.push_back({700, 380});
    pathPoints.push_back({900, 200});

    // Add a straight vertical line going up from the final point
    {
        Vector2 last = pathPoints.back();
        const float UP_LENGTH = 200.0f;
        pathPoints.push_back({ last.x, last.y - UP_LENGTH });
    }

    // Append an extra cubic Bezier curve at the very end (mirrored)
    Vector2 endStart = pathPoints.back();
    Vector2 endCtrl1 = { endStart.x - 60, endStart.y - 40 };
    Vector2 endCtrl2 = { endStart.x - 160, endStart.y + 80 };
    Vector2 endTarget = { endStart.x - 220, endStart.y + 40 };

    for (int i = 1; i <= BEZIER_STEPS; ++i) {
        float t = i / (float)BEZIER_STEPS;
        pathPoints.push_back(CubicBezier(endStart, endCtrl1, endCtrl2, endTarget, t));
    }

    // Add a straight vertical line going up from current final point
    {
        Vector2 last = pathPoints.back();
        const float EXTRA_UP_LENGTH = 160.0f;
        pathPoints.push_back({ last.x, last.y - EXTRA_UP_LENGTH });
    }

    // Append a straight horizontal line going left from current final point
    {
        Vector2 last = pathPoints.back();
        const float LEFT_LENGTH = 590.0f; // distance to extend left
        Vector2 leftEnd = { last.x - LEFT_LENGTH, last.y };
        pathPoints.push_back(leftEnd);
    }

    // Append a straight vertical line going down to meet starting point
    {
        Vector2 last = pathPoints.back();
        Vector2 start = pathPoints.front();
        float DOWN_LENGTH = start.y - last.y; // distance to reach start point vertically
        pathPoints.push_back({ last.x, last.y + DOWN_LENGTH });
    }

    // --- CENTER PATH IN WINDOW ---
    {
        float minX = 1e9f, minY = 1e9f;
        float maxX = -1e9f, maxY = -1e9f;
        for (auto &pt : pathPoints) {
            if (pt.x < minX) minX = pt.x;
            if (pt.y < minY) minY = pt.y;
            if (pt.x > maxX) maxX = pt.x;
            if (pt.y > maxY) maxY = pt.y;
        }

        float pathCenterX = (minX + maxX) / 2.0f;
        float pathCenterY = (minY + maxY) / 2.0f;

        float dx = screenW / 2.0f - pathCenterX;
        float dy = screenH / 2.0f - pathCenterY;

        for (auto &pt : pathPoints) {
            pt.x += dx;
            pt.y += dy;
        }
    }

    const float PATH_THICK = 18.0f;

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(WHITE);

            for (size_t i = 1; i < pathPoints.size(); ++i) {
                DrawLineEx(pathPoints[i-1], pathPoints[i], PATH_THICK, BLACK);
            }
            for (auto &pt : pathPoints) DrawCircleV(pt, PATH_THICK/2.0f, BLACK);

            DrawText("Black path on white. Path centered in window.", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
