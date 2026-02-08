#pragma once
#include "raylib.h"
#include <vector>

struct TerrainType {
    float minHeight;
    float maxHeight;
    Color minColor;
    Color maxColor;
    float lerpAdjust;
};

class Terrain {
public:
    Terrain(int width, int height, unsigned int seed);
    void Generate(float offsetX, float offsetY, float zoom);
    void Draw(Texture2D& texture);

private:
    int width, height;
    unsigned int seed;
    std::vector<std::vector<float>> heightMap;
    std::vector<TerrainType> terrainTypes;

    Color GetColor(float noise);
};
