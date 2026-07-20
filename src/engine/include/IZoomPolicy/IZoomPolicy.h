#ifndef i_zoom_policy_h
#define i_zoom_policy_h

class IZoomPolicy
{
public:
    virtual ~IZoomPolicy() = default;
    virtual bool applyZoom() const = 0;
};

#endif // i_zoom_policy_h
