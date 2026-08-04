#ifndef component_registry_h
#define component_registry_h

class ComponentManager;

class ComponentRegistry
{
public:
    ComponentRegistry() = delete;
    static void registerAll(ComponentManager& comp);
};

#endif // component_registry_h
