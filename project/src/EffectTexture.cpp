#include "EffectTexture.h"

#include <d3dcompiler.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

EffectTexture::EffectTexture(ID3D11Device* pDevice, const std::vector<Vertex_PosTexture>& vertices, const std::vector<unsigned int>& indices)
{
    m_pCurrentEffect = LoadEffect(pDevice, L"Resources/PosTex3D.fx");
    m_pCurrentTechnique = m_pCurrentEffect->GetTechniqueByName("DefaultTechnique");

    D3D11_BUFFER_DESC vertexBufferDesc{};
    vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vertexBufferDesc.ByteWidth = sizeof(Vertex_PosTexture) * static_cast<uint32_t>(vertices.size());
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = vertices.data();
    HRESULT result = pDevice->CreateBuffer(&vertexBufferDesc, &initData, &m_pVertexBuffer);

    if (FAILED(result))
        std::cerr << "Can't create buffer\n";


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
    m_pCurrentTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

    result = pDevice->CreateInputLayout(vertexDesc, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pInputLayout);
    if (FAILED(result))
        std::cerr << "Can't create Input Layout " << std::hex << result << '\n' ;

    D3D11_BUFFER_DESC indexBufferDesc{};
    indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexBufferDesc.ByteWidth = sizeof(uint32_t) * static_cast<uint32_t>(indices.size());
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    initData.pSysMem = indices.data();
    result = pDevice->CreateBuffer(&indexBufferDesc, &initData, &m_pIndexBuffer);

    if (FAILED(result))
        std::cerr << "Can't create index buffer";;
}

EffectTexture::~EffectTexture()
{
    m_pVertexBuffer->Release();
    m_pIndexBuffer->Release();
    m_pInputLayout->Release();
    
    m_pCurrentTechnique->Release();
    m_pCurrentEffect->Release();
}

ID3DX11Effect* EffectTexture::GetEffect() const
{
    return m_pCurrentEffect;
}

ID3D11InputLayout* EffectTexture::GetInputLayout() const
{
    return m_pInputLayout;
}

ID3D11Buffer* EffectTexture::GetVertexBuffer()
{
    return m_pVertexBuffer;
}

ID3D11Buffer* EffectTexture::GetIndexBuffer()
{
    return m_pIndexBuffer;
}

ID3DX11EffectTechnique* EffectTexture::GetTechnique()
{
    return m_pCurrentTechnique;
}

ID3DX11Effect* EffectTexture::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
    HRESULT result;
    ID3D10Blob* pErrorBlob{nullptr};
    ID3DX11Effect* pEffect;

    DWORD shaderFlags{};
    shaderFlags |= D3DCOMPILE_DEBUG;
    shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;

    result = D3DX11CompileEffectFromFile(assetFile.c_str(),
        nullptr,
        nullptr,
        shaderFlags,
        0,
        pDevice,
        &pEffect,
        &pErrorBlob
    );
    
    if (FAILED(result))
    {
        if (pErrorBlob != nullptr)
        {
            const char* pErrors = static_cast<const char*>(pErrorBlob->GetBufferPointer());
            std::wstringstream ss;
            for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); ++i)
            {
                ss << pErrors[i];
            }
            OutputDebugStringW(ss.str().c_str());
            pErrorBlob->Release();

            std::wcout << ss.str() << std::endl;
        }
        else
        {
            std::wcout << "Failed to load\n";
        }
    }

    return pEffect;
}
