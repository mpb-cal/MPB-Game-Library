
#include "MPBSoundBite.h"
#include "MPBVector.h"


MPBSoundBite::MPBSoundBite( LPDIRECTSOUNDBUFFER soundBuffer, LPDIRECTSOUND3DBUFFER sound3DBuffer ):
	m_soundBuffer( soundBuffer ),
	m_sound3DBuffer( sound3DBuffer )
{
}

MPBSoundBite::~MPBSoundBite()
{
	if (m_soundBuffer)
	{
		m_soundBuffer->Stop();
		m_soundBuffer->Release();
		m_soundBuffer = NULL;
	}

	if (m_sound3DBuffer)
	{
		m_sound3DBuffer->Release();
		m_sound3DBuffer = NULL;
	}
}

// velocity: meters per second
// position: meters
void MPBSoundBite::updatePosition( const MPBVector& position, const MPBVector& velocity )
{
	if (m_sound3DBuffer)
	{
		m_sound3DBuffer->SetPosition(	(float)position.x / SOUND_POSITION_DIVISOR,
												(float)position.y / SOUND_POSITION_DIVISOR,
												(float)position.z / SOUND_POSITION_DIVISOR,
												DS3D_IMMEDIATE	);

		m_sound3DBuffer->SetVelocity(	(float)velocity.x / SOUND_POSITION_DIVISOR,
												(float)velocity.y / SOUND_POSITION_DIVISOR,
												(float)velocity.z / SOUND_POSITION_DIVISOR,
												DS3D_IMMEDIATE	);
	}
}


