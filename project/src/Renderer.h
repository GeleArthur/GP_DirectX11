#pragma once
#include "Camera.h"
#include "EffectTexture.h"

class Texture;
class EffectTexture;

namespace dae
{
	struct Camera;
}

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
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;
		void ToggleSampleMode();

	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };
		//ID3D11Debug* d3d11Debug;

		HRESULT InitializeDirectX();

		Camera m_Camera{};
		
		//std::vector<Vertex_PosTexture> vertices{
		//		{{0,3.0f, 2.0f}, {1.0f, 0.0f, 0.0f} ,{1.0f, 0.0f}},
		//		{{3.0f,-3.0f, 2.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
		//		{{-3.0f,-3.0f, 2.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
		//};

		//std::vector<uint32_t> indices{0,1,2};

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
