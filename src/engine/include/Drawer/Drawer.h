#ifndef drawer_h
#define drawer_h

#include "../Font/Font.h"
#include "../RenderContext/RenderContext.h"
#include "../Renderer/Renderer.h"
#include "../Texture/Texture.h"

#include "../../../domain/include/Color/Color.h"
#include "../../../domain/include/Geometry/Geometry.h"

class Drawer
{
public:
    virtual ~Drawer() = default;

    virtual void draw(RenderContext& ctx) = 0;

protected:
    struct SpriteCommand
    {
        Texture* texture = nullptr;
        Rectangle dest;
        float rotation = 0.0f;
        bool flipX = false, flipY = false;
        int layer = 0, zIndex = 0;
        size_t order = 0;
        Rectangle source;
        bool useSourceRect = false;
        Color tint = Color::WHITE();
        Renderer::BlendMode blend = Renderer::BlendMode::Normal;
    };

    struct TextCommand
    {
        std::string text;
        Font* font = nullptr;
        Rectangle dest;
        int fontSize = 16;
        Color color = Color::WHITE();
        int layer = 0, zIndex = 0;
        size_t order = 0;
    };
};

#endif // drawer_h
