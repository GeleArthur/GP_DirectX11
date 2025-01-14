#include "TextureManager.h"

#include "Texture.h"

std::unordered_map<std::string, std::weak_ptr<Texture>> TextureManager::m_TextureDataBase;


std::shared_ptr<Texture> TextureManager::GetTexture(const std::string& fileName, ID3D11Device* pDevice)
{
    if (m_TextureDataBase.contains(fileName))
    {
        return m_TextureDataBase[fileName].lock();
    }

    auto texture = std::make_shared<Texture>(fileName,pDevice);
    m_TextureDataBase[fileName] = texture;
    return texture;
}
