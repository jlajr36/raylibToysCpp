#include "raylib.h"
#include <cmath>

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Sci-Fi DNA 3D Visualization - Pi Day");

    Camera3D camera = {0};
    camera.position = {0.0f, 200.0f, 600.0f};
    camera.target = {0.0f, 200.0f, 0.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;

    SetTargetFPS(60);

    float helixRadius = 50.0f;
    float helixHeight = 400.0f;
    int numPoints = 200;
    float angle = 0.0f;

    while (!WindowShouldClose()) {
        angle += 0.02f;

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(camera);

        for (int i = 0; i < numPoints; i++) {
            float t = (float)i / numPoints;
            float y = t * helixHeight;

            float x1 = helixRadius * cos(2 * PI * t + angle);
            float z1 = helixRadius * sin(2 * PI * t + angle);

            float x2 = helixRadius * cos(2 * PI * t + PI + angle);
            float z2 = helixRadius * sin(2 * PI * t + PI + angle);

            Color col1 = Color{(unsigned char)(128 + z1), (unsigned char)(50 + z1), 255, 255};
            Color col2 = Color{255, (unsigned char)(128 + z2), (unsigned char)(50 + z2), 255};

            DrawSphere(Vector3{x1, y, z1}, 5.0f, col1);
            DrawSphere(Vector3{x2, y, z2}, 5.0f, col2);

            // Approximate base pair with a thin vertical cylinder
            if (i % 2 == 0) {
                Vector3 mid = {(x1 + x2) / 2, y, (z1 + z2) / 2};
                DrawCylinder(mid, 1.0f, 1.0f, 1.0f, 8, WHITE);
            }
        }

        EndMode3D();

        DrawText("Happy Pi Day!", 20, 20, 30, SKYBLUE);
        DrawText("pi = 3.14159...", 20, 60, 20, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}