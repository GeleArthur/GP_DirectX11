#pragma once
#include <d3dx11effect.h>

class BaseMesh
{
public:
	BaseMesh() = default;
	virtual ~BaseMesh() = default;
	BaseMesh(BaseMesh& other) = delete;
	BaseMesh(BaseMesh&& other) = delete;
	BaseMesh& operator=(BaseMesh& other) = delete;
	BaseMesh& operator=(BaseMesh&& other) = delete;
	
	virtual ID3DX11Effect* GetEffect() const = 0;
	virtual void DrawIndexed() const = 0;

private:
};
