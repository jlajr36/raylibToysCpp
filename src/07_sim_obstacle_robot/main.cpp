#include "raylib.h"
#include <cmath>
#include <vector>
#include <limits>

// ---------- Helpers ----------
float ClampFloat(float value, float minVal, float maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

float LerpFloat(float a, float b, float t) {
    return a + t * (b - a);
}

float Vector2Dist(Vector2 a, Vector2 b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

// ---------------- ROBOT ----------------
struct Robot {
    Vector2 pos;
    float heading;
    float width;

    float vl, vr;
    float minSpeed, maxSpeed;

    float minObsDist;
    float countDown;

    Robot(Vector2 start, float w) {
        pos = start;
        heading = 0.0f;
        width = w;

        minSpeed = 40.0f;
        maxSpeed = 80.0f;
        vl = vr = minSpeed;

        minObsDist = 100.0f;
        countDown = 5.0f;
    }

    void MoveForward() { vl = vr = minSpeed; }
    void MoveBackward() { vr = -minSpeed; vl = -minSpeed * 0.5f; }

    void Kinematics(float dt) {
        float v = (vl + vr) * 0.5f;
        pos.x += v * cosf(heading) * dt;
        pos.y -= v * sinf(heading) * dt;
        heading += (vr - vl) / width * dt;
        heading = fmodf(heading, 2 * PI);

        vl = ClampFloat(vl, -maxSpeed, maxSpeed);
        vr = ClampFloat(vr, -maxSpeed, maxSpeed);
    }

    void AvoidObstacles(const std::vector<Vector2>& points, float dt) {
        float closest = std::numeric_limits<float>::max();
        for (auto& p : points) {
            float d = Vector2Dist(pos, p);
            if (d < closest) closest = d;
        }

        if (closest < minObsDist && countDown > 0) {
            countDown -= dt;
            MoveBackward();
        } else {
            countDown = 5.0f;
            MoveForward();
        }
    }
};

// ---------------- SENSOR ----------------
struct SensorRay {
    Vector2 start;
    Vector2 end;
    bool hitObstacle;
};

std::vector<SensorRay> SenseObstacles(const Robot& robot, Image& mapImg, Color* pixels, float range, float fov) {
    std::vector<SensorRay> rays;
    int w = mapImg.width;
    int h = mapImg.height;

    float startAngle = robot.heading - fov;
    float endAngle = robot.heading + fov;
    int rayCount = 10;

    for (int r = 0; r < rayCount; r++) {
        float angle = LerpFloat(startAngle, endAngle, r / (float)rayCount);
        Vector2 endPt = { robot.pos.x + range * cosf(angle),
                          robot.pos.y - range * sinf(angle) };

        SensorRay ray;
        ray.start = robot.pos;
        ray.end = endPt;
        ray.hitObstacle = false;

        for (int i = 0; i <= 100; i++) {
            float u = i / 100.0f;
            int x = (int)LerpFloat(robot.pos.x, endPt.x, u);
            int y = (int)LerpFloat(robot.pos.y, endPt.y, u);

            if (x < 0 || y < 0 || x >= w || y >= h) break;

            Color c = pixels[y * w + x];
            if (c.r == 0 && c.g == 0 && c.b == 0) {
                ray.end = { (float)x, (float)y };
                ray.hitObstacle = true;
                break;
            }
        }

        rays.push_back(ray);
    }

    return rays;
}

// ---------------- MAIN ----------------
int main() {
    const int SCREEN_W = 1200;
    const int SCREEN_H = 600;

    InitWindow(SCREEN_W, SCREEN_H, "Obstacle Avoidance Robot");
    SetTargetFPS(60);

    Texture2D mapTex = LoadTexture("assets/background.png");
    Texture2D botTex = LoadTexture("assets/bot.png");

    if (mapTex.id == 0 || botTex.id == 0) {
        TraceLog(LOG_ERROR, "Failed to load textures! Make sure files exist in assets/");
        CloseWindow();
        return 1;
    }

    Image mapImg = LoadImageFromTexture(mapTex);
    Color* pixels = LoadImageColors(mapImg);

    Robot robot({200, 200}, 40.0f);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        auto rays = SenseObstacles(robot, mapImg, pixels, 250.0f, DEG2RAD * 40);

        // Extract obstacle points for avoidance
        std::vector<Vector2> points;
        for (auto& r : rays)
            if (r.hitObstacle) points.push_back(r.end);

        robot.AvoidObstacles(points, dt);
        robot.Kinematics(dt);

        BeginDrawing();
        ClearBackground(WHITE);

        DrawTexture(mapTex, 0, 0, WHITE);

        // Draw sensor rays
        for (auto& r : rays) {
            DrawLineV(r.start, r.end, (Color){0, 0, 255, 150}); // semi-transparent blue
            if (r.hitObstacle) DrawCircleV(r.end, 3, RED);       // red obstacle points
        }

        DrawTexturePro(
            botTex,
            {0, 0, (float)botTex.width, (float)botTex.height},
            {robot.pos.x, robot.pos.y, (float)botTex.width, (float)botTex.height},
            {botTex.width / 2.0f, botTex.height / 2.0f},
            -RAD2DEG * robot.heading,
            WHITE
        );

        EndDrawing();
    }

    UnloadImageColors(pixels);
    UnloadImage(mapImg);
    UnloadTexture(mapTex);
    UnloadTexture(botTex);
    CloseWindow();
    return 0;
}
