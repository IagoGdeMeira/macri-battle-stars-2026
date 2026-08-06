#include "Scene/Scene.h"

#include "CommandBuffer/CommandBuffer.h"

#include "domain/utils/Logger/Logger.h"

void Scene::update(float deltaTime)
{
    LOG_DEBUG("Scene: updating with deltaTime={:.4f}", deltaTime);

    CommandBuffer commands;
    UpdateContext context { this->localWorld, this->eventBus, commands, deltaTime };
    this->systemManager.update(context);
    commands.flush(this->localWorld);
}
