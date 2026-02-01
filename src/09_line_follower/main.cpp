#include "raylib.h"
#include <vector>
#include <cmath>

// --- Vector helper functions ---
// Add two Vector2 structs
Vector2 Vector2Add(Vector2 a, Vector2 b) { return { a.x + b.x, a.y + b.y }; }
// Scale a Vector2 by a scalar
Vector2 Vector2Scale(Vector2 v, float s) { return { v.x * s, v.y * s }; }

// --- Robot struct ---
// Represents the robot's position, heading, and movement speed
struct Robot {
    Vector2 pos;   // Current position of the robot
    float angle;   // Heading angle in radians (0 = facing right)
    float speed;   // Movement speed in pixels per frame
};

// --- Cubic Bezier function ---
// Calculate a point along a cubic Bezier curve given t in [0,1]
// p0 = start point, p1/p2 = control points, p3 = end point
Vector2 CubicBezier(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, float t) {
    float u = 1.0f - t;
    float tt = t*t, uu = u*u, uuu = uu*u, ttt = tt*t;
    Vector2 p = {0, 0};
    p.x = uuu*p0.x + 3*uu*t*p1.x + 3*u*tt*p2.x + ttt*p3.x;
    p.y = uuu*p0.y + 3*uu*t*p1.y + 3*u*tt*p2.y + ttt*p3.y;
    return p;
}

// --- Line detection ---
// Check if a given pixel position corresponds to the black path
bool IsOnLine(Vector2 p, Image &lineImage) {
    // If the point is out of bounds, consider it off the line
    if (p.x < 0 || p.y < 0 || p.x >= lineImage.width || p.y >= lineImage.height)
        return false;
    Color c = GetImageColor(lineImage, (int)p.x, (int)p.y);
    // Consider any sufficiently dark pixel as part of the path
    return (c.r < 50 && c.g < 50 && c.b < 50);
}

// --- Line-following sensor ---
// Computes the average horizontal offset of the path under multiple forward-facing sensors
float LinePositionError(Robot &bot, Image &lineImage, int numSamples = 7, float scanWidth = 20.0f, float lookAhead = 15.0f) {
    // Forward direction vector based on current heading
    Vector2 forward = { cosf(bot.angle), sinf(bot.angle) };
    // Right vector perpendicular to forward direction
    Vector2 right = { -forward.y, forward.x };

    float errorSum = 0.0f;
    int hitCount = 0;

    // Sample multiple points across the width of the sensor line
    for (int i = 0; i < numSamples; i++) {
        float offset = ((float)i / (numSamples - 1) - 0.5f) * scanWidth; // evenly spaced left to right
        Vector2 samplePos = Vector2Add(bot.pos, Vector2Scale(forward, lookAhead)); // forward
        samplePos = Vector2Add(samplePos, Vector2Scale(right, offset));            // offset sideways

        if (IsOnLine(samplePos, lineImage)) {
            errorSum += offset;
            hitCount++;
        }
    }

    if (hitCount == 0) return 0.0f; // no line detected
    return errorSum / hitCount;      // return average horizontal offset as error
}

int main() {
    const int screenW = 1024;
    const int screenH = 775;

    InitWindow(screenW, screenH, "Robot Following Exact Bezier Path");
    SetTargetFPS(60);

    // --- Generate the exact path points (copied from Code 1) ---
    std::vector<Vector2> pathPoints;
    pathPoints.push_back({100, 100});   // Start point
    pathPoints.push_back({400, 100});   // Horizontal line to the right
    pathPoints.push_back({400, 300});   // Vertical drop (sharp turn down)
    pathPoints.push_back({350, 300});   // Small left turn
    pathPoints.push_back({150, 300});   // Long horizontal left

    // First cubic Bezier curve (smooth curved section)
    Vector2 b0 = pathPoints.back();
    Vector2 b1 = {100, 300}, b2 = {50, 450}, b3 = {250, 500};
    const int BEZIER_STEPS = 40; // number of points to interpolate
    for(int i=1;i<=BEZIER_STEPS;i++){
        float t = i/(float)BEZIER_STEPS;
        pathPoints.push_back(CubicBezier(b0,b1,b2,b3,t));
    }

    // Continue with straight segments
    pathPoints.push_back({500,520});
    pathPoints.push_back({500,380});
    pathPoints.push_back({700,380});
    pathPoints.push_back({900,200});

    // Vertical line upwards
    Vector2 last = pathPoints.back();
    pathPoints.push_back({ last.x, last.y - 200 });

    // Final cubic Bezier curve at the end
    Vector2 endStart = pathPoints.back();
    Vector2 endCtrl1 = { endStart.x - 60, endStart.y - 40 };
    Vector2 endCtrl2 = { endStart.x - 160, endStart.y + 80 };
    Vector2 endTarget = { endStart.x - 220, endStart.y + 40 };
    for(int i=1;i<=BEZIER_STEPS;i++){
        float t = i/(float)BEZIER_STEPS;
        pathPoints.push_back(CubicBezier(endStart,endCtrl1,endCtrl2,endTarget,t));
    }

    // Add vertical and horizontal lines to close the path
    last = pathPoints.back();
    pathPoints.push_back({ last.x, last.y - 160 });       // vertical up
    Vector2 leftEnd = { last.x - 590, last.y };
    pathPoints.push_back(leftEnd);                         // horizontal left
    Vector2 start = pathPoints.front();
    pathPoints.push_back({ leftEnd.x, leftEnd.y + (start.y - leftEnd.y) }); // vertical down to start

    // --- Center the entire path in the window ---
    float minX=1e9f, minY=1e9f, maxX=-1e9f, maxY=-1e9f;
    for(auto &pt:pathPoints){
        if(pt.x<minX) minX=pt.x;
        if(pt.y<minY) minY=pt.y;
        if(pt.x>maxX) maxX=pt.x;
        if(pt.y>maxY) maxY=pt.y;
    }
    float dx = screenW/2.0f - (minX+maxX)/2.0f;
    float dy = screenH/2.0f - (minY+maxY)/2.0f;
    for(auto &pt:pathPoints){ pt.x+=dx; pt.y+=dy; }

    // --- Draw the path to an image for pixel-perfect robot detection ---
    Image lineImage = GenImageColor(screenW, screenH, WHITE); // white background
    const float PATH_THICK = 18.0f;                            // thickness of path
    for(size_t i=1;i<pathPoints.size();i++){
        Vector2 diff = { pathPoints[i].x - pathPoints[i-1].x, pathPoints[i].y - pathPoints[i-1].y };
        float length = sqrtf(diff.x*diff.x + diff.y*diff.y);
        int steps = (int)length;
        // Draw circles along each line segment to maintain consistent thickness
        for(int s=0;s<=steps;s++){
            float t = s/(float)steps;
            int px = (int)(pathPoints[i-1].x + diff.x * t);
            int py = (int)(pathPoints[i-1].y + diff.y * t);
            ImageDrawCircle(&lineImage, px, py, PATH_THICK/2, BLACK);
        }
    }

    Texture2D lineTexture = LoadTextureFromImage(lineImage);

    // --- Initialize the robot ---
    Robot bot;
    bot.pos = { 50.0f, 50.0f }; // starting off the path
    bot.angle = 0.0f;
    bot.speed = 2.0f;

    bool foundLine = false; // has the robot found the path yet?
    float Kp = 0.06f;       // proportional gain for line-following steering

    // --- Main loop ---
    while(!WindowShouldClose()){
        // Allow repositioning the robot with the mouse
        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            bot.pos = GetMousePosition();
            foundLine = false; // need to reacquire the path
        }

        if(!foundLine){
            // If the robot hasn't found the path yet, move it toward the center
            Vector2 toCenter = { screenW/2.0f - bot.pos.x, screenH/2.0f - bot.pos.y };
            float dist = sqrtf(toCenter.x*toCenter.x + toCenter.y*toCenter.y);
            if(dist>1.0f){
                bot.angle = atan2f(toCenter.y, toCenter.x);
                bot.pos.x += cosf(bot.angle) * bot.speed;
                bot.pos.y += sinf(bot.angle) * bot.speed;
            }
            // Check if we reached the path
            if(IsOnLine(bot.pos, lineImage))
                foundLine = true;
        } else {
            // --- Normal line-following behavior ---
            float error = LinePositionError(bot, lineImage, 9, 28.0f, 15.0f); // compute lateral error
            bot.angle += error * Kp;  // adjust heading
            bot.pos.x += cosf(bot.angle) * bot.speed;
            bot.pos.y += sinf(bot.angle) * bot.speed;
        }

        // --- Drawing ---
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(lineTexture, 0, 0, WHITE);

        // Draw the robot as a blue circle with red forward line
        Vector2 forward = { cosf(bot.angle), sinf(bot.angle) };
        DrawCircleV(bot.pos, 10, BLUE);
        DrawLineV(bot.pos, Vector2Add(bot.pos, Vector2Scale(forward, 20)), RED);

        // Draw sensor visualization (green if over path, gray otherwise)
        if(foundLine){
            Vector2 rightVec = { -forward.y, forward.x };
            for(int i=0;i<9;i++){
                float offset = ((float)i/8 - 0.5f)*28.0f;
                Vector2 samplePos = Vector2Add(bot.pos, Vector2Scale(forward, 15.0f));
                samplePos = Vector2Add(samplePos, Vector2Scale(rightVec, offset));
                DrawPixelV(samplePos, IsOnLine(samplePos, lineImage) ? GREEN : GRAY);
            }
        }

        DrawText("Robot Following Exact Bezier Path", 10,10,20,DARKGRAY);
        EndDrawing();
    }

    // --- Cleanup ---
    UnloadTexture(lineTexture);
    UnloadImage(lineImage);
    CloseWindow();
    return 0;
}
