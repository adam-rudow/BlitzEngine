// Outer-Engine includes

// Inter-Engine includes
#include "Application.h"
#include "Rendering/IRenderer.h"
#include "Game/IInputManager.h"
#include "Game/InputManager.h"
#include "Game/IGame.h"

#include "Utils/RandomGen.h"

// Sibling/Children includes

typedef std::chrono::milliseconds			ms;
typedef std::chrono::nanoseconds			ns;
typedef std::chrono::duration<double>		dsec;

SYSTEM_EXPORT int GWindowWidth = 1280;
SYSTEM_EXPORT int GWindowHeight = 720;

//ImplSingleton(Application)
Application* Application::sInstance = nullptr;

Application::Application()
{
	mSystemStartTime = new Time();
	*mSystemStartTime = Clock::now();
	mPrevFrameTime = new Time();
	*mPrevFrameTime = *mSystemStartTime;
	mPrevTimerTime = 0;

	ZeroMemory(&mAVGfpsList[0], MOVING_AVG_COUNT * sizeof(double));
	mAVGfpsIdx = 0;
	mCurrentAverageFrameTime = 0;
	mPrevLoggedGameTime = 0;
	mNumFramesSinceLog = 0;
	mCurTotalAvgFrameTime = 0;
}

Application::~Application() 
{
	delete(mInputManager);
	delete(mRenderer);
}

bool Application::Init(HINSTANCE hInstance, int nCmdShow)
{
	//IInputManager::StaticConstruct<InputManager>();
	//IRenderer::Instance()->Init(hInstance, nCmdShow);

	IInputManager* inputManager = ConstructInputManager();
	IRenderer* renderer = ConstructRenderer(hInstance, nCmdShow);
	//Profiler::Construct();
	RandomGen::Construct();
	RandomGen::Init();

	//TODO: Make this work here
	// If all engine initializations succeed
	//IGame::Instance()->Init();

	return true;
}

void Application::RunLoop(HINSTANCE hInstance, int nCmdShow)
{
	IInputManager* inputManager = ConstructInputManager();
	IRenderer* renderer = ConstructRenderer(hInstance, nCmdShow);
	renderer->Init(hInstance, nCmdShow);

	//Profiler::Construct();
	RandomGen::Construct();
	RandomGen::Init();

	Init_Internal();

	while (!mQuitGame)
	{
		ProcessInput();
		Update();
		GenerateOutput();
	}
}


void Application::Update()
{
	mFrameTime = QueryCPUFrameTime();
	mFrameRate = S_CAST(float, 1.0f / mFrameTime);
	mSystemTime += mFrameTime;
}

void Application::ProcessInput()
{
	IInputManager::Instance()->Update();
}

void Application::GenerateOutput()
{
	// Print debug num of triangles in game
	IRenderer* renderer = IRenderer::Instance();
	std::string numTriangles = "NumTriangles:\t " + std::to_string(renderer->GetNumTrianglesInScene());
	renderer->DrawString(numTriangles.c_str(), 20, 365);

	renderer->RenderFrame();


}
double Application::QueryCPUFrameTime()
{
	Time curTime = Clock::now();
	dsec ds = curTime - *mPrevFrameTime;
	*mPrevFrameTime = curTime;

	ns nanoSecs = std::chrono::duration_cast<ns>(ds);
	mPrevTimerTime = nanoSecs.count() * 1e-9;

	// Increment for averaged frame time/rate
	mCurTotalAvgFrameTime += mPrevTimerTime;
	++mNumFramesSinceLog;

	// Update averaged frameRate/Time every 0.5 seconds for readibility of onscreen text
	if (/*Game::Instance()->GetGameTime()*/mSystemTime > mPrevLoggedGameTime + 0.5)
	{
		mCurrentAverageFrameTime = mCurTotalAvgFrameTime / S_CAST(double, mNumFramesSinceLog);
		mCurTotalAvgFrameTime = 0;
		mNumFramesSinceLog = 0;
		mPrevLoggedGameTime = mSystemTime;
	}

	// Adjust frameTime average
	//mCurrentAverageFrameTime -= mAVGfpsList[mAVGfpsIdx];
	//mAVGfpsList[mAVGfpsIdx] = mPrevTimerTime;
	//mCurrentAverageFrameTime += mPrevTimerTime;
	//++mAVGfpsIdx;
	//if (mAVGfpsIdx == MOVING_AVG_COUNT)
	//	mAVGfpsIdx = 0;

	// return frame time;
	return mPrevTimerTime;
}

bool Application::CreateGameWindow(HINSTANCE hInstance, HWND& outHwnd)
{
	// struct that holds into for the window class
	WNDCLASSEX wc;

	// clear out the window class for use
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	// fill in the struct with the needed info
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = "WindowClass1";
	//if(!m_IsFullScreen)
	//	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

	// register the window class
	RegisterClassEx(&wc);

	// Resize client vs window sizes
	RECT wr = { 0, 0, GWindowWidth, GWindowHeight };	// set size but not pos
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);	// adjust the size

														// create the window and use the result as the handle
	hWnd = CreateWindowEx(NULL,
		"WindowClass1",		// name of the window class
		"BlitzEngine",		// title of the window
		WS_OVERLAPPEDWINDOW,// window style
		300,				// x-pos
		200,				// y-pos
		GWindowWidth,		// width
		GWindowHeight,		// height
		NULL,				// no parent window
		NULL,				// no menus
		hInstance,			// application handle
		NULL);				// used with multiple windows

	outHwnd = hWnd;

	return true;
}


// Main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (message)
	{
		// this message is read when the window is closed
	case WM_DESTROY:
	{
		// close the application entirely
		PostQuitMessage(0);
		return 0;
	}
	break;
	}

	IInputManager::Instance()->GetInputMessages(message, wParam);

	// Handle any messages the switch statement didnt 
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void Application::Quit(WPARAM retParam)
{
	mRetParam = retParam;
	mQuitGame = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

