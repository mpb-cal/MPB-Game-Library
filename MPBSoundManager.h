#ifndef __MPBSOUNDMANAGER_H__
#define __MPBSOUNDMANAGER_H__

#include "MPBLibrary.h"
#include "MPBList.h"
#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#include "OpenAL/alu.h"
#include "OpenAL/alut.h"


class MPBVector;
class MPBPhysicsObject;
class Plane;
class MPBSound;
class MPBStreamingSound;


class MPBSoundManager
{
public:

	static void init( char* waveDir );
	static void cleanUp();
//	static void updateFrame( MPBPhysicsObject* listener );
	static void updateFrame( 
		MPBVector listenerPosition,
		MPBVector listenerVelocity,
		MPBVector listenerFacing
	);
	static MPBSound* getSound( const char* filename );

	static void checkALError();
	static ALuint createSoundSource( const char* filename );
	static void addStreamingSound( MPBStreamingSound* s );
	static void removeStreamingSound( MPBStreamingSound* s );
	static void destroySoundSource( ALuint source );
	static void playSound(	const char* waveName, 
									const MPBVector& position,
									const MPBVector& velocity,
									MPBFLOAT gain	);	// 0.0 - 1.0

	static void collisionSound( MPBVector collisionNormal, MPBVector velocity, MPBFLOAT minKPH, MPBVector position, char* wave );
	static void collisionSound( Plane* plane, MPBVector velocity, MPBFLOAT minKPH, MPBVector position, char* wave );

	static MPBFLOAT soundPositionDivisor;

private:

	static void loadWaves( char* dir, bool _3D );
	static void loadWave( char* dir, char* filename, bool _3D );

	static MPBList								m_sounds;
	static MPBList								m_streamingSounds;
	static MPBList								m_playingSources;

};


#endif
