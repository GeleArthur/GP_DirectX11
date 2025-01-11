#include "BaseMeshEffect.h"
#include <d3dcompiler.h>
#include <sstream>
#include <iostream>

ID3DX11Effect* BaseMeshEffect::LoadEffect(ID3D11Device *pDevice, const std::wstring &assetFile)
{
    HRESULT result;
    ID3D10Blob *pErrorBlob{nullptr};
    ID3DX11Effect *pEffect;

    DWORD shaderFlags{};
#ifndef NDEBUG
    shaderFlags |= D3DCOMPILE_DEBUG;
    shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    result = D3DX11CompileEffectFromFile(
        assetFile.c_str(),
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
            try
            {
                const auto pErrors = static_cast<const char *>(pErrorBlob->GetBufferPointer());
                std::wstringstream ss;
                for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); ++i) { ss << pErrors[i]; }
                pErrorBlob->Release();

                OutputDebugStringW(ss.str().c_str());
                std::wcout << ss.str() << std::endl;
            }
            catch (...)
            {
                pErrorBlob->Release();
            }
        }
        else
        {
            std::wcout << "Failed to load Effect\n";
        }
    }

    return pEffect;
}
