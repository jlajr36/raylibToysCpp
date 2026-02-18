#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>

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
    float   r;        // radius
    float   opacity;  // 0.0 – 1.0
    float   dop;      // change in opacity per frame (can be negative)
    Color   color;
};

// Helper function: gives a random float number between min and max
float GetRandom(float min, float max) {
    // This is our random number generator.
    // "static" means we only make it once, and it remembers its state for next time.
    static mt19937 gen(random_device{}()); 
    
    // This sets up a range from min to max
    uniform_real_distribution<float> dist(min, max); 
    
    // Get a random number from the generator within the range and return it
    return dist(gen);
}

// Initialise a single particle (called on start and on reset)
void ResetParticle(ConfettiParticle& p, int screenWidth, int screenHeight) {
    p.r       = GetRandom(2, 6);
    p.pos     = { GetRandom(0, (float)screenWidth),
                  GetRandom(-20.0f, (float)screenHeight) };
    p.vel     = { GetRandom(-1.0f, 1.0f),
                  0.7f * p.r + GetRandom(-1.0f, 1.0f) };
    p.color   = COLORS[rand() % 5];
    p.opacity = 0.0f;                                 // start invisible
    p.dop     = 0.03f * GetRandom(1.0f, 4.0f);        // positive → fade‑in
}


// Initialise the whole vector
void InitConfetti(vector<ConfettiParticle>& confetti,
                 int screenWidth, int screenHeight) {
    for (auto& c : confetti) ResetParticle(c, screenWidth, screenHeight);
}

// Update – now includes fade‑in → fade‑out logic
void UpdateConfetti(vector<ConfettiParticle>& confetti,
                    int screenWidth, int screenHeight) {
    for (auto& c : confetti) {
        // ---- movement -------------------------------------------------
        c.pos.x += c.vel.x;
        c.pos.y += c.vel.y;

        // ---- opacity fade ------------------------------------------------
        c.opacity += c.dop;                     // dop may be positive or negative

        if (c.opacity >= 1.0f) {                // reached full opacity → start fading out
            c.opacity = 1.0f;
            c.dop = -c.dop;                     // flip sign
        }
        if (c.opacity <= 0.0f) {                // fully transparent again → respawn
            ResetParticle(c, screenWidth, screenHeight);
            continue;                           // skip further checks for this frame
        }

        // ---- respawn when falling off the bottom -------------------------
        if (c.pos.y > screenHeight) {
            ResetParticle(c, screenWidth, screenHeight);
            continue;
        }

        // ---- horizontal wrap --------------------------------------------
        if (c.pos.x < 0) c.pos.x += screenWidth;
        if (c.pos.x > screenWidth) c.pos.x -= screenWidth;
    }
}

// Draw – opacity is encoded in the alpha channel
void DrawConfetti(const vector<ConfettiParticle>& confetti) {
    for (const auto& c : confetti) {
        Color col = c.color;
        col.a = static_cast<unsigned char>(c.opacity * 255);
        DrawCircleV(c.pos, c.r, col);
    }
}

int main(void) {
    srand(static_cast<unsigned>(time(nullptr)));   // seed RNG once

    const int screenWidth  = 540;
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
