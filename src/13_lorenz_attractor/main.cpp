#include "raylib.h"
#include <deque>
#include <cmath>

struct Point3D {
    Vector3 pos;
    float hue;
};

// Linear interpolation
float Lerp(float a, float b, float t) {
    return a + t * (b - a);
}

int main() {
    InitWindow(800, 600, "3D Lorenz Attractor");
    SetTargetFPS(60);

    Camera3D camera = {0};
    camera.position = {30.0f, 30.0f, 30.0f};
    camera.target = {0.0f, 0.0f, 0.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;

    // Lorenz system initial values
    float x = 0.01f, y = 0.0f, z = 0.0f;
    const float x0 = 0.01f, y0 = 0.0f, z0 = 0.0f;

    float a = 10.0f, b = 28.0f, c = 8.0f / 3.0f;
    float dt = 0.01f;

    std::deque<Point3D> points;
    const int maxPoints = 2000;

    float angle = 0.0f;     // camera rotation
    float radius = 50.0f;   // camera distance

    // Random starting hue for trail
    float startHue = GetRandomValue(0, 359);

    while (!WindowShouldClose()) {
        // --- Reset if space bar pressed ---
        if (IsKeyPressed(KEY_SPACE)) {
            points.clear();
            x = x0; y = y0; z = z0;
            angle = 0.0f;
            radius = 50.0f;
            startHue = GetRandomValue(0, 359);
        }

        // --- Lorenz integration (Euler) ---
        float dx = a * (y - x) * dt;
        float dy = (x * (b - z) - y) * dt;
        float dz = (x * y - c * z) * dt;
        x += dx; y += dy; z += dz;

        // Add new point
        points.push_back({{x, y, z}, 0.0f});
        if (points.size() > maxPoints) points.pop_front(); // O(1) removal

        // --- Compute center ---
        Vector3 center = {0.0f, 0.0f, 0.0f};
        for (const auto& p : points) {
            center.x += p.pos.x;
            center.y += p.pos.y;
            center.z += p.pos.z;
        }
        if (!points.empty()) {
            center.x /= points.size();
            center.y /= points.size();
            center.z /= points.size();
        }

        // --- Keyboard-controlled rotation ---
        const float rotationSpeed = 0.03f;
        if (IsKeyDown(KEY_RIGHT)) angle += rotationSpeed;
        if (IsKeyDown(KEY_LEFT))  angle -= rotationSpeed;

        // --- Keyboard-controlled zoom ---
        const float zoomSpeed = 1.0f;
        if (IsKeyDown(KEY_UP))    radius -= zoomSpeed;
        if (IsKeyDown(KEY_DOWN))  radius += zoomSpeed;
        if (radius < 10.0f) radius = 10.0f;
        if (radius > 200.0f) radius = 200.0f;

        // --- Smooth camera update ---
        float camX = center.x + sin(angle) * radius;
        float camZ = center.z + cos(angle) * radius;
        camera.position.x = Lerp(camera.position.x, camX, 0.1f);
        camera.position.z = Lerp(camera.position.z, camZ, 0.1f);
        camera.position.y = Lerp(camera.position.y, center.y + 30.0f, 0.1f);
        camera.target.x = Lerp(camera.target.x, center.x, 0.1f);
        camera.target.y = Lerp(camera.target.y, center.y, 0.1f);
        camera.target.z = Lerp(camera.target.z, center.z, 0.1f);

        // --- Animate hue ---
        startHue += 0.5f;
        if (startHue > 360.0f) startHue -= 360.0f;

        // --- Drawing ---
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);

        if (points.size() > 1) {
            for (size_t i = 1; i < points.size(); i++) {
                // Color gradient along trail with fading brightness
                float t = (float)i / points.size();      // 0 oldest -> 1 newest
                float hue = fmod(startHue + t * 360.0f, 360.0f);
                Color col = ColorFromHSV(hue, 1.0f, t); // fade brightness
                DrawLine3D(points[i-1].pos, points[i].pos, col);
            }
        }

        EndMode3D();
        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}