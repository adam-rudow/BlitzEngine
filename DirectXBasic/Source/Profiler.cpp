
// Outer-Engine includes

// Inter-Engine includes
#include "Profiler.h"
#include "BlitzSystem/System.h"
//#include "Game.h"

// Sibling/Children includes

typedef std::chrono::milliseconds			ms;
typedef std::chrono::nanoseconds			ns;
typedef std::chrono::duration<double>		dsec;

ImplSingleton(Profiler)
    
Profiler::Profiler()
{
	mGameStartTime = Clock::now();
	mPrevFrameTime = mGameStartTime;
	mPrevTimerTime = 0;
	
	ZeroMemory(&mAVGfpsList[0], MOVING_AVG_COUNT * sizeof(double));
	mAVGfpsIdx = 0;
	mCurrentAverageFrameTime = 0;
	mPrevLoggedGameTime = 0;
	mNumFramesSinceLog = 0;
	mCurTotalAvgFrameTime = 0;
}

double Profiler::QueryCPUFrameTime()
{
	Time curTime = Clock::now();
	dsec ds = curTime - mPrevFrameTime;
	mPrevFrameTime = curTime;

	ns nanoSecs = std::chrono::duration_cast<ns>(ds);
	mPrevTimerTime = nanoSecs.count() * 1e-9;

	// Increment for averaged frame time/rate
	mCurTotalAvgFrameTime += mPrevTimerTime;
	++mNumFramesSinceLog;

	// Update averaged frameRate/Time every 0.5 seconds for readibility of onscreen text
	if (System::Instance()->GetSystemTime() > mPrevLoggedGameTime + 0.5)
	{
		mCurrentAverageFrameTime = mCurTotalAvgFrameTime / S_CAST(double, mNumFramesSinceLog);
		mCurTotalAvgFrameTime = 0;
		mNumFramesSinceLog = 0;
		mPrevLoggedGameTime = System::Instance()->GetSystemTime();
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

