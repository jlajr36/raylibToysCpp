/******************************************************************
 * Stack Visualization: Robot Exploration & Backtracking
 *
 * Legend:
 *  R = Robot current cell
 *  S = stack top (next backtracking point)
 *  . = visited cell
 *  # = wall
 *
 * Example 3x3 maze:
 *
 * Initial state:
 * Stack: [(0,0)]
 * R . .
 * . # .
 * . . .
 *
 * Step 1: Move right to (1,0)
 * Stack: [(0,0),(1,0)]
 *   R .
 * . # .
 * . . .
 *
 * Step 2: Move down to (1,1)
 * Stack: [(0,0),(1,0),(1,1)]
 *   . R
 * . # .
 * . . .
 *
 * Step 3: Dead-end → backtrack
 * Stack: [(0,0),(1,0)]    // pop (1,1)
 * Robot moves back to (1,0)
 *
 * Step 4: Explore next neighbor from (1,0)
 * Move down to (1,2)
 * Stack: [(0,0),(1,0),(1,2)]
 * Robot moves to (1,2)
 *
 * Step 5: Another dead-end → backtrack
 * Stack: [(0,0),(1,0)]    // pop (1,2)
 * Robot moves back to (1,0)
 *
 * Step 6: Continue exploring until all cells are visited
 * Stack gradually shrinks as backtracking occurs
 *
 * Key takeaways:
 * - Every time the robot moves to a new unvisited cell, push it onto the stack.
 * - When the robot hits a dead-end (no unvisited neighbors), pop the stack.
 * - The top of the stack always shows the next backtracking point.
 * - Exploration finishes when the stack is empty.
 ******************************************************************/

#include "raylib.h"
#include <vector>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// Each cell in the maze has walls on all sides and a visited flag
struct Cell {
    bool visited = false;
    bool topWall = true;
    bool rightWall = true;
    bool bottomWall = true;
    bool leftWall = true;
};

const int cols = 30;   // number of columns in the maze
const int rows = 15;   // number of rows in the maze
const int cellSize = 40; // size of each cell in pixels

// Directions used for removing walls
enum Direction {TOP, RIGHT, BOTTOM, LEFT};

// State of the program: generating maze, exploring maze, or finished
enum State {GENERATING, EXPLORING, FINISHED};

State state = GENERATING; // start with maze generation

// Check for unvisited neighbors around the current cell
bool getUnvisitedNeighbor(int x, int y,
                          std::vector<std::vector<Cell>>& grid,
                          int& nx, int& ny, Direction& dir)
{
    std::vector<std::pair<std::pair<int,int>, Direction>> neighbors;

    // Add unvisited neighbors to the list
    if (y > 0 && !grid[y-1][x].visited) neighbors.push_back({{x,y-1}, TOP});
    if (x < cols-1 && !grid[y][x+1].visited) neighbors.push_back({{x+1,y}, RIGHT});
    if (y < rows-1 && !grid[y+1][x].visited) neighbors.push_back({{x,y+1}, BOTTOM});
    if (x > 0 && !grid[y][x-1].visited) neighbors.push_back({{x-1,y}, LEFT});

    if (!neighbors.empty()) {
        // Pick a random neighbor to continue maze generation
        int i = rand() % neighbors.size();
        nx = neighbors[i].first.first;
        ny = neighbors[i].first.second;
        dir = neighbors[i].second;
        return true;
    }
    return false; // no unvisited neighbors left
}

// Remove the wall between two adjacent cells
void removeWall(Cell &current, Cell &next, Direction dir) {
    if (dir == TOP) { current.topWall = false; next.bottomWall = false; }
    else if (dir == RIGHT) { current.rightWall = false; next.leftWall = false; }
    else if (dir == BOTTOM) { current.bottomWall = false; next.topWall = false; }
    else if (dir == LEFT) { current.leftWall = false; next.rightWall = false; }
}

// Robot structure for exploring the maze
struct Robot {
    int cellX = 0;  // current cell X coordinate
    int cellY = 0;  // current cell Y coordinate

    Vector2 position; // actual pixel position (for smooth movement)
    Vector2 target;   // target pixel position to move toward

    std::stack<std::pair<int,int>> stack; // stack to keep track of backtracking
    std::vector<std::vector<bool>> visited; // visited cells
};

// Mark the robot's current cell as discovered
void senseCell(int x, int y,
               std::vector<std::vector<Cell>>& grid,
               std::vector<std::vector<Cell>>& known,
               std::vector<std::vector<bool>>& discovered)
{
    known[y][x] = grid[y][x]; // robot “learns” the cell layout
    discovered[y][x] = true;  // mark as explored
}

// Move the robot one step in the maze using DFS logic
bool explorationStep(Robot& robot,
                     std::vector<std::vector<Cell>>& grid,
                     std::vector<std::vector<Cell>>& known,
                     std::vector<std::vector<bool>>& discovered)
{
    int x = robot.cellX;
    int y = robot.cellY;

    // Mark current cell as known
    senseCell(x, y, grid, known, discovered);

    std::vector<std::pair<int,int>> neighbors;
    Cell& c = known[y][x];

    // Check which directions the robot can move (no wall and not visited)
    if (!c.topWall && y > 0 && !robot.visited[y-1][x])
        neighbors.push_back({x, y-1});
    if (!c.rightWall && x < cols-1 && !robot.visited[y][x+1])
        neighbors.push_back({x+1, y});
    if (!c.bottomWall && y < rows-1 && !robot.visited[y+1][x])
        neighbors.push_back({x, y+1});
    if (!c.leftWall && x > 0 && !robot.visited[y][x-1])
        neighbors.push_back({x-1, y});

    if (!neighbors.empty()) {
        // Move to a random unvisited neighbor
        auto [nx, ny] = neighbors[rand() % neighbors.size()];

        // Push the new cell onto the stack for future backtracking
        robot.stack.push({nx, ny});
        robot.visited[ny][nx] = true; // mark as visited

        // Update robot's current cell and target position
        robot.cellX = nx;
        robot.cellY = ny;
        robot.target = {nx * cellSize + cellSize/2.0f,
                        ny * cellSize + cellSize/2.0f};
    } else {
        // No unvisited neighbors → need to backtrack
        // Pop the current cell from the stack
        robot.stack.pop();

        // If stack is empty, robot has explored everything
        if (robot.stack.empty()) return true;

        // Peek at the next cell to backtrack to
        auto [bx, by] = robot.stack.top();

        // Move robot back to this previous cell
        robot.cellX = bx;
        robot.cellY = by;
        robot.target = {bx * cellSize + cellSize/2.0f,
                        by * cellSize + cellSize/2.0f};

        // NOTE: The robot visually moves back over time in the main loop
        //       due to the position lerp toward the target
    }

    return false;
}

int main() {

    srand(time(NULL));

    // Initialize window
    InitWindow(cols * cellSize, rows * cellSize,
               "Autonomous Maze Exploration");
    SetTargetFPS(60);

    // Maze grid and robot knowledge
    std::vector<std::vector<Cell>> grid(rows, std::vector<Cell>(cols));
    std::vector<std::vector<Cell>> known(rows, std::vector<Cell>(cols));
    std::vector<std::vector<bool>> discovered(rows,
        std::vector<bool>(cols, false));

    // Stack for maze generation
    std::stack<std::pair<int,int>> genStack;
    int gx = 0, gy = 0;
    grid[0][0].visited = true;
    genStack.push({0,0});

    Robot robot;

    while (!WindowShouldClose()) {

        // ---------- UPDATE ----------
        if (state == GENERATING) {
            // Maze generation using recursive backtracking
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
                // Maze generation finished → initialize robot
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

            // Timer for robot movement speed
            static float timer = 0;
            timer += GetFrameTime();

            if (timer > 0.05f) {
                bool done = explorationStep(robot, grid,
                                            known, discovered);
                // Check if robot reached goal
                if (robot.cellX == cols-1 &&
                    robot.cellY == rows-1) {
                    state = FINISHED;
                }
                timer = 0;
            }

            // Smoothly move robot toward its target position
            // The *0.2f multiplier creates a lerp effect for smooth animation
            robot.position.x +=
                (robot.target.x - robot.position.x) * 0.2f;
            robot.position.y +=
                (robot.target.y - robot.position.y) * 0.2f;
        }

        // ---------- DRAW ----------
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw maze cells
        for (int y=0;y<rows;y++) {
            for (int x=0;x<cols;x++) {

                int px = x * cellSize;
                int py = y * cellSize;

                Cell& c = grid[y][x];

                // Draw cell walls
                if (c.topWall) DrawLine(px,py,px+cellSize,py,WHITE);
                if (c.rightWall) DrawLine(px+cellSize,py,
                                          px+cellSize,py+cellSize,WHITE);
                if (c.bottomWall) DrawLine(px,py+cellSize,
                                           px+cellSize,py+cellSize,WHITE);
                if (c.leftWall) DrawLine(px,py,
                                         px,py+cellSize,WHITE);

                // Darken unexplored cells
                if (state != GENERATING &&
                    !discovered[y][x]) {
                    DrawRectangle(px,py,cellSize,cellSize,
                                  Fade(BLACK,0.6f));
                }
            }
        }

        // Draw start (green) and end (red) cells
        DrawRectangle(2,2,cellSize-4,cellSize-4,GREEN);
        DrawRectangle(cols*cellSize-cellSize+2,
                      rows*cellSize-cellSize+2,
                      cellSize-4,cellSize-4,RED);

        // Draw robot
        if (state != GENERATING)
            DrawCircleV(robot.position,
                        cellSize/4, ORANGE);

        // Display message when goal is reached
        if (state == FINISHED) {
            DrawText("Goal Reached",
                     20,20,30,YELLOW);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
