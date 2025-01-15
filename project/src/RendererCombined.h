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

enum class TextureSampleMethod : uint8_t
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
	void ToggleSceneBackGround();
	void NextCullMode();
	void ToggleRotation();
	
private:
	HRESULT InitializeDirectX();
	void InitSoftware();
	void SetCullMode() const;
	
	SDL_Window* m_pWindow{};

	int m_Width{};
	int m_Height{};
	
	Scene m_ActiveScene{};

	bool m_UseSceneBackgroundColor{false};
	CullMode m_ActiveCullMode{CullMode::back};
	
	// --------- SOFTWARE ---------

	std::unique_ptr<SoftwareRendererHelper> m_SoftwareHelper;
	SDL_Surface* m_pFrontBuffer{};
	SDL_Surface* m_pBackBuffer{};

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
	
	ID3D11RasterizerState* m_RasterizerStateCullNone;
	ID3D11RasterizerState* m_RasterizerStateCullFront;
	ID3D11RasterizerState* m_RasterizerStateCullBack;
};
