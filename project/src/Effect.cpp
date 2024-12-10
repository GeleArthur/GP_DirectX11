#include "Effect.h"

#include <d3dcompiler.h>
#include <iostream>
#include <sstream>
#include <string>


ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
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
