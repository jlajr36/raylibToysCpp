#include "raylib.h"
#include "rlgl.h"
#include "Perlin2D.h"
#include <vector>
#include <cmath>

int main() {
    // 1. Initialization
    const int screenWidth = 1200;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Synthwave Frontier - 80s Retro");

    // Grid configuration
    const int scl = 20;   // Size of each grid square
    const int w = 2400;   // Total width of terrain
    const int h = 1600;   // Total depth of terrain
    const int cols = w / scl;
    const int rows = h / scl;

    float flying = 0.0f;
    std::vector<float> terrain(cols * rows);

    // Setup 3D Camera
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 150.0f, 600.0f }; // Elevated view
    camera.target = (Vector3){ 0.0f, 40.0f, 0.0f };      // Looking toward horizon
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // --- Update Logic ---
        flying -= 0.07f; // Controls speed of "driving"
        float yoff = flying;
        for (int y = 0; y < rows; y++) {
            float xoff = 0;
            for (int x = 0; x < cols; x++) {
                // Generate terrain using your Perlin2D implementation
                float noiseVal = perlin2D(xoff, yoff, 42);
                terrain[x + y * cols] = noiseVal * 130.0f; 
                xoff += 0.18f;
            }
            yoff += 0.18f;
        }

        // --- Render Logic ---
        BeginDrawing();
        ClearBackground(BLACK);

        // A. BACKGROUND GRADIENT (The Sky)
        // Fades from Deep Purple/Black at the top to Hot Pink at the horizon
        DrawRectangleGradientV(0, 0, screenWidth, screenHeight / 2, BLACK, (Color){ 60, 0, 100, 255 });
        DrawRectangleGradientV(0, screenHeight / 2, screenWidth, screenHeight / 2, (Color){ 60, 0, 100, 255 }, MAGENTA);

        BeginMode3D(camera);
            
            // B. STAR FIELD
            for (int i = 0; i < 250; i++) {
                float sx = sinf(i * 1.5f) * 1200.0f;
                float sy = cosf(i * 0.7f) * 600.0f + 300.0f;
                // Positioned far back but before the clipping plane
                DrawPoint3D((Vector3){ sx, sy, -500.0f }, WHITE); 
            }

            // C. CELESTIAL SPHERES (Wireframe shells with glowing cores)
            // Left Sphere
            DrawSphereWires((Vector3){ -450, 250, -300 }, 80, 16, 16, SKYBLUE);
            DrawSphere((Vector3){ -450, 250, -300 }, 35, MAGENTA);
            
            // Right Sphere
            DrawSphereWires((Vector3){ 450, 200, -350 }, 65, 12, 12, SKYBLUE);
            DrawSphere((Vector3){ 450, 200, -350 }, 25, MAGENTA);

            // D. THE WIREFRAME TERRAIN
            rlPushMatrix();
                // Move terrain so it's centered and stretches into the distance
                rlTranslatef(-w / 2.0f, 0.0f, -h / 2.0f);

                for (int y = 0; y < rows - 1; y++) {
                    rlBegin(RL_LINES);
                    
                    // COLOR GRADIENT: Near = Blue, Far = Pink/Magenta
                    // This creates the "backlit" effect from the horizon
                    float distFactor = (float)y / rows; 
                    Color lineCol = (Color){ 
                        (unsigned char)(distFactor * 255),          // Red increases
                        (unsigned char)((1.0f - distFactor) * 200), // Blue/Green mix
                        255, 255 
                    };
                    rlColor4ub(lineCol.r, lineCol.g, lineCol.b, lineCol.a);

                    for (int x = 0; x < cols; x++) {
                        // Current vertex
                        float xPos = (float)x * scl;
                        float zPos = (float)y * scl;
                        
                        // Vertical Lines (Connecting to the next row)
                        rlVertex3f(xPos, terrain[x + y * cols], zPos);
                        rlVertex3f(xPos, terrain[x + (y + 1) * cols], zPos + scl);

                        // Horizontal Lines (Connecting to the next column)
                        if (x < cols - 1) {
                            rlVertex3f(xPos, terrain[x + y * cols], zPos);
                            rlVertex3f(xPos + scl, terrain[(x + 1) + y * cols], zPos);
                        }
                    }
                    rlEnd();
                }
            rlPopMatrix();

        EndMode3D();

        // E. SCREEN GLOW (Overlay)
        // Adds a very slight pink tint to the whole image to simulate neon bloom
        DrawRectangleV((Vector2){0,0}, (Vector2){(float)screenWidth, (float)screenHeight}, (Color){255, 0, 255, 15});

        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}