#pragma once
#include <memory>
#include "BaseMeshEffect.h"
#include "Camera.h"
#include "Matrix.h"
#include "SoftwareRendererHelpers.h"
#include "Utils.h"

class Texture;

struct FireFXData
{
    Vector3 position;
    Vector2 uv;
    Vector3 normal;
    Vector3 tangent;
};

struct FireFXDataVertexOut
{
    Vector4 position;
    Vector2 uv;

    auto AsTuple() const{return std::make_tuple(position, uv);}
    static FireFXDataVertexOut FromTuple(const decltype(std::make_tuple(position, uv))& in)
    {
        return {std::get<0>(in), std::get<1>(in)};
    }
};

class FireFX final : public BaseMeshEffect
{
public:
    explicit FireFX(ID3D11Device* pDevice);
    ~FireFX() override;
    FireFX(FireFX& other) = delete;
    FireFX(FireFX&& other) = delete;
    FireFX& operator=(FireFX& other) = delete;
    FireFX& operator=(FireFX&& other) = delete;

    void RenderDirectX(ID3D11DeviceContext *pDeviceContext, const Scene& scene) override;
    void RenderSoftware(SoftwareRendererHelper* softwareRendererHelper, const Scene& scene) override;
    void LoadMeshData(std::vector<FireFXData>&& vertexData, std::vector<uint32_t>&& indices, const std::string& diffuseTextureFilePath);
    void SetWorldMatrix(Matrix<float> matrix) override;
    void ToggleEnabled();
    bool IsEnabled() override;

    bool m_IsEnabled{ true };

private:
    // -- SoftWare
    std::vector<FireFXData> m_VertexData;
    std::vector<uint32_t> m_Indices;
    std::vector<FireFXDataVertexOut> m_VertexDataOut;
    std::vector<Triangle<FireFXDataVertexOut>> m_TrianglesOut;

    void VertexStage(const std::vector<FireFXData>& vertices_in, std::vector<FireFXDataVertexOut>& vertices_out, const Camera& camera) const;

    // --- Shared ---
    Matrix<float> m_WorldMatrix{};
    std::shared_ptr<Texture> m_DiffuseTexture{};
    
    // --- HardWare ---
    ID3D11Device* m_pDevice{};
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
