#ifndef ui_factory_h
#define ui_factory_h

#include "../../../domain/include/Color/Color.h"
#include "../../../domain/include/Entity/Entity.h"
#include "../../../domain/include/Geometry/Geometry.h"

#include <string>

class IFontFactory;
class ITextureFactory;
class World;

class UIFactory
{
public:
    UIFactory(World& world, IFontFactory& fontFactory, ITextureFactory& textureFactory) :
        world(world), fontFactory(fontFactory), textureFactory(textureFactory) {}

    Entity createPanel(const Rectangle& rect);
    Entity createButton(const std::string& text, const std::string& actionId, const Rectangle& rect);
    Entity createText(const std::string& text, float fontSize, const Color& color, const Position& position);
    Entity createImage(const std::string& texturePath, const Rectangle& rect);

private:
    World& world;
    IFontFactory& fontFactory;
    ITextureFactory& textureFactory;

    void applyDefaultBoxModel(Entity entity);
};

#endif // ui_factory_h
