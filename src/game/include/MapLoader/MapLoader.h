#ifndef map_loader_h
#define map_loader_h

#include "domain/components/MapComponent.h"
#include "domain/include/Entity/Entity.h"

#include "engine/include/DataParser/DataParser.h"

class EntityFactory;
class PushboxLoader;
class World;

class MapLoader
{
public:
    MapLoader(DataParser& parser, EntityFactory& factory, PushboxLoader& pushboxLoader) :
        parser(parser), factory(factory), pushboxLoader(pushboxLoader) {}

    Entity load(World& world, const std::string& path);

private:
    DataParser& parser;
    EntityFactory& factory;
    PushboxLoader& pushboxLoader;

    MapComponent parseMapComponent(const std::unique_ptr<DataNode>& root);
    void createBackgrounds(const std::unique_ptr<DataNode>& root, Entity mapEntity);
    void createCollisionGeometry(const std::unique_ptr<DataNode>& root, Entity mapEntity);

    struct BackgroundParams
    { const std::unique_ptr<DataNode>& layer; Entity mapEntity; Position parallax{1.f, 1.f}, position{0.f, 0.f}; };
    void createBackgroundTexture(const BackgroundParams& params);
    void createBackgroundRectangle(const BackgroundParams& params);
    void createBackgroundCircle(const BackgroundParams& params);
    void createBackgroundAnimated(const BackgroundParams& params);
};

#endif // map_loader_h
