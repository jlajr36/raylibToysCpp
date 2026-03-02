# 3D Lorenz Attractor

This project visualizes the **Lorenz attractor** in 3D using **Raylib**, with smooth camera movement, color trails, and keyboard interaction.

---

## Table of Contents

1. [Introduction](#introduction)
2. [Dependencies](#dependencies)
3. [Code Breakdown](#code-breakdown)

   * [1. Includes and Structs](#1-includes-and-structs)
   * [2. Linear Interpolation Function](#2-linear-interpolation-function)
   * [3. Main Function](#3-main-function)

     * [Window & Camera Setup](#window--camera-setup)
     * [Lorenz System Initialization](#lorenz-system-initialization)
     * [Trail Storage](#trail-storage)
     * [Main Loop](#main-loop)

       * [Resetting the System](#resetting-the-system)
       * [Lorenz Integration](#lorenz-integration)
       * [Updating the Trail](#updating-the-trail)
       * [Calculating the Center](#calculating-the-center)
       * [Camera Controls](#camera-controls)
       * [Hue Animation](#hue-animation)
       * [Drawing the Attractor](#drawing-the-attractor)
4. [Enhancements and Tips](#enhancements-and-tips)
5. [Conclusion](#conclusion)

---

## Introduction

The **Lorenz attractor** is a system of differential equations originally developed to model atmospheric convection. It is famous for producing **chaotic yet visually appealing patterns**.

This program simulates the Lorenz system in **3D** and visualizes it with a **color-changing trail**, while allowing interactive camera rotation and zoom.

---

## Dependencies

To run this code, you need:

* **C++ Compiler** (e.g., g++, clang++)
* **Raylib** (a simple C/C++ graphics library)

Install Raylib according to your system: [https://www.raylib.com/](https://www.raylib.com/)

---

## Code Breakdown

### 1. Includes and Structs

```cpp
#include "raylib.h"
#include <deque>
#include <cmath>

struct Point3D {
    Vector3 pos;
    float hue;
};
```

* `raylib.h` → Graphics and input functions.
* `deque` → Double-ended queue for efficient trail management.
* `cmath` → Math functions (`sin`, `cos`, `fmod`).
* `Point3D` → Stores a 3D point (`x, y, z`) and a hue for coloring the trail.

---

### 2. Linear Interpolation Function

```cpp
float Lerp(float a, float b, float t) {
    return a + t * (b - a);
}
```

* **Lerp** (Linear Interpolation) calculates a value between `a` and `b` based on `t` (0 → `a`, 1 → `b`).
* Used for **smooth camera movement**.

---

### 3. Main Function

```cpp
int main() {
    InitWindow(800, 600, "3D Lorenz Attractor");
    SetTargetFPS(60);
```

* Creates a window 800×600 pixels.
* Sets the frame rate to 60 FPS.

---

#### Window & Camera Setup

```cpp
Camera3D camera = {0};
camera.position = {30.0f, 30.0f, 30.0f};
camera.target = {0.0f, 0.0f, 0.0f};
camera.up = {0.0f, 1.0f, 0.0f};
camera.fovy = 45.0f;
```

* `Camera3D` defines a **3D viewpoint**.
* `position` → Camera location in 3D space.
* `target` → Point camera looks at.
* `up` → Which direction is “up”.
* `fovy` → Field of view angle.

---

#### Lorenz System Initialization

```cpp
float x = 0.01f, y = 0.0f, z = 0.0f;
const float x0 = 0.01f, y0 = 0.0f, z0 = 0.0f;

float a = 10.0f, b = 28.0f, c = 8.0f / 3.0f;
float dt = 0.01f;
```

* `(x, y, z)` → Current position in the Lorenz system.
* `(x0, y0, z0)` → Initial starting point.
* `a, b, c` → Lorenz parameters (control chaos).
* `dt` → Time step for simulation.

---

#### Trail Storage

```cpp
std::deque<Point3D> points;
const int maxPoints = 2000;
```

* Stores the last 2000 points of the attractor for drawing a trail.
* `deque` allows fast **add/remove** from both ends.

---

### Main Loop

```cpp
while (!WindowShouldClose()) {
    ...
}
```

* Runs until the window is closed.

---

#### Resetting the System

```cpp
if (IsKeyPressed(KEY_SPACE)) {
    points.clear();
    x = x0; y = y0; z = z0;
    angle = 0.0f;
    radius = 50.0f;
    startHue = GetRandomValue(0, 359);
}
```

* Press **Space** to reset the attractor and camera.
* `startHue` gives a new random starting color for the trail.

---

#### Lorenz Integration

```cpp
float dx = a * (y - x) * dt;
float dy = (x * (b - z) - y) * dt;
float dz = (x * y - c * z) * dt;
x += dx; y += dy; z += dz;
```

* **Euler integration** for the Lorenz system:

  * Simple way to advance the system step by step.

---

#### Updating the Trail

```cpp
points.push_back({{x, y, z}, 0.0f});
if (points.size() > maxPoints) points.pop_front();
```

* Adds new points to the trail.
* Removes oldest points when exceeding `maxPoints`.

---

#### Calculating the Center

```cpp
Vector3 center = {0.0f, 0.0f, 0.0f};
for (const auto& p : points) {
    center.x += p.pos.x;
    center.y += p.pos.y;
    center.z += p.pos.z;
}
if (!points.empty()) {
    center.x /= points.size();
    center.y /= points.size();
    center.z /= points.size();
}
```

* Computes **center of all points** to keep the camera focused.

---

#### Camera Controls

```cpp
if (IsKeyDown(KEY_RIGHT)) angle += 0.03f;
if (IsKeyDown(KEY_LEFT))  angle -= 0.03f;
if (IsKeyDown(KEY_UP))    radius -= 1.0f;
if (IsKeyDown(KEY_DOWN))  radius += 1.0f;
```

* Arrow keys rotate (`LEFT/RIGHT`) and zoom (`UP/DOWN`) the camera.
* Smooth camera updates using `Lerp` for a **fluid motion effect**:

```cpp
camera.position.x = Lerp(camera.position.x, camX, 0.1f);
camera.position.z = Lerp(camera.position.z, camZ, 0.1f);
```

---

#### Hue Animation

```cpp
startHue += 0.5f;
if (startHue > 360.0f) startHue -= 360.0f;
```

* Moves the starting hue over time for a **shifting color effect**.

---

#### Drawing the Attractor

```cpp
BeginDrawing();
ClearBackground(BLACK);
BeginMode3D(camera);

for (size_t i = 1; i < points.size(); i++) {
    float t = (float)i / points.size();
    float hue = fmod(startHue + t * 360.0f, 360.0f);
    Color col = ColorFromHSV(hue, 1.0f, t);
    DrawLine3D(points[i-1].pos, points[i].pos, col);
}

EndMode3D();
DrawFPS(10, 10);
EndDrawing();
```

* Clears the screen to black.
* Draws each segment of the trail with **color fading and hue gradient**.
* `DrawLine3D` → Draws a line between consecutive points.

---

## Enhancements and Tips

1. **Better integration**: Use RK4 instead of Euler for smoother, more accurate motion.
2. **Camera motion**: Add vertical rotation or orbit for more dynamic views.
3. **Trail shapes**: Use spheres or cubes instead of lines for 3D points.
4. **Speed control**: Adjust `dt` in real time.
5. **Motion blur**: Partially clear the background for ghost-like trails.

---

## Conclusion

This program is a **beginner-friendly visualization** of chaotic systems in 3D, demonstrating:

* Physics simulation (Lorenz attractor)
* 3D graphics with Raylib
* Camera interpolation and user controls
* Animated color trails

It can be extended into more advanced **interactive visualizations or generative art projects**.