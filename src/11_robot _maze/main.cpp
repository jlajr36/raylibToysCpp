#include "raylib.h"
#include <vector>
#include <stack>
#include <cstdlib>
#include <ctime>

struct Cell {
    bool visited = false;
    bool topWall = true;
    bool rightWall = true;
    bool bottomWall = true;
    bool leftWall = true;
};

// Maze dimensions
const int cols = 30;  // wider than tall
const int rows = 15;
const int cellSize = 40;

enum Direction {TOP, RIGHT, BOTTOM, LEFT};

bool getUnvisitedNeighbor(int x, int y, const std::vector<std::vector<Cell>>& grid, int& nx, int& ny, Direction& dir) {
    std::vector<std::pair<std::pair<int,int>, Direction>> neighbors;

    if (y > 0 && !grid[y-1][x].visited) neighbors.push_back({{x, y-1}, TOP});
    if (x < cols-1 && !grid[y][x+1].visited) neighbors.push_back({{x+1, y}, RIGHT});
    if (y < rows-1 && !grid[y+1][x].visited) neighbors.push_back({{x, y+1}, BOTTOM});
    if (x > 0 && !grid[y][x-1].visited) neighbors.push_back({{x-1, y}, LEFT});

    if (!neighbors.empty()) {
        int index = rand() % neighbors.size();
        nx = neighbors[index].first.first;
        ny = neighbors[index].first.second;
        dir = neighbors[index].second;
        return true;
    }
    return false;
}

void removeWall(Cell &current, Cell &next, Direction dir) {
    if (dir == TOP) { current.topWall = false; next.bottomWall = false; }
    else if (dir == RIGHT) { current.rightWall = false; next.leftWall = false; }
    else if (dir == BOTTOM) { current.bottomWall = false; next.topWall = false; }
    else if (dir == LEFT) { current.leftWall = false; next.rightWall = false; }
}

int main() {
    srand(time(NULL));
    InitWindow(cols * cellSize, rows * cellSize, "Maze Generator");

    std::vector<std::vector<Cell>> grid(rows, std::vector<Cell>(cols));

    // Maze generation using stack (recursive backtracking)
    std::stack<std::pair<int,int>> stack;
    int cx = 0, cy = 0;
    grid[cy][cx].visited = true;
    stack.push({cx, cy});

    while (!stack.empty()) {
        auto [x, y] = stack.top();
        int nx, ny;
        Direction dir;
        if (getUnvisitedNeighbor(x, y, grid, nx, ny, dir)) {
            grid[ny][nx].visited = true;
            removeWall(grid[y][x], grid[ny][nx], dir);
            stack.push({nx, ny});
        } else {
            stack.pop();
        }
    }

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw maze
        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                int px = x * cellSize;
                int py = y * cellSize;
                Cell &c = grid[y][x];

                if (c.topWall) DrawLine(px, py, px + cellSize, py, BLACK);
                if (c.rightWall) DrawLine(px + cellSize, py, px + cellSize, py + cellSize, BLACK);
                if (c.bottomWall) DrawLine(px, py + cellSize, px + cellSize, py + cellSize, BLACK);
                if (c.leftWall) DrawLine(px, py, px, py + cellSize, BLACK);
            }
        }

        // Draw start and end
        DrawRectangle(2, 2, cellSize-4, cellSize-4, GREEN); // start
        DrawRectangle(cols*cellSize-cellSize+2, rows*cellSize-cellSize+2, cellSize-4, cellSize-4, RED); // end

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
