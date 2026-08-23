#ifndef hud_component_registry_h
#define hud_component_registry_h

class ComponentManager;

class HUDComponentRegistry
{
public:
    HUDComponentRegistry() = delete;
    static void registerAll(ComponentManager& comp);
};

#endif // hud_component_registry_h
