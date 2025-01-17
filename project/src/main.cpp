#include "pch.h"

#ifdef _DEBUG
#include "vld.h"
#endif

#undef main
#include <array>

#include "RendererCombined.h"


void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

ID3D11Debug* d3d11Debug;

constexpr static auto MAGENTA = "\033[35m";
constexpr static auto YELLOW = "\033[33m";
constexpr static auto GREEN = "\033[32m";
constexpr static auto RESET = "\033[0m";

int main(int argc, char* args[])
{
	std::cout << YELLOW << R"(
[Key Bindings - SHARED]
	[F1] Toggle Rasterizer Mode(HARDWARE / SOFTWARE)
	[F2]  Toggle Vehicle Rotation(ON / OFF)
	[F3] Toggle FireFX(ON / OFF)
	[F9]  Cycle CullMode(BACK / FRONT / NONE)
	[F10] Toggle Uniform ClearColor(ON / OFF)
	[F11] Toggle Print FPS(ON / OFF))";

	std::cout << GREEN << R"(
[Key Bindings - HARDWARE]
	[F4] Cycle Sampler State(POINT / LINEAR / ANISOTROPIC))";

	std::cout << MAGENTA << R"(
[Key Bindings - SOFTWARE]
	[F5] Cycle Shading Mode(COMBINED / OBSERVED_AREA / DIFFUSE / SPECULAR)
	[F6] Toggle NormalMap(ON / OFF)
	[F7] Toggle DepthBuffer Visualization(ON / OFF)
	[F8] Toggle BoundingBox Visualization(ON / OFF))";
	std::cout << RESET << std::endl;


	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;

	SDL_Window* pWindow = SDL_CreateWindow(
		"DirectX/CPU - Arthur van den Barselaar 2GD10",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	// SDL_SetRelativeMouseMode(SDL_TRUE);

	//Initialize "framework"
	Timer pTimer = Timer();
	auto pRenderer = std::make_unique<RendererCombined>(pWindow);
	bool renderSoftWare{};
	bool printFps{false};

	//Start loop
	pTimer.Start();
	float printTimer = 0.f;
	bool isLooping = true;

	
	
	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
				
			case SDL_KEYUP:
				// SHARED
				if (e.key.keysym.scancode == SDL_SCANCODE_F1)
				{
					renderSoftWare = !renderSoftWare;
					std::cout << YELLOW << "**(SHARED) Rasterizer Mode = ";
					if (renderSoftWare) std::cout << "SOFTWARE";
					else std::cout << "HARDWARE";
					std::cout << RESET << '\n';

				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F2)
				{
					pRenderer->ToggleRotation();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F9)
				{
					pRenderer->NextCullMode();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F10)
				{
					pRenderer->ToggleSceneBackGround();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F11)
				{
					printFps = !printFps;
					std::cout << YELLOW << "**(SHARED) Print FPS = ";
					if (printFps) std::cout << "ON";
					else std::cout << "OFF";
					std::cout << RESET << '\n';
					
				}

				// HARDWARE
				if (e.key.keysym.scancode == SDL_SCANCODE_F3)
				{
					pRenderer->DisableAllFireFx();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F4)
				{
					pRenderer->ToggleSampleMode();
				}

				// SOFTWARE
				if (e.key.keysym.scancode == SDL_SCANCODE_F5)
				{
					pRenderer->NextShadingMode();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F6)
				{
					pRenderer->ToggleNormalMap();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F7)
				{
					pRenderer->ToggleDepthBuffer();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F8)
				{
					pRenderer->ToggleBoundingBoxDraw();
				}
				break;
			default: ;
			}
		}

		//--------- Update ---------
		pRenderer->Update(pTimer);

		//--------- Render ---------
		if (renderSoftWare)
			pRenderer->RenderSoftware();
		else
			pRenderer->RenderDirectX();

		//--------- Timer ---------
		pTimer.Update();
		printTimer += pTimer.GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			if (printFps) std::cout << "dFPS: " << pTimer.GetdFPS() << '\n';
		}
	}
	pTimer.Stop();

	//Shutdown "framework"
	pRenderer.reset();

#ifdef _DEBUG
	OutputDebugStringA("REPORT LEAKS\n");
	d3d11Debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
	OutputDebugStringA("DONE LEAKS\n");
#endif

	if (d3d11Debug)
		d3d11Debug->Release();

	ShutDown(pWindow);
	return 0;
}
