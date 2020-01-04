
#include "MPBGlobal.h"
#include "MPBFrameCounter.h"
#include "mmgr.h"

LARGE_INTEGER	MPBFrameCounter::perfFrequency;
MPBFLOAT 		MPBFrameCounter::framesPerSecond;
int				MPBFrameCounter::framesDrawn;
int				MPBFrameCounter::framesSkipped;
MPBFLOAT 		MPBFrameCounter::secondsElapsed;
LARGE_INTEGER	MPBFrameCounter::startTime = { 0 };
MPBFLOAT 		MPBFrameCounter::totalSecondsElapsed;
int				MPBFrameCounter::msPerFrame;

void MPBFrameCounter::startGameLoop()
{
	BOOL b = QueryPerformanceFrequency( &perfFrequency );
	MPB_ASSERT( b );
}

MPBFLOAT MPBFrameCounter::perfCountToSeconds( LONGLONG perfCount )
{
	MPB_ASSERT( perfFrequency.QuadPart );
	return (float)perfCount / (float)perfFrequency.QuadPart;
}

// call once per frame.
void MPBFrameCounter::updateFrame()
{
	static DWORD previousFrameCount = { 0 };
	static LARGE_INTEGER previousPerfCount = { 0 };
	static MPBFLOAT secondsSinceFPSCalculated = 0;

	framesDrawn++;
	framesSkipped = 0;

	LARGE_INTEGER perfCount;

	bool msPerFrameSet = false;
	while (true)
	{
		QueryPerformanceCounter( &perfCount );

		if (startTime.QuadPart == 0) startTime = perfCount;
		totalSecondsElapsed = perfCountToSeconds( perfCount.QuadPart - startTime.QuadPart );

		secondsElapsed = perfCountToSeconds( perfCount.QuadPart - previousPerfCount.QuadPart );
		MPB_ASSERT( secondsElapsed );
		if (!msPerFrameSet) msPerFrame = (int)(secondsElapsed * 1000);
		msPerFrameSet = true;

		if (0)//(secondsElapsed < .01)
		{
			framesSkipped++;
			continue;
		}

		break;
	}

	previousPerfCount = perfCount;

//	if (secondsElapsed < .001f) secondsElapsed = .001f;
	if (secondsElapsed > .05f) secondsElapsed = .05f;

	secondsSinceFPSCalculated += secondsElapsed;

	if (secondsSinceFPSCalculated > .5)
	{
		framesPerSecond = (framesDrawn - previousFrameCount) / secondsSinceFPSCalculated;
		previousFrameCount = framesDrawn;
		secondsSinceFPSCalculated = 0;
	}
}




