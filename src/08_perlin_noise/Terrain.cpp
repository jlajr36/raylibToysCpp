#include "Terrain.h"
#include "Utils.h"
#include "NoiseUtils.h"
#include <cmath> // for std::pow

Terrain::Terrain(int w, int h, unsigned int s) : width(w), height(h), seed(s) {
    heightMap.resize(height, std::vector<float>(width));

    terrainTypes = {
        {0.0f, 0.4f, {30,176,251,255}, {40,255,255,255}, 0.0f},   // Water
        {0.4f, 0.5f, {215,192,158,255}, {255,246,193,255}, 0.3f}, // Sand
        {0.5f, 0.7f, {2,166,155,255}, {118,239,124,255}, 0.0f},   // Grass
        {0.7f, 1.0f, {22,181,141,255}, {10,145,113,255}, -0.3f}   // Trees
    };
}

void Terrain::Generate(float offsetX, float offsetY, float zoom) {
    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            float nx = (x - width*0.5f + offsetX)/zoom;
            float ny = (y - height*0.5f + offsetY)/zoom;
            float n = fractalPerlin(nx, ny, seed, 5, 0.5f);
            n = (n+1.0f)*0.5f;
            heightMap[y][x] = Clamp(n, 0.0f, 1.0f);
        }
    }
}

Color Terrain::GetColor(float n) {
    for (auto& t : terrainTypes)
        if (n <= t.maxHeight) return lerpColor(t.minColor, t.maxColor, normalize(n, t.minHeight, t.maxHeight) + t.lerpAdjust);
    return terrainTypes.back().maxColor;
}

void Terrain::Draw(Texture2D& texture) {
    Image img = GenImageColor(width, height, BLANK);
    Color* pixels = (Color*)img.data;

    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            float n = heightMap[y][x];

            // Simple shadow
            float dx = (x<width-1 ? heightMap[y][x+1] : n) - (x>0 ? heightMap[y][x-1] : n);
            float dy = (y<height-1 ? heightMap[y+1][x] : n) - (y>0 ? heightMap[y-1][x] : n);
            float light = Clamp(0.5f + 0.5f*(-dx-dy+1.0f), 0.0f, 1.0f);
            light = std::pow(light, 1.5f);

            Color shaded = GetColor(n);
            shaded.r = (unsigned char)(shaded.r * light);
            shaded.g = (unsigned char)(shaded.g * light);
            shaded.b = (unsigned char)(shaded.b * light);

            pixels[y*width + x] = shaded;
        }
    }

    UpdateTextureRec(texture, (Rectangle){0,0,(float)width,(float)height}, pixels);
    UnloadImage(img);
}
