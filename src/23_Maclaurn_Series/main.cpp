#include "raylib.h"
#include <iostream>
#include <vector>
#include <cmath>

enum w_type {
    SIN,
    COS
};

std::vector<int> odds(int count) {
    std::vector<int> values;
    for (int i = 0; i < count; i++) {
        values.push_back(2*i+1);
    }
    return values;
}

std::vector<int> evens(int count) {
    std::vector<int> values;
    for (int i = 0; i < count; i++) {
        values.push_back(2*(i+1));
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

double maclauren(double x, w_type w) {
    double value = 0.0;
    if (w == COS) {value = 1.0;}
    double numerator = 0.0;
    double denominator = 0.0;
    double term = 0.0;
    double sign = 0.0;
    std::vector<int> values;
    if (w == SIN) {
        values = odds(15);
    } else {
        values = evens(15);
    }
    for (int i = 0; i < values.size(); i++) {
        numerator = pow(x, values[i]);
        denominator = factorial(values[i]);
        term = numerator / denominator;
        sign = (values[i] / 2 % 2 == 0) ? 1.0 : -1.0;
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

    std::cout << "Maclauren sin: " << maclauren(0.5, SIN) << '\n';
    std::cout << "sin:           " << sin(0.5) << '\n';
    std::cout << "Maclauren cos: " << maclauren(0.5, COS) << '\n';
    std::cout << "cos:           " << cos(0.5) << '\n';

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
