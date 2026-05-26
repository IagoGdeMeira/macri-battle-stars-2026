#ifndef draw_batch_h
#define draw_batch_h

#include <algorithm>
#include <vector>

class Renderer;

template <typename CommandType>
class DrawBatch
{
public:
    virtual ~DrawBatch() = default;

    void add(const CommandType& cmd) { this->commands.push_back(cmd); }
    void clear() { this->commands.clear(); }

    virtual void submit(Renderer& renderer) = 0;

protected:
    std::vector<CommandType> commands;

    void sort()
    {
        std::stable_sort(this->commands.begin(), this->commands.end(), [](
            const CommandType& a, const CommandType& b
        ) {
            if (a.layer != b.layer) return a.layer < b.layer;
            if (a.zIndex != b.zIndex) return a.zIndex < b.zIndex;
            return a.order < b.order;
        });
    }
};

#endif // draw_batch_h
