#include "../include/AnimationSystem/AnimationSystem.h"

#include "../../domain/components/AnimationComponent.h"
#include "../../domain/components/SpriteComponent.h"

#include "../../domain/include/View/View.h"
#include "../../engine/include/UpdateContext/UpdateContext.h"

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

            if (anim.currentFrame >= (int)anim.animation.frames.size())
            {
                if (anim.animation.loop) anim.currentFrame = 0;
                else anim.currentFrame = (int)anim.animation.frames.size() - 1;
            }
        }

        const auto& frame = anim.animation.frames[anim.currentFrame];

        sprite.srcX = frame.x;
        sprite.srcY = frame.y;
        sprite.srcWidth = frame.width;
        sprite.srcHeight = frame.height;
        sprite.useSourceRect = true;
    }
}