#pragma once
#include <memory>

#include "BaseMeshEffect.h"

class DiffuseMesh : public BaseMeshEffect
{
public:

    explicit DiffuseMesh(ID3D11Device* pDevice);
    ~DiffuseMesh() override;

    void SetIndicesData(const std::vector<uint32_t> &positionData) override;
    void SetVertexData(const std::vector<Vector3> &positionData) override;
    void Render(ID3D11DeviceContext *pDeviceContext) override;

private:
    ID3D11Device* m_pDevice;

    // Hold the references in this instances
    std::shared_ptr<ID3D11InputLayout> m_InputLayout;
    std::shared_ptr<ID3DX11Effect> m_Effect;
    std::shared_ptr<ID3DX11EffectTechnique> m_Technique;

    // Resources for all DiffuseMesh classes
    static std::weak_ptr<ID3D11InputLayout> resourceInputLayout;
    static std::weak_ptr<ID3DX11Effect> resourceEffect;
    static std::weak_ptr<ID3DX11EffectTechnique> resourceTechnique;
    
};
