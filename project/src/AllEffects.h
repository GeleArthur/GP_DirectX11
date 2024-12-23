//
// Created by a on 22/12/2024.
//

#ifndef ALLEFFECTS_H
#define ALLEFFECTS_H
#include <string>
#include <iostream>

struct AttributePosition{static constexpr bool isPresent = true; };
struct AttributeNormal{static constexpr bool isPresent = true; };
struct AttributeTexCoord{static constexpr bool isPresent = true; };
struct AttributeViewDirection{static constexpr bool isPresent = true; };


class AllEffects {
public:
    template<typename... Attributes>
    explicit constexpr AllEffects()
    {
        (configureFor<Attributes>(), ...);
    }

    template <typename Attribute>
    static constexpr void configureFor() {
        if constexpr (std::is_same_v<Attribute, AttributePosition> && Attribute::isPresent) {
            std::cout << "Shader: Configuring for Position\n";
        }
        if constexpr (std::is_same_v<Attribute, AttributeNormal> && Attribute::isPresent) {
            std::cout << "Shader: Configuring for Normal\n";
        }
        if constexpr (std::is_same_v<Attribute, AttributeTexCoord> && Attribute::isPresent) {
            std::cout << "Shader: Configuring for TexCoord\n";
        }
    }
};

#endif //ALLEFFECTS_H
