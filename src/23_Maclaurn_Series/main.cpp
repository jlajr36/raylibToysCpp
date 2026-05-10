#include "raylib.h"
#include <iostream>
#include <cmath>

enum w_type {
    SIN,
    COS
};

double factorial(int n) {
    double result = 1.0;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

// Clean Maclaurin series
double maclauren(double x, w_type type) {
    double result = (type == COS) ? 1.0 : x;
    int terms = 15;

    for (int n = 1; n < terms; n++) {
        int power = (type == SIN) ? (2 * n + 1) : (2 * n);
        double sign = (n % 2 == 0) ? 1.0 : -1.0;

        result += sign * pow(x, power) / factorial(power);
    }

    return result;
}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Maclaurin Series");
    SetTargetFPS(60);

    double x = 0.5;

    std::cout << "Maclaurin sin: " << maclauren(x, SIN) << '\n';
    std::cout << "std::sin:      " << sin(x) << '\n';
    std::cout << "Maclaurin cos: " << maclauren(x, COS) << '\n';
    std::cout << "std::cos:      " << cos(x) << '\n';

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Maclaurin Series Demo", 20, 20, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}