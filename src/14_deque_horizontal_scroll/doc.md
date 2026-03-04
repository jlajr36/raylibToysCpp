# Creating a Smooth Scrolling Horizontal Line with Raylib and C++

## **1. Including Libraries**

```cpp
#include "raylib.h"
#include <deque>
```

* `raylib.h` → This is the **Raylib library**, which lets you easily create windows, draw shapes, and handle input.
* `<deque>` → This is a **double-ended queue** from the C++ standard library. It allows you to add or remove elements from the **front or back** efficiently, which is perfect for a scrolling line.

---

## **2. Setting Up the Window**

```cpp
const int screenWidth = 800;
const int screenHeight = 450;

InitWindow(screenWidth, screenHeight, "Scrolling Horizontal Line with Deque");
SetTargetFPS(60);
```

* `screenWidth` and `screenHeight` → The size of the window in pixels (width × height).

  * 800 pixels wide, 450 pixels tall.
* `InitWindow()` → Opens a window with that size and a title.
* `SetTargetFPS(60)` → Makes the program try to run **60 frames per second**, so the animation is smooth.

---

## **3. Preparing the Points**

```cpp
std::deque<Vector2> points;

float y = screenHeight / 2.0f;  // Center vertically
const int lineLength = 300;      // Total length of the line in pixels
const int spacing = 10;          // Distance between each point in pixels
int numPoints = lineLength / spacing;

// Center the line horizontally
int startX = (screenWidth - lineLength) / 2;

for (int i = 0; i <= numPoints; i++)
{
    points.push_back({ (float)(startX + i * spacing), y });
}
```

**What this does:**

1. `std::deque<Vector2> points;` → Creates a list to store all the points of the line. `Vector2` stores **x and y positions**.
2. `y = screenHeight / 2.0f;` → Places the line **vertically in the middle** of the screen.
3. `lineLength = 300;` → The line will be 300 pixels long.
4. `spacing = 10;` → Each point is 10 pixels apart.
5. `numPoints = lineLength / spacing;` → Calculates how many points fit in the line.
6. `startX = (screenWidth - lineLength) / 2;` → Centers the line horizontally.
7. The `for` loop → Adds all points to the deque, spaced evenly along the x-axis.

---

## **4. Scrolling Variables**

```cpp
int direction = 1; // 1 = move right, -1 = move left
float speed = 2.0f; // pixels per frame
```

* `direction` → Tells the line which way to move. `1` = right, `-1` = left.
* `speed` → How many pixels each point moves per frame (per image drawn). 2 pixels per frame is smooth but visible.

---

## **5. Main Loop**

```cpp
while (!WindowShouldClose())
{
    // Move the line
    for (auto& p : points)
        p.x += direction * speed;
```

* `WindowShouldClose()` → Checks if the user clicked the **close button**.
* The `for` loop → Updates each point’s x-position to make the line move horizontally.

---

## **6. Handling Scrolling & Edge Wrapping**

```cpp
if (direction == 1 && points.front().x > screenWidth)
{
    points.push_back({ points.front().x - lineLength - spacing, y });
    points.pop_front();
}
else if (direction == -1 && points.back().x < 0)
{
    points.push_front({ points.back().x + lineLength + spacing, y });
    points.pop_back();
}
```

**Explanation:**

* As the line moves, points eventually go **off the screen**.
* Instead of letting the line disappear, we **wrap points around**:

  * Scrolling right (`direction == 1`) → if the **front point** is past the right edge, remove it and add a new one at the back.
  * Scrolling left (`direction == -1`) → if the **back point** is past the left edge, remove it and add a new one at the front.
* This makes the line **scroll continuously**.

---

## **7. Drawing the Points**

```cpp
BeginDrawing();
ClearBackground(RAYWHITE);

for (const auto& p : points)
    DrawCircleV(p, 3, BLUE);

EndDrawing();
```

* `BeginDrawing()` and `EndDrawing()` → Everything drawn between these calls is shown on the screen.
* `ClearBackground(RAYWHITE)` → Fills the screen with white before drawing the line.
* `DrawCircleV(p, 3, BLUE)` → Draws a circle at each point:

  * `p` → The point’s position
  * `3` → The radius in pixels
  * `BLUE` → Color

---

## **8. Closing the Window**

```cpp
CloseWindow();
return 0;
```

* `CloseWindow()` → Properly closes the window and cleans up resources.
* `return 0;` → Ends the program successfully.

---

## **Summary of Numbers**

| Number/Variable | Meaning                               |
| --------------- | ------------------------------------- |
| 800, 450        | Window width & height                 |
| 300             | Length of the line in pixels          |
| 10              | Distance between points               |
| 2.0             | Pixels per frame (speed of scrolling) |
| 3               | Radius of each drawn circle           |

---

✅ **What the program does:**
It creates a horizontal line made of small blue circles and makes it **scroll left and right** continuously across the screen. By using a `deque`, we can efficiently remove points that leave the screen and add new ones at the opposite side.