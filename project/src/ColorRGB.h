#pragma once
#include <algorithm>

struct ColorRGB {
    float r{};
    float g{};
    float b{};

    void MaxToOne();
    static ColorRGB Lerp(const ColorRGB &c1, const ColorRGB &c2, float factor);

    const ColorRGB &operator+=(const ColorRGB &c);
    ColorRGB operator+(const ColorRGB &c) const;

    ColorRGB operator-(const ColorRGB &c) const;
    const ColorRGB &operator-=(const ColorRGB &c);

    const ColorRGB &operator*=(const ColorRGB &c);
    const ColorRGB &operator*=(float s);
    ColorRGB operator*(const ColorRGB &c) const;
    ColorRGB operator*(float s) const;

    const ColorRGB &operator/=(const ColorRGB &c);
    const ColorRGB &operator/=(float s);
    ColorRGB operator/(float s) const;
};

ColorRGB operator*(float s, const ColorRGB &c);

namespace colors {
    constexpr static ColorRGB Red{1, 0, 0};
    constexpr static ColorRGB Blue{0, 0, 1};
    constexpr static ColorRGB Green{0, 1, 0};
    constexpr static ColorRGB Yellow{1, 1, 0};
    constexpr static ColorRGB Cyan{0, 1, 1};
    constexpr static ColorRGB Magenta{1, 0, 1};
    constexpr static ColorRGB White{1, 1, 1};
    constexpr static ColorRGB Black{0, 0, 0};
    constexpr static ColorRGB Gray{0.5f, 0.5f, 0.5f};
}
