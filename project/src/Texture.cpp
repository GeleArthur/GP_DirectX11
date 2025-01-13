#include "Texture.h"

#include <d3d11.h>
#include <dxgiformat.h>
#include <iostream>
#include <SDL_image.h>

#include "DirectXUtils.h"

Texture::Texture(const std::string& fileName, ID3D11Device* pDevice)
{
    m_pSurface = IMG_Load(fileName.c_str());
    if (m_pSurface == nullptr)
    {
        std::cerr << "Can't find texture\n";
        return;
    }

    m_pSurfacePixels = static_cast<uint32_t*>(m_pSurface->pixels);

    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = m_pSurface->w;
    desc.Height = m_pSurface->h;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = format;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = m_pSurface->pixels;
    initData.SysMemPitch = static_cast<UINT>(m_pSurface->pitch);
    initData.SysMemSlicePitch = static_cast<UINT>(m_pSurface->h * m_pSurface->pitch);
    
    CallDirectX(pDevice->CreateTexture2D(&desc, &initData, &m_pResource));

    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
    SRVDesc.Format = format;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MipLevels = 1;

    CallDirectX(pDevice->CreateShaderResourceView(m_pResource, &SRVDesc, &m_pSRV));
}

Texture::~Texture()
{
    if (m_pSRV) m_pSRV->Release();
    if (m_pResource) m_pResource->Release();
    SDL_FreeSurface(m_pSurface);
}

ID3D11ShaderResourceView* Texture::D3D11GetTexture2D() const
{
    return m_pSRV;
}

ColorRGB Texture::Sample(const Vector2& uvCoord, bool normalMap) const
{
    Uint8 red, green, blue;
    Vector2 clamped = { std::clamp(uvCoord.x, 0.0f, 1.0f), std::clamp(uvCoord.y, 0.0f, 1.0f) };

    const uint32_t pixel = m_pSurfacePixels[
        static_cast<int>(clamped.x * static_cast<float>(m_pSurface->w - 1)) +
        static_cast<int>(clamped.y * static_cast<float>(m_pSurface->h - 1)) * (m_pSurface->w)
    ];

    SDL_GetRGB(pixel, m_pSurface->format, &red, &green, &blue);
    
    return ColorRGB{static_cast<float>(red) / 255.0f, static_cast<float>(green) / 255.0f, static_cast<float>(blue) / (normalMap ? 255.0f : 128.0f)};
}
