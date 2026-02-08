#pragma once
#include "raylib.h"

class CameraController {
public:
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    float zoom = 150.0f;

    void Update(float dt);
};

