#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <cmath>

// --- OpenSimplex / Perlin Noise Logic ---
float Noise2D(float x, float y) {
    int ix = (int)floorf(x);
    int iy = (int)floorf(y);
    float fx = x - (float)ix;
    float fy = y - (float)iy;

    auto fade = [](float t) { return t * t * t * (t * (t * 6 - 15) + 10); };
    auto lerp = [](float t, float a, float b) { return a + t * (b - a); };
    auto grad = [](int hash, float x, float y) {
        int h = hash & 15;
        float u = h < 8 ? x : y;
        float v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    };

    auto p = [](int i) {
        static const int perm[] = { 151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,49,192,256,31,181,199,106,180,150,215,153 };
        return perm[i & 255];
    };

    int A = p(ix) + iy, AA = p(A), AB = p(A + 1);
    int B = p(ix + 1) + iy, BA = p(B), BB = p(B + 1);

    return lerp(fade(fy), lerp(fade(fx), grad(p(AA), fx, fy), grad(p(BA), fx - 1, fy)),
                          lerp(fade(fx), grad(p(AB), fx, fy - 1), grad(p(BB), fx - 1, fy - 1)));
}

float FractalNoise(float v) {
    float amp = 1.0f, f = 1.0f, sum = 0.0f;
    for (int i = 0; i < 6; i++) {
        amp *= 0.5f;
        sum += amp * (Noise2D(v * f, 0) + 1.0f) * 0.5f;
        f *= 2.0f;
    }
    return sum;
}

// --- Interactive Point Structure ---
struct ControlPoint {
    Vector2 pos;
    float baseRadius = 7.0f;
    float currentRadius = 7.0f;
    bool isDragging = false;

    void Update() {
        Vector2 mouse = GetMousePosition();
        bool isHovered = CheckCollisionPointCircle(mouse, pos, 30.0f);

        float targetRadius = isHovered ? baseRadius * 2.0f : baseRadius;
        currentRadius += (targetRadius - currentRadius) * 0.1f;

        if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) isDragging = true;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) isDragging = false;

        if (isDragging) pos = mouse;
    }

    void Draw() {
        // Corrected Draw Calls
        DrawCircleGradient((int)pos.x, (int)pos.y, currentRadius * 3.0f, Fade(WHITE, 0.4f), BLANK);
        DrawCircleV(pos, currentRadius, WHITE);
        DrawCircleV(pos, currentRadius * 0.6f, (Color){ 20, 20, 20, 255 });
    }
};

// --- Lightning Rendering Logic ---
struct Lightning {
    float off = 0.0f;

    void Draw(Vector2 start, Vector2 end) {
        float length = Vector2Distance(start, end);
        int stepCount = (int)fmaxf(5, ceilf(length / 10.0f));
        Vector2 diff = Vector2Subtract(end, start);
        float angle = atan2f(diff.y, diff.x);

        off += 0.025f;
        float waveWidth = fminf(length, 750.0f);

        std::vector<Vector2> points;
        for (int i = 0; i <= stepCount; i++) {
            float t = (float)i / stepCount;
            float n = (float)i / 60.0f;
            float m = sinf(PI * t);

            float displacement = (FractalNoise(n - off) - FractalNoise(n + off)) * waveWidth * 0.5f;

            Vector2 p = Vector2Add(start, Vector2Scale(diff, t));
            p.x += cosf(angle + PI/2.0f) * displacement * m;
            p.y += sinf(angle + PI/2.0f) * displacement * m;
            points.push_back(p);
        }

        // Apply additive blending for that "high-fidelity" glow
        BeginBlendMode(BLEND_ADDITIVE);
        for (size_t i = 0; i < points.size() - 1; i++) {
            // Soft Blue Outer Glow
            DrawLineEx(points[i], points[i+1], 8.0f, Fade(SKYBLUE, 0.15f));
            // Bright White Core
            DrawLineEx(points[i], points[i+1], 1.5f, WHITE);
        }
        EndBlendMode();
    }
};

int main() {
    // Initialization
    const int screenWidth = 1200;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Lightning");
    SetTargetFPS(60);

    ControlPoint p1 = { { (float)screenWidth * 0.25f, (float)screenHeight * 0.5f } };
    ControlPoint p2 = { { (float)screenWidth * 0.75f, (float)screenHeight * 0.5f } };
    Lightning bolt;

    while (!WindowShouldClose()) {
        // Update Logic
        if (p1.isDragging) p1.Update();
        else if (p2.isDragging) p2.Update();
        else {
            p1.Update();
            p2.Update();
        }

        // Render Logic
        BeginDrawing();
            // Background color #0b5693
            ClearBackground((Color){ 11, 86, 147, 255 });

            // Subtle vignetting gradient
            DrawCircleGradient(screenWidth/2, screenHeight/2, screenWidth*0.8f, Fade(BLACK, 0.0f), Fade(BLACK, 0.4f));

            bolt.Draw(p1.pos, p2.pos);

            p1.Draw();
            p2.Draw();

            DrawText("Drag the endpoints to move the bolt", 10, 10, 20, RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
