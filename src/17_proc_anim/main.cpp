#include "raylib.h"
#include <vector>
#include <cmath>

int main() {
    const int screenWidth = 600;
    const int screenHeight = 400;

    InitWindow(screenWidth, screenHeight, "FABRIK Chain (raylib)");
    SetTargetFPS(60);

    std::vector<Vector2> joints;

    int linkSize = 20;
    int jointCount = 6;

    // Setup
    Vector2 start = { screenWidth / 2.0f, screenHeight / 2.0f };
    joints.push_back(start);

    for (int i = 1; i < jointCount; i++) {
        Vector2 prev = joints[i - 1];
        Vector2 next = { prev.x, prev.y + linkSize };
        joints.push_back(next);
    }

    while (!WindowShouldClose()) {

        // Target = mouse
        Vector2 target = GetMousePosition();

        // ---- FABRIK (same logic as your code) ----

        // Forward pass
        joints[0] = target;

        for (size_t i = 1; i < joints.size(); i++) {
            Vector2 dir = {
                joints[i].x - joints[i - 1].x,
                joints[i].y - joints[i - 1].y
            };

            float length = sqrtf(dir.x * dir.x + dir.y * dir.y);
            if (length != 0) {
                dir.x /= length;
                dir.y /= length;
            }

            dir.x *= linkSize;
            dir.y *= linkSize;

            joints[i].x = joints[i - 1].x + dir.x;
            joints[i].y = joints[i - 1].y + dir.y;
        }

        // Backward pass
        Vector2 anchor = joints[joints.size() - 1];
        joints[joints.size() - 1] = anchor;

        for (int i = (int)joints.size() - 2; i >= 0; i--) {
            Vector2 dir = {
                joints[i].x - joints[i + 1].x,
                joints[i].y - joints[i + 1].y
            };

            float length = sqrtf(dir.x * dir.x + dir.y * dir.y);
            if (length != 0) {
                dir.x /= length;
                dir.y /= length;
            }

            dir.x *= linkSize;
            dir.y *= linkSize;

            joints[i].x = joints[i + 1].x + dir.x;
            joints[i].y = joints[i + 1].y + dir.y;
        }

        // ---- Drawing ----
        BeginDrawing();
        ClearBackground(Color{40, 44, 52, 255});

        // Draw links
        for (size_t i = 0; i < joints.size() - 1; i++) {
            DrawLineEx(joints[i], joints[i + 1], 4.0f, WHITE);
        }

        // Draw joints
        for (auto &j : joints) {
            DrawCircleV(j, 5, WHITE);
        }

        // Draw target
        DrawCircleV(target, 6, RED);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}