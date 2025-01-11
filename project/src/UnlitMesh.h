#pragma once
#include <memory>
#include "BaseMeshEffect.h"
#include "Utils.h"

class Texture;

class UnlitMesh final : public BaseMeshEffect
{
public:
    explicit UnlitMesh(ID3D11Device* pDevice);
    ~UnlitMesh() override;
    UnlitMesh(UnlitMesh& other) = delete;
    UnlitMesh(UnlitMesh&& other) = delete;
    UnlitMesh& operator=(UnlitMesh& other) = delete;
    UnlitMesh& operator=(UnlitMesh&& other) = delete;

    void RenderDirectX(ID3D11DeviceContext *pDeviceContext, const Camera& camera) override;
    void RenderSoftware() override;
    void LoadMeshData(std::vector<Vector3>&& position, std::vector<Vector2>&& inputUv, std::vector<uint32_t>&& indices, const std::string& diffuseTextureFilePath);

private:
    ID3D11Device* m_pDevice{};
    std::vector<Vector3> m_Positions;
    std::vector<uint32_t> m_Indices;
    std::vector<Vector2> m_Uv;
    Texture* m_DiffuseTexture{};
    
    std::unique_ptr<ID3D11Buffer, callRelease<ID3D11Buffer>> m_pVertexBuffer;
    std::unique_ptr<ID3D11Buffer, callRelease<ID3D11Buffer>> m_pIndexBuffer;

    // Hold the references in this instances
    std::shared_ptr<ID3D11InputLayout> m_InputLayout;
    std::shared_ptr<ID3DX11Effect> m_Effect;
    std::shared_ptr<ID3DX11EffectTechnique> m_Technique;

    // Resources for all DiffuseMesh classes
    static std::weak_ptr<ID3D11InputLayout> resourceInputLayout;
    static std::weak_ptr<ID3DX11Effect> resourceEffect;
    static std::weak_ptr<ID3DX11EffectTechnique> resourceTechnique;
};
