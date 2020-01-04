
#include "MPBGlobal.h"
#include "MPBSoundManager.h"
#include "MPBSound.h"
#include "MPBPhysics.h"
#include "MPBPlane.h"
#include "MPBMessageWindow.h"
#include "MPBStreamingSound.h"
#include "MPBPhysicsObject.h"

#include "PortAudio/UnitGen.h"
#include "mmgr.h"

#pragma comment(lib, "openal32.lib")
#pragma comment(lib, "alut.lib")



MPBList							MPBSoundManager::m_sounds;
MPBList							MPBSoundManager::m_streamingSounds;
MPBList							MPBSoundManager::m_playingSources;
MPBFLOAT							MPBSoundManager::soundPositionDivisor = 10.0f;


void MPBSoundManager::init( char* waveDir )
{
	DWORD rc = GetFileAttributes( waveDir );
	MPB_ASSERT( rc != -1 );//&& rc & FILE_ATTRIBUTE_DIRECTORY == FILE_ATTRIBUTE_DIRECTORY);

	alutInit( NULL, 0 );
	alGetError();

	UnitGen::setSampleRate( SAMPLE_RATE );

	char dir[MAX_PATH];

	sprintf( dir, "%s/3D", waveDir );
	loadWaves( dir, true );

	sprintf( dir, "%s/2D", waveDir );
	loadWaves( dir, false );
}

void MPBSoundManager::cleanUp()
{
	MPB_LIST_DELETE( m_sounds, MPBSound* );

	m_playingSources.clear();
	m_streamingSounds.clear();

	alutExit();
}

void MPBSoundManager::checkALError()
{
	ALenum error = alGetError();
	if (error == AL_NO_ERROR) return;

	const unsigned char* msg = alGetString( error );
//	MPB_ASSERT( 0 );
}

void MPBSoundManager::loadWaves( char* dir, bool _3D )
{
	char wildcard[MAX_PATH];
	WIN32_FIND_DATA findData;

	sprintf( wildcard, "%s/*.wav", dir );
	HANDLE handle = FindFirstFile( wildcard, &findData );

	while (true)
	{
		loadWave( dir, findData.cFileName, _3D );
		if (!FindNextFile( handle, &findData )) break;
	}

	FindClose( handle );

}

void MPBSoundManager::loadWave( char* dir, char* filename, bool _3D )
{
	char path[1000];

	strcpy( path, dir );
	strcat( path, "/" );
	strcat( path, filename );

	ALuint buffer;
	ALenum format;
	ALvoid* data;
	ALsizei size;
	ALsizei freq;
	ALboolean loop;

	alGenBuffers( 1, &buffer );
	checkALError();

	alutLoadWAVFile( path, &format, &data, &size, &freq, &loop );

	MPB_ASSERT( format == AL_FORMAT_MONO8 || format == AL_FORMAT_MONO16 );

	alBufferData( buffer, format, data, size, freq );
	alutUnloadWAV( format, data, size, freq );

	m_sounds.push_back( new MPBSound( buffer, filename ) );
}

/*
void MPBSoundManager::updateFrame( MPBPhysicsObject* listener )
{
	MPB_ASSERT( listener );
	updateFrame( listener->getPosition(), listener->getVelocity(), listener->getOrientationVector() );
}
*/

void MPBSoundManager::updateFrame( 
	MPBVector listenerPosition,
	MPBVector listenerVelocity,
	MPBVector listenerFacing
)
{
	// update listener

	float arr[3];

	arr[0] = listenerPosition.x / soundPositionDivisor;
	arr[1] = listenerPosition.y / soundPositionDivisor;
	arr[2] = listenerPosition.z / soundPositionDivisor;
	alListenerfv( AL_POSITION, arr );
	checkALError();

	arr[0] = listenerVelocity.x / soundPositionDivisor;
	arr[1] = listenerVelocity.y / soundPositionDivisor;
	arr[2] = listenerVelocity.z / soundPositionDivisor;
	alListenerfv( AL_VELOCITY, arr );
	checkALError();

	float arr2[6];

	arr2[0] = listenerFacing.x;
	arr2[1] = 0;//listenerFacing.y;
	arr2[2] = listenerFacing.z;
	arr2[3] = 0;
	arr2[4] = 1;
	arr2[5] = 0;
	alListenerfv( AL_ORIENTATION, arr2 );
	checkALError();

	// clean up stopped sounds
	MPBList stoppedSources;
	MPB_LIST_LOOP( m_playingSources, source, ALuint )
	{
		ALint state;
		alGetSourcei( source, AL_SOURCE_STATE, &state );
		if (state == AL_STOPPED)
		{
			alDeleteSources( 1, &source );
//			checkALError();
			stoppedSources.push_back( (void*)source );
		}
	}

	MPB_LIST_LOOP( stoppedSources, s, ALuint )
	{
		m_playingSources.remove( (void*)s );
	}


	MPB_LIST_LOOP( m_streamingSounds, sound, MPBStreamingSound* )
	{
		sound->updateFrame();
	}
}

MPBSound* MPBSoundManager::getSound( const char* filename )
{
	MPB_LIST_LOOP( m_sounds, sound, MPBSound* )
	{
		if (strcmp( sound->m_filename, filename ) == 0)
		{
			return sound;
		}
	}

	MPB_ASSERT( 0 );
	return NULL;
}

// velocity: meters per second
// position: meters
void MPBSoundManager::playSound(	const char* filename, 
											const MPBVector& position,
											const MPBVector& velocity,
											MPBFLOAT gain	)	// 0.0 - 1.0
{
	ALuint source = createSoundSource( filename );

	if (source)
	{
		alSourcef( source, AL_PITCH,    1.0f );
		alSourcef( source, AL_GAIN,     (float)gain );
		alSourcei( source, AL_LOOPING,  false );

		float alPos[3];
		alPos[0] = position.x / soundPositionDivisor;
		alPos[1] = position.y / soundPositionDivisor;
		alPos[2] = position.z / soundPositionDivisor;
		alSourcefv( source, AL_POSITION, alPos );
		checkALError();

		float alVel[3];
		alVel[0] = velocity.x / soundPositionDivisor;
		alVel[1] = velocity.y / soundPositionDivisor;
		alVel[2] = velocity.z / soundPositionDivisor;
		alSourcefv( source, AL_VELOCITY, alVel );
		checkALError();

		alSourcePlay( source );
		checkALError();
		m_playingSources.push_back( (void*)source );
	}
}

ALuint MPBSoundManager::createSoundSource( const char* filename )
{
	ALuint source;
	alGenSources( 1, &source );
//	checkALError();
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) return 0;

	alSourcei( source, AL_BUFFER, getSound( filename )->m_buffer );

	return source;
}

void MPBSoundManager::destroySoundSource( ALuint source )
{
	alDeleteSources( 1, &source );
	checkALError();
}

void MPBSoundManager::addStreamingSound( MPBStreamingSound* s )
{
	m_streamingSounds.push_back( s );
}

void MPBSoundManager::removeStreamingSound( MPBStreamingSound* s )
{
	m_streamingSounds.remove( s );
}

// speed of  5 mps, angle of 0 --> gain = .5
// speed of 10 mps, angle of 0 --> gain = 1
void MPBSoundManager::collisionSound( 
	Plane* plane, 
	MPBVector velocity, 
	MPBFLOAT minKPH,		// ignored
	MPBVector position, 
	char* wave 
)
{
	collisionSound( plane->getNormal(), velocity, minKPH, position, wave );
}

// speed of  5 mps, angle of 0 --> gain = .5
// speed of 10 mps, angle of 0 --> gain = 1
void MPBSoundManager::collisionSound( 
	MPBVector collisionNormal, 
	MPBVector velocity, 
	MPBFLOAT minKPH,		// ignored
	MPBVector position, 
	char* wave 
)
{
	MPBFLOAT speed = velocity.getLength();
	if (speed)
	{
		velocity.normalize();
		MPBFLOAT cosine = fabs( velocity.dotProduct( collisionNormal ) );	// 0 (skid) to 1 (direct landing)

//		const MPBFLOAT MIN_SPEED_MPS = minKPH * 1000 / 3600;
//		const int LOWEST_VOLUME = 1000;
//		LONG volume = speed * cosine - MIN_SPEED_MPS - LOWEST_VOLUME;

		MPBFLOAT gain = speed * .1f * cosine;

		if (gain > 0.1f) 
			playSound( wave, position, velocity, gain );
	}
}





