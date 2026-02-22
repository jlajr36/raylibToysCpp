#include "raylib.h"
#include <vector>
#include <cmath>

struct Joint {
    Vector3 pos;
};

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "IK Robot Simulator");
    SetTargetFPS(60);

    // --- Camera settings ---
    float cameraAngle = 45.0f;
    const float radius = 8.0f;
    const float cameraHeight = 4.0f;
    const float rotationSpeed = 60.0f;

    Camera camera = {0};
    camera.target = {0.0f, 1.0f, 0.0f}; // default target
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 35.0f;

    // --- Robot arm settings ---
    float xOffset = 0.5f;
    float zOffset = -0.5f;

    std::vector<Joint> arm(3);
    // Base joint
    arm[0].pos = {xOffset, 0.1f, zOffset};
    // First joint
    arm[1].pos = {xOffset - 1.0f, 1.5f, zOffset};
    // Second joint / end-effector
    arm[2].pos = {xOffset + 1.0f, 2.7f, zOffset};

    Vector3 basePos = {xOffset, 0.0f, zOffset};

    while(!WindowShouldClose()) {

        float dt = GetFrameTime();

        // --- Camera orbit controls ---
        if (IsKeyDown(KEY_RIGHT)) cameraAngle += rotationSpeed * dt;
        if (IsKeyDown(KEY_LEFT))  cameraAngle -= rotationSpeed * dt;

        float rad = cameraAngle * (PI / 180.0f);

        camera.position.x = radius * sin(rad);
        camera.position.y = cameraHeight;
        camera.position.z = radius * cos(rad);

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(camera);

            // --- Grid ---
            DrawGrid(20, 1.0f);

            // --- Robot base ---
            DrawCube(basePos, 1.0f, 0.2f, 1.0f, GRAY);
            DrawCubeV({xOffset + 0.5f, 0.25f, zOffset}, {0.5f, 0.05f, 0.05f}, GREEN);

            // --- Servo caps ---
            DrawCylinder(basePos, 0.2f, 0.2f, 0.05f, 32, DARKGRAY);
            DrawCylinder(arm[0].pos, 0.2f, 0.2f, 0.05f, 32, DARKGRAY);
            DrawCylinder(arm[1].pos, 0.2f, 0.2f, 0.05f, 32, DARKGRAY);

            // --- Arm segments with metallic shading ---
            for (size_t i = 0; i < arm.size() - 1; i++)
            {
                float brightness = 0.6f + 0.4f * (arm[i].pos.y / 3.0f);
                Color segmentColor = { (unsigned char)(70 * brightness),
                                       (unsigned char)(130 * brightness),
                                       (unsigned char)(180 * brightness), 255 };
                DrawCylinderEx(arm[i].pos, arm[i+1].pos, 0.1f, 0.08f, 32, segmentColor);
            }

            // --- Joints ---
            for (size_t i = 0; i < arm.size() - 1; i++)
            {
                DrawSphere(arm[i].pos, 0.16f, BLUE);
            }
            // End-effector with glow
            DrawSphere(arm[2].pos, 0.18f, RED);
            DrawSphereWires(arm[2].pos, 0.2f, 16, 16, ORANGE);

        EndMode3D();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}