#pragma once
#include <memory>
#include "BaseMeshEffect.h"
#include "Camera.h"
#include "Matrix.h"
#include "SoftwareRendererHelpers.h"
#include "Utils.h"

class Texture;

struct PhongMeshData
{
    Vector3 position;
    Vector2 uv;
    Vector3 normal;
    Vector3 tangent;
};

struct PhongMeshDataVertexOut
{
    Vector4 position;
    Vector2 uv;
    Vector3 normal;
    Vector3 tangent;
    Vector3 viewDirection;

    // Wonder if there is a better way.
    // TODO: Template these
    auto AsTuple() const{return std::make_tuple(position, uv, normal, tangent, viewDirection);}
    static PhongMeshDataVertexOut FromTuple(const decltype(std::make_tuple(position, uv, normal, tangent, viewDirection))& in)
    {
        return {std::get<0>(in), std::get<1>(in), std::get<2>(in), std::get<3>(in), std::get<4>(in)};
    }
};


class PhongMesh final : public BaseMeshEffect
{
public:
    explicit PhongMesh(ID3D11Device* pDevice);
    ~PhongMesh() override;
    PhongMesh(PhongMesh& other) = delete;
    PhongMesh(PhongMesh&& other) = delete;
    PhongMesh& operator=(PhongMesh& other) = delete;
    PhongMesh& operator=(PhongMesh&& other) = delete;

    void RenderDirectX(ID3D11DeviceContext *pDeviceContext, const Scene& scene) override;
    void RenderSoftware(SoftwareRendererHelper* softwareRendererHelper, const Scene& scene) override;
    void LoadMeshData(std::vector<PhongMeshData>&& vertexData, std::vector<uint32_t>&& indices,
                      const std::string& diffuseTextureFilePath, const std::string& normalMapTexture,
                      const std::string& glossTexture, const std::string& specularTexture);
    void SetWorldMatrix(Matrix<float> matrix) override;
    void ToggleNormalMap();
    void SetShadingMode(ShadingMode shadingMode);
    void SetCullMode(CullMode mode);
    void SetSamplelingMode(SampleMethod shadingMode);

private:
    // -- SoftWare
    std::vector<PhongMeshData> m_VertexData;
    std::vector<uint32_t> m_Indices;
    std::vector<PhongMeshDataVertexOut> m_VertexDataOut;
    std::vector<Triangle<PhongMeshDataVertexOut>> m_TrianglesOut;
    bool m_UseNormalMaps{true};
    float m_DiffuseReflectance{7.0f};
    float m_Shininess{25.0f};
    ShadingMode m_ShadingMode{ShadingMode::combined};
    SampleMethod m_SampleMode{SampleMethod::point};



    void VertexStage(const std::vector<PhongMeshData>& vertices_in, std::vector<PhongMeshDataVertexOut>& vertices_out, const Camera& camera) const;

    // --- Shared ---
    Matrix<float> m_WorldMatrix{};
    std::shared_ptr<Texture> m_DiffuseTexture{};
    std::shared_ptr<Texture> m_NormalTexture{};
    std::shared_ptr<Texture> m_GlossTexture{};
    std::shared_ptr<Texture> m_SpecularTexture{};
    CullMode m_ActiveCullMode;
    
    // --- HardWare ---
    ID3D11Device* m_pDevice{};
    std::unique_ptr<ID3D11Buffer, callRelease<ID3D11Buffer>> m_pVertexBuffer;
    std::unique_ptr<ID3D11Buffer, callRelease<ID3D11Buffer>> m_pIndexBuffer;

    std::unique_ptr<ID3D11RasterizerState, callRelease<ID3D11RasterizerState>> m_RasterizerStateCullNone;
    std::unique_ptr<ID3D11RasterizerState, callRelease<ID3D11RasterizerState>> m_RasterizerStateCullFront;
    std::unique_ptr<ID3D11RasterizerState, callRelease<ID3D11RasterizerState>> m_RasterizerStateCullBack;

    // Hold the references in this instances
    std::shared_ptr<ID3D11InputLayout> m_InputLayout;
    std::shared_ptr<ID3DX11Effect> m_Effect;
    std::shared_ptr<ID3DX11EffectTechnique> m_Technique;

    // Resources for all DiffuseMesh classes
    static std::weak_ptr<ID3D11InputLayout> resourceInputLayout;
    static std::weak_ptr<ID3DX11Effect> resourceEffect;
    static std::weak_ptr<ID3DX11EffectTechnique> resourceTechnique;
};
