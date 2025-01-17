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

int main(int argc, char* args[])
{
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
	bool printFps{true};

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
					std::cout << "Toggle RenderMode\n";
					renderSoftWare = !renderSoftWare;
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
