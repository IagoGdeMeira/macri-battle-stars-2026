#include "FaceOffSystem/FaceOffSystem.h"

#include "domain/components/OrientationComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/events/OrientationChangedEvent.h"
#include "domain/include/View/View.h"
#include "domain/utils/Logger/Logger.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <algorithm>
#include <vector>

void FaceOffSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<TransformComponent, PlayerComponent, OrientationComponent>(comp);

    std::vector<PlayerInfo> players;

    for (auto [entity, transform, p_, o_] : view) players.push_back({entity, transform.position.x});

    if (players.size() < 2) return;

    for (size_t i = 0; i < players.size() - 1; ++i) for (size_t j = i + 1; j < players.size(); ++j)
    { if (players[i].x > players[j].x) std::swap(players[i], players[j]); }

    for (size_t i = 0; i < players.size(); ++i)
    {
        auto& orientation = comp.get<OrientationComponent>(players[i].entity);
        Orientation newOrientation = (i == 0) ? Orientation::Right : Orientation::Left;

        if (orientation.direction == newOrientation) continue;
        
        Orientation previous = orientation.direction;
        orientation.direction = newOrientation;

        auto& playerComp = comp.get<PlayerComponent>(players[i].entity);
        LOG_DEBUG("FaceOffSystem: Orientation changed for player {} (entity {}) to {}",
            playerComp.id, players[i].entity.id, (newOrientation == Orientation::Left ? "Left" : "Right"));

        this->bus.emit<OrientationChangedEvent>(OrientationChangedEvent{players[i].entity, previous, newOrientation});
    }
}
