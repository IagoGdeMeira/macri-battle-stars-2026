#ifndef texture_h
#define texture_h

class Texture
{
public:
    virtual ~Texture() = default;

    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
};

#endif // texture_h
