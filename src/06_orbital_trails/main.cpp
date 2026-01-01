#include <raylib.h>
#include <deque>
#include <cmath>

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Orbital Pixel Trail");

    // Orbit parameters
    const Vector2 center = {(float)screenWidth / 2, (float)screenHeight / 2};
    const float radius = 150.0f;
    const float speed = 2.0f;               // radians per second
    const size_t maxTrailLength = 200;      // number of pixels to keep

    std::deque<Vector2> trail;
    float angle = 0.0f;

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        // ---- Update -------------------------------------------------
        float dt = GetFrameTime();
        angle += speed * dt;                 // advance angle
        if (angle > 2 * PI) angle -= 2 * PI; // keep within [0,2π)

        // Compute new pixel position
        Vector2 newPos = {
            center.x + radius * std::cos(angle),
            center.y + radius * std::sin(angle)
        };
        trail.push_back(newPos);
        if (trail.size() > maxTrailLength) trail.pop_front();

        // ---- Draw ---------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw trail with fading opacity
        for (size_t i = 0; i < trail.size(); ++i) {
            float t = (float)i / trail.size();          // 0 = oldest, 1 = newest
            Color col = Fade(WHITE, t);                 // older pixels more transparent
            DrawPixelV(trail[i], col);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
