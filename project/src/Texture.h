#pragma once
#include <d3d11.h>
#include <SDL_system.h>
#include <string>

#include "ColorRGB.h"
#include "Vector.h"

class Texture
{
public:
    Texture(const std::string& fileName, ID3D11Device* pDevice);
    ~Texture();

    ID3D11ShaderResourceView* D3D11GetTexture2D() const;
    ColorRGB Sample(const Vector2 *uvCoord, bool normalMap = false) const;


private:
    ID3D11Texture2D* m_pResource{};
    ID3D11ShaderResourceView* m_pSRV{};

    SDL_Surface* m_pSurface{};
    uint32_t* m_pSurfacePixels{ };
};
