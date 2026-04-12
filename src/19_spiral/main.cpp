#include "raylib.h"
#include <math.h>

const int MAX_DISTANCE  = 500;
const int SEGMENT_STEP  = 3;
const float BASE_WIND   = 8.0f;
const float PEAK_HEIGHT = MAX_DISTANCE * 0.3f;

int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);
    InitWindow(1200, 800, "Vortex Simulation");
    SetTargetFPS(60);

    float elapsed = 0.0f;
    float currentTiltX = 0.0f;
    float currentTiltY = 0.0f;

    while (!WindowShouldClose()) {
        elapsed += GetFrameTime();

        float targetX = (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) ? (GetMouseX() - GetScreenWidth()/2.0f) / (GetScreenWidth()/2.0f) : 0.0f;
        float targetY = (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) ? (GetMouseY() - GetScreenHeight()/2.0f) / (GetScreenHeight()/2.0f) : 0.0f;

        currentTiltX += (targetX - currentTiltX) * 0.1f;
        currentTiltY += (targetY - currentTiltY) * 0.1f;

        BeginDrawing();
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){ 0, 0, 0, 18 });

        BeginBlendMode(BLEND_ADDITIVE);

        float centerX = GetScreenWidth() / 2.0f;
        float centerY = (GetScreenHeight() / 2.0f) - 100.0f;

        Vector2 lastPoint = { 0, 0 };

        for (int i = (MAX_DISTANCE / SEGMENT_STEP); i > 0; i--) {
            float dist = (float)i * SEGMENT_STEP - fmodf(elapsed * 100.0f, (float)SEGMENT_STEP);
            
            float pulse = sinf(elapsed * 2.0f) * 0.5f;
            float theta = (dist / (BASE_WIND + pulse)) + (elapsed * 1.2f);

            float x = sinf(theta) * dist;
            float y = cosf(theta) * dist * 0.35f; 
            
            float normDist = dist / MAX_DISTANCE;
            float falloff = 1.0f - (fminf(dist, PEAK_HEIGHT) / PEAK_HEIGHT);
            
            y -= powf(falloff, 2) * 250; 
            y += normDist * 300;
            
            x += currentTiltX * dist * 0.5f;
            y += currentTiltY * dist * 0.8f;

            Vector2 currentPoint = { centerX + x, centerY + y };
            
            float hue = fmodf(dist + elapsed * 100.0f, 360.0f);
            float alpha = 1.0f - normDist;
            
            if (i < (MAX_DISTANCE / SEGMENT_STEP)) {
                DrawLineEx(lastPoint, currentPoint, 5.0f, Fade(ColorFromHSV(hue, 0.8f, 0.5f), alpha * 0.4f));
                DrawLineEx(lastPoint, currentPoint, 1.2f, Fade(WHITE, alpha * 0.8f));
            }
            
            lastPoint = currentPoint;
        }

        EndBlendMode();        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}