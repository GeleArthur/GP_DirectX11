#pragma once
#include <memory>
#include "BaseMeshEffect.h"
#include "Camera.h"
#include "ColorRGB.h"
#include "Matrix.h"
#include "SoftwareRendererHelpers.h"
#include "Utils.h"

class Texture;

struct UnlitData
{
    Vector3 position;
    Vector2 uv;
    

    // auto AsTuple(){return std::tie(position, uv);}
};

struct UnlitDataVertexOut
{
    Vector4 position;
    Vector2 uv;

    // Wonder if there is a better way
    auto AsTuple() const{return std::make_tuple(position, uv);}
    static UnlitDataVertexOut FromTuple(const decltype(std::make_tuple(position, uv))& in)
    {
        return {std::get<0>(in), std::get<1>(in)};
    }
};

// using UnlitData = std::tuple<Vector3, Vector2>;

// using UnlitDataVertexOut = std::tuple<Vector4, Vector2>;

class UnlitMesh final : public BaseMeshEffect
{
public:
    explicit UnlitMesh(ID3D11Device* pDevice);
    ~UnlitMesh() override;
    UnlitMesh(UnlitMesh& other) = delete;
    UnlitMesh(UnlitMesh&& other) = delete;
    UnlitMesh& operator=(UnlitMesh& other) = delete;
    UnlitMesh& operator=(UnlitMesh&& other) = delete;

    void RenderDirectX(ID3D11DeviceContext *pDeviceContext, const Scene& scene) override;
    void RenderSoftware(SoftwareRendererHelper* softwareRendererHelper, const Scene& scene) override;
    void LoadMeshData(std::vector<UnlitData>&& vertexData, std::vector<uint32_t>&& indices, const std::string& diffuseTextureFilePath);
    void SetWorldMatrix(Matrix<float> matrix) override;

private:
    // -- SoftWare
    std::vector<UnlitData> m_VertexData;
    std::vector<uint32_t> m_Indices;
    std::vector<UnlitDataVertexOut> m_VertexDataOut;
    std::vector<Triangle<UnlitDataVertexOut>> m_TrianglesOut;

    void VertexStage(const std::vector<UnlitData>& vertices_in, std::vector<UnlitDataVertexOut>& vertices_out, const Camera& camera) const;

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
