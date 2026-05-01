#include "raylib.h"
#include <vector>
#include <cmath>

// Constants
const int screenWidth = 800;
const int screenHeight = 600;

struct Light {
    Vector2 pos = { 160, 200 };
};

class Box {
public:
    float x, y, r, half_size;
    float shadow_length = 2000.0f;
    Color color;

    Box() {
        half_size = (float)(GetRandomValue(1, 50));
        x = (float)GetRandomValue(0, screenWidth);
        y = (float)GetRandomValue(0, screenHeight);
        r = (float)GetRandomValue(0, 360) * DEG2RAD;
        
        Color colors[] = { 
            { 245, 193, 86, 255 }, // #f5c156
            { 230, 97, 107, 255 }, // #e6616b
            { 92, 211, 173, 255 }  // #5cd3ad
        };
        color = colors[GetRandomValue(0, 2)];
    }

    std::vector<Vector2> GetDots() {
        std::vector<Vector2> dots(4);
        float full = (PI * 2.0f) / 4.0f;
        for (int i = 0; i < 4; i++) {
            dots[i] = {
                x + half_size * sinf(r + full * i),
                y + half_size * cosf(r + full * i)
            };
        }
        return dots;
    }

    void Rotate() {
        float speed = (60.0f - half_size) / 20.0f;
        r += speed * 0.002f;
        x += speed;
        y += speed;

        if (y - half_size > screenHeight) y -= (screenHeight + 100);
        if (x - half_size > screenWidth) x -= (screenWidth + 100);
    }

    void DrawShadow(Vector2 lightPos) {
        auto dots = GetDots();
        for (int i = 0; i < 4; i++) {
            int n = (i + 1) % 4;
            
            Vector2 p1 = dots[i];
            Vector2 p2 = dots[n];

            float angle1 = atan2f(lightPos.y - p1.y, lightPos.x - p1.x);
            float angle2 = atan2f(lightPos.y - p2.y, lightPos.x - p2.x);

            Vector2 s1 = { p1.x + shadow_length * sinf(-angle1 - PI/2), 
                           p1.y + shadow_length * cosf(-angle1 - PI/2) };
            Vector2 s2 = { p2.x + shadow_length * sinf(-angle2 - PI/2), 
                           p2.y + shadow_length * cosf(-angle2 - PI/2) };

            // Draw shadow quadrilateral using two triangles
            DrawTriangleFan((Vector2[4]){ p1, p2, s2, s1 }, 4, Color{ 44, 52, 63, 255 });
        }
    }

    void Draw() {
        auto dots = GetDots();
        DrawTriangleFan(dots.data(), 4, color);
    }
};

int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Raylib Shadow Engine");
    SetTargetFPS(60);

    Light light;
    std::vector<Box> boxes;
    for (int i = 0; i < 14; i++) boxes.push_back(Box());

    while (!WindowShouldClose()) {
        // Update
        light.pos = GetMousePosition();
        for (auto& b : boxes) b.Rotate();

        // Draw
        BeginDrawing();
            // Background Gradient (Simulating your JS drawLight)
            ClearBackground(Color{ 44, 52, 63, 255 });
            DrawCircleGradient(light.pos.x, light.pos.y, 1000, Color{ 59, 70, 84, 255 }, Color{ 44, 52, 63, 255 });

            // Shadows first
            for (auto& b : boxes) b.DrawShadow(light.pos);
            
            // Boxes second
            for (auto& b : boxes) b.Draw();

            // Light bulb effect
            DrawCircleGradient(light.pos.x, light.pos.y, 20, WHITE, Color{ 59, 70, 84, 0 });
        EndDrawing();
    }

    CloseWindow();
    return 0;
}