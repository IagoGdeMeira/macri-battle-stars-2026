#include "../include/Scene/Scene.h"

void Scene::update(float deltaTime)
{
    UpdateContext context { world, eventBus, deltaTime };
    systems.update(context);
}
