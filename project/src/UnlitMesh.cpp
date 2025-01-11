#include "UnlitMesh.h"

#include <iostream>

#include "DirectXUtils.h"
#include "RendererCombined.h"
#include "Texture.h"
#include "Utils.h"

// Everything in here is using the same layout just the buffers are diffrent
std::weak_ptr<ID3DX11Effect> UnlitMesh::resourceEffect{};
std::weak_ptr<ID3DX11EffectTechnique> UnlitMesh::resourceTechnique{};
std::weak_ptr<ID3D11InputLayout> UnlitMesh::resourceInputLayout{};

struct UnlitData
{
	Vector3 position;
	Vector2 uv;
};

UnlitMesh::UnlitMesh(ID3D11Device* pDevice): m_pDevice(pDevice)
{
    if (const auto effect = resourceEffect.lock())
    {
        m_Effect = effect;
    }
    else
    {
        m_Effect = std::shared_ptr<ID3DX11Effect>(
            LoadEffect(pDevice, L"Resources/UntilMesh.fx"),
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

        vertexDesc[1].SemanticName = "TEXCOORD";
        vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        vertexDesc[1].AlignedByteOffset = 12;
        vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

        D3DX11_PASS_DESC passDesc{};
        CallDirectX(m_Technique->GetPassByIndex(0)->GetDesc(&passDesc));

        ID3D11InputLayout* inputLayout;
        CallDirectX(pDevice->CreateInputLayout(vertexDesc, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &inputLayout));

        m_InputLayout = std::shared_ptr<ID3D11InputLayout>(inputLayout, callRelease<ID3D11InputLayout>());
        resourceInputLayout = m_InputLayout;
    }
}

UnlitMesh::~UnlitMesh()
{
}


void UnlitMesh::RenderDirectX(ID3D11DeviceContext *pDeviceContext, const Camera& camera) // Camera should not be here
{
	auto sampler = m_Effect->GetVariableByName("gSampleMode")->AsSampler();
	if (!sampler->IsValid())
		return;
	
	// switch (m_CurrentSampleMode)
	// {
	// case TextureSampleMethod::point:
	// 	sampler->SetSampler(0, m_pPointMode);
	// 	break;
	// case TextureSampleMethod::linear:
	// 	sampler->SetSampler(0, m_pLinearMode);
	// 	break;
	// case TextureSampleMethod::anisotropic:
	// 	sampler->SetSampler(0, m_pAnisotropicMode);
	// 	break;
	// }
	
	
	
	ID3DX11EffectShaderResourceVariable* diffuseMap = m_Effect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	diffuseMap->SetResource(m_DiffuseTexture->D3D11GetTexture2D());
	
	
	ID3DX11EffectMatrixVariable* projectionMatrix = m_Effect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!projectionMatrix->IsValid())
		return;
	
	projectionMatrix->SetMatrix((camera.GetViewProjectionMatrixAsFloatArray()));
	
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetInputLayout(m_InputLayout.get()); // Source of bad
	
	constexpr UINT stride = static_cast<UINT>(sizeof(Vertex_PosTexture));
	constexpr UINT offset = 0;

	
	ID3D11Buffer* vertexBuffer = m_pVertexBuffer.get(); // ??????? why
	pDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride ,&offset);
	
	ID3D11Buffer* indexBuffer = m_pIndexBuffer.get();
	pDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	
	D3DX11_TECHNIQUE_DESC techDesc;
	m_Technique->GetDesc(&techDesc);
	
	for (UINT i = 0; i < techDesc.Passes; ++i)
	{
		ID3DX11EffectPass* index = m_Technique->GetPassByIndex(i);
		index->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(static_cast<UINT>(m_Indices.size()), 0, 0); // m_Indices is from software should be moved into it own number
		index->Release();
	}
	
}

void UnlitMesh::RenderSoftware()
{
    
}



void UnlitMesh::LoadMeshData(std::vector<Vector3>&& position, std::vector<Vector2>&& inputUv, std::vector<uint32_t>&& indices, const std::string& diffuseTextureFilePath)
{
    m_Positions = std::move(position);
    m_Uv = std::move(inputUv);
    m_Indices = std::move(indices);
    m_DiffuseTexture = new Texture(diffuseTextureFilePath, m_pDevice); // TODO: MANAGER

    assert(m_Positions.size() == m_Uv.size() && "Position and uv do not have the same size!!! Are they the same mesh?");
    
    D3D11_BUFFER_DESC vertexBuffer{};
    vertexBuffer.Usage = D3D11_USAGE_IMMUTABLE;
    vertexBuffer.ByteWidth = sizeof(UnlitData) * static_cast<uint32_t>(m_Positions.size());
    vertexBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBuffer.CPUAccessFlags = 0;
    vertexBuffer.MiscFlags = 0;

    std::vector<UnlitData> layoutData;
    layoutData.reserve(m_Positions.size());

    for (size_t i = 0; i < m_Positions.size(); ++i)
    {
        layoutData.push_back({.position= m_Positions[i], .uv= m_Uv[i]});
    }
    
    ID3D11Buffer* pVertexBuffer;

    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = layoutData.data();
    CallDirectX(m_pDevice->CreateBuffer(&vertexBuffer, &initData, &pVertexBuffer));

    m_pVertexBuffer = std::unique_ptr<ID3D11Buffer, callRelease<ID3D11Buffer>>(pVertexBuffer, callRelease<ID3D11Buffer>());

    D3D11_BUFFER_DESC indexBufferDesc{};
    indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexBufferDesc.ByteWidth = sizeof(uint32_t) * static_cast<uint32_t>(indices.size());
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    initData.pSysMem = indices.data();
    ID3D11Buffer* pIndexBuffer;
    CallDirectX(m_pDevice->CreateBuffer(&indexBufferDesc, &initData, &pIndexBuffer));
    
    m_pIndexBuffer = std::unique_ptr<ID3D11Buffer, callRelease<ID3D11Buffer>>(pIndexBuffer, callRelease<ID3D11Buffer>());
}

