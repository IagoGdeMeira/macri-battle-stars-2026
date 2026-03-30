#ifndef input_adapter_h
#define input_adapter_h

class InputAdapter
{
public:
    virtual ~InputAdapter() = default;

    virtual void poll() = 0;
};

#endif // input_adapter_h
