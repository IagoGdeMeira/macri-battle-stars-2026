#ifndef render_queue_h
#define render_queue_h

#include "DrawCommand/DrawCommand.h"
#include "Renderer/Renderer.h"

#include <algorithm>
#include <memory>
#include <vector>

class RenderQueue
{
public:
    template <typename T, typename... Args>
    T& emplace(Args&&... args)
    {
        auto cmd = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = cmd.get();
        this->commands.push_back(std::move(cmd));
        return *ptr;
    }

    void add(std::unique_ptr<DrawCommand> cmd) { this->commands.push_back(std::move(cmd)); }

    void submit(Renderer& renderer)
    {
        std::stable_sort(this->commands.begin(), this->commands.end(),
            [](const std::unique_ptr<DrawCommand>& a, const std::unique_ptr<DrawCommand>& b)
            { return a->getSortKey() < b->getSortKey(); });

        for (const auto& cmd : this->commands) renderer.draw(*cmd);
        this->clear();
    }

    void clear() { this->commands.clear(); }

private:
    std::vector<std::unique_ptr<DrawCommand>> commands;
};

#endif // render_queue_h