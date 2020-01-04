
#include "MPBGlobal.h"
#include "MPBSoundManager.h"
#include "MPB3DObject.h"
#include "MPBPhysics.h"
#include "MPBPlane.h"
#include "MPBMessageWindow.h"


const int SOUNDBITE_COPIES = 5;


LPDIRECTSOUND							MPBSoundManager::m_directSound;
LPDIRECTSOUND3DLISTENER				MPBSoundManager::m_listener;

IDirectMusicLoader8*					MPBSoundManager::m_dmLoader8;
IDirectMusicPerformance8*			MPBSoundManager::m_dmPerformance8;
IDirectSound3DListener8*			MPBSoundManager::m_ds3DListener8;

set<MPBSoundBite*>					MPBSoundManager::m_playingSoundSet;
vector<MPBWave*>						MPBSoundManager::m_waveVector;



// listenerWave can be any wave file in waveDir
void MPBSoundManager::init( char* waveDir, char* listenerWave )
{
	m_directSound = NULL;
	m_listener = NULL;
	m_dmLoader8 = NULL;
	m_dmPerformance8 = NULL;
	m_ds3DListener8 = NULL;

	HRESULT hr;

	if (DirectSoundCreate( NULL, &m_directSound, NULL ) != DS_OK)
	{
		writeLog( "DirectSoundCreate failed.\n" );
	}

	if (m_directSound->SetCooperativeLevel( g_hwnd, DSSCL_NORMAL ) != DS_OK)
	{
		writeLog( "SetCooperativeLevel failed (sound).\n" );
	}

	// load waves

	WIN32_FIND_DATA findData;
	char wildcard[MAX_PATH];
	sprintf( wildcard, "%s/*.wav", waveDir );
	HANDLE handle = FindFirstFile( wildcard, &findData );

	while (true)
	{
		bool _3D = true;
		if (!strcmp( findData.cFileName, "ocean_edge.wav" )) _3D = false;
		if (!strcmp( findData.cFileName, "water-flowing1.wav" )) _3D = false;

		MPBWave* wave = new MPBWave( waveDir, findData.cFileName, _3D );
		m_waveVector.push_back( wave );
		loadWave( wave );

		if (!FindNextFile( handle, &findData )) break;
	}

	FindClose( handle );

	// create listener

	LPDIRECTSOUNDBUFFER primaryBuffer;

	char path[MAX_PATH];
	strcpy( path, waveDir );
	strcat( path, "/" );
	strcat( path, listenerWave );

	if (LoadWaveFile(	m_directSound, 
							primaryBuffer,  
							path, 
							true, 
							true ) != true)
	{
		writeLog( "LoadWaveFile failed (primaryBuffer).\n" );
		return;
	}

	hr = primaryBuffer->QueryInterface( IID_IDirectSound3DListener, (void**)&m_listener ); 

	if (SUCCEEDED( hr )) 
	{
//			m_listener->SetDistanceFactor( .02f, DS3D_IMMEDIATE );
	} 
	else
	{
		writeLog( "QueryInterface failed (createListener()).\n" );
	}
}

void MPBSoundManager::cleanUp()
{
	DELETE_SET( m_playingSoundSet, MPBSoundBite* );
	DELETE_VECTOR( m_waveVector, MPBWave* );

	if (m_listener)
	{
		m_listener->Release();
		m_listener = NULL;
	}

	if (m_directSound)
	{
		m_directSound->Release();
		m_directSound = NULL;
	}
}

void MPBSoundManager::updateFrame( MPBPhysicsObject* listener )
{
	if (listener)
	{
		setListenerPosition( listener->getPosition() );
		setListenerVelocity( listener->getVelocity() );
		setListenerOrientation(	listener->getOrientationVector() * -1,
										MPBVector( 0, 1, 0 )	);
	}

//	cleanupDuplicates();
}

void MPBSoundManager::loadWave( MPBWave* wave )
{
	HRESULT hr;

	char path[1000];

	strcpy( path, wave->getDir() );
	strcat( path, "/" );
	strcat( path, wave->getFileName() );

	char buf[1000];

	LPDIRECTSOUNDBUFFER sound = NULL;
	LPDIRECTSOUND3DBUFFER sound3D = NULL;

	if (LoadWaveFile( m_directSound, sound, path, wave->is3D(), false ) != true)
	{
		sprintf( buf, "LoadWaveFile failed: %s.\n", path );
		writeLog( buf );
		return;
	}

	if (wave->is3D())
	{
		hr = sound->QueryInterface( IID_IDirectSound3DBuffer, (void**)&sound3D ); 
		_ASSERTE( hr == S_OK );
	}

	MPBSoundBite* sb = new MPBSoundBite( sound, sound3D );
//	wave->setSoundBite( sb );

	for (int i=0; i<SOUNDBITE_COPIES; i++)
	{
		wave->addSoundBite( duplicateSoundBite( sb ) );
	}

	delete sb;
}

MPBSoundBite* MPBSoundManager::getSoundBite( const char* fileName )
{
	VECTOR_LOOP_CONST( m_waveVector, MPBWave* )
	{
		if (strcmp( (*itr)->getFileName(), fileName ) == 0)
		{
			return (*itr)->getSoundBite();
		}
	}}

	_ASSERTE( 0 );
	return NULL;
}

// velocity: meters per second
// position: meters
MPBSoundBite* MPBSoundManager::playSound(	const char* fileName, 
														const MPBVector& position,
														const MPBVector& velocity,
														LONG volume,	// -10000 - 0
														bool loop	)
{
	return playSound( getSoundBite( fileName ), position, velocity, volume, loop );
}

// velocity: meters per second
// position: meters
// SoundBite will get deleted when sound stops
// unless loop is true, in which case caller has
// to delete it.
MPBSoundBite* MPBSoundManager::playSound(	MPBSoundBite* sb, 
														const MPBVector& position,
														const MPBVector& velocity,
														LONG volume,	// -10000 - 0
														bool loop	)
{
	HRESULT hr;

//	_ASSERTE( volume > DSBVOLUME_MIN && volume < DSBVOLUME_MAX );
	if (volume > DSBVOLUME_MAX ) volume = DSBVOLUME_MAX;
	if (volume < DSBVOLUME_MIN ) volume = DSBVOLUME_MIN;

//	MPBSoundBite* duplicate = duplicateSoundBite( sb );
//	_ASSERTE( duplicate );

	sb->updatePosition( position, velocity );

	sb->m_soundBuffer->SetVolume( volume );

	if (loop) 
	{
		sb->m_soundBuffer->Play( 0, 0, DSBPLAY_LOOPING );
	}
	else
	{
		///////////////////////////////////////////////
		// setup notification for end of playback
	
/*		LPDIRECTSOUNDNOTIFY lpdsn; 
		hr = sb->m_soundBuffer->QueryInterface(	IID_IDirectSoundNotify, 
																		(LPVOID *)&lpdsn	);

		_ASSERTE( hr == S_OK );
		DSBPOSITIONNOTIFY dsbpn;
		sb->m_eventHandle = CreateEvent( NULL, FALSE, FALSE, NULL );
		dsbpn.dwOffset = DSBPN_OFFSETSTOP;
		dsbpn.hEventNotify = sb->m_eventHandle;
		lpdsn->SetNotificationPositions( 1, &dsbpn );
		m_playingSoundSet.insert( sb );
*/
		sb->m_soundBuffer->Play( 0, 0, 0 );
	}

	return sb;
}

MPBSoundBite* MPBSoundManager::duplicateSoundBite( const char* fileName )
{
	return duplicateSoundBite( getSoundBite( fileName ) );
}

MPBSoundBite* MPBSoundManager::duplicateSoundBite( MPBSoundBite* sb )
{
	HRESULT hr;

	MPBSoundBite* duplicate = NULL;

	LPDIRECTSOUNDBUFFER newBuffer = NULL;
	LPDIRECTSOUND3DBUFFER new3DBuffer = NULL;

	if (m_directSound->DuplicateSoundBuffer( 
			sb->m_soundBuffer, &newBuffer) == DS_OK)
	{
		if (sb->m_sound3DBuffer != NULL)
		{
			hr = newBuffer->QueryInterface( IID_IDirectSound3DBuffer, (void**)&new3DBuffer ); 
			_ASSERTE( SUCCEEDED( hr ) );
		}

		duplicate = new MPBSoundBite( newBuffer, new3DBuffer );
	}
	else
	{
		writeLog( "duplicateSoundBite failed.\n" );
	}

	return duplicate;
}

void MPBSoundManager::setListenerPosition( const MPBVector& position )
{
//	HRESULT hr;

	if (!m_listener) return;

	m_listener->SetPosition(	position.x / SOUND_POSITION_DIVISOR,
										position.y / SOUND_POSITION_DIVISOR,
										position.z / SOUND_POSITION_DIVISOR,
										DS3D_IMMEDIATE	);
}

void MPBSoundManager::setListenerOrientation(	const MPBVector& front,
																const MPBVector& top	)
{
//	HRESULT hr;

	if (!m_listener) return;

	m_listener->SetOrientation(	front.x,
											front.y,
											front.z,
											top.x,
											top.y,
											top.z,
											DS3D_IMMEDIATE	);
}

void MPBSoundManager::setListenerVelocity( const MPBVector& velocity )
{
//	HRESULT hr;

	if (!m_listener) return;

	m_listener->SetVelocity(	velocity.x / SOUND_POSITION_DIVISOR,
										velocity.y / SOUND_POSITION_DIVISOR,
										velocity.z / SOUND_POSITION_DIVISOR,
										DS3D_IMMEDIATE	);
}

/*
void MPBSoundManager::cleanupDuplicates()
{
	// cull stopped sounds (don't use SET_LOOP here)
	for (	set<MPBSoundBite*>::iterator itr = m_playingSoundSet.begin(); 
			itr != m_playingSoundSet.end(); )
	{
		MPBSoundBite* sb = *itr;

		if (WaitForSingleObject( sb->m_eventHandle, 0 ) == WAIT_OBJECT_0)
		{
         set<MPBSoundBite*>::iterator itr2 = itr;
         itr2++;
			m_playingSoundSet.erase( itr );
			itr = itr2;
			
			NULLIFY( sb );
		}
		else
		{
			++itr;
		}
	}
}

*/

void MPBSoundManager::collisionSound( Plane* plane, MPBVector velocity, double minKPH, MPBVector position, char* wave )
{
	double speed = velocity.getLength();
	if (speed)
	{
		velocity.normalize();
		MPBVector normal = plane->getNormal();
		double cosine = fabs( velocity.dotProduct( normal ) );	// 0 (skid) to 1 (direct landing)

		const double MIN_SPEED_KPH = 5;
		const double MIN_SPEED_MPS = minKPH * 1000 / 3600;
		const int LOWEST_VOLUME = 1000;
		LONG volume = speed * cosine - MIN_SPEED_MPS - LOWEST_VOLUME;
		if (volume > -LOWEST_VOLUME) 
			playSound( wave, position, velocity, volume, false );
	}
}

MPBSoundBite* MPBSoundManager::createStreamingSoundBite()
{
	LPDIRECTSOUNDBUFFER sound = NULL;
	LPDIRECTSOUND3DBUFFER sound3D = NULL;

	if (0)
	{
		unsigned char* waveFile = readFile( "sounds/rightfoot.wav" );

		int formatStart = 0x14;
		int dataStart = 0x24;

		WAVEFORMATEX* pWaveFormat = (WAVEFORMATEX*)(waveFile + formatStart);
		// find the text "data" after the waveformatex
		while (true)
		{
			if (	*(waveFile + dataStart) == 'd' &&
					*(waveFile + dataStart + 1) == 'a' &&
					*(waveFile + dataStart + 2) == 't' &&
					*(waveFile + dataStart + 3) == 'a'	)
			{
				break;
			}

			++dataStart;
			_ASSERTE( dataStart < 100 );
		}

		DWORD dataSize = *((DWORD*)(waveFile + dataStart + 4));
		unsigned char* lpbSoundData = waveFile + dataStart + 8;

		DSBUFFERDESC bufferDescription;
		memset( &bufferDescription, 0, sizeof(DSBUFFERDESC) );
		bufferDescription.dwSize = sizeof(DSBUFFERDESC);
//		if (_3D) bufferDescription.dwFlags = DSBCAPS_CTRL3D; 

		bufferDescription.dwFlags |= DSBCAPS_CTRLPOSITIONNOTIFY;
		bufferDescription.dwBufferBytes = dataSize; 
		bufferDescription.lpwfxFormat = pWaveFormat; 

		m_directSound->CreateSoundBuffer( &bufferDescription, &sound, NULL );

		LPVOID lpbuf1 = NULL;
		LPVOID lpbuf2 = NULL;
		DWORD dwsize1 = 0;
		DWORD dwsize2 = 0;
		DWORD dwOffset = 0;

		sound->Lock( dwOffset, dataSize, &lpbuf1, &dwsize1, &lpbuf2, &dwsize2, 0 );
		CopyMemory( lpbuf1, lpbSoundData, dwsize1 );
		if (lpbuf2) CopyMemory( lpbuf2, lpbSoundData + dwsize1, dwsize2 ); 
		sound->Unlock( lpbuf1, dwsize1, lpbuf2, dwsize2 );
		delete[] waveFile;
	}
	else
	{
		WAVEFORMATEX wfx;
		memset( &wfx, 0, sizeof(WAVEFORMATEX) );
		wfx.wFormatTag = WAVE_FORMAT_PCM;
		wfx.nChannels = 1;
		wfx.nSamplesPerSec = 8000;
		wfx.wBitsPerSample = 16;
		wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8.0;
		wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

		DSBUFFERDESC bufferDescription;
		memset( &bufferDescription, 0, sizeof(DSBUFFERDESC) );
		bufferDescription.dwSize = sizeof(DSBUFFERDESC);
		bufferDescription.dwFlags |= DSBCAPS_CTRLVOLUME;
		bufferDescription.dwBufferBytes = 3 * wfx.nAvgBytesPerSec; 
		bufferDescription.lpwfxFormat = &wfx; 

		if (m_directSound->CreateSoundBuffer( &bufferDescription, &sound, NULL ) != DS_OK)
		{
			writeLastError( "CreateSoundBuffer failed.\n" );
		}
	}

//	hr = sound->QueryInterface( IID_IDirectSound3DBuffer, (void**)&sound3D ); 
//	_ASSERTE( hr == S_OK );

	return new MPBSoundBite( sound, sound3D );
}






