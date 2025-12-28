#include "raylib.h"
#include <vector>
#include <cstdlib>

using namespace std;

// Number of confetti particles
const int NUM_CONFETTI = 350;

// Confetti colors
Color COLORS[] = {
    { 85,  71, 106, 255 },  // Deep Purple
    {174,  61,  99, 255 },  // Medium Violet-Red
    {219,  56,  83, 255 },  // Crimson
    {244,  92,  68, 255 },  // Coral
    {248, 182,  70, 255 }   // Goldenrod
};

// Confetti particle structure
struct ConfettiParticle {
    Vector2 pos;
    Vector2 vel;
    float r;        // radius
    float opacity;  // 0.0 - 1.0
    float dop;      // opacity change per frame
    Color color;
};

// Helper function: random float in range [min, max)
float GetRandom(float min, float max) {
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

// Initialize confetti particles
void InitConfetti(vector<ConfettiParticle>& confetti, int screenWidth, int screenHeight) {
    for (auto& c : confetti) {
        c.r = GetRandom(2, 6);
        c.pos = { GetRandom(0, (float)screenWidth), GetRandom(-20.0f, (float)screenHeight) };
        c.vel = { GetRandom(-1.0f, 1.0f), 0.7f * c.r + GetRandom(-1.0f, 1.0f) };
        int colorIndex = rand() % 5;
        c.color = COLORS[colorIndex];
        c.opacity = 0.0f;
        c.dop = 0.03f * GetRandom(1.0f, 4.0f);
    }
}

// Update confetti particles
void UpdateConfetti(vector<ConfettiParticle>& confetti, int screenWidth, int screenHeight) {
    for (auto& c : confetti) {
        // Move confetti
        c.pos.x += c.vel.x;
        c.pos.y += c.vel.y;

        // Fade in to full opacity
        c.opacity += c.dop;
        if (c.opacity > 1.0f) c.opacity = 1.0f;

        // Reset if below the screen
        if (c.pos.y > screenHeight) {
            c.pos.x = GetRandom(0.0f, (float)screenWidth);
            c.pos.y = GetRandom(-20.0f, -5.0f);
            c.vel.x = GetRandom(-1.0f, 1.0f);
            c.vel.y = 0.7f * c.r + GetRandom(-1.0f, 1.0f);
            c.opacity = 0.0f;
            c.dop = 0.03f * GetRandom(1.0f, 4.0f);
        }

        // Wrap around X-axis
        if (c.pos.x < 0) c.pos.x += screenWidth;
        if (c.pos.x > screenWidth) c.pos.x -= screenWidth;
    }
}

// Draw confetti particles
void DrawConfetti(const vector<ConfettiParticle>& confetti) {
    for (const auto& c : confetti) {
        Color col = c.color;
        col.a = (unsigned char)(c.opacity * 255);
        DrawCircleV(c.pos, c.r, col);
    }
}

int main(void)
{
    // Window initialization
    const int screenWidth = 540;
    const int screenHeight = 960;
    InitWindow(screenWidth, screenHeight, "Falling Confetti");
    SetTargetFPS(60);

    // Initialize confetti
    vector<ConfettiParticle> confetti(NUM_CONFETTI);
    InitConfetti(confetti, screenWidth, screenHeight);

    // Main loop
    while (!WindowShouldClose()) {
        UpdateConfetti(confetti, screenWidth, screenHeight);

        BeginDrawing();
            ClearBackground(BLACK);
            DrawConfetti(confetti);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
