#pragma once
#include <d3dx11effect.h>
#include <SDL_pixels.h>
#include <string>

#include "Vector.h"
#include <vector>

#include "Matrix.h"

class SoftwareRendererHelper;
struct Camera;

class BaseMeshEffect
{
public:
	BaseMeshEffect() = default;
	virtual ~BaseMeshEffect() = default;
	BaseMeshEffect(BaseMeshEffect& other) = delete;
	BaseMeshEffect(BaseMeshEffect&& other) = delete;
	BaseMeshEffect& operator=(BaseMeshEffect& other) = delete;
	BaseMeshEffect& operator=(BaseMeshEffect&& other) = delete;
	
	virtual void RenderDirectX(ID3D11DeviceContext* pDeviceContext, const Camera& camera) = 0;
	virtual void RenderSoftware(SoftwareRendererHelper* softwareRendererHelper, const Camera& camera) = 0;
	virtual void SetWorldMatrix(Matrix<float> matrix) = 0;


	// virtual ID3DX11Effect* GetEffect() const = 0;
	// virtual void DrawIndexed() const = 0;

protected:

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

};
