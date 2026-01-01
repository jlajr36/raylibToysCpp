#include <raylib.h>
#include <vector>
#include <cmath>
#include <cstdlib>

struct Orb {
    Vector2 center;    // orbit center
    float radius;      // distance from center
    float angle;       // current angle in radians
    float speed;       // angular speed
    float size;        // line thickness
    float hue;         // for color cycling
    Vector2 pos;       // current position
    Vector2 lastPos;   // previous position
};

float RandomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

Color HSVtoRGB(float h, float s, float v) {
    float r, g, b;

    int i = int(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch(i % 6){
        case 0: r=v, g=t, b=p; break;
        case 1: r=q, g=v, b=p; break;
        case 2: r=p, g=v, b=t; break;
        case 3: r=p, g=q, b=v; break;
        case 4: r=t, g=p, b=v; break;
        case 5: r=v, g=p, b=q; break;
    }

    return Color{(unsigned char)(r*255),(unsigned char)(g*255),(unsigned char)(b*255),255};
}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Orbital Trail");
    SetTargetFPS(60);

    // Center of the screen
    Vector2 center = {(float)screenWidth/2, (float)screenHeight/2};

    // Multiple orbs
    const int orbCount = 200;
    std::vector<Orb> orbs;
    for(int i=0; i<orbCount; ++i){
        float r = RandomFloat(50, 250);
        float angle = RandomFloat(0, 2*PI);
        float speed = RandomFloat(0.5f, 2.0f)/100.0f; // slow orbital speed
        float size = RandomFloat(1.0f, 3.0f);
        orbs.push_back(Orb{center, r, angle, speed, size, RandomFloat(0,1), 
                            {center.x + cos(angle)*r, center.y + sin(angle)*r},
                            {center.x + cos(angle)*r, center.y + sin(angle)*r}});
    }

    bool trail = true;

    while(!WindowShouldClose()){
        float dt = GetFrameTime();

        // Update orbs
        for(auto &orb : orbs){
            orb.lastPos = orb.pos;
            orb.angle += orb.speed;
            orb.pos.x = orb.center.x + cos(orb.angle) * orb.radius;
            orb.pos.y = orb.center.y + sin(orb.angle) * orb.radius;

            orb.hue += dt * 0.1f;
            if(orb.hue > 1.0f) orb.hue -= 1.0f;
        }

        // Draw
        BeginDrawing();
        if(trail){
            DrawRectangle(0,0,screenWidth,screenHeight,Fade(BLACK,0.1f)); // fading trail
        } else {
            ClearBackground(BLACK);
        }

        for(const auto &orb : orbs){
            Color col = HSVtoRGB(orb.hue, 1.0f, 1.0f);
            DrawLineV(orb.lastPos, orb.pos, col);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
