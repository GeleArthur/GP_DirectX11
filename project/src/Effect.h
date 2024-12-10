#include <d3dx11effect.h>
#include <string>

class Effect
{
public:
    static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

private:

};
