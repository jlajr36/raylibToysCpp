#include "raylib.h"
#include <vector>
#include <cmath>
#include <cstdlib>

struct Orb {
    float centerX;
    float centerY;
    float radius;
    float angle;
    float speed;
    float size;

    float x, y;
    float lastX, lastY;
};

float Rand(float min, float max) {
    return min + (float)rand() / RAND_MAX * (max - min);
}

// Convert HSL to Color (raylib uses RGB)
Color HSLtoRGB(float h, float s, float l) {
    float c = (1.0f - fabsf(2.0f * l - 1.0f)) * s;
    float x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f));
    float m = l - c / 2.0f;

    float r = 0, g = 0, b = 0;

    if (h < 60)      { r = c; g = x; }
    else if (h < 120){ r = x; g = c; }
    else if (h < 180){ g = c; b = x; }
    else if (h < 240){ g = x; b = c; }
    else if (h < 300){ r = x; b = c; }
    else             { r = c; b = x; }

    return {
        (unsigned char)((r + m) * 255),
        (unsigned char)((g + m) * 255),
        (unsigned char)((b + m) * 255),
        255
    };
}

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    const int ORB_COUNT = 300;

    InitWindow(screenWidth, screenHeight, "Orb Trail");
    SetTargetFPS(60);

    std::vector<Orb> orbs;
    float maxRadius = fminf(screenWidth, screenHeight) * 0.45f;

    // Create orbs
    for (int i = 0; i < ORB_COUNT; i++) {
        Orb orb;
        orb.centerX = screenWidth / 2.0f;
        orb.centerY = screenHeight / 2.0f;
        orb.radius = Rand(20.0f, maxRadius);
        orb.angle = Rand(0.0f, PI * 2.0f);
        orb.speed = Rand(0.005f, 0.02f);
        orb.size = Rand(0.5f, 1.5f);

        orb.x = orb.centerX + cosf(orb.angle) * orb.radius;
        orb.y = orb.centerY + sinf(orb.angle) * orb.radius;
        orb.lastX = orb.x;
        orb.lastY = orb.y;

        orbs.push_back(orb);
    }

    // Main loop
    while (!WindowShouldClose()) {

        BeginDrawing();

        // Trail fade (equivalent to rgba(0,0,0,0.1))
        DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 25});

        for (auto& orb : orbs) {
            orb.lastX = orb.x;
            orb.lastY = orb.y;

            orb.angle += orb.speed;

            orb.x = orb.centerX + cosf(orb.angle) * orb.radius;
            orb.y = orb.centerY + sinf(orb.angle) * orb.radius;

            float hue = fmodf(orb.angle * 180.0f / PI, 360.0f);
            Color col = HSLtoRGB(hue, 1.0f, 0.5f);

            DrawLineEx(
                { orb.lastX, orb.lastY },
                { orb.x, orb.y },
                orb.size,
                col
            );
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
