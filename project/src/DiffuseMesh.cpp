#include "DiffuseMesh.h"

#include <iostream>

// BHAHAHAHAH TEMPLATES BAHAHHAHAH
template<typename T>
class callRelease
{
public:
    void operator()(T* p)
    {
        p->Release();
    }
};

// Everything in here is using the same layout just the buffers are diffrent
std::weak_ptr<ID3DX11Effect> DiffuseMesh::resourceEffect{};
std::weak_ptr<ID3DX11EffectTechnique> DiffuseMesh::resourceTechnique{};
std::weak_ptr<ID3D11InputLayout> DiffuseMesh::resourceInputLayout{};

DiffuseMesh::DiffuseMesh(ID3D11Device* pDevice): m_pDevice(pDevice)
{
    if (const auto effect = resourceEffect.lock())
    {
        m_Effect = effect;
    }
    else
    {
        m_Effect = std::shared_ptr<ID3DX11Effect>(
            LoadEffect(pDevice, L"Resources/PosTex3D.fx"),
            callRelease<ID3DX11Effect>());
        resourceEffect = m_Effect;
    }

    if (const auto technique = resourceTechnique.lock())
    {
        m_Technique = technique;
    }
    else
    {
        m_Technique = std::shared_ptr<ID3DX11EffectTechnique>(
            m_Effect->GetTechniqueByName("DefaultTechnique"),
            callRelease<ID3DX11EffectTechnique>());
        resourceTechnique = m_Technique;
    }

    if (const auto layout = resourceInputLayout.lock())
    {
        m_InputLayout = layout;
    }
    else
    {
        static constexpr uint32_t numElements{ 3 };
        D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

        vertexDesc[0].SemanticName = "POSITION";
        vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        vertexDesc[0].AlignedByteOffset = 0;
        vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

        vertexDesc[1].SemanticName = "COLOR";
        vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        vertexDesc[1].AlignedByteOffset = 12;
        vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

        vertexDesc[2].SemanticName = "TEXCOORD";
        vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
        vertexDesc[2].AlignedByteOffset = 24;
        vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

        D3DX11_PASS_DESC passDesc{};
        HRESULT result = m_Technique->GetPassByIndex(0)->GetDesc(&passDesc);
        if (FAILED(result))
            std::cerr << "Can't get first pass " << std::hex << result << '\n' << std::dec ;

        ID3D11InputLayout* inputLayout;
        result = pDevice->CreateInputLayout(vertexDesc, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &inputLayout);
        if (FAILED(result))
            std::cerr << "Can't create Input Layout " << std::hex << result << '\n' << std::dec ;

        m_InputLayout = std::shared_ptr<ID3D11InputLayout>(inputLayout, callRelease<ID3D11InputLayout>());
        resourceInputLayout = m_InputLayout;
    }

}

DiffuseMesh::~DiffuseMesh()
{
}

void DiffuseMesh::SetVertexData(const std::vector<Vector3> &positionData)
{

}

void DiffuseMesh::Render(ID3D11DeviceContext *pDeviceContext)
{

}

void DiffuseMesh::SetIndicesData(const std::vector<uint32_t> &positionData)
{

}
