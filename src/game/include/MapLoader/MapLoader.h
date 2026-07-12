#ifndef map_loader_h
#define map_loader_h

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
};

#endif // map_loader_h
