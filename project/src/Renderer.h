#pragma once
#include "Camera.h"

namespace dae
{
	struct Camera;
}

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	struct Vertex_PosCol
	{
		Vector3 position;
		ColorRGB color;
	};
	
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

	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

		HRESULT InitializeDirectX();

		Camera m_Camera{};

		std::vector<Vertex_PosCol> vertices{
				{{0,3.0f, 2.0f}, {1.0f, 0.0f, 0.0f}},
				{{3.0f,-3.0f, 2.0f}, {0.0f, 0.0f, 1.0f}},
				{{-3.0f,-3.0f, 2.0f}, {0.0f, 1.0f, 0.0f}}
		};

		std::vector<uint32_t> indices{0,1,2};

		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGISwapChain* m_pSwapChain;
		ID3D11Texture2D* m_pDepthStecilBuffer;
		ID3D11DepthStencilView* m_pDepthStecilView;
		ID3D11Resource* m_pRenderTargetBuffer;
		ID3D11RenderTargetView* m_pRenderTargetView;

		ID3DX11Effect* m_pCurrentEffect;
		ID3DX11EffectTechnique* m_pCurrentTechnique;

		ID3D11Buffer* m_pVertexBuffer;
		ID3D11InputLayout* m_pInputLayout;
		ID3D11Buffer* m_pIndexBuffer;
	};
}
