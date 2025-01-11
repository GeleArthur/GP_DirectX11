#pragma once
#include "Camera.h"
#include "EffectTexture.h"
#include "Scene.h"

class BaseMeshEffect;
class Texture;
class EffectTexture;

struct Camera;
struct SDL_Window;
struct SDL_Surface;

enum class TextureSampleMethod
{
	point,
	linear,
	anisotropic
};

class RendererCombined final
{
public:
	explicit RendererCombined(SDL_Window* pWindow);
	~RendererCombined();

	RendererCombined(const RendererCombined&) = delete;
	RendererCombined(RendererCombined&&) noexcept = delete;
	RendererCombined& operator=(const RendererCombined&) = delete;
	RendererCombined& operator=(RendererCombined&&) noexcept = delete;

	void Update(const Timer& pTimer);
	void RenderDirectX() const;
	void RenderSoftware() const;
	void ToggleSampleMode();
	void LoadScene();
	
private:
	HRESULT InitializeDirectX();
	void InitSoftware();
	
	SDL_Window* m_pWindow{};

	int m_Width{};
	int m_Height{};
	
	Scene m_ActiveScene{};
	
	// --------- SOFTWARE ---------
	
	SDL_Surface* m_pFrontBuffer{};
	SDL_Surface* m_pBackBuffer{};
	uint32_t* m_pBackBufferPixels{};
	std::vector<float> m_DepthBuffer{};
	
	// --------- DirectX ---------
	ID3D11Device* m_pDevice{};
	ID3D11DeviceContext* m_pDeviceContext{};
	IDXGISwapChain* m_pSwapChain{};
	ID3D11Texture2D* m_pDepthStecilBuffer{};
	ID3D11DepthStencilView* m_pDepthStecilView{};
	ID3D11Resource* m_pRenderTargetBuffer{};
	ID3D11RenderTargetView* m_pRenderTargetView{};

	TextureSampleMethod m_CurrentSampleMode{TextureSampleMethod::anisotropic};

	ID3D11SamplerState* m_pLinearMode{};
	ID3D11SamplerState* m_pPointMode{};
	ID3D11SamplerState* m_pAnisotropicMode{};
};
