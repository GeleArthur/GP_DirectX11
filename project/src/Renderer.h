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

namespace dae
{
	class Renderer final
	{
	public:
		explicit Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;
		void ToggleSampleMode();

	private:
		HRESULT InitializeDirectX();


		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

		Camera m_Camera{};
		Scene m_activeScene{};

		std::vector<Vertex_PosTexture> verties;
		std::vector<uint32_t> indicies;

		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGISwapChain* m_pSwapChain;
		ID3D11Texture2D* m_pDepthStecilBuffer;
		ID3D11DepthStencilView* m_pDepthStecilView;
		ID3D11Resource* m_pRenderTargetBuffer;
		ID3D11RenderTargetView* m_pRenderTargetView;

		std::unique_ptr<EffectTexture> m_CurrentEffect;
		std::unique_ptr<Texture> m_CurrentTexture;

		TextureSampleMethod m_CurrentSampleMode{TextureSampleMethod::anisotropic};

		ID3D11SamplerState* m_pLinearMode{};
		ID3D11SamplerState* m_pPointMode{};
		ID3D11SamplerState* m_pAnisotropicMode{};
	};
}
