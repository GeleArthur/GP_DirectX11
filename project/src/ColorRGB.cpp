#include "ColorRGB.h"
#include "Utils.h"


void ColorRGB::MaxToOne() {
    const float maxValue = std::max({r, g, b});
    if (maxValue > 1.f)
        *this /= maxValue;
}

ColorRGB ColorRGB::Lerp(const ColorRGB &c1, const ColorRGB &c2, const float factor) {
    return {Utils::Lerpf(c1.r, c2.r, factor), Utils::Lerpf(c1.g, c2.g, factor), Utils::Lerpf(c1.b, c2.b, factor)};
}

const ColorRGB &ColorRGB::operator+=(const ColorRGB &c) {
    r += c.r;
    g += c.g;
    b += c.b;

    return *this;
}

ColorRGB ColorRGB::operator+(const ColorRGB &c) const {
    return {r + c.r, g + c.g, b + c.b};
}

const ColorRGB & ColorRGB::operator-=(const ColorRGB &c) {
    r -= c.r;
    g -= c.g;
    b -= c.b;

    return *this;
}

ColorRGB ColorRGB::operator-(const ColorRGB &c) const {
    return {r - c.r, g - c.g, b - c.b};
}

const ColorRGB &ColorRGB::operator*=(const ColorRGB &c) {
    r *= c.r;
    g *= c.g;
    b *= c.b;

    return *this;
}

ColorRGB ColorRGB::operator*(const ColorRGB &c) const {
    return {r * c.r, g * c.g, b * c.b};
}

const ColorRGB &ColorRGB::operator/=(const ColorRGB &c) {
    r /= c.r;
    g /= c.g;
    b /= c.b;

    return *this;
}

const ColorRGB &ColorRGB::operator*=(const float s) {
    r *= s;
    g *= s;
    b *= s;

    return *this;
}

ColorRGB ColorRGB::operator*(const float s) const {
    return {r * s, g * s, b * s};
}

const ColorRGB &ColorRGB::operator/=(const float s) {
    r /= s;
    g /= s;
    b /= s;

    return *this;
}

ColorRGB ColorRGB::operator/(const float s) const {
    return {r / s, g / s, b / s};
}

//ColorRGB (Global) Operators
ColorRGB operator*(const float s, const ColorRGB &c) {
    return c * s;
}
