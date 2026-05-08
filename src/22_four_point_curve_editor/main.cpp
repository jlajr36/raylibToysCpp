#include "raylib.h"

// Returns true if the slider is being hovered or dragged to prevent clicking points "through" the UI
bool GuiSlider(Rectangle bounds, float min, float max, float *value) {
    Vector2 mouse = GetMousePosition();
    bool hovering = CheckCollisionPointRec(mouse, bounds);
    static bool dragging = false;

    if (hovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) dragging = true;
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) dragging = false;

    if (dragging) {
        float t = (mouse.x - bounds.x) / bounds.width;
        if (t < 0.0f) t = 0.0f; 
        if (t > 1.0f) t = 1.0f;
        *value = min + t * (max - min);
    }

    // Draw Bar
    DrawRectangleRec(bounds, LIGHTGRAY);
    // Draw Handle
    float handlePos = bounds.x + ((*value - min) / (max - min)) * bounds.width;
    DrawCircle(handlePos, bounds.y + bounds.height / 2, 8, DARKGRAY);

    return (hovering || dragging);
}

int main() {
    const int screenWidth = 1200;
    const int screenHeight = 800;

    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "4-Point Curve Editor");
    SetTargetFPS(60);

    // 1. Storage: Using an array makes logic cleaner and works directly with DrawSpline
    Vector2 p[4] = {
        { 400, 600 }, // p0
        { 500, 200 }, // p1
        { 800, 200 }, // p2
        { 900, 600 }  // p3
    };

    float thickness = 4.0f;
    int selectedIndex = -1;
    const float grabRadius = 12.0f;

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        bool uiBusy = false;

        // --- Logic ---

        // Update UI first
        Rectangle sliderBounds = { 20, 80, 200, 10 };
        uiBusy = GuiSlider(sliderBounds, 1.0f, 30.0f, &thickness);

        // Point Selection (only if UI isn't being used)
        if (!uiBusy) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                for (int i = 0; i < 4; i++) {
                    if (CheckCollisionPointCircle(mousePos, p[i], grabRadius)) {
                        selectedIndex = i;
                        break;
                    }
                }
            }
        }

        // Handle Dragging
        if (selectedIndex != -1) {
            p[selectedIndex] = mousePos;
            SetMouseCursor(MOUSE_CURSOR_RESIZE_ALL);
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) selectedIndex = -1;
        } else {
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }

        // --- Render ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw Help Info
        DrawText("4-Point Bezier Editor", 20, 20, 20, DARKGRAY);
        DrawText(TextFormat("Thickness: %.1f", thickness), 20, 55, 18, GRAY);

        // 2. The Hull (Control Polygon) - Connects points to show the "pull"
        DrawLineStrip(p, 4, Fade(LIGHTGRAY, 0.6f));

        // 3. The Curve
        DrawSplineBezierCubic(p, 4, thickness, BLUE);

        // 4. Control Points (Visuals)
        for (int i = 0; i < 4; i++) {
            // Anchor points (red) vs Handles (green)
            Color pointColor = (i == 0 || i == 3) ? RED : LIME;
            
            // Subtle shadow/outline if selected
            if (selectedIndex == i) DrawCircleV(p[i], grabRadius + 2, MAROON);
            
            DrawCircleV(p[i], grabRadius, pointColor);
            DrawCircleLinesV(p[i], grabRadius, DARKGRAY); // Outline for clarity
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}