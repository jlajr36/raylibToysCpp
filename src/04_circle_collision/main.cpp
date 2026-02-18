#include "raylib.h"
#include "raymath.h"
#include <math.h>

int main(void) {
    const int screenWidth = 800;
    const int screenHeigh = 450;

    InitWindow(screenWidth, screenWidth, "Mouse Follow");

    Vector2 sclerLeftPosition = { GetScreenWidth()/2.0f - 100.0f, GetScreenHeight()/2.0f };
    Vector2 sclerRightPosition = { GetScreenWidth()/2.0f + 100.0f, GetScreenHeight()/2.0f };
    float sclerRadius = 80;

    Vector2 irisLeftPosition = sclerLeftPosition;
    Vector2 irisRightPosition = sclerRightPosition;
    float irisRadius = 24;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();

        // left eye
        Vector2 dirLeft = Vector2Subtract(mouse, sclerLeftPosition);
        float lenLeft = Vector2Length(dirLeft);

        if (lenLeft > (sclerRadius - irisRadius)) {
            dirLeft = Vector2Scale(Vector2Normalize(dirLeft), sclerRadius - irisRadius);
        }

        irisLeftPosition = Vector2Add(sclerLeftPosition, dirLeft);

        // right eye
        Vector2 dirRight = Vector2Subtract(mouse, sclerRightPosition);
        float lenRight = Vector2Length(dirRight);

        if (lenRight > (sclerRadius - irisRadius)) {
            dirRight = Vector2Scale(Vector2Normalize(dirRight), sclerRadius - irisRadius);
        }

        irisRightPosition = Vector2Add(sclerRightPosition, dirRight);

        // drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // sclera - white part
        DrawCircleV(sclerLeftPosition, sclerRadius, LIGHTGRAY);
        DrawCircleV(sclerRightPosition, sclerRadius, LIGHTGRAY);

        // iris
        DrawCircleV(irisLeftPosition, irisRadius, BLUE);
        DrawCircleV(irisRightPosition, irisRadius, BLUE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}