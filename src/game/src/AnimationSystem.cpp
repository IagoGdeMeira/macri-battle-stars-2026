#include "AnimationSystem/AnimationSystem.h"

#include "domain/components/AnimationComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/include/View/View.h"
#include "domain/utils/Logger/Logger.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

void AnimationSystem::update(UpdateContext& ctx)
{
    auto view = View<AnimationComponent, SpriteComponent>(ctx.world.components());
    for (auto [entity, anim, sprite] : view)
    {
        if (anim.animation.frames.empty()) continue;

        anim.elapsedTime += ctx.deltaTime;

        const float frameDuration = anim.animation.frameDuration;
        while (anim.elapsedTime >= frameDuration)
        {
            anim.elapsedTime -= frameDuration;
            anim.currentFrame++;

            if (anim.currentFrame < (int)anim.animation.frames.size()) continue;
            
            if (anim.animation.loop) anim.currentFrame = 0;
            else anim.currentFrame = (int)anim.animation.frames.size() - 1;
        }

        const auto& frame = anim.animation.frames[anim.currentFrame];

        auto& pos = sprite.source.position;
        auto& size = sprite.source.size;

        pos.x = static_cast<float>(frame.x);
        pos.y = static_cast<float>(frame.y);
        size.width = static_cast<float>(frame.width);
        size.height = static_cast<float>(frame.height);
        sprite.useSourceRect = true;
    }
}
