#pragma once

// Outer-Engine includes
#include <chrono>


// Inter-Engine includes
#include "Utils/Utility.h"

// Sibling/Children includes

#define MOVING_AVG_COUNT 10


typedef std::chrono::high_resolution_clock				Clock;
typedef std::chrono::high_resolution_clock::time_point	Time;

    
class Profiler
{
	DeclSingleton(Profiler)

public:
	Profiler();
	~Profiler() {}

	inline double GetFrameTime() { return mPrevTimerTime; }

	double QueryCPUFrameTime();
	inline double GetAverageFrameTime() { return mCurrentAverageFrameTime; }// / MOVING_AVG_COUNT; }
        
private:
	Time mGameStartTime;
	Time mPrevFrameTime;

	double mPrevTimerTime;

	double mAVGfpsList[MOVING_AVG_COUNT];
	int mAVGfpsIdx;

	double mCurrentAverageFrameTime;
	double mCurTotalAvgFrameTime;

	double mPrevLoggedGameTime;
	int mNumFramesSinceLog;
};
   
