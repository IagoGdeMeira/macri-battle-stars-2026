#ifndef i_component_storage_h
#define i_component_storage_h

class IComponentStorage
{
public:
    virtual ~IComponentStorage() = default;
    virtual void remove(Entity e) = 0;
};

#endif // i_component_storage_h
