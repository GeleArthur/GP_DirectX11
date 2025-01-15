#include "pch.h"

#ifndef NDEBUG
#include "vld.h"
#endif

#undef main
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

	const uint32_t width = 1920;
	const uint32_t height = 1080;

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
				if (e.key.keysym.scancode == SDL_SCANCODE_F2)
				{
					pRenderer->ToggleSampleMode();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F1)
				{
					renderSoftWare  = !renderSoftWare;
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F10)
				{
					pRenderer->ToggleSceneBackGround();
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
			std::cout << "dFPS: " << pTimer.GetdFPS() << '\n';
		}
	}
	pTimer.Stop();

	//Shutdown "framework"
	pRenderer.reset();

#ifdef _DEBUG
	OutputDebugString("REPORT LEAKS\n");
	d3d11Debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
	OutputDebugString("DONE LEAKS\n");
#endif

	d3d11Debug->Release();

	ShutDown(pWindow);
	return 0;
}
