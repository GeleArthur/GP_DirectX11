#include "UnlitMesh.h"

#include <iostream>

#include "DirectXUtils.h"
#include "RendererCombined.h"
#include "Texture.h"
#include "Utils.h"
#include <execution>

#include "SoftwareRendererHelpers.h"

// Everything in here is using the same layout just the buffers are diffrent
std::weak_ptr<ID3DX11Effect> UnlitMesh::resourceEffect{};
std::weak_ptr<ID3DX11EffectTechnique> UnlitMesh::resourceTechnique{};
std::weak_ptr<ID3D11InputLayout> UnlitMesh::resourceInputLayout{};

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
        static constexpr uint32_t numElements{ 2 };
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
	delete m_DiffuseTexture;
}


void UnlitMesh::RenderDirectX(ID3D11DeviceContext *pDeviceContext, const Camera& camera) // Camera should not be here
{
	// auto sampler = m_Effect->GetVariableByName("gSampleMode")->AsSampler();
	// if (!sampler->IsValid())
	// 	return;
	
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
	
	constexpr UINT stride = static_cast<UINT>(sizeof(UnlitData));
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

void UnlitMesh::RenderSoftware(SoftwareRendererHelper* softwareRendererHelper, const Camera& camera)
{
    VertexStage(m_VertexData, m_VertexDataOut, camera);
	m_TrianglesOut.clear();
	softwareRendererHelper->GetTriangles(m_Indices.begin(), m_Indices.end(), m_VertexDataOut.begin(), m_TrianglesOut);

    for (const Triangle<UnlitDataVertexOut>& triangle : m_TrianglesOut)
    {
    	softwareRendererHelper->RasterizeTriangle<UnlitDataVertexOut>(triangle, [&](const UnlitDataVertexOut& in)
	    {
    		const ColorRGB albedoTexture = m_DiffuseTexture->Sample(in.uv);
    		return albedoTexture;
    		
		    return ColorRGB{in.uv.x, in.uv.y, 0};
	    });
    }
}



void UnlitMesh::LoadMeshData(std::vector<UnlitData>&& vertexData, std::vector<uint32_t>&& indices, const std::string& diffuseTextureFilePath)
{
    m_VertexData = std::move(vertexData);
    m_Indices = std::move(indices);
    m_DiffuseTexture = new Texture(diffuseTextureFilePath, m_pDevice); // TODO: MANAGER
	
    D3D11_BUFFER_DESC vertexBuffer{};
    vertexBuffer.Usage = D3D11_USAGE_IMMUTABLE;
    vertexBuffer.ByteWidth = sizeof(UnlitData) * static_cast<uint32_t>(m_VertexData.size());
    vertexBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBuffer.CPUAccessFlags = 0;
    vertexBuffer.MiscFlags = 0;
    
    ID3D11Buffer* pVertexBuffer;

    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = m_VertexData.data();
    CallDirectX(m_pDevice->CreateBuffer(&vertexBuffer, &initData, &pVertexBuffer));

    m_pVertexBuffer = std::unique_ptr<ID3D11Buffer, callRelease<ID3D11Buffer>>(pVertexBuffer, callRelease<ID3D11Buffer>());

    D3D11_BUFFER_DESC indexBufferDesc{};
    indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexBufferDesc.ByteWidth = sizeof(uint32_t) * static_cast<uint32_t>(m_Indices.size());
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	
    initData.pSysMem = m_Indices.data();
    ID3D11Buffer* pIndexBuffer;
    CallDirectX(m_pDevice->CreateBuffer(&indexBufferDesc, &initData, &pIndexBuffer));
    
    m_pIndexBuffer = std::unique_ptr<ID3D11Buffer, callRelease<ID3D11Buffer>>(pIndexBuffer, callRelease<ID3D11Buffer>());
}

void UnlitMesh::VertexStage(const std::vector<UnlitData>& vertices_in, std::vector<UnlitDataVertexOut>& vertices_out, const Camera& camera) const
{
	vertices_out.resize(vertices_in.size());
	const Matrix<float> worldViewProjectionMatrix = m_WorldMatrix * camera.GetViewProjectionMatrix();

	std::transform(std::execution::par, vertices_in.cbegin(), vertices_in.cend(), vertices_out.begin(),
	   [&worldViewProjectionMatrix](const UnlitData& v)
	   {
		   Vector<4, float> transformedPoint = worldViewProjectionMatrix.TransformPoint(Vector<4, float>{v.position, 1});

		   transformedPoint.x = transformedPoint.x / transformedPoint.w;
		   transformedPoint.y = transformedPoint.y / transformedPoint.w;
		   transformedPoint.z = transformedPoint.z / transformedPoint.w;
           
		   return UnlitDataVertexOut{
			   .position = transformedPoint,
			   .uv = v.uv,
		   };
	   });
}

ColorRGB UnlitMesh::FragmentStage(const UnlitDataVertexOut& vertexIN) const
{
	return {};
}