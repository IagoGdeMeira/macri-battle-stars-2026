#ifndef font_h
#define font_h

class Font
{
public:
    virtual ~Font() = default;

    virtual int getAscent(int size) const = 0;
    virtual int getDescent(int size) const = 0;
    virtual int getHeight(int size) const = 0;
};

#endif // font_h
