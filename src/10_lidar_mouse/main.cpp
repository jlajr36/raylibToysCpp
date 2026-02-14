#include <raylib.h>
#include <cmath>
#include <vector>
#include <random>

// ------------------ Sensor Measurement ------------------
struct SensorMeasurement {
    float distance;
    float angle;
    Vector2 position;
};

// ------------------ Laser Sensor ------------------
class LaserSensor {
public:
    float Range;
    Vector2 position;
    float sigma_distance, sigma_angle;

    LaserSensor(float range, float sigma_d, float sigma_a)
        : Range(range), sigma_distance(sigma_d), sigma_angle(sigma_a), position({0,0}) {}

    float distanceTo(Vector2 obstacle) {
        float dx = obstacle.x - position.x;
        float dy = obstacle.y - position.y;
        return sqrt(dx*dx + dy*dy);
    }

    std::vector<SensorMeasurement> senseObstacles(Image &map) {
        std::vector<SensorMeasurement> data;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<float> dist_noise(0, sigma_distance);
        std::normal_distribution<float> angle_noise(0, sigma_angle);

        for(int i=0; i<60; i++) {
            float angle = (2*M_PI) * i / 60.0f;
            for(int j=0; j<100; j++) {
                float u = j / 100.0f;
                float x = position.x * (1-u) + (position.x + Range * cos(angle)) * u;
                float y = position.y * (1-u) + (position.y - Range * sin(angle)) * u;

                if (x >=0 && x < map.width && y >=0 && y < map.height) {
                    Color c = GetImageColor(map, (int)x, (int)y);
                    if (c.r == 0 && c.g == 0 && c.b == 0) { // obstacle
                        SensorMeasurement m;
                        m.distance = distanceTo({x, y}) + dist_noise(gen);
                        m.angle = fmod(angle + angle_noise(gen), 2*M_PI);
                        m.position = position;
                        data.push_back(m);
                        break;
                    }
                }
            }
        }
        return data;
    }
};

// ------------------ Environment ------------------
class Environment {
public:
    Image originalMapImage;
    Image revealedMapImage;
    Texture2D revealedMapTexture;
    std::vector<Vector2> pointCloud;

    Environment(const char* filename) {
        originalMapImage = LoadImage(filename); // load floor plan
        revealedMapImage = GenImageColor(originalMapImage.width, originalMapImage.height, BLACK);
        revealedMapTexture = LoadTextureFromImage(revealedMapImage);
    }

    Vector2 polarToCartesian(float distance, float angle, Vector2 robotPos) {
        return { robotPos.x + distance * cos(angle),
                 robotPos.y - distance * sin(angle) };
    }

    void storeData(std::vector<SensorMeasurement>& data) {
        for(auto& m : data) {
            Vector2 p = polarToCartesian(m.distance, m.angle, m.position);
            pointCloud.push_back(p);

            // reveal original map pixel at this point
            if (p.x >= 0 && p.x < revealedMapImage.width &&
                p.y >= 0 && p.y < revealedMapImage.height) {

                Color origColor = GetImageColor(originalMapImage, (int)p.x, (int)p.y);
                ImageDrawPixel(&revealedMapImage, (int)p.x, (int)p.y, origColor);
            }
        }
        UpdateTexture(revealedMapTexture, revealedMapImage.data);
    }

    void drawSensorData() {
        for(auto& p : pointCloud) {
            DrawPixelV(p, RED);
        }
    }

    ~Environment() {
        UnloadTexture(revealedMapTexture);
        UnloadImage(originalMapImage);
        UnloadImage(revealedMapImage);
    }
};

// ------------------ Main ------------------
int main() {
    const int width = 1200;
    const int height = 600;
    InitWindow(width, height, "Laser Sensor Map Reveal");

    Environment env("assets/floor_plan.png");
    LaserSensor laser(200, 0.5f, 0.01f);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Sensor follows mouse like Python version
        Vector2 mousePos = GetMousePosition();
        laser.position = mousePos;

        // Sense obstacles
        auto sensorData = laser.senseObstacles(env.originalMapImage);
        env.storeData(sensorData);

        // Draw everything
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(env.revealedMapTexture, 0, 0, WHITE);
        env.drawSensorData(); // red points
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
