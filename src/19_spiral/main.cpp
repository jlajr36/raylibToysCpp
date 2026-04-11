#include "raylib.h"
#include <math.h>

const int MAX_OFFSET = 400;
const int SPACING = 4;
const float POINTS_PER_LAP = 6.0f;
const float PEAK = MAX_OFFSET * 0.25f;

int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(800, 800, "Spiral");
    SetTargetFPS(60);

    float time = 0.0f;
    float velocity = 0.1f;
    float velocityTarget = 0.1f;

    while (!WindowShouldClose()) {
        time += velocity;
        velocity += (velocityTarget - velocity) * 0.3f;

        BeginDrawing();
        ClearBackground(BLACK);

        BeginBlendMode(BLEND_ADDITIVE);

        float cx = GetScreenWidth() / 2.0f;
        float cy = GetScreenHeight() / 2.0f;

        Vector2 lastPos = { 0, 0 };

        for (int i = (MAX_OFFSET / SPACING); i > 0; i--) {
            
            float value = (float)i * SPACING + fmodf(time, (float)SPACING);
            
            float ax = sinf(value / POINTS_PER_LAP) * PI;
            float ay = cosf(value / POINTS_PER_LAP) * PI;

            float x = ax * value;
            float y = ay * value * 0.35f;
            
            float o = 1.0f - (fminf(value, PEAK) / PEAK);
            y -= powf(o, 2) * 200;
            y += 200 * value / MAX_OFFSET;
            y += x / cx * GetScreenWidth() * 0.1f;

            Vector2 currentPos = { cx + x, cy + y };
            
            float alpha = 1.0f - (value / MAX_OFFSET);
            Color color = Fade(WHITE, alpha);

            if (i < (MAX_OFFSET / SPACING)) {
                DrawLineEx(lastPos, currentPos, 2.0f, color);
            }
            
            lastPos = currentPos;
        }

        DrawLine(cx, cy - 200, cx, 0, Fade(WHITE, 0.3f));

        EndBlendMode();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}