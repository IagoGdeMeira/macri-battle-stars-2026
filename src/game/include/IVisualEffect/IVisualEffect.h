#ifndef i_visual_effect_h
#define i_visual_effect_h

template <typename BatchType, typename CommandType>
class IVisualEffect
{
public:
    virtual ~IVisualEffect() = default;
    virtual void apply(BatchType& batch, const CommandType& base) const = 0;
};

#endif // i_visual_effect_h
