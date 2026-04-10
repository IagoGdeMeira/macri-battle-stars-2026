#ifndef texture_loader_h
#define texture_loader_h

#include "../../engine/include/ResourceLoader/ResourceLoader.h"
#include "../../engine/include/Renderer/Renderer.h"

class TextureLoader : public ResourceLoader<Texture>
{
public:
    TextureLoader(Renderer& renderer) : renderer(renderer) {}

    std::shared_ptr<Texture> load(const std::string& path) override
    { return renderer.createTexture(path); }

private:
    Renderer& renderer;
};

#endif // texture_loader_h
