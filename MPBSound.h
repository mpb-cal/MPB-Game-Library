#ifndef __MPBSOUND_H__
#define __MPBSOUND_H__

#include "MPBLibrary.h"

class MPBSoundManager;

class MPBSound
{
public:

	MPBSound( ALuint buffer, char* filename );
	~MPBSound();

//	void updatePosition( const MPBVector& position, const MPBVector& velocity );

protected:

	ALuint	m_buffer;
	char		m_filename[MAX_PATH];

	friend class MPBSoundManager;
};


#endif
