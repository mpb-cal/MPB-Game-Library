#ifndef __MPBSTREAMINGSOUND_H__
#define __MPBSTREAMINGSOUND_H__

#include "MPBLibrary.h"
#include "OpenAL/al.h"

#include "../MPBLibrary/PortAudio/Unit_WhiteNoise.h"
#include "../MPBLibrary/PortAudio/Unit_LowpassFilter.h"
#include "../MPBLibrary/PortAudio/Unit_RedNoise.h"
#include "../MPBLibrary/PortAudio/Unit_SlewRateLimiter.h"
#include "../MPBLibrary/PortAudio/Unit_SineOscillator.h"
#include "../MPBLibrary/PortAudio/Unit_ImpulseOscillator.h"
#include "../MPBLibrary/PortAudio/Unit_HighpassFilter.h"
#include "../MPBLibrary/PortAudio/Unit_Add.h"
#include "../MPBLibrary/PortAudio/Unit_Delay.h"
#include "../MPBLibrary/PortAudio/Unit_MixingBus.h"
#include "../MPBLibrary/PortAudio/Unit_Multiply.h"


#define SAMPLE_RATE (8000.0f)

class MPBStreamingSound
{
public:

	MPBStreamingSound();
	virtual ~MPBStreamingSound();

	void start();
	void pause();
	void resume();
	void updateFrame();
	void updatePosition( MPBVector position, MPBVector velocity );
//	void generateWave( MPBFLOAT* output, int size );

	virtual GGSynthFloat* process() = 0;

protected:

	void stream( ALuint buffer );

	ALuint		m_buffers[2];
	ALuint		m_source;
	bool			m_paused;
	bool			m_started;
};


#endif
