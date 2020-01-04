
#ifndef __MPBFRAMECOUNTER_H__
#define __MPBFRAMECOUNTER_H__

#include "MPBLibrary.h"


class MPBFrameCounter
{

public:

	static void updateFrame();
	static void startGameLoop();

	static MPBFLOAT 			framesPerSecond;
	static int					framesDrawn;
	static int					framesSkipped;
	static MPBFLOAT 			secondsElapsed;
	static MPBFLOAT 			totalSecondsElapsed;
	static int					msPerFrame;

private:

	static LARGE_INTEGER		perfFrequency;
	static LARGE_INTEGER		startTime;

	static MPBFLOAT perfCountToSeconds( LONGLONG perfCount );
};


#endif
