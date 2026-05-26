#ifndef visual_effects_component_h
#define visual_effects_component_h

#include <functional>
#include <vector>

class DrawTextureBatch;
class DrawFontBatch;
class DrawRectangleBatch;
class DrawCircleBatch;

struct DrawTextureCommand;
struct DrawFontCommand;
struct DrawRectangleCommand;
struct DrawCircleCommand;

struct VisualEffectsComponent
{
    using TextureEffect = std::function<void(DrawTextureBatch&, DrawTextureCommand&)>;
    using FontEffect = std::function<void(DrawFontBatch&, DrawFontCommand&)>;
    using RectangleEffect = std::function<void(DrawRectangleBatch&, DrawRectangleCommand&)>;
    using CircleEffect = std::function<void(DrawCircleBatch&, DrawCircleCommand&)>;

    std::vector<TextureEffect> textureEffects;
    std::vector<FontEffect> fontEffects;
    std::vector<RectangleEffect> rectangleEffects;
    std::vector<CircleEffect> circleEffects;
};

#endif // visual_effects_component_h
