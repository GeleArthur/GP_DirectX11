#pragma once
#include "Camera.h"
#include "CullMode.h"
#include "Scene.h"

class BaseMeshEffect;
class Texture;
class EffectTexture;

struct Camera;
struct SDL_Window;
struct SDL_Surface;

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
	
	void ToggleSceneBackGround();
	void NextCullMode();
	void ToggleRotation();
	void ToggleDepthBuffer() const;
	void ToggleBoundingBoxDraw() const;
	void ToggleNormalMap() const;
	void NextShadingMode();
	void DisableAllFireFx() const;


private:
	HRESULT InitializeDirectX();
	void InitSoftware();
	
	SDL_Window* m_pWindow{};	
	int m_Width{};
	int m_Height{};
	

	// --------- Shared ---------
	Scene m_ActiveScene{};
	bool m_UseSceneBackgroundColor{false};
	CullMode m_ActiveCullMode{CullMode::back};
	
	// --------- Software ---------
	std::unique_ptr<SoftwareRendererHelper> m_SoftwareHelper;
	SDL_Surface* m_pFrontBuffer{};
	SDL_Surface* m_pBackBuffer{};

	// --------- Hardware ---------
	ShadingMode m_ShadingMode{ShadingMode::combined};
	
	ID3D11Device* m_pDevice{};
	ID3D11DeviceContext* m_pDeviceContext{};
	IDXGISwapChain* m_pSwapChain{};
	ID3D11Texture2D* m_pDepthStecilBuffer{};
	ID3D11DepthStencilView* m_pDepthStecilView{};
	ID3D11Resource* m_pRenderTargetBuffer{};
	ID3D11RenderTargetView* m_pRenderTargetView{};

	SampleMethod m_SampleMode{SampleMethod::anisotropic};

	ID3D11SamplerState* m_pLinearMode{};
	ID3D11SamplerState* m_pPointMode{};
	ID3D11SamplerState* m_pAnisotropicMode{};
};
