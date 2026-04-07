#include "../include/RenderSystem/RenderSystem.h"

#include "../include/UpdateContext/UpdateContext.h"

#include "../../domain/include/View/View.h"

void RenderSystem::update(UpdateContext& ctx)
{
    renderer.beginFrame();

    auto view = View<TransformComponent, SpriteComponent>(ctx.world.components());
    for (auto [entity, transform, sprite] : view) renderer.draw(transform, sprite);

    renderer.endFrame();
}
