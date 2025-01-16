#pragma once

enum class CullMode : uint8_t
{
    none,
    front,
    back
};

enum class SampleMethod : uint8_t
{
    point,
    linear,
    anisotropic
};

enum class ShadingMode : uint8_t
{
    observed_area,
    diffuse,
    specular,
    combined
};
