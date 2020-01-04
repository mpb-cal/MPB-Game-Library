#ifndef __MPBSOUNDBITE_H__
#define __MPBSOUNDBITE_H__

#include <oaidl.h>
#include <dmusici.h>

#define SOUND_POSITION_DIVISOR	(5.0f)


class MPBVector;

class MPBSoundBite
{
public:

	MPBSoundBite( LPDIRECTSOUNDBUFFER soundBuffer, LPDIRECTSOUND3DBUFFER sound3DBuffer );
	~MPBSoundBite();

	void updatePosition( const MPBVector& position, const MPBVector& velocity );

	LPDIRECTSOUNDBUFFER			m_soundBuffer;
	LPDIRECTSOUND3DBUFFER		m_sound3DBuffer;
};


#endif
