#ifndef resource_loader_h
#define resource_loader_h

#include <memory>
#include <string>

template<typename T>
class ResourceLoader
{
public:
    virtual ~ResourceLoader() = default;

    virtual std::shared_ptr<T> load(const std::string& path) = 0;
};

#endif // resource_loader_h
