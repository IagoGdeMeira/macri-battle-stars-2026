#include "../include/Scene/Scene.h"

#include "../include/CommandBuffer/CommandBuffer.h"

void Scene::update(float deltaTime)
{
    CommandBuffer commands;

    UpdateContext context { world, eventBus, commands, deltaTime };
    
    this->systems.update(context);

    commands.flush(this->world);
}
