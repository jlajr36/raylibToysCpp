#include "raylib.h"
#include <vector>
#include <cstdlib>

const int w = 2;
int cols, rows;

std::vector<int> grid;
std::vector<float> velocityGrid;
std::vector<int> activeParticles;

float gravity = 0.1f;
float hueValue = 200.0f;

// Helper to convert (x,y) to 1D index
inline int index(int x, int y) { return y * cols + x; }

// Check if a position is inside the grid
bool withinGrid(int x, int y) {
    return x >= 0 && x < cols && y >= 0 && y < rows;
}

int main() {
    InitWindow(600, 500, "Falling Sand");
    SetTargetFPS(60);

    cols = GetScreenWidth() / w;
    rows = GetScreenHeight() / w;

    grid.resize(cols * rows, 0);
    velocityGrid.resize(cols * rows, 1.0f);

    while (!WindowShouldClose()) {

        // Spawn particles on mouse press
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            int mouseCol = GetMouseX() / w;
            int mouseRow = GetMouseY() / w;
            int brushSize = 5;
            int extent = brushSize / 2;

            for (int i = -extent; i <= extent; i++) {
                for (int j = -extent; j <= extent; j++) {
                    if (rand() % 100 < 75) {
                        int col = mouseCol + i;
                        int row = mouseRow + j;
                        if (withinGrid(col, row)) {
                            int idx = index(col, row);
                            grid[idx] = static_cast<int>(hueValue);
                            velocityGrid[idx] = 1.0f;
                            activeParticles.push_back(idx);
                        }
                    }
                }
            }

            // Increment hue for color cycling
            hueValue += 0.5f;
            if (hueValue > 360.0f) hueValue = 1.0f;
        }

        // Update active particles
        std::vector<int> newActiveParticles;

        for (int idx : activeParticles) {
            int i = idx % cols;
            int j = idx / cols;

            int state = grid[idx];
            float velocity = velocityGrid[idx];
            bool moved = false;

            if (state > 0) {
                velocity += gravity;
                int newPos = j + static_cast<int>(velocity);
                if (newPos >= rows) newPos = rows - 1;

                for (int y = newPos; y > j; y--) {
                    int dir = (rand() % 2) ? 1 : -1;

                    // Safely check below and diagonals
                    int below = withinGrid(i, y) ? grid[index(i, y)] : -1;
                    int belowA = withinGrid(i + dir, y) ? grid[index(i + dir, y)] : -1;
                    int belowB = withinGrid(i - dir, y) ? grid[index(i - dir, y)] : -1;

                    if (below == 0) {
                        grid[index(i, y)] = state;
                        velocityGrid[index(i, y)] = velocity;
                        grid[idx] = 0;
                        velocityGrid[idx] = 0.0f;
                        newActiveParticles.push_back(index(i, y));
                        moved = true;
                        break;
                    } else if (belowA == 0) {
                        grid[index(i + dir, y)] = state;
                        velocityGrid[index(i + dir, y)] = velocity;
                        grid[idx] = 0;
                        velocityGrid[idx] = 0.0f;
                        newActiveParticles.push_back(index(i + dir, y));
                        moved = true;
                        break;
                    } else if (belowB == 0) {
                        grid[index(i - dir, y)] = state;
                        velocityGrid[index(i - dir, y)] = velocity;
                        grid[idx] = 0;
                        velocityGrid[idx] = 0.0f;
                        newActiveParticles.push_back(index(i - dir, y));
                        moved = true;
                        break;
                    }
                }
            }

            if (state > 0 && !moved) {
                velocityGrid[idx] = velocity;
                newActiveParticles.push_back(idx);
            }
        }

        activeParticles = newActiveParticles;

        // Render grid
        BeginDrawing();
        ClearBackground(BLACK);

        for (int i = 0; i < cols; i++) {
            for (int j = 0; j < rows; j++) {
                int idx = index(i, j);
                if (grid[idx] > 0) {
                    Color c = ColorFromHSV(static_cast<float>(grid[idx]), 1.0f, 1.0f);
                    DrawRectangle(i * w, j * w, w, w, c);
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}