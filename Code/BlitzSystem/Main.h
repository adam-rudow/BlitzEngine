#pragma once

#include <windows.h>
#include <windowsx.h>

#include "Application.h"

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)

int main(int argc, char* argv[])
{
	Application* application = ConstructApplication();
	application->RunLoop();

	WPARAM quitParam = application->GetQuitParam();
	delete(application);

	return quitParam;
}
