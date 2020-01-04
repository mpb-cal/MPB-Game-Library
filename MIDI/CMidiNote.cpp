/*
	CMidiNote.cpp
*/

#include <windows.h>
#include "CMidiNote.h"
#include "MidiConstant.h"

//musical construction definitions:
static const int major_chord[] = {0,4,3,5};
static const int minor_chord[] = {0,3,4,5};
static const int major_scale[] = {0,2,4,5,7,9,11};
static const int minor_scale[] = {0,2,3,5,7,8,10};
static const int major_pentatonic[] = {0,2,4,7,9};
static const int minor_pentatonic[] = {0,3,5,7,10};

int get_ticks(int note_value)
{
	int ticks;
	if(note_value == 0) return 0;
	if(note_value < 0)
	{
		ticks = 32 / abs(note_value);
		ticks += ticks / 2;
	}
	else ticks = 32 / note_value;
	return ticks;
}



//--------------------------------------------------------------------------
//CMidiNote class:


CMidiNote::CMidiNote( int iPitch, int iLength, int iChannel, int iVelocity )
{
	m_iPitch = iPitch;
	m_iLength = iLength;
	m_iChannel = iChannel;
	m_iVelocity = iVelocity;

	_ASSERTE( m_iPitch >= 0 && m_iPitch <= NOTE_TOPPITCH );
	_ASSERTE( m_iLength >= 0 );
	_ASSERTE( m_iChannel >= 0 && m_iChannel < 16 );
	_ASSERTE( m_iVelocity >= 0 && m_iVelocity < 256 );
}
/*
// return # of bytes written
int CMidiNote::Write( FILE* fp, int& riDelay )
{
	int bytesWritten = 0;

	if (m_iPitch == NOTE_REST)
	{
		riDelay += m_iLength;
		return 0;
	}

	// delay before starting note
	int iDelaySize;
	BYTE a_bDelay[ 2 ];

	if(riDelay < 128)
	{
		iDelaySize = 1;
		a_bDelay[ 0 ] = (BYTE)riDelay;
	}
	else
	{
		iDelaySize = 2;

		BYTE length0 = (BYTE)((riDelay % 0x100) & 0x7f);
		BYTE length1 = (BYTE)((riDelay / 0x100) << 1);
		length1 = (BYTE)( length1 | 0x80 + (((riDelay % 0x100) & 0x80) >> 7) );
		a_bDelay[ 0 ] = length1;
		a_bDelay[ 1 ] = length0;
	}
	fwrite( a_bDelay, iDelaySize, 1, fp );
	bytesWritten += iDelaySize;

	riDelay = 0;

	BYTE a_bNoteon[ 3 ] =
	{
		(BYTE)(MN_NOTEON + m_iChannel),
		(BYTE)m_iPitch,
		(BYTE)m_iVelocity
	};
	fwrite( a_bNoteon, 3, 1, fp );
	bytesWritten += 3;

	// delay before note ends
	if(m_iLength < 128)
	{
		iDelaySize = 1;
		a_bDelay[ 0 ] = (BYTE)m_iLength;
	}
	else
	{
		iDelaySize = 2;

		BYTE length0 = (BYTE)((m_iLength % 0x100) & 0x7f);
		BYTE length1 = (BYTE)((m_iLength / 0x100) << 1);
		length1 = (BYTE)( length1 | 0x80 + (((m_iLength % 0x100) & 0x80) >> 7) );
		a_bDelay[ 0 ] = length1;
		a_bDelay[ 1 ] = length0;
	}
	fwrite( a_bDelay, iDelaySize, 1, fp );
	bytesWritten += iDelaySize;

	//stop note:
	BYTE a_bNoteoff[ 3 ] =
	{
		(BYTE)(MN_NOTEOFF + m_iChannel),
		(BYTE)m_iPitch,
		(BYTE)m_iVelocity
	};
	fwrite( a_bNoteoff, 3, 1, fp );
	bytesWritten += 3;

	return bytesWritten;
}
*/
// return # of bytes written
void CMidiNote::Write( vector<BYTE>& buffer, int& riDelay )
{
	if (m_iPitch == NOTE_REST)
	{
		riDelay += m_iLength;
		return;
	}

	// delay before starting note
//	int iDelaySize;
//	BYTE a_bDelay[ 2 ];

	if(riDelay < 128)
	{
//		iDelaySize = 1;
//		a_bDelay[ 0 ] = (BYTE)riDelay;
		buffer.push_back( (BYTE)riDelay );
	}
	else
	{
//		iDelaySize = 2;

		BYTE length0 = (BYTE)((riDelay % 0x100) & 0x7f);
		BYTE length1 = (BYTE)((riDelay / 0x100) << 1);
		length1 = (BYTE)( length1 | 0x80 + (((riDelay % 0x100) & 0x80) >> 7) );
//		a_bDelay[ 0 ] = length1;
//		a_bDelay[ 1 ] = length0;
		buffer.push_back( length1 );
		buffer.push_back( length0 );
	}
//	fwrite( a_bDelay, iDelaySize, 1, fp );

	riDelay = 0;

	buffer.push_back( (BYTE)(MN_NOTEON + m_iChannel) );
	buffer.push_back( (BYTE)m_iPitch );
	buffer.push_back( (BYTE)m_iVelocity );
//	BYTE a_bNoteon[ 3 ] =
//	{
//		(BYTE)(MN_NOTEON + m_iChannel),
//		(BYTE)m_iPitch,
//		(BYTE)m_iVelocity
//	};
//	fwrite( a_bNoteon, 3, 1, fp );

	// delay before note ends
	if(m_iLength < 128)
	{
//		iDelaySize = 1;
//		a_bDelay[ 0 ] = (BYTE)m_iLength;
		buffer.push_back( (BYTE)m_iLength );
	}
	else
	{
//		iDelaySize = 2;

		BYTE length0 = (BYTE)((m_iLength % 0x100) & 0x7f);
		BYTE length1 = (BYTE)((m_iLength / 0x100) << 1);
		length1 = (BYTE)( length1 | 0x80 + (((m_iLength % 0x100) & 0x80) >> 7) );
//		a_bDelay[ 0 ] = length1;
//		a_bDelay[ 1 ] = length0;
		buffer.push_back( length1 );
		buffer.push_back( length0 );
	}
//	fwrite( a_bDelay, iDelaySize, 1, fp );

	buffer.push_back( (BYTE)(MN_NOTEOFF + m_iChannel) );
	buffer.push_back( (BYTE)m_iPitch );
	buffer.push_back( (BYTE)m_iVelocity );
	//stop note:
//	BYTE a_bNoteoff[ 3 ] =
//	{
//		(BYTE)(MN_NOTEOFF + m_iChannel),
//		(BYTE)m_iPitch,
//		(BYTE)m_iVelocity
//	};
//	fwrite( a_bNoteoff, 3, 1, fp );
}
