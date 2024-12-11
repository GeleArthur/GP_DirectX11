#pragma once
#include <d3d11.h>
#include <SDL_system.h>
#include <string>

class Texture
{
public:
    Texture(const std::string& fileName, ID3D11Device* pDevice);
    ~Texture();

    ID3D11ShaderResourceView* GetTexture2D() const;

private:
    ID3D11Texture2D* m_pResource;
    ID3D11ShaderResourceView* m_pSRV;
};
