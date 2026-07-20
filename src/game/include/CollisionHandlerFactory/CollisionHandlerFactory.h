#ifndef collision_handler_factory_h
#define collision_handler_factory_h

#include "ICollisionHandler/ICollisionHandler.h"

#include <memory>

class CollisionHandlerFactory
{
public:
    static std::unique_ptr<ICollisionHandler> createForEntity(
        UpdateContext& ctx, ICollisionHandler::EntityParams params
    );
};

#endif // collision_handler_factory_h
