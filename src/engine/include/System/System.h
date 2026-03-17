#ifndef system_h
#define system_h

struct UpdateContext;

class System
{
public:
    virtual ~System() = default;

    virtual void update(UpdateContext& context) = 0;
};

#endif // system_h
