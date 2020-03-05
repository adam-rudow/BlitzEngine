#pragma once

#include <windows.h>
#include <windowsx.h>

#include "Application.h"
#include "Logging/Log.h"

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)

int main(int argc, char* argv[])
{
	Log::Init();

	Application* application = ConstructApplication();
	application->RunLoop();

	WPARAM quitParam = application->GetQuitParam();
	delete(application);

	return quitParam;
}
