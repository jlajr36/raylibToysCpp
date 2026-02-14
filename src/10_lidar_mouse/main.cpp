#include <raylib.h>
#include <cmath>
#include <vector>
#include <random>

// ------------------ Sensor Measurement ------------------
struct SensorMeasurement {
    float distance;   // measured distance
    float angle;      // measured angle
    Vector2 position; // sensor position
};

// ------------------ Laser Sensor ------------------
class LaserSensor {
public:
    float Range;            // max sensor range
    Vector2 position;       // sensor position
    float sigma_distance;   // distance noise
    float sigma_angle;      // angle noise

    // Random number generator moved outside function for efficiency
    std::mt19937 gen;
    std::normal_distribution<float> dist_noise;
    std::normal_distribution<float> angle_noise;

    LaserSensor(float range, float sigma_d, float sigma_a)
        : Range(range), sigma_distance(sigma_d), sigma_angle(sigma_a), position({0,0}),
          gen(std::random_device{}()),
          dist_noise(0, sigma_d),
          angle_noise(0, sigma_a) {}

    // Compute Euclidean distance to obstacle
    float distanceTo(Vector2 obstacle) {
        float dx = obstacle.x - position.x;
        float dy = obstacle.y - position.y;
        return sqrt(dx*dx + dy*dy);
    }

    // Sense obstacles on the map image
    std::vector<SensorMeasurement> senseObstacles(Image &map) {
        std::vector<SensorMeasurement> data;

        const int numRays = 60;    // number of laser rays
        const int stepsPerRay = 100; // samples along each ray

        for(int i=0; i<numRays; i++) {
            float angle = (2*M_PI) * i / numRays;

            for(int j=0; j<stepsPerRay; j++) {
                float u = j / (float)stepsPerRay;
                float x = position.x * (1-u) + (position.x + Range * cos(angle)) * u;
                float y = position.y * (1-u) + (position.y - Range * sin(angle)) * u;

                if (x >=0 && x < map.width && y >=0 && y < map.height) {
                    Color c = GetImageColor(map, (int)x, (int)y);
                    if (c.r == 0 && c.g == 0 && c.b == 0) { // obstacle
                        SensorMeasurement m;
                        m.distance = distanceTo({x, y}) + dist_noise(gen); // add noise
                        m.angle = fmod(angle + angle_noise(gen), 2*M_PI);  // add angle noise
                        m.position = position;
                        data.push_back(m);
                        break; // stop at first obstacle
                    }
                }
            }
        }
        return data;
    }

    // Draw the sensor and its rays
    void drawLaserRays(const std::vector<SensorMeasurement>& measurements) {
        // Draw all rays
        for (auto& m : measurements) {
            float x = m.position.x + m.distance * cos(m.angle);
            float y = m.position.y - m.distance * sin(m.angle);
            DrawLineV(m.position, {x, y}, GREEN); // ray
        }

        // Draw the sensor as a blue circle
        DrawCircleV(position, 5, BLUE);
    }
};

// ------------------ Environment ------------------
class Environment {
public:
    Image originalMapImage;     // the original floor plan
    Image revealedMapImage;     // what sensor has seen
    Texture2D revealedMapTexture;
    std::vector<Vector2> pointCloud; // scanned points

    Environment(const char* filename) {
        originalMapImage = LoadImage(filename); // load floor plan
        revealedMapImage = GenImageColor(originalMapImage.width, originalMapImage.height, BLACK);
        revealedMapTexture = LoadTextureFromImage(revealedMapImage);
    }

    // Convert polar coordinates to Cartesian
    Vector2 polarToCartesian(float distance, float angle, Vector2 robotPos) {
        return { robotPos.x + distance * cos(angle),
                 robotPos.y - distance * sin(angle) };
    }

    // Store sensor data: update point cloud and reveal map
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

    // Draw scanned points (red)
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

    // Load environment
    Environment env("assets/floor_plan.png");

    // Create laser sensor
    LaserSensor laser(200, 0.5f, 0.01f);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Move sensor to mouse position
        Vector2 mousePos = GetMousePosition();
        laser.position = mousePos;

        // Sense obstacles
        auto sensorData = laser.senseObstacles(env.originalMapImage);
        env.storeData(sensorData);

        // Draw everything
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw the progressively revealed map
        DrawTexture(env.revealedMapTexture, 0, 0, WHITE);

        // Draw LIDAR rays and sensor
        laser.drawLaserRays(sensorData);

        // Draw accumulated point cloud
        env.drawSensorData();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
