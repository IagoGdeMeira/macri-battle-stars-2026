#ifndef i_scene_factory_h
#define i_scene_factory_h

#include <any>
#include <memory>

enum class SceneId;
class Scene;

class ISceneFactory
{
public:
    virtual ~ISceneFactory() = default;
    virtual std::unique_ptr<Scene> createScene(SceneId id, std::any data) = 0;
};

#endif // i_scene_factory_h
