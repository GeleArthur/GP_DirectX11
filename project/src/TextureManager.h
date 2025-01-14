#pragma once
#include <memory>
#include <SDL_system.h>
#include <string>
#include <unordered_map>

class Texture;

class TextureManager
{
public:
    static std::shared_ptr<Texture> GetTexture(const std::string& fileName, ID3D11Device* pDevice);

private:
    static std::unordered_map<std::string, std::weak_ptr<Texture>> m_TextureDataBase;
};
