#ifndef i_render_format_h
#define i_render_format_h

struct RenderContext;
struct Viewport;

class IRenderFormat
{
public:
    virtual ~IRenderFormat() = default;
    
    virtual void render(RenderContext& ctx) = 0;
    virtual void setViewport(const Viewport& vp) = 0;
};

#endif // i_render_format_h
