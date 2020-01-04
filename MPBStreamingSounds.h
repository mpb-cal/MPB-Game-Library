#ifndef __MPBSTREAMINGSOUNDS_H__
#define __MPBSTREAMINGSOUNDS_H__

#include "MPBStreamingSound.h"


class MPBStreamingSound1: public MPBStreamingSound
{
public:

	GGSynthFloat* process()
	{
		m_umb.clear();
		m_uwn.gen( .1f );
		m_umb.gen( m_uwn.outputs );

		return m_umb.outputs;
	}

protected:

	Unit_MixingBus				m_umb;
	Unit_WhiteNoise			m_uwn;
};

class MPBStreamingSound2: public MPBStreamingSound
{
public:

	MPBStreamingSound2::MPBStreamingSound2():
		MPBStreamingSound(),
		m_highpassFrequency( 1000.0f ),
		m_resonance( 1.0f ),
		m_amplitude( .1f )
	{}

	GGSynthFloat* process()
	{
		m_umb.clear();

		m_uwn.gen( (float)m_amplitude );
		m_uhf.calculateCoefficients( (float)m_highpassFrequency, (float)m_resonance );
		m_uhf.gen( m_uwn.outputs );
		m_umb.gen( m_uhf.outputs );

		return m_umb.outputs;
	}

	MPBFLOAT 	m_amplitude;
	MPBFLOAT 	m_highpassFrequency;
	MPBFLOAT 	m_resonance;

protected:

	Unit_MixingBus				m_umb;
	Unit_WhiteNoise			m_uwn;
	Unit_HighpassFilter		m_uhf;
};

class MPBStreamingSound3: public MPBStreamingSound
{
public:

	MPBStreamingSound3::MPBStreamingSound3():
		MPBStreamingSound(),
		m_lowpassFrequency( 200.0f ),
		m_resonance( 1.0f ),
		m_amplitude( .1f )
	{}

	GGSynthFloat* process()
	{
		m_umb.clear();

		m_uwn.gen( (float)m_amplitude );
		m_ulf.calculateCoefficients( (float)m_lowpassFrequency, (float)m_resonance );
		m_ulf.gen( m_uwn.outputs );
		m_umb.gen( m_ulf.outputs );

		return m_umb.outputs;
	}

	MPBFLOAT 	m_amplitude;
	MPBFLOAT 	m_lowpassFrequency;
	MPBFLOAT 	m_resonance;

protected:

	Unit_MixingBus				m_umb;
	Unit_WhiteNoise			m_uwn;
	Unit_LowpassFilter		m_ulf;
};


#endif
