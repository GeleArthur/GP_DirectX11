#pragma once
#include <d3dx11effect.h>
#include <string>

#include "Vector.h"
#include <vector>

class BaseMeshEffect
{
public:
	BaseMeshEffect() = default;
	virtual ~BaseMeshEffect() = default;
	BaseMeshEffect(BaseMeshEffect& other) = delete;
	BaseMeshEffect(BaseMeshEffect&& other) = delete;
	BaseMeshEffect& operator=(BaseMeshEffect& other) = delete;
	BaseMeshEffect& operator=(BaseMeshEffect&& other) = delete;

	virtual void SetVertexData(const std::vector<Vector3>& positionData) = 0;
	virtual void SetIndicesData(const std::vector<uint32_t>& positionData) = 0;

	virtual void Render(ID3D11DeviceContext* pDeviceContext) = 0;

	// virtual ID3DX11Effect* GetEffect() const = 0;
	// virtual void DrawIndexed() const = 0;

protected:

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

};
