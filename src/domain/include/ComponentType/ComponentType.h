#ifndef component_type_h
#define component_type_h

#include <cstddef>

class ComponentType
{
public:
    template <typename T>
    static size_t id()
    {
        static const size_t typeId = ComponentType::nextId();
        return typeId;
    }

private:
    static size_t nextId()
    {
        static size_t counter = 0;
        return counter++;
    }
};

#endif // component_type_h
