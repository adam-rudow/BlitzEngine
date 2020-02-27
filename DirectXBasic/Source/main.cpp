#include <windows.h>
#include <windowsx.h>
//#include <d3d11.h>
//#include <d3dx11.h>
//#include <d3dx10.h>

#include <string>
#include <iostream>

#include <fstream>

#include "BlitzSystem/System.h"
#include "BlitzSystem/Rendering/IRenderer.h"
#include "BlitzSystem/Game/IInputManager.h"

#include "Renderer.h"
#include "Game/InputManager.h"

#include "Game.h"

// include Direct3D Library files
//#pragma comment (lib, "d3d11.lib")
//#pragma comment (lib, "d3dx11.lib")
//#pragma comment (lib, "d3dx10.lib")

//// the entry point for any windows program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	System::Construct();
	System* system = System::Instance();

	IGame::StaticConstruct<Game>();
	IGame* game = IGame::Instance();

	// Init engine Aspects. TODO: make this dependency cleaner - should not be in game code/project
	IRenderer::StaticConstruct<Renderer>();
	IRenderer* renderer = IRenderer::Instance();

	if (/*system && */system->Init(hInstance, nCmdShow))
	{
		IGame::Instance()->Init();

		system->SetGame(game);
		system->RunLoop();
	}

	WPARAM quitParam = system->GetQuitParam();
	delete(IGame::Instance());
	delete(system);

	return quitParam;
}