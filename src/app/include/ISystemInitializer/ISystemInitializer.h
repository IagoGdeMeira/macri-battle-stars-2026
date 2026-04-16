#ifndef i_system_initializer_h
#define i_system_initializer_h

class ISystemInitializer
{
public:
    virtual ~ISystemInitializer() = default;
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
};

#endif // i_system_initializer_h
