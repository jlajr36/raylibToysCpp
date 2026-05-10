#include "raylib.h"
#include <iostream>
#include <vector>
#include <cmath>

std::vector<int> odds(int count) {
    std::vector<int> values;
    for (int i = 0; i < count; i++) {
        values.push_back(2*i+1);
    }
    return values;
}

double factorial(int value) {
    double _factorial = 1;
    for (int i = value; i >= 1; i--) {
        _factorial *= i;
    }
    return _factorial;
}

double maclauren(double x) {
    double value = 0.0;
    double numerator = 0.0;
    double denominator = 0.0;
    double term = 0.0;
    double sign = 0.0;
    std::vector<int> odd_values = odds(15);
    for (int i = 0; i < odd_values.size(); i++) {
        numerator = pow(x, odd_values[i]);
        denominator = factorial(odd_values[i]);
        term = numerator / denominator;
        sign = (i % 2 == 0) ? 1.0 : -1.0;
        value += (sign * term);
    }
    return value;
}

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Maclaurn Series");
    SetTargetFPS(60);

    std::cout << "Maclauren: " << maclauren(0.5) << '\n';
    std::cout << "sin:       " << sin(0.5) << '\n';

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
