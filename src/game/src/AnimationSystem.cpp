#include "AnimationSystem/AnimationSystem.h"

#include "domain/components/AnimationComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/include/View/View.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

void AnimationSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<AnimationComponent, SpriteComponent>(comp);

    for (auto [entity, anim, sprite] : view)
    {
        auto& frames = anim.currentAnimation.frames;
        if (frames.empty()) continue;

        auto getFrameDuration = [&](int index) -> float
        {
            float dur = frames[index].duration;
            return dur > 0.f ? dur : anim.currentAnimation.frameDuration;
        };

        anim.elapsedTime += ctx.deltaTime;

        while (anim.elapsedTime >= getFrameDuration(anim.currentFrame))
        {
            anim.elapsedTime -= getFrameDuration(anim.currentFrame);
            anim.currentFrame++;

            if (anim.currentFrame < static_cast<int>(frames.size())) continue;

            if (anim.currentAnimation.loop) anim.currentFrame = 0;
            else
            {
                anim.currentFrame = static_cast<int>(frames.size()) - 1;
                anim.elapsedTime = 0.f;
                break;
            }
        }

        const auto& frame = frames[anim.currentFrame];
        auto& pos = sprite.source.position;
        auto& size = sprite.source.size;
        pos.x = static_cast<float>(frame.x);
        pos.y = static_cast<float>(frame.y);
        size.width = static_cast<float>(frame.width);
        size.height = static_cast<float>(frame.height);
        sprite.useSourceRect = true;
    }
}
