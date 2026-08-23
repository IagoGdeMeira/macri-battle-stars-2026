#ifndef game_component_registry_h
#define game_component_registry_h

class ComponentManager;

class GameComponentRegistry
{
public:
    GameComponentRegistry() = delete;
    static void registerAll(ComponentManager& comp);
};

#endif // game_component_registry_h
