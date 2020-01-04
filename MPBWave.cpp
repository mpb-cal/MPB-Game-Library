
#include "MPBLibrary.h"


HWAVEOUT m_hwo = 0;


void mpbWaveInit()
{
	MMRESULT mmR;

	WORD nChannels = 1;
	DWORD nSamplesPerSec = 8000;
	WORD wBitsPerSample = 8;	// 8 or 16
	WORD nBlockAlign = nChannels * wBitsPerSample / 8;
	DWORD nAvgBytesPerSec = nSamplesPerSec * nBlockAlign;

	WAVEFORMATEX wfx = 
	{
		WAVE_FORMAT_PCM,
		nChannels,
		nSamplesPerSec,
		nAvgBytesPerSec,
		nBlockAlign,
		wBitsPerSample,
		0
	};

	mmR = waveOutOpen( &m_hwo, WAVE_MAPPER, &wfx, 0, 0, 0 );

//	if (mmR == MMSYSERR_NOERROR)
}

void mpbWavePlay( char* data, DWORD dataSize )
{
	MMRESULT mmR;

	WAVEHDR wh = 
	{ 
		data,
		dataSize,
		0,
		0,
		0,
		1,
		0,
		0
	};

	mmR = waveOutPrepareHeader( m_hwo, &wh, sizeof( WAVEHDR ) ); 
	if (mmR != MMSYSERR_NOERROR) return;

	mmR = waveOutWrite( m_hwo, &wh, sizeof( WAVEHDR ) );
	if (mmR != MMSYSERR_NOERROR) return;

	mmR = waveOutUnprepareHeader( m_hwo, &wh, sizeof( WAVEHDR ) ); 
	if (mmR != MMSYSERR_NOERROR) return;
}

