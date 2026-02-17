#include "raylib.h"
#include <vector>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <algorithm>

struct Cell {
    bool visited = false;
    bool topWall = true;
    bool rightWall = true;
    bool bottomWall = true;
    bool leftWall = true;
};

const int cols = 30;
const int rows = 15;
const int cellSize = 40;

enum Direction {TOP, RIGHT, BOTTOM, LEFT};
enum State {GENERATING, EXPLORING, FINISHED};

State state = GENERATING;

bool getUnvisitedNeighbor(int x, int y,
                          std::vector<std::vector<Cell>>& grid,
                          int& nx, int& ny, Direction& dir)
{
    std::vector<std::pair<std::pair<int,int>, Direction>> neighbors;

    if (y > 0 && !grid[y-1][x].visited) neighbors.push_back({{x,y-1}, TOP});
    if (x < cols-1 && !grid[y][x+1].visited) neighbors.push_back({{x+1,y}, RIGHT});
    if (y < rows-1 && !grid[y+1][x].visited) neighbors.push_back({{x,y+1}, BOTTOM});
    if (x > 0 && !grid[y][x-1].visited) neighbors.push_back({{x-1,y}, LEFT});

    if (!neighbors.empty()) {
        int i = rand() % neighbors.size();
        nx = neighbors[i].first.first;
        ny = neighbors[i].first.second;
        dir = neighbors[i].second;
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

struct Robot {
    int cellX = 0;
    int cellY = 0;

    Vector2 position;
    Vector2 target;

    std::stack<std::pair<int,int>> stack;
    std::vector<std::vector<bool>> visited;
};

void senseCell(int x, int y,
               std::vector<std::vector<Cell>>& grid,
               std::vector<std::vector<Cell>>& known,
               std::vector<std::vector<bool>>& discovered)
{
    known[y][x] = grid[y][x];
    discovered[y][x] = true;
}

bool explorationStep(Robot& robot,
                     std::vector<std::vector<Cell>>& grid,
                     std::vector<std::vector<Cell>>& known,
                     std::vector<std::vector<bool>>& discovered)
{
    int x = robot.cellX;
    int y = robot.cellY;

    senseCell(x, y, grid, known, discovered);

    std::vector<std::pair<int,int>> neighbors;
    Cell& c = known[y][x];

    if (!c.topWall && y > 0 && !robot.visited[y-1][x])
        neighbors.push_back({x, y-1});
    if (!c.rightWall && x < cols-1 && !robot.visited[y][x+1])
        neighbors.push_back({x+1, y});
    if (!c.bottomWall && y < rows-1 && !robot.visited[y+1][x])
        neighbors.push_back({x, y+1});
    if (!c.leftWall && x > 0 && !robot.visited[y][x-1])
        neighbors.push_back({x-1, y});

    if (!neighbors.empty()) {
        auto [nx, ny] = neighbors[rand() % neighbors.size()];
        robot.stack.push({nx, ny});
        robot.visited[ny][nx] = true;

        robot.cellX = nx;
        robot.cellY = ny;
        robot.target = {nx * cellSize + cellSize/2.0f,
                        ny * cellSize + cellSize/2.0f};
    } else {
        robot.stack.pop();
        if (robot.stack.empty()) return true;

        auto [bx, by] = robot.stack.top();
        robot.cellX = bx;
        robot.cellY = by;
        robot.target = {bx * cellSize + cellSize/2.0f,
                        by * cellSize + cellSize/2.0f};
    }

    return false;
}

int main() {

    srand(time(NULL));

    InitWindow(cols * cellSize, rows * cellSize,
               "Autonomous Maze Exploration");
    SetTargetFPS(60);

    std::vector<std::vector<Cell>> grid(rows, std::vector<Cell>(cols));
    std::vector<std::vector<Cell>> known(rows, std::vector<Cell>(cols));
    std::vector<std::vector<bool>> discovered(rows,
        std::vector<bool>(cols, false));

    std::stack<std::pair<int,int>> genStack;
    int gx = 0, gy = 0;
    grid[0][0].visited = true;
    genStack.push({0,0});

    Robot robot;

    while (!WindowShouldClose()) {

        // ---------- UPDATE ----------
        if (state == GENERATING) {

            if (!genStack.empty()) {
                auto [x,y] = genStack.top();
                int nx, ny;
                Direction dir;

                if (getUnvisitedNeighbor(x,y,grid,nx,ny,dir)) {
                    grid[ny][nx].visited = true;
                    removeWall(grid[y][x], grid[ny][nx], dir);
                    genStack.push({nx,ny});
                } else {
                    genStack.pop();
                }
            } else {
                // generation complete → initialize robot
                robot.cellX = 0;
                robot.cellY = 0;
                robot.position = {cellSize/2.0f, cellSize/2.0f};
                robot.target = robot.position;
                robot.visited =
                    std::vector<std::vector<bool>>(rows,
                        std::vector<bool>(cols,false));
                robot.visited[0][0] = true;
                robot.stack.push({0,0});

                state = EXPLORING;
            }
        }

        else if (state == EXPLORING) {

            static float timer = 0;
            timer += GetFrameTime();

            if (timer > 0.05f) {
                bool done = explorationStep(robot, grid,
                                            known, discovered);
                if (robot.cellX == cols-1 &&
                    robot.cellY == rows-1) {
                    state = FINISHED;
                }
                timer = 0;
            }

            robot.position.x +=
                (robot.target.x - robot.position.x) * 0.2f;
            robot.position.y +=
                (robot.target.y - robot.position.y) * 0.2f;
        }

        // ---------- DRAW ----------
        BeginDrawing();
        ClearBackground(BLACK);

        for (int y=0;y<rows;y++) {
            for (int x=0;x<cols;x++) {

                int px = x * cellSize;
                int py = y * cellSize;

                Cell& c = grid[y][x];

                if (c.topWall) DrawLine(px,py,px+cellSize,py,WHITE);
                if (c.rightWall) DrawLine(px+cellSize,py,
                                          px+cellSize,py+cellSize,WHITE);
                if (c.bottomWall) DrawLine(px,py+cellSize,
                                           px+cellSize,py+cellSize,WHITE);
                if (c.leftWall) DrawLine(px,py,
                                         px,py+cellSize,WHITE);

                if (state != GENERATING &&
                    !discovered[y][x]) {
                    DrawRectangle(px,py,cellSize,cellSize,
                                  Fade(BLACK,0.6f));
                }
            }
        }

        DrawRectangle(2,2,cellSize-4,cellSize-4,GREEN);
        DrawRectangle(cols*cellSize-cellSize+2,
                      rows*cellSize-cellSize+2,
                      cellSize-4,cellSize-4,RED);

        if (state != GENERATING)
            DrawCircleV(robot.position,
                        cellSize/4, ORANGE);

        if (state == FINISHED) {
            DrawText("Goal Reached",
                     20,20,30,YELLOW);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
