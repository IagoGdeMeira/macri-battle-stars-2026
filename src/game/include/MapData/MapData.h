#ifndef map_data_h
#define map_data_h

#include <cstdint>
#include <string>
#include <vector>

struct Rectangle { float x, y, width, height; };

struct BackgroundLayer
{
    std::string texturePath;
    float parallaxFactorX, parallaxFactorY;
    int zIndex;
    bool repeatX;
};

struct SpawnPoint { std::uint32_t playerId; float x; };

struct MapData
{
    std::string name;
    std::vector<BackgroundLayer> backgroundLayers;
    float floorY;
    std::string floorTexture;
    float floorWidth;
    float floorHeight;
    std::vector<Rectangle> walls;
    std::vector<SpawnPoint> spawnPoints;
};

#endif // map_data_h
