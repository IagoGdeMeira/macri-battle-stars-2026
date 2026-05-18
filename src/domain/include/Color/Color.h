#ifndef color_h
#define color_h

#include <cstdint>

struct Color
{
    std::uint8_t r = 255, g = 255, b = 255, a = 255;

    bool operator==(const Color& other) const
    {
        if (this == &other) return true;

        if (this->r != other.r) return false;
        if (this->g != other.g) return false;
        if (this->b != other.b) return false;
        if (this->a != other.a) return false;
        return true;
    }

    bool operator!=(const Color& other) const { return !(*this == other); }

    static constexpr Color WHITE() { return Color{255, 255, 255, 255}; }
    static constexpr Color BLACK() { return Color{0, 0, 0, 255}; }
    static constexpr Color RED() { return Color{255, 0, 0, 255}; }
    static constexpr Color GREEN() { return Color{0, 255, 0, 255}; }
    static constexpr Color BLUE() { return Color{0, 0, 255, 255}; }
};

#endif // color_h
