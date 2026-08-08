#include "Scene/Scene.h"

#include "CommandBuffer/CommandBuffer.h"

void Scene::update(float deltaTime)
{
    CommandBuffer commands;
    UpdateContext context { this->localWorld, this->eventBus, commands, deltaTime };
    this->systemManager.update(context);
    commands.flush(this->localWorld);
}
