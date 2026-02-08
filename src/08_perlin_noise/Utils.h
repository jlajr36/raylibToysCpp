#pragma once
#include "raylib.h"

// Math helpers
inline float Clamp(float v, float min, float max) { return (v<min)?min:(v>max)?max:v; }
inline float Lerp(float a, float b, float t) { return a + t*(b-a); }
inline float normalize(float value, float min, float max) { return Clamp((value-min)/(max-min),0.0f,1.0f); }
inline Color lerpColor(Color a, Color b, float t) {
    t = Clamp(t,0,1);
    return Color{(unsigned char)Lerp(a.r,b.r,t),(unsigned char)Lerp(a.g,b.g,t),(unsigned char)Lerp(a.b,b.b,t),255};
}
