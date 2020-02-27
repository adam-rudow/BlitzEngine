#pragma once

// Outer-Engine includes
#include <chrono>
#include <windowsx.h>
//#include <windows.h>

// Inter-Engine includes
#include "SystemDefines.h"
#include "Utils/Utility.h"

// Sibling/Children includes

#define MOVING_AVG_COUNT 10

typedef std::chrono::high_resolution_clock				Clock;
typedef std::chrono::high_resolution_clock::time_point	Time;

class IGame;

class SYSTEM_EXPORT System
{
	DeclSingleton(System)

public:
	System();
	~System() {}

	bool			Init(HINSTANCE hInstance, int nCmdShow);

	void			RunLoop();
	void			Tick();

	inline HWND		GetHWnd() const			{ return hWnd; }

	inline void		SetGame(IGame* game)	{ mGame = game; }
	inline IGame*	GetGame()		const	{ return mGame; }

	double			QueryCPUFrameTime();
	inline double	GetAverageFrameTime()	{ return mCurrentAverageFrameTime; }// / MOVING_AVG_COUNT; }

	inline double	GetFrameTime()	const	{ return mFrameTime;  }
	inline float	GetFrameRate()	const	{ return mFrameRate;  }
	inline double	GetPrevFrameTime()		{ return mPrevTimerTime; }
	inline double	GetSystemTime() const	{ return mSystemTime; }

	static int		SysVersion()			{ return 99; }

	WPARAM			GetQuitParam() const	{ return mRetParam; }
	bool			CreateGameWindow(HINSTANCE hInstance, HWND& outHwnd);

	void			Quit(WPARAM retParam);

private:
	void ProcessInput();
	void GenerateOutput();	

	HWND hWnd;

	IGame* mGame;

	bool mQuitGame = false;
	WPARAM mRetParam;

	double	mSystemTime;
	double	mFrameTime;
	float	mFrameRate;
	//double mGameTime;

	Time*	mSystemStartTime;
	Time*	mPrevFrameTime;

	double	mPrevTimerTime;

	double	mAVGfpsList[MOVING_AVG_COUNT];
	int		mAVGfpsIdx;

	double	mCurrentAverageFrameTime;
	double	mCurTotalAvgFrameTime;

	double	mPrevLoggedGameTime;
	int		mNumFramesSinceLog;
};

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

SYSTEM_EXPORT extern int GWindowWidth;
SYSTEM_EXPORT extern int GWindowHeight;
