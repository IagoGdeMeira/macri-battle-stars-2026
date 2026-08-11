#ifndef hud_visibility_system_h
#define hud_visibility_system_h

#include "domain/include/Entity/Entity.h"

#include "engine/include/System/System.h"

class World;

class HUDVisibilitySystem : public System
{
public:
    void update(UpdateContext& ctx) override;
    void setVisible(bool visible);

private:
    bool targetVisibility = true, currentVisibility = true;
    
    void setActiveRecursive(World& world, Entity entity, bool active);
};

#endif // hud_visibility_system_h
