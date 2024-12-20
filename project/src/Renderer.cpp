#include "pch.h"
#include "Renderer.h"

#include "EffectTexture.h"
#include "magic_enum.hpp"
#include "Texture.h"
#include "Utils.h"

extern ID3D11Debug* d3d11Debug;

namespace dae {
	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		m_Camera.Initialize(static_cast<float>(m_Width)/ static_cast<float>(m_Height), 45, {0,0,-10.0f});

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}
	}

	Renderer::~Renderer()
	{
		m_pLinearMode->Release();
		m_pAnisotropicMode->Release();
		m_pPointMode->Release();
		m_pDepthStecilView->Release();
		m_pDepthStecilBuffer->Release();
		m_pRenderTargetView->Release();
		m_pRenderTargetBuffer->Release();
		m_pSwapChain->Release();

		
		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}

		m_pDevice->Release();


	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_Camera.Update(pTimer);
	}


	void Renderer::Render() const
	{
		if (!m_IsInitialized)
			return;

		constexpr float color[4] = { 0.0f, 0.0f, 0.3f, 1.0f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStecilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);
		
		auto sampler = m_CurrentEffect->GetEffect()->GetVariableByName("gSampleMode")->AsSampler();
		if (!sampler->IsValid())
			return;

		switch (m_CurrentSampleMode)
		{
		case TextureSampleMethod::point:
			sampler->SetSampler(0, m_pPointMode);
			break;
		case TextureSampleMethod::linear:
			sampler->SetSampler(0, m_pLinearMode);
			break;
		case TextureSampleMethod::anisotropic:
			sampler->SetSampler(0, m_pAnisotropicMode);
			break;
		}
		


		ID3DX11EffectShaderResourceVariable* diffuseMap = m_CurrentEffect->GetEffect()->GetVariableByName("gDiffuseMap")->AsShaderResource();
		diffuseMap->SetResource(m_CurrentTexture->D3D11GetTexture2D());

		
		ID3DX11EffectMatrixVariable* projectionMatrix = m_CurrentEffect->GetEffect()->GetVariableByName("gWorldViewProj")->AsMatrix();
		if (!projectionMatrix->IsValid())
			return;
		
		projectionMatrix->SetMatrix((m_Camera.GetViewProjectionMatrixAsFloatArray()));
		
		m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_pDeviceContext->IASetInputLayout(m_CurrentEffect->GetInputLayout()); // Source of bad

		constexpr UINT stride = static_cast<UINT>(sizeof(Vertex_PosTexture));
		constexpr UINT offset = 0;

		
		ID3D11Buffer* vertexBuffer = m_CurrentEffect->GetVertexBuffer();
		m_pDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer,&stride , &offset);

		ID3D11Buffer* indexBuffer = m_CurrentEffect->GetIndexBuffer();
		m_pDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		D3DX11_TECHNIQUE_DESC techDesc;
		m_CurrentEffect->GetTechnique()->GetDesc(&techDesc);

		for (UINT i = 0; i < techDesc.Passes; ++i)
		{
			ID3DX11EffectPass* index = m_CurrentEffect->GetTechnique()->GetPassByIndex(i);
			index->Apply(0, m_pDeviceContext);
			m_pDeviceContext->DrawIndexed(static_cast<UINT>(indicies.size()), 0, 0);
			index->Release();
		}
		
		m_pSwapChain->Present(0, 0);

	}

	void Renderer::ToggleSampleMode()
	{
		switch (m_CurrentSampleMode)
		{
		case TextureSampleMethod::point:
			m_CurrentSampleMode = TextureSampleMethod::linear;
			break;
		case TextureSampleMethod::linear:
			m_CurrentSampleMode = TextureSampleMethod::anisotropic;
			break;
		case TextureSampleMethod::anisotropic:
			m_CurrentSampleMode = TextureSampleMethod::point;
			break;
		}

		std::cout << "Using: " << magic_enum::enum_name(m_CurrentSampleMode) << std::endl;
		
	}

	HRESULT Renderer::InitializeDirectX()
	{
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlags = 0;
		createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, &featureLevel, 1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);


		m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3d11Debug));

		if (FAILED(result))
			return result;


		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));

		if (FAILED(result))
			return result;

		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		SDL_SysWMinfo sysWMInfo{};
		SDL_GetVersion(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
		if (FAILED(result))
			return result;

		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStecilBuffer);
		if (FAILED(result))
			return result;

		result = m_pDevice->CreateDepthStencilView(m_pDepthStecilBuffer, &depthStencilViewDesc, &m_pDepthStecilView);
		if (FAILED(result))
			return result;

		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));

		m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result))
			return result;

		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStecilView);

		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		m_pDeviceContext->RSSetViewports(1, &viewport);


		Utils::ParseOBJ("resources/vehicle.obj", verties, indicies);


		m_CurrentEffect = std::make_unique<EffectTexture>(m_pDevice, verties, indicies);
		m_CurrentTexture = std::make_unique<Texture>("resources/vehicle_diffuse.png" ,m_pDevice);

		D3D11_SAMPLER_DESC config{};
		config.Filter = D3D11_FILTER_ANISOTROPIC;
		config.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		config.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		config.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		config.MipLODBias = 0.0f;
		config.MaxAnisotropy = 16; // Set according to your needs, typical values are 1-16.
		config.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		config.BorderColor[0] = 0.0f; // RGBA for border color
		config.BorderColor[1] = 0.0f;
		config.BorderColor[2] = 0.0f;
		config.BorderColor[3] = 0.0f;
		config.MinLOD = 0.0f;
		config.MaxLOD = D3D11_FLOAT32_MAX;
		
		config.Filter = D3D11_FILTER_ANISOTROPIC;
		result = m_pDevice->CreateSamplerState(&config, &m_pAnisotropicMode);
		
		config.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		result = m_pDevice->CreateSamplerState(&config, &m_pPointMode);
		
		config.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		result = m_pDevice->CreateSamplerState(&config, &m_pLinearMode);




		return S_OK;
	}
}
