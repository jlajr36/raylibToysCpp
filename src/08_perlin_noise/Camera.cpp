#include "Camera.h"

void CameraController::Update(float dt) {
    float panSpeed = 200.0f / zoom;

    if (IsKeyDown(KEY_LEFT))  offsetX -= panSpeed;
    if (IsKeyDown(KEY_RIGHT)) offsetX += panSpeed;
    if (IsKeyDown(KEY_UP))    offsetY -= panSpeed;
    if (IsKeyDown(KEY_DOWN))  offsetY += panSpeed;

    if (IsKeyDown(KEY_W)) zoom *= 1.02f;
    if (IsKeyDown(KEY_S)) zoom /= 1.02f;

    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f) zoom *= (wheel > 0) ? 1.1f : 0.9f;
}
