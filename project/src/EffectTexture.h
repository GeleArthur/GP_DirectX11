#pragma once
#include <d3dx11effect.h>
#include <string>
#include <vector>

#include "Vector.h"

struct Vertex_PosTexture
{
    Vector3 position;
    Vector2 uv;
};


class EffectTexture
{
public:
    EffectTexture(ID3D11Device* pDevice, const std::vector<Vertex_PosTexture>& vertices, const std::vector<unsigned int>& indices);
    ~EffectTexture();

    ID3DX11Effect* GetEffect() const;
    ID3D11InputLayout* GetInputLayout() const;
    ID3D11Buffer* GetVertexBuffer();
    ID3D11Buffer* GetIndexBuffer();
    ID3DX11EffectTechnique* GetTechnique();

private:

    static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

    ID3DX11Effect* m_pCurrentEffect;
    ID3DX11EffectTechnique* m_pCurrentTechnique;

    ID3D11Buffer* m_pVertexBuffer;
    ID3D11InputLayout* m_pInputLayout;
    ID3D11Buffer* m_pIndexBuffer;
};
