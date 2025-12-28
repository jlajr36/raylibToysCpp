#include "raylib.h"
#include <math.h>

int main(void) {
    const int screenWidth = 800;
    const int screenHeigh = 450;

    InitWindow(screenWidth, screenWidth, "Mouse Follow");

    Vector2 sclerLeftPosition = { GetScreenWidth()/2.0f - 100.0f, GetScreenHeight()/2.0f };
    Vector2 sclerRightPosition = { GetScreenWidth()/2.0f + 100.0f, GetScreenHeight()/2.0f };
    float sclerRadius = 80;

    Vector2 irisLeftPosition = { GetScreenWidth()/2.0f - 100.0f, GetScreenHeight()/2.0f };
    Vector2 irisRightPosition = { GetScreenWidth()/2.0f + 100.0f, GetScreenHeight()/2.0f };
    float irisRadius = 24;

    float angle = 0.0f;
    float dx = 0.0f, dy = 0.0f, dxx = 0.0f, dyy = 0.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        iris
    }

    CloseWindow();
    return 0;
}