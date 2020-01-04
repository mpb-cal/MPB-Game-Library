
#include "MPBStreamingSoundWhiteNoise.h"



GGSynthFloat* MPBStreamingSoundWhiteNoise::process()
{
	m_umb.clear();
	m_uwn.gen( .1f );
	m_umb.gen( m_uwn.outputs );

	return m_umb.outputs;
}


