#ifndef __MPBSOUNDMANAGER_H__
#define __MPBSOUNDMANAGER_H__

#include "MPBSoundBite.h"
#include "MPBWave.h"
#include <dmusici.h>

class MPBVector;
class MPB3DObject;
class MPBPhysicsObject;
class Plane;

class MPBSoundManager
{
public:

	static void init( char* waveDir, char* listenerWave );
	static void cleanUp();
	static void updateFrame( MPBPhysicsObject* listener );

	// plays a sound once or loops it
	static MPBSoundBite* playSound(	const char* waveName, 
												const MPBVector& position,
												const MPBVector& velocity,
												LONG volume,	// -10000 - 0
												bool loop	);

	static void setListenerPosition( const MPBVector& position );
	static void setListenerOrientation(	const MPBVector& front,
													const MPBVector& top	);
	static void setListenerVelocity( const MPBVector& velocity );
	static void collisionSound( Plane* plane, MPBVector velocity, double minKPH, MPBVector position, char* wave );
	static MPBSoundBite* createStreamingSoundBite();


private:

	static MPBSoundBite* getSoundBite( const char* fileName );
	static void loadWave( MPBWave* wave );
	// plays a sound bite once or loops it
	static MPBSoundBite* playSound(	MPBSoundBite* sb, 
												const MPBVector& position,
												const MPBVector& velocity,
												LONG volume,	// -10000 - 0
												bool loop	);

	// returns a SoundBite that can be played and moved
	static MPBSoundBite* duplicateSoundBite( const char* waveName );
	static MPBSoundBite* duplicateSoundBite( MPBSoundBite* sb );
	static void cleanupDuplicates();


	static LPDIRECTSOUND								m_directSound;
	static LPDIRECTSOUND3DLISTENER				m_listener;

	static IDirectMusicLoader8*					m_dmLoader8;
	static IDirectMusicPerformance8*				m_dmPerformance8;
	static IDirectSound3DListener8*				m_ds3DListener8;

	static set<MPBSoundBite*>						m_playingSoundSet;
	static vector<MPBWave*>							m_waveVector;
};


#endif
