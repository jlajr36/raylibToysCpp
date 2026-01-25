#include <raylib.h>
#include <random>
#include <vector>

#include <iostream>
#include <iomanip>

using namespace std;

struct Orbit
{
    float centerX;
    float centerY;
    float radius;
    float angle;
    float speed;
    float size;
    float x, y;
    float lastX, lastY;
};

inline std::ostream& operator<<(std::ostream& os, const Orbit& o)
{
    os << "Orbit{ "
       << "cx=" << fixed << setprecision(2) << o.centerX << ' '
       << "cy=" << fixed << setprecision(2) << o.centerY << ' '
       << "r="  << fixed << setprecision(2) << o.radius  << ' '
       << "a="  << fixed << setprecision(2) << o.angle   << ' '
       << "s="  << fixed << setprecision(3) << o.speed   << ' '
       << "sz=" << fixed << setprecision(2) << o.size    << ' '
       << "x="  << fixed << setprecision(2) << o.x       << ' '
       << "y="  << fixed << setprecision(2) << o.y
       << " }";
    return os;
}

float Rand(float min, float max) {
    static mt19937 engine{random_device{}()};
    uniform_real_distribution<float> dist(min, max);
    return dist(engine);
}

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
    const int screenWidth  = 800;
    const int screenHeight = 450;
    const int numOrbs = 300;

    InitWindow(screenWidth, screenHeight, "Raylib C++ Template");
    SetTargetFPS(60);

    float maxRadius = fminf(screenWidth, screenHeight) * 0.45f;

    vector<Orbit> orbits;
    for (int i = 0; i < numOrbs; i++) {
        Orbit orbit;

        orbit.centerX = screenWidth / 2.0f;
        orbit.centerY = screenHeight / 2.0f;
        orbit.radius = Rand(20.0f, maxRadius);
        orbit.angle = Rand(0.0f, PI * 2.0f);
        orbit.speed = Rand(0.005f, 0.02f);
        orbit.size = Rand(0.5f, 1.5f);

        orbit.x = orbit.centerX + cosf(orbit.angle) * orbit.radius;
        orbit.y = orbit.centerY + sinf(orbit.angle) * orbit.radius;
        orbit.lastX = orbit.x;
        orbit.lastY = orbit.y;

        orbits.push_back(orbit);
    }

    while (!WindowShouldClose()) {
        BeginDrawing();

        DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 25});
        for (auto& orbit : orbits) {
            orbit.lastX = orbit.x;
            orbit.lastY = orbit.y;

            orbit.angle += orbit.speed;

            orbit.x = orbit.centerX + cosf(orbit.angle) * orbit.radius;
            orbit.y = orbit.centerY + sinf(orbit.angle) * orbit.radius;

            float hue = fmodf(orbit.angle * 180.0f / PI, 360.0f);
            Color col = HSLtoRGB(hue, 1.0f, 0.5f);

            DrawLineEx(
                { orbit.lastX, orbit.lastY },
                { orbit.x, orbit.y },
                orbit.size,
                col
            );
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
