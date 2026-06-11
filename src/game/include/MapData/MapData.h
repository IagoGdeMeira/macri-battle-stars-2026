#ifndef map_data_h
#define map_data_h

#include "../../domain/include/Geometry/Geometry.h"

#include <cstdint>
#include <string>
#include <vector>

struct BackgroundLayer
{
    std::string texturePath;
    Position parallaxFactor { 1.f, 1.f };
    int zIndex;
    bool repeatX;
};

struct SpawnPoint { std::uint32_t playerId; float x; };

struct MapData
{
    std::string name;
    std::vector<BackgroundLayer> backgroundLayers;
    float floorY, gravity = 980.f, floorFriction = 5.f, airFriction = 2.f;
    std::string floorTexture;
    Dimension2D floorSize { 0.f, 0.f };
    std::vector<Rectangle> walls;
    std::vector<SpawnPoint> spawnPoints;
};

#endif // map_data_h
