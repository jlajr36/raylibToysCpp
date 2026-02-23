#include "raylib.h"
#include <vector>
#include <cmath>

// --- Vector3 Math Functions ---
Vector3 Vector3Subtract(const Vector3& a, const Vector3& b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vector3 Vector3Add(const Vector3& a, const Vector3& b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vector3 Vector3Scale(const Vector3& v, float s) {
    return {v.x * s, v.y * s, v.z * s};
}

Vector3 Vector3Normalize(const Vector3& v) {
    float len = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    if (len == 0) return {0,0,0};
    return {v.x/len, v.y/len, v.z/len};
}

float Vector3Distance(const Vector3& a, const Vector3& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return sqrtf(dx*dx + dy*dy + dz*dz);
}

Vector3 Vector3Lerp(const Vector3& a, const Vector3& b, float t) {
    return {a.x + (b.x - a.x)*t,
            a.y + (b.y - a.y)*t,
            a.z + (b.z - a.z)*t};
}

// --- Joint Structure ---
struct Joint {
    Vector3 pos;
};

// --- FABRIK IK Solver ---
void SolveIK(std::vector<Joint>& arm, Vector3 target, Vector3 basePos) {
    const int maxIter = 10;
    const float tolerance = 0.01f;

    std::vector<float> lengths;
    for (size_t i = 0; i < arm.size() - 1; i++)
        lengths.push_back(Vector3Distance(arm[i].pos, arm[i + 1].pos));

    if (Vector3Distance(basePos, target) > lengths[0] + lengths[1]) {
        Vector3 dir = Vector3Normalize(Vector3Subtract(target, basePos));
        arm[1].pos = Vector3Add(basePos, Vector3Scale(dir, lengths[0]));
        arm[2].pos = Vector3Add(arm[1].pos, Vector3Scale(dir, lengths[1]));
        return;
    }

    for (int iter = 0; iter < maxIter; iter++) {
        // Forward
        arm.back().pos = target;
        for (int i = (int)arm.size() - 2; i >= 0; i--) {
            Vector3 dir = Vector3Normalize(Vector3Subtract(arm[i].pos, arm[i + 1].pos));
            arm[i].pos = Vector3Add(arm[i + 1].pos, Vector3Scale(dir, lengths[i]));
            if (arm[i].pos.y < basePos.y) arm[i].pos.y = basePos.y;
        }
        // Backward
        arm[0].pos = basePos;
        for (size_t i = 0; i < arm.size() - 1; i++) {
            Vector3 dir = Vector3Normalize(Vector3Subtract(arm[i + 1].pos, arm[i].pos));
            arm[i + 1].pos = Vector3Add(arm[i].pos, Vector3Scale(dir, lengths[i]));
            if (arm[i + 1].pos.y < basePos.y) arm[i + 1].pos.y = basePos.y;
        }
        if (Vector3Distance(arm.back().pos, target) < tolerance) break;
    }
}

// --- Move target along a triangle path ---
Vector3 GetTriangleTarget(float dt, std::vector<Vector3>& path, int& seg, float& t, float speed) {
    Vector3 start = path[seg];
    Vector3 end = path[seg + 1];
    float dist = Vector3Distance(start, end);
    t += speed * dt / dist;
    if (t >= 1.0f) {
        t = 0.0f;
        seg = (seg + 1) % (path.size() - 1);
        start = path[seg];
        end = path[seg + 1];
    }
    return Vector3Lerp(start, end, t);
}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "IK Robot Simulator");
    SetTargetFPS(60);

    // --- Camera settings ---
    float cameraAngle = 45.0f;
    const float radius = 10.0f;
    const float cameraHeight = 5.0f;
    const float rotationSpeed = 60.0f;

    Camera camera = {0};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 35.0f;

    // --- Robot Arm ---
    float xOffset = 0.0f;
    float zOffset = 0.0f;
    std::vector<Joint> arm(3);
    Vector3 basePos = {xOffset, 0.0f, zOffset};

    arm[0].pos = {xOffset, 0.1f, zOffset};
    arm[1].pos = {xOffset - 1.0f, 1.5f, zOffset};
    arm[2].pos = {xOffset + 1.0f, 2.7f, zOffset};

    // --- Triangle closer and lower in front of robot ---
    std::vector<Vector3> triangle = {
        {basePos.x - 1.0f, 1.5f, basePos.z + 1.5f}, // left-bottom
        {basePos.x + 1.0f, 1.5f, basePos.z + 1.5f}, // right-bottom
        {basePos.x, 3.0f, basePos.z + 1.5f},        // top
        {basePos.x - 1.0f, 1.5f, basePos.z + 1.5f}  // back to start
    };
    int segment = 0;
    float t = 0.0f;
    float triangleSpeed = 1.0f;

    // --- Pen stroke positions ---
    std::vector<Vector3> penStroke;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // --- Camera Orbit ---
        if (IsKeyDown(KEY_RIGHT)) cameraAngle += rotationSpeed * dt;
        if (IsKeyDown(KEY_LEFT)) cameraAngle -= rotationSpeed * dt;
        float rad = cameraAngle * (PI / 180.0f);
        camera.position.x = basePos.x + radius * sin(rad);
        camera.position.y = cameraHeight;
        camera.position.z = basePos.z + radius * cos(rad);

        // Center the robot
        camera.target = {basePos.x, 1.5f, basePos.z};

        // --- Update target ---
        Vector3 target = GetTriangleTarget(dt, triangle, segment, t, triangleSpeed);

        // --- Solve IK ---
        SolveIK(arm, target, basePos);

        // --- Record pen stroke ---
        penStroke.push_back(arm.back().pos);

        // --- Drawing ---
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);

        DrawGrid(20, 1.0f);
        DrawCube(basePos, 1.0f, 0.2f, 1.0f, GRAY);

        // --- Servo caps ---
        DrawCylinder(basePos, 0.2f, 0.2f, 0.05f, 32, DARKGRAY);
        DrawCylinder(arm[0].pos, 0.2f, 0.2f, 0.05f, 32, DARKGRAY);
        DrawCylinder(arm[1].pos, 0.2f, 0.2f, 0.05f, 32, DARKGRAY);

        // --- Arm segments ---
        for (size_t i = 0; i < arm.size() - 1; i++) {
            float brightness = 0.6f + 0.4f * (arm[i].pos.y / 3.0f);
            Color segmentColor = {(unsigned char)(70 * brightness),
                                  (unsigned char)(130 * brightness),
                                  (unsigned char)(180 * brightness), 255};
            DrawCylinderEx(arm[i].pos, arm[i + 1].pos, 0.1f, 0.08f, 32, segmentColor);
        }

        // --- Joints ---
        for (size_t i = 0; i < arm.size() - 1; i++)
            DrawSphere(arm[i].pos, 0.16f, BLUE);

        // End-effector
        DrawSphere(arm[2].pos, 0.18f, RED);
        DrawSphereWires(arm[2].pos, 0.2f, 16, 16, ORANGE);

        // --- Draw pen stroke ---
        for (auto& pos : penStroke)
            DrawSphere(pos, 0.05f, YELLOW);

        EndMode3D();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}