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
class Font;

struct UIElement;

class UIFactory
{
public:
    UIFactory(World& world, IFontFactory& fontFactory, ITextureFactory& textureFactory) :
        factoryWorld(world), fontFactory(fontFactory), textureFactory(textureFactory) {}

    World& world() { return this->factoryWorld; }

    Entity createPanel(const Rectangle& rect);
    Entity createButton(const std::string& text, const Rectangle& rect, std::shared_ptr<IUIAction> action);
    Entity createImage(const std::string& texturePath, const Rectangle& rect);

    struct ShapeParams { const Color& color; bool filled = true; };
    Entity createRectangleShape(const ShapeParams& params, const Rectangle& rect);
    Entity createCircleShape(const ShapeParams& params, const Circle& circle);

    struct TextParams { const std::string& text; float fontSize; const Color& color; const Position& position; };
    Entity createText(const TextParams& params);
    Entity createText(const TextParams& params, std::shared_ptr<Font> font);
    
    Entity createFromElement(const UIElement& element);

private:
    World& factoryWorld;
    IFontFactory& fontFactory;
    ITextureFactory& textureFactory;

    void applyDefaultBoxModel(Entity& entity);
};

#endif // ui_factory_h
