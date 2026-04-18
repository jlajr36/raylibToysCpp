#include "raylib.h"
#include "rlgl.h"
#include "Perlin2D.h"
#include <vector>

int main() {
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Terrain Visualization");

    // Grid configuration
    const int scl = 20;
    const int w = 2000;
    const int h = 1600;
    const int cols = w / scl;
    const int rows = h / scl;

    float flying = 0.0f;
    std::vector<float> terrain(cols * rows);

    // Setup 3D Camera
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 180.0f, 500.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Update: Calculate movement and noise map
        flying -= 0.1f;
        float yoff = flying;
        for (int y = 0; y < rows; y++) {
            float xoff = 0;
            for (int x = 0; x < cols; x++) {
                // Using your Perlin2D.h implementation
                float noiseVal = perlin2D(xoff, yoff, 42);
                terrain[x + y * cols] = noiseVal * 100.0f;
                xoff += 0.2f;
            }
            yoff += 0.2f;
        }

        // Render
        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(camera);
            rlPushMatrix();
                // Center the mesh on screen
                rlTranslatef(-w / 2.0f, 0.0f, -h / 2.0f);

                for (int y = 0; y < rows - 1; y++) {
                    rlBegin(RL_LINES); // Use RL_TRIANGLE_STRIP for solid mesh
                    rlColor3f(0.0f, 0.8f, 1.0f); // Bright blue wireframe

                    for (int x = 0; x < cols; x++) {
                        // Current row vertex
                        rlVertex3f(x * scl, terrain[x + y * cols], y * scl);
                        // Next row vertex (creates the strip effect)
                        rlVertex3f(x * scl, terrain[x + (y + 1) * cols], (y + 1) * scl);

                        // Horizontal connectors
                        if (x < cols - 1) {
                            rlVertex3f(x * scl, terrain[x + y * cols], y * scl);
                            rlVertex3f((x + 1) * scl, terrain[(x + 1) + y * cols], y * scl);
                        }
                    }
                    rlEnd();
                }
            rlPopMatrix();
        EndMode3D();

        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
