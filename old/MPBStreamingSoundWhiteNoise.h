#ifndef __MPBSTREAMINGSOUND_WHITENOISE_H__
#define __MPBSTREAMINGSOUND_WHITENOISE_H__

#include "MPBStreamingSound.h"


class MPBStreamingSoundWhiteNoise: public MPBStreamingSound
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
	Unit_RedNoise				m_urn;
	Unit_SineOscillator		m_uso;
	Unit_LowpassFilter		m_ulf;
};


#endif
