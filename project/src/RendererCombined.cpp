#include "pch.h"
#include "RendererCombined.h"

#include "UnlitMesh.h"
#include "magic_enum.hpp"
#include "Texture.h"
#include "Utils.h"
#include "DirectXUtils.h"

extern ID3D11Debug* d3d11Debug;

RendererCombined::RendererCombined(SDL_Window* pWindow) :
	m_pWindow(pWindow)
{
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	
	CallDirectX(InitializeDirectX());
	InitSoftware();

	LoadScene();
}

RendererCombined::~RendererCombined()
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

HRESULT RendererCombined::InitializeDirectX()
{
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
	uint32_t createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif
	CallDirectX(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, &featureLevel, 1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext));


	m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3d11Debug));

	IDXGIFactory1* pDxgiFactory{};
	CallDirectX(CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory)));

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

	CallDirectX(pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain));

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

	CallDirectX(m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStecilBuffer));
	CallDirectX(m_pDevice->CreateDepthStencilView(m_pDepthStecilBuffer, &depthStencilViewDesc, &m_pDepthStecilView));
	CallDirectX(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer)));
	CallDirectX(m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView));

	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStecilView);

	D3D11_VIEWPORT viewport{};
	viewport.Width = static_cast<float>(m_Width);
	viewport.Height = static_cast<float>(m_Height);
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_pDeviceContext->RSSetViewports(1, &viewport);

	
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
	CallDirectX(m_pDevice->CreateSamplerState(&config, &m_pAnisotropicMode));

	config.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	CallDirectX(m_pDevice->CreateSamplerState(&config, &m_pPointMode));

	config.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	CallDirectX(m_pDevice->CreateSamplerState(&config, &m_pLinearMode));

	return S_OK;
}

void RendererCombined::InitSoftware()
{
	m_pFrontBuffer = SDL_GetWindowSurface(m_pWindow);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0,0, 0,0);
	m_SoftwareHelper = std::make_unique<SoftwareRendererHelper>(m_Width, m_Height, m_pBackBuffer);
}

void RendererCombined::LoadScene()
{
	m_ActiveScene.SetupCamera(static_cast<float>(m_Width)/ static_cast<float>(m_Height), 45, {0,0,-10.0f});
	
	std::vector<uint32_t> indicies;
	std::vector<Vertex_PosTexture> verties;
	Utils::ParseOBJ("resources/vehicle.obj", verties, indicies);

	std::vector<UnlitData> unlitData;
	unlitData.reserve(verties.size());

	for (const Vertex_PosTexture& vertexData : verties)
	{
		unlitData.push_back({.position= vertexData.position, .uv= vertexData.uv});
	}
	
	auto mesh = std::make_unique<UnlitMesh>(m_pDevice);
	mesh->LoadMeshData(std::move(unlitData), std::move(indicies), "Resources/vehicle_diffuse.png");
	m_ActiveScene.AddMesh(std::move(mesh));


	// YES

	
	indicies.clear();
	verties.clear();
	Utils::ParseOBJ("resources/plane.obj", verties, indicies);

	unlitData.clear();
	unlitData.reserve(verties.size());

	for (const Vertex_PosTexture& vertexData : verties)
	{
		unlitData.push_back({.position= vertexData.position, .uv= vertexData.uv});
	}
	
	mesh = std::make_unique<UnlitMesh>(m_pDevice);
	mesh->LoadMeshData(std::move(unlitData), std::move(indicies), "Resources/uv_grid_2.png");
	m_ActiveScene.AddMesh(std::move(mesh));
}

void RendererCombined::Update(const Timer& pTimer)
{
	m_ActiveScene.Update(pTimer);
}

void RendererCombined::RenderDirectX() const
{
	constexpr float color[4] = { 0.0f, 0.0f, 0.3f, 1.0f };
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStecilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);

	for (const std::unique_ptr<BaseMeshEffect>& mesh : m_ActiveScene.GetAllMeshes())
	{
		mesh->RenderDirectX(m_pDeviceContext, m_ActiveScene.GetCamera());
	}

	m_pSwapChain->Present(0, 0);

	
	


}

void RendererCombined::RenderSoftware() const
{
	SDL_FillRect(m_pBackBuffer, nullptr, SDL_MapRGB(m_pBackBuffer->format, 100, 100, 100));
	SDL_LockSurface(m_pBackBuffer);

	m_SoftwareHelper->ClearDepthBuffer();

	for (const std::unique_ptr<BaseMeshEffect>& mesh : m_ActiveScene.GetAllMeshes())
	{
		mesh->RenderSoftware(m_SoftwareHelper.get(), m_ActiveScene.GetCamera());
	}

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, nullptr, m_pFrontBuffer, nullptr);
	SDL_UpdateWindowSurface(m_pWindow);
}

void RendererCombined::ToggleSampleMode()
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


