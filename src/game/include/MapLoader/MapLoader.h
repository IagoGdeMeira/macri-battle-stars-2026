#ifndef map_loader_h
#define map_loader_h

#include "../../domain/components/MapComponent.h"
#include "../../domain/include/Entity/Entity.h"

#include "../../engine/include/DataParser/DataParser.h"

class EntityFactory;
class World;

class MapLoader
{
public:
    MapLoader(DataParser& parser, EntityFactory& factory) : parser(parser), factory(factory) {}
    Entity load(World& world, const std::string& path);

private:
    DataParser& parser;
    EntityFactory& factory;

    MapComponent parseMapComponent(const std::unique_ptr<DataNode>& root);
    void createBackgrounds(const std::unique_ptr<DataNode>& root, Entity mapEntity);
    void createFloor(const std::unique_ptr<DataNode>& root, Entity mapEntity, World& world);
    void createWalls(const std::unique_ptr<DataNode>& root, Entity mapEntity);
};

#endif // map_loader_h
