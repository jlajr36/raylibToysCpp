#include "raylib.h"
#include <vector>
#include <cmath>

const float MY_PI = 3.14159265358979323846f;
const float MY_TWO_PI = 2.0f * MY_PI;

struct Vec {
    float x, y;
    Vec() : x(0), y(0) {}
    Vec(float x_, float y_) : x(x_), y(y_) {}
    Vec operator+(const Vec& o) const { return Vec(x+o.x, y+o.y); }
    Vec operator-(const Vec& o) const { return Vec(x-o.x, y-o.y); }
    Vec operator*(float s) const { return Vec(x*s, y*s); }
    Vec& operator+=(const Vec& o){ x+=o.x; y+=o.y; return *this; }
};

float mag(const Vec& v){ return sqrtf(v.x*v.x + v.y*v.y); }
Vec setMag(const Vec& v, float m){
    float current = mag(v);
    if (current == 0) return Vec(m, 0);
    return Vec(v.x * (m/current), v.y * (m/current));
}
float heading(const Vec& v){ return atan2f(v.y, v.x); }
Vec fromAngle(float a){ return Vec(cosf(a), sinf(a)); }

float simplifyAngle(float angle){
    while (angle >= MY_TWO_PI) angle -= MY_TWO_PI;
    while (angle < 0) angle += MY_TWO_PI;
    return angle;
}

// i.e. How many radians do you need to turn the angle to match the anchor?
float relativeAngleDiff(float angle, float anchor){
    angle = simplifyAngle(angle + MY_PI - anchor);
    anchor = MY_PI;
    return anchor - angle;
}

// Constrain the angle to be within a certain range of the anchor
float constrainAngle(float angle, float anchor, float constraint){
    if (fabsf(relativeAngleDiff(angle, anchor)) <= constraint){
        return simplifyAngle(angle);
    }
    if (relativeAngleDiff(angle, anchor) > constraint){
        return simplifyAngle(anchor - constraint);
    }
    return simplifyAngle(anchor + constraint);
}

// Constrain the vector to be at a certain range of the anchor
Vec constrainDistance(const Vec& pos, const Vec& anchor, float constraint){
    Vec diff = Vec(pos.x - anchor.x, pos.y - anchor.y);
    Vec set = setMag(diff, constraint);
    return Vec(anchor.x + set.x, anchor.y + set.y);
}

struct Chain {
    std::vector<Vec> joints;
    std::vector<float> angles;
    int linkSize;
    float angleConstraint; // radians

    Chain(const Vec& origin, int jointCount, int linkSize_, float angleConstraint_ = MY_TWO_PI)
        : linkSize(linkSize_), angleConstraint(angleConstraint_)
    {
        joints.reserve(jointCount);
        angles.reserve(jointCount);
        joints.push_back(origin);
        angles.push_back(0.0f);
        for (int i = 1; i < jointCount; ++i){
            joints.push_back(Vec(joints[i-1].x, joints[i-1].y + linkSize_));
            angles.push_back(0.0f);
        }
    }

    void resolve(const Vec& pos){
        if (joints.empty()) return;
        angles[0] = heading(Vec(pos.x - joints[0].x, pos.y - joints[0].y));
        joints[0] = pos;
        for (size_t i = 1; i < joints.size(); ++i){
            Vec cur = Vec(joints[i-1].x - joints[i].x, joints[i-1].y - joints[i].y);
            float curAngle = heading(cur);
            angles[i] = constrainAngle(curAngle, angles[i-1], angleConstraint);
            Vec offset = fromAngle(angles[i]);
            offset = setMag(offset, (float)linkSize);
            joints[i] = Vec(joints[i-1].x - offset.x, joints[i-1].y - offset.y);
        }
    }

    void fabrikResolve(const Vec& pos, const Vec& anchor){
        if (joints.empty()) return;
        // Forward pass
        joints[0] = pos;
        for (size_t i = 1; i < joints.size(); ++i){
            joints[i] = constrainDistance(joints[i], joints[i-1], (float)linkSize);
        }
        // Backward pass
        joints.back() = anchor;
        for (int i = (int)joints.size() - 2; i >= 0; --i){
            joints[i] = constrainDistance(joints[i], joints[i+1], (float)linkSize);
        }
    }

    void draw() const {
        for (size_t i = 0; i+1 < joints.size(); ++i){
            DrawLineV({joints[i].x, joints[i].y}, {joints[i+1].x, joints[i+1].y}, WHITE);
        }
        for (const auto& j : joints){
            DrawCircleV({j.x, j.y}, 16, Color{42,44,53,255});
            DrawCircleLines((int)j.x, (int)j.y, 16, WHITE);
        }
    }
};

int main(){
    const int screenW = 800;
    const int screenH = 600;
    InitWindow(screenW, screenH, "Chain - raylib");
    SetTargetFPS(60);

    Chain chain(Vec(screenW/2.0f, screenH/2.0f), 10, 30, MY_PI/3.0f); // angleConstraint = 60 degrees

    bool useFABRIK = false;
    Vec anchor(screenW/2.0f, screenH/2.0f + 30.0f * 9); // default anchor at chain end

    while (!WindowShouldClose()){
        if (IsKeyPressed(KEY_SPACE)) useFABRIK = !useFABRIK;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) anchor = Vec((float)GetMouseX(), (float)GetMouseY());

        Vec target((float)GetMouseX(), (float)GetMouseY());

        if (useFABRIK){
            chain.fabrikResolve(target, anchor);
        } else {
            chain.resolve(target);
        }

        BeginDrawing();
        ClearBackground(Color{40,44,52,255});
        chain.draw();

        DrawText(useFABRIK ? "Mode: FABRIK (SPACE to toggle)" : "Mode: Angle (SPACE to toggle)", 10, 10, 20, WHITE);
        DrawText("Left-click to move anchor (FABRIK)", 10, 34, 12, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
