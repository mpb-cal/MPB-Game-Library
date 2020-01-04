
#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#include "OpenAL/alu.h"
#include "OpenAL/alut.h"

#include "PortAudio/ggsynth.h"

#include "MPBStreamingSound.h"
#include "MPBSoundManager.h"
#include "mmgr.h"

const int BUFFER_SIZE = GGSYNTH_FRAMES_PER_BLOCK * 256;


MPBStreamingSound::MPBStreamingSound():
	m_paused( false ),
	m_started( false )
{
	alGenBuffers( 2, m_buffers );
	MPBSoundManager::checkALError();

	alGenSources( 1, &m_source );
	MPBSoundManager::checkALError();
}

MPBStreamingSound::~MPBStreamingSound()
{
	alSourceStop( m_source );
	MPBSoundManager::checkALError();

	alDeleteSources( 1, &m_source );
	MPBSoundManager::checkALError();
	alDeleteBuffers( 2, m_buffers );
	MPBSoundManager::checkALError();
}

void MPBStreamingSound::start()
{
	if (!m_started)
	{
		stream( m_buffers[0] );
		stream( m_buffers[1] );
		alSourceQueueBuffers( m_source, 2, m_buffers );

		alSourcePlay( m_source );
	}

	m_started = true;
}

void MPBStreamingSound::pause()
{
	m_paused = true;
	alSourcePause( m_source );
}

void MPBStreamingSound::resume()
{
	if (m_paused)
	{
		alSourcePlay( m_source );
	}

	m_paused = false;
}

void MPBStreamingSound::updateFrame()
{
	int processed;
	alGetSourcei( m_source, AL_BUFFERS_PROCESSED, &processed );
	MPBSoundManager::checkALError();

	while (processed--)
	{
		ALuint buffer;

		alSourceUnqueueBuffers( m_source, 1, &buffer );
		stream( buffer );
		alSourceQueueBuffers( m_source, 1, &buffer );
	}
}

void MPBStreamingSound::updatePosition( MPBVector position, MPBVector velocity )
{
	float alPos[3];
	alPos[0] = (float)(position.x / MPBSoundManager::soundPositionDivisor);
	alPos[1] = (float)(position.y / MPBSoundManager::soundPositionDivisor);
	alPos[2] = (float)(position.z / MPBSoundManager::soundPositionDivisor);
	alSourcefv( m_source, AL_POSITION, alPos );
	MPBSoundManager::checkALError();

	float alVel[3];
	alVel[0] = (float)(velocity.x / MPBSoundManager::soundPositionDivisor);
	alVel[1] = (float)(velocity.y / MPBSoundManager::soundPositionDivisor);
	alVel[2] = (float)(velocity.z / MPBSoundManager::soundPositionDivisor);
	alSourcefv( m_source, AL_VELOCITY, alVel );
	MPBSoundManager::checkALError();
}

void MPBStreamingSound::stream( ALuint buffer )
{
	unsigned char data[BUFFER_SIZE];
	MPBFLOAT floatData[BUFFER_SIZE];

	int i;
	memset( floatData, 0, BUFFER_SIZE );

	for (i=0; i<BUFFER_SIZE; i+=GGSYNTH_FRAMES_PER_BLOCK)
	{
//		generateWave( floatData + i, GGSYNTH_FRAMES_PER_BLOCK );
		GGSynthFloat* block = process();
		memcpy( floatData + i, block, GGSYNTH_FRAMES_PER_BLOCK * sizeof( MPBFLOAT ) );
	}

	for (i=0; i<BUFFER_SIZE; i++)
	{
		MPBFLOAT sample = (floatData[i] + 1.0f) * 128.0f;
		data[i] = (unsigned char)(sample);
	}

	alBufferData( buffer, AL_FORMAT_MONO8, data, BUFFER_SIZE, (ALsizei)SAMPLE_RATE );
}



