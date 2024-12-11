#include "Texture.h"

#include <d3d11.h>
#include <dxgiformat.h>
#include <iostream>
#include <SDL_image.h>

Texture::Texture(const std::string& fileName, ID3D11Device* pDevice)
{
    SDL_Surface* pSurface = IMG_Load(fileName.c_str());
    if (pSurface == nullptr)
    {
        std::cerr << "Can't find texture\n";
    }

    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = pSurface->w;
    desc.Height = pSurface->h;
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
    initData.pSysMem = pSurface->pixels;
    initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
    initData.SysMemSlicePitch = static_cast<UINT>(pSurface->h * pSurface->pitch);
    
    HRESULT result = pDevice->CreateTexture2D(&desc, &initData, &m_pResource);
    if (FAILED(result))
        std::cerr << "Can't load texture\n";

    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
    SRVDesc.Format = format;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MipLevels = 1;

    result = pDevice->CreateShaderResourceView(m_pResource, &SRVDesc, &m_pSRV);

    
    SDL_FreeSurface(pSurface);
}

Texture::~Texture()
{
    m_pSRV->Release();
    m_pResource->Release();
}

ID3D11ShaderResourceView* Texture::GetTexture2D() const
{
    return m_pSRV;
}
