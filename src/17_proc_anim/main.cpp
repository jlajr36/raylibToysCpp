#include "raylib.h"
#include <vector>
#include <cmath>
#include <cstdlib>

// --- HSB to RGB Conversion ---
Color HSBtoRGB(float h, float s, float b) {
    float c = b * s / 100.0f;
    float x = c * (1 - fabs(fmod(h / 60.0f, 2) - 1));
    float m = b - c;
    float r = 0, g = 0, bv = 0;

    if (h < 60) { r = c; g = x; bv = 0; }
    else if (h < 120) { r = x; g = c; bv = 0; }
    else if (h < 180) { r = 0; g = c; bv = x; }
    else if (h < 240) { r = 0; g = x; bv = c; }
    else if (h < 300) { r = x; g = 0; bv = c; }
    else { r = c; g = 0; bv = x; }

    return Color{
        (unsigned char)((r + m) * 255),
        (unsigned char)((g + m) * 255),
        (unsigned char)((bv + m) * 255),
        255
    };
}

// --- Segment Class ---
struct Segment {
    float x, y, angle, distance, radius;
    Segment(float nx, float ny, float a, float d, float r)
        : x(nx), y(ny), angle(a), distance(d), radius(r) {}

    void update(const Segment &prev) {
        angle = atan2(prev.y - y, prev.x - x);
        float d = sqrt((prev.x - x)*(prev.x - x) + (prev.y - y)*(prev.y - y));
        if (d > distance) {
            float delta = d - distance;
            x += delta * cos(angle);
            y += delta * sin(angle);
        }
    }

    void display(float H, float S, float B) {
        Color c = HSBtoRGB(H, S, B);
        DrawCircle((int)x, (int)y, radius, c);
    }
};

// --- Creature Class ---
struct Creature {
    std::vector<Segment> body;
    int len = 12;
    float speed;
    float BODY_H, BODY_S, BODY_B, FIN_H, FIN_S, FIN_B;

    Creature(int screenWidth, int screenHeight, float palette[6]) {
        BODY_H = palette[0]; BODY_S = palette[1]; BODY_B = palette[2];
        FIN_H = palette[3]; FIN_S = palette[4]; FIN_B = palette[5];

        speed = 2.0f + static_cast<float>(rand()) / RAND_MAX * 2.0f; // 2 - 4

        float r1 = 6.0f;
        for (int i = 0; i < len; i++) {
            float r = r1 - i * (r1 / (len - 1));
            body.emplace_back(rand() % screenWidth, rand() % screenHeight,
                              static_cast<float>(rand()) / RAND_MAX * 2.0f * PI,
                              r, r);
        }
    }

    void applyBoidRules(const std::vector<Creature> &others) {
        float neighDist = 50.0f; // reasonable neighborhood
        float ax=0, ay=0, cx=0, cy=0, sx=0, sy=0;
        int count=0;

        Segment &head = body[0];

        for (const auto &other : others) {
            if (&other == this) continue;
            const Segment &oHead = other.body[0];
            float dx = oHead.x - head.x;
            float dy = oHead.y - head.y;
            float d = sqrt(dx*dx + dy*dy);
            if (d < neighDist && d > 0.001f) {
                ax += cos(oHead.angle);
                ay += sin(oHead.angle);
                cx += oHead.x;
                cy += oHead.y;
                sx += (head.x - oHead.x)/d;  // stronger, distance-sensitive separation
                sy += (head.y - oHead.y)/d;
                count++;
            }
        }

        if (count > 0) {
            ax /= count; ay /= count;
            cx = cx / count - head.x; cy = cy / count - head.y;

            // apply tweaked multipliers
            ax *= 0.01f; ay *= 0.01f;  // weaker alignment
            cx *= 0.005f; cy *= 0.005f; // very weak cohesion
            sx *= 0.25f; sy *= 0.25f;   // strong separation

            float fx = ax + cx + sx;
            float fy = ay + cy + sy;
            float desiredAngle = atan2(fy, fx);
            float delta = desiredAngle - head.angle;
            while (delta > PI) delta -= 2*PI;
            while (delta < -PI) delta += 2*PI;
            head.angle += delta * 0.05f;

            // tiny random jitter to break perfect alignment
            head.angle += ((rand()%21)-10)*0.001f;
        }
    }

    void update(int screenWidth, int screenHeight) {
        Segment &head = body[0];
        head.x += speed * cos(head.angle);
        head.y += speed * sin(head.angle);

        // Bounce off edges
        if (head.x < 0) head.angle = PI - head.angle;
        if (head.x > screenWidth) head.angle = PI - head.angle;
        if (head.y < 0) head.angle = -head.angle;
        if (head.y > screenHeight) head.angle = -head.angle;

        for (int i = 1; i < len; i++) {
            body[i].update(body[i - 1]);
        }
    }

    void display() {
        // Body
        for (int i = 0; i < len; i++) body[i].display(BODY_H, BODY_S, BODY_B);

        // Fins
        for (int i = 1; i < 4; i++) {
            Segment &s = body[i];
            float x0 = s.x, y0 = s.y;
            float x1 = x0 - 3*s.radius*cos(s.angle);
            float y1 = y0 - 3*s.radius*sin(s.angle);
            Color c = HSBtoRGB(FIN_H, FIN_S, FIN_B);
            DrawLine((int)x0, (int)y0, (int)x1, (int)y1, c);
        }
    }
};

// --- Main Program ---
int main() {
    const int screenWidth = 1200;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Procedural Animated Boids");
    SetTargetFPS(60);

    // Color palettes: {Body H,S,B, Fin H,S,B}
    float palettes[3][6] = {
        {180, 60, 100, 50, 80, 90},
        {210, 70, 70, 0, 80, 85},
        {270, 30, 95, 330, 60, 85}
    };

    // Create creatures
    std::vector<Creature> school;
    for (int i = 0; i < 200; i++) {
        int p = rand() % 3;
        school.emplace_back(screenWidth, screenHeight, palettes[p]);
    }

    // Main loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        for (auto &c : school) {
            c.applyBoidRules(school);
            c.update(screenWidth, screenHeight);
            c.display();
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
