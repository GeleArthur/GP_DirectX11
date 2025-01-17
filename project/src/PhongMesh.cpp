#include "PhongMesh.h"

#include <execution>

#include "DirectXUtils.h"
#include "RendererCombined.h"
#include "Texture.h"
#include "Utils.h"

#include "SoftwareRendererHelpers.h"
#include "TextureManager.h"


// Everything in here is using the same layout just the buffers are diffrent
std::weak_ptr<ID3DX11Effect> PhongMesh::resourceEffect{};
std::weak_ptr<ID3DX11EffectTechnique> PhongMesh::resourceTechnique{};
std::weak_ptr<ID3D11InputLayout> PhongMesh::resourceInputLayout{};

PhongMesh::PhongMesh(ID3D11Device* pDevice): m_pDevice(pDevice)
{
    if (const auto effect = resourceEffect.lock())
    {
        m_Effect = effect;
    }
    else
    {
        m_Effect = std::shared_ptr<ID3DX11Effect>(
            LoadEffect(pDevice, L"Resources/PhongMesh.fx"),
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
        static constexpr uint32_t numElements{ 4 };
        D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

        vertexDesc[0].SemanticName = "POSITION";
        vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        vertexDesc[0].AlignedByteOffset = 0;
        vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

        vertexDesc[1].SemanticName = "TEXCOORD";
        vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        vertexDesc[1].AlignedByteOffset = 12;
        vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

    	vertexDesc[2].SemanticName = "NORMAL";
    	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    	vertexDesc[2].AlignedByteOffset = 20;
    	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

    	vertexDesc[3].SemanticName = "TANGENT";
    	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    	vertexDesc[3].AlignedByteOffset = 32;
    	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    	
        D3DX11_PASS_DESC passDesc{};
        CallDirectX(m_Technique->GetPassByIndex(0)->GetDesc(&passDesc));

        ID3D11InputLayout* inputLayout;
        CallDirectX(pDevice->CreateInputLayout(vertexDesc, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &inputLayout));

        m_InputLayout = std::shared_ptr<ID3D11InputLayout>(inputLayout, callRelease<ID3D11InputLayout>());
        resourceInputLayout = m_InputLayout;
    }

	D3D11_RASTERIZER_DESC rasterizerConfig{};
	rasterizerConfig.FillMode = D3D11_FILL_SOLID;
	rasterizerConfig.FrontCounterClockwise = false;
	rasterizerConfig.DepthClipEnable = true;

	rasterizerConfig.CullMode = D3D11_CULL_NONE;
	ID3D11RasterizerState* rasterizerState;
	CallDirectX(m_pDevice->CreateRasterizerState(&rasterizerConfig, &rasterizerState));
	m_RasterizerStateCullNone = std::unique_ptr<ID3D11RasterizerState, callRelease<ID3D11RasterizerState>>(rasterizerState);
	
	rasterizerConfig.CullMode = D3D11_CULL_FRONT;
	CallDirectX(m_pDevice->CreateRasterizerState(&rasterizerConfig, &rasterizerState));
	m_RasterizerStateCullFront = std::unique_ptr<ID3D11RasterizerState, callRelease<ID3D11RasterizerState>>(rasterizerState);

	rasterizerConfig.CullMode = D3D11_CULL_BACK;
	CallDirectX(m_pDevice->CreateRasterizerState(&rasterizerConfig, &rasterizerState));
	m_RasterizerStateCullBack = std::unique_ptr<ID3D11RasterizerState, callRelease<ID3D11RasterizerState>>(rasterizerState);


	D3D11_SAMPLER_DESC config{};
	config.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	config.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	config.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	config.MipLODBias = 0.0f;
	config.MaxAnisotropy = 16;
	config.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	config.BorderColor[0] = 0.0f; 
	config.BorderColor[1] = 0.0f;
	config.BorderColor[2] = 0.0f;
	config.BorderColor[3] = 0.0f;
	config.MinLOD = 0.0f;
	config.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* samplerState;

	
	config.Filter = D3D11_FILTER_ANISOTROPIC;
	CallDirectX(m_pDevice->CreateSamplerState(&config, &samplerState));
	m_pAnisotropicMode = std::unique_ptr<ID3D11SamplerState, callRelease<ID3D11SamplerState>>(samplerState);

	config.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	CallDirectX(m_pDevice->CreateSamplerState(&config, &samplerState));
	m_pPointMode = std::unique_ptr<ID3D11SamplerState, callRelease<ID3D11SamplerState>>(samplerState);

	config.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	CallDirectX(m_pDevice->CreateSamplerState(&config, &samplerState));
	m_pLinearMode = std::unique_ptr<ID3D11SamplerState, callRelease<ID3D11SamplerState>>(samplerState);
	
}

void PhongMesh::LoadMeshData(std::vector<PhongMeshData>&& vertexData, std::vector<uint32_t>&& indices,
	const std::string& diffuseTextureFilePath,
	const std::string& normalMapTexture,
	const std::string& glossTexture,
	const std::string& specularTexture
	)
{
	m_VertexData = std::move(vertexData);
	m_Indices = std::move(indices);
	m_DiffuseTexture = TextureManager::GetTexture(diffuseTextureFilePath, m_pDevice);
	m_NormalTexture = TextureManager::GetTexture(normalMapTexture, m_pDevice);
	m_GlossTexture = TextureManager::GetTexture(glossTexture, m_pDevice);
	m_SpecularTexture = TextureManager::GetTexture(specularTexture, m_pDevice);
	
	D3D11_BUFFER_DESC vertexBuffer{};
	vertexBuffer.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBuffer.ByteWidth = sizeof(PhongMeshData) * static_cast<uint32_t>(m_VertexData.size());
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


PhongMesh::~PhongMesh() = default;

void PhongMesh::RenderDirectX(ID3D11DeviceContext *pDeviceContext, const Scene& scene)
{
	// TODO Move searching for variables to meshload
	ID3DX11EffectShaderResourceVariable* diffuseMap = m_Effect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	CallDirectX(diffuseMap->SetResource(m_DiffuseTexture->D3D11GetTexture2D()));

	ID3DX11EffectShaderResourceVariable* normalMap = m_Effect->GetVariableByName("gNormalMap")->AsShaderResource();
	CallDirectX(normalMap->SetResource(m_NormalTexture->D3D11GetTexture2D()));

	ID3DX11EffectShaderResourceVariable* glossMap = m_Effect->GetVariableByName("gGlossMap")->AsShaderResource();
	CallDirectX(glossMap->SetResource(m_GlossTexture->D3D11GetTexture2D()));

	ID3DX11EffectShaderResourceVariable* specularMap = m_Effect->GetVariableByName("gSpecularMap")->AsShaderResource();
	CallDirectX(specularMap->SetResource(m_SpecularTexture->D3D11GetTexture2D()));
	
	ID3DX11EffectMatrixVariable* projectionMatrix = m_Effect->GetVariableByName("gWorldViewProj")->AsMatrix();
	CallDirectX(projectionMatrix->SetMatrix((m_WorldMatrix * scene.GetCamera().GetViewProjectionMatrix()).GetFloatArray()));

	ID3DX11EffectMatrixVariable* worldMatrix = m_Effect->GetVariableByName("gWorldMatrix")->AsMatrix();
	CallDirectX(worldMatrix->SetMatrix((m_WorldMatrix).GetFloatArray()));

	ID3DX11EffectVectorVariable* worldPostion = m_Effect->GetVariableByName("gWorldPosition")->AsVector();
	CallDirectX(worldPostion->SetFloatVector(Vector4{scene.GetCamera().GetWorldPosition(), 0.0f}.GetFloatArray()));
	
	ID3DX11EffectVectorVariable* lightDirection = m_Effect->GetVariableByName("gLightDirection")->AsVector();
	CallDirectX(lightDirection->SetFloatVector(Vector4{scene.GetLights()[0], 0.0f}.GetFloatArray()));

	ID3DX11EffectScalarVariable* shininess = m_Effect->GetVariableByName("gShininess")->AsScalar();
	CallDirectX(shininess->SetFloat(Shininess));

	ID3DX11EffectScalarVariable* diffuseReflectance = m_Effect->GetVariableByName("gDiffuseReflectance")->AsScalar();
	CallDirectX(diffuseReflectance->SetFloat(DiffuseReflectance));

	ID3DX11EffectSamplerVariable* sampleMode = m_Effect->GetVariableByName("sampleMode")->AsSampler();
	switch (SampleMode)
	{
	case SampleMethod::point:
		CallDirectX(sampleMode->SetSampler(0, m_pPointMode.get()));
		break;
	case SampleMethod::linear:
		CallDirectX(sampleMode->SetSampler(0, m_pLinearMode.get()));
		break;
	case SampleMethod::anisotropic:
		CallDirectX(sampleMode->SetSampler(0, m_pAnisotropicMode.get()));
		break;
	}

	switch (CullMode)
	{
	case CullMode::none:
		pDeviceContext->RSSetState(m_RasterizerStateCullNone.get());
		break;
	case CullMode::front:
		pDeviceContext->RSSetState(m_RasterizerStateCullFront.get());
		break;
	case CullMode::back:
		pDeviceContext->RSSetState(m_RasterizerStateCullBack.get());
		break;
	}
	
	
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetInputLayout(m_InputLayout.get()); // Source of bad
	
	constexpr UINT stride = static_cast<UINT>(sizeof(PhongMeshData));
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

void PhongMesh::RenderSoftware(SoftwareRendererHelper* softwareRendererHelper, const Scene& scene)
{
	softwareRendererHelper->WriteToDepthBuffer = true;
	softwareRendererHelper->m_CullMode = CullMode;

    VertexStage(m_VertexData, m_VertexDataOut, scene.GetCamera());
	m_TrianglesOut.clear();
	softwareRendererHelper->GetTriangles(m_Indices.begin(), m_Indices.end(), m_VertexDataOut.begin(), m_TrianglesOut);
    softwareRendererHelper->RasterizeTriangle<PhongMeshDataVertexOut>(m_TrianglesOut, [&](PhongMeshDataVertexOut vertexIn)
    {
    	ColorRGB finalColor{0};

		Vector3 normal;
		if (UseNormalMaps)
		{
			const Vector3 realNormal = vertexIn.normal.Normalized();
			const Vector3 realTangent = vertexIn.tangent.Normalized();
			// Do this in the vertex stage?
			const Vector3 binormal = Vector3::Cross(realNormal, realTangent).Normalized();
			Matrix tangentSpaceAxis{
				Vector4{realTangent, 0},
				Vector4{binormal, 0},
				Vector4{realNormal, 0},
				Vector4::Zero
			};
		    
			Vector3 sampledNormal = Vector3{m_NormalTexture->Sample(vertexIn.uv, true)};
			normal = Vector3{(2.0f * sampledNormal.x) - 1.0f, (2.0f * sampledNormal.y) - 1.0f, 2.0f * sampledNormal.z - 1.0f};
			normal = tangentSpaceAxis.TransformVector(normal);
		}
		else
		{
			normal = vertexIn.normal;
		}

    	normal = normal.Normalized();

    	// return ColorRGB{normal.x,normal.y,normal.z};
    	
		const Vector3 lightDirection = scene.GetLights().at(0).Normalized(); // TODO: Multilight
		const ColorRGB albedoTexture = m_DiffuseTexture->Sample(vertexIn.uv);


		// lambert
		const float observableArea = std::max<float>(0.0f, Vector3::Dot(-lightDirection, normal));
		const ColorRGB lambert = (DiffuseReflectance * albedoTexture) / Utils::PI;

		// Phong
		const float specularReflectance = m_SpecularTexture->Sample(vertexIn.uv).r;
		const float gloss = m_GlossTexture->Sample(vertexIn.uv).r * Shininess;

		const Vector3 reflect = Vector3::Reflect(-lightDirection, normal);
		const float cosAngle = std::max(0.0f,Vector3::Dot(reflect, -vertexIn.viewDirection));

		const float phong = specularReflectance * std::pow(cosAngle, gloss);

		switch (ShadingMode)
		{
		case ShadingMode::observed_area:
			finalColor = ColorRGB{observableArea};
			break;
		case ShadingMode::diffuse:
			finalColor = lambert * observableArea;
			break;
		case ShadingMode::specular:
			finalColor = ColorRGB{phong};
			break;
		case ShadingMode::combined:
			finalColor = (albedoTexture * 0.3f) + ColorRGB{phong} + lambert * observableArea;
			break;
		}

		return finalColor;
    });
}

void PhongMesh::VertexStage(const std::vector<PhongMeshData>& vertices_in, std::vector<PhongMeshDataVertexOut>& vertices_out, const Camera& camera) const
{
	vertices_out.resize(vertices_in.size());
	const Matrix<float> worldViewProjectionMatrix = m_WorldMatrix * camera.GetViewProjectionMatrix();

	std::transform(std::execution::seq, vertices_in.cbegin(), vertices_in.cend(), vertices_out.begin(),
				   [&](const PhongMeshData& v)
				   {
					   Vector4 transformedPoint = worldViewProjectionMatrix.TransformPoint(Vector4{v.position, 1});

					   // TODO: Move this after the frustum culling
					   transformedPoint.x = transformedPoint.x / transformedPoint.w;
					   transformedPoint.y = transformedPoint.y / transformedPoint.w;
					   transformedPoint.z = transformedPoint.z / transformedPoint.w;

					   return PhongMeshDataVertexOut{
						   .position = transformedPoint,
						   .uv = v.uv,
						   .normal = m_WorldMatrix.TransformVector(v.normal).Normalized(), // TODO: I dont think I need to normalize
						   .tangent = m_WorldMatrix.TransformVector(v.tangent).Normalized(),
						   .viewDirection = (camera.GetWorldPosition() - (m_WorldMatrix.TransformPoint(v.position))).Normalized()
					   };
				   });
}



void PhongMesh::SetWorldMatrix(const Matrix<float> matrix)
{
	m_WorldMatrix = matrix;
}


