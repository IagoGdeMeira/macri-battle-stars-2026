#ifndef ui_factory_h
#define ui_factory_h

#include "domain/include/Entity/Entity.h"
#include "domain/value_objects/Color/Color.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include <memory>
#include <string>

class IFontFactory;
class ITextureFactory;
class IUIAction;
class World;

struct UIElement;

class UIFactory
{
public:
    UIFactory(World& world, IFontFactory& fontFactory, ITextureFactory& textureFactory) :
        world(world), fontFactory(fontFactory), textureFactory(textureFactory) {}

    Entity createPanel(const Rectangle& rect);
    Entity createButton(const std::string& text, const Rectangle& rect, std::shared_ptr<IUIAction> action);
    Entity createText(const std::string& text, float fontSize, const Color& color, const Position& position);
    Entity createImage(const std::string& texturePath, const Rectangle& rect);

    Entity createFromElement(const UIElement& element);

private:
    World& world;
    IFontFactory& fontFactory;
    ITextureFactory& textureFactory;

    void applyDefaultBoxModel(Entity& entity);
};

#endif // ui_factory_h
