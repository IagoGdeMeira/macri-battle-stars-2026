#ifndef map_data_h
#define map_data_h

#include "../../../domain/include/Geometry/Geometry.h"

#include <cstdint>
#include <string>
#include <vector>

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
    float floorWidth, floorHeight;
    std::vector<Rectangle> walls;
    std::vector<SpawnPoint> spawnPoints;
    float gravity = 980.0f;
    float floorFriction = 5.0f;
    float airFriction = 2.0f;
};

#endif // map_data_h
