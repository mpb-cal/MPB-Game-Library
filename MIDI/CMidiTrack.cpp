/*
	CMidiTrack.cpp
*/

#include <windows.h>
#include <fstream.h>
#include "CMidiTrack.h"
#include "MidiConstant.h"


CMidiTrack::CMidiTrack() :
	m_ppNotes( NULL ),
	m_iNumberNotes( 0 ),
	m_iPatch( 0 ),
	m_iPan( 0 ),
	m_iChannel( 0 ),
	m_iVelocity( 0 ),
	m_iKeySignature( 0 ),
	m_iMode( 0 ),
	m_iTimesigN( 0 ),
	m_iTimesigD( 0 ),
	m_iLengthInBytes( 0 ),
	m_pucHeader( NULL ),
	m_pucFooter( NULL )
{
	m_pucHeader = new unsigned char[ MT_HEADERSIZE ];
	m_pucFooter = new unsigned char[ MT_FOOTERSIZE ];
}

CMidiTrack::CMidiTrack(	int iSize, 
						int iPatch, 
						int iPan, 
						int iChannel, 
						int iVelocity,
						int iKeySignature, 
						int iMode, 
						int iTimesigN, 
						int iTimesigD	) :
	m_ppNotes( NULL ),
	m_iNumberNotes( iSize ),
	m_iPatch( iPatch ),
	m_iPan( iPan ),
	m_iChannel( iChannel - 1 ),
	m_iVelocity( iVelocity ),
	m_iKeySignature( iKeySignature ),
	m_iMode( iMode ),
	m_iTimesigN( iTimesigN ),
	m_iTimesigD( iTimesigD ),
	m_iLengthInBytes( 0 ),
	m_pucHeader( NULL ),
	m_pucFooter( NULL )
{
	//the 3 is for the patch change message:
	m_ppNotes = new CMidiNote*[ iSize ];
	for (int i = 0; i < iSize; i++)
	{
		m_ppNotes[ i ] = NULL;
	}

	m_pucHeader = new unsigned char[ MT_HEADERSIZE ];
	m_pucHeader[0] = 'M';
	m_pucHeader[1] = 'T';
	m_pucHeader[2] = 'r';
	m_pucHeader[3] = 'k';
	m_pucHeader[4] = 0;
	m_pucHeader[5] = 0;
	m_pucHeader[6] = 0;
	m_pucHeader[7] = 0;

	m_pucFooter = new unsigned char[ MT_FOOTERSIZE ];
	m_pucFooter[0] = 0;
	m_pucFooter[1] = 255;
	m_pucFooter[2] = 47;
	m_pucFooter[3] = 0;
}

CMidiTrack::~CMidiTrack()
{
	if (m_ppNotes)
	{
		for (int i = 0; i < m_iNumberNotes; i++)
		{
			if (m_ppNotes[ i ])
			{
				delete m_ppNotes[ i ];
				m_ppNotes[ i ] = NULL;
			}
		}
		m_ppNotes = NULL;
	}

	if (m_pucFooter) delete[] m_pucFooter;
	if (m_pucHeader) delete[] m_pucHeader;
}

void CMidiTrack::edit_note( int nNumber, int nPitch, int nLength )
{
	DWORD dwValue = nLength;// * NOTE2MN_FACTOR;

	m_ppNotes[ nNumber ] = new CMidiNote( nPitch, dwValue, m_iChannel, m_iVelocity );
}

void CMidiTrack::Write( vector<BYTE>& buffer )
{
	int i;
	int bytesWritten = 0;

	// remember start position in stream
//	long startpos = ftell( fp );
	int startpos = buffer.size();

	// write header
	for (i=0; i<MT_HEADERSIZE; i++) buffer.push_back( m_pucHeader[i] );
//	fwrite( m_pucHeader, MT_HEADERSIZE, 1, fp );

	//set patch:
	buffer.push_back( (BYTE)0 );
	buffer.push_back( (BYTE)(MN_PATCHCHANGE + m_iChannel) );
	buffer.push_back( (BYTE)m_iPatch );
//	BYTE a_bPatchEvent[] = 
//	{ 
//		(BYTE)0, 
//		(BYTE)(MN_PATCHCHANGE + m_iChannel), 
//		(BYTE)m_iPatch
//	};

//	fwrite( a_bPatchEvent, 3, 1, fp );

	//set pan:
	buffer.push_back( (BYTE)0 );
	buffer.push_back( (BYTE)(MN_CONTROLCHANGE + m_iChannel) );
	buffer.push_back( (BYTE)10 );
	buffer.push_back( (BYTE)m_iPan );
//	BYTE a_bPanEvent[] = 
//	{ 
//		(BYTE)0, 
//		(BYTE)(MN_CONTROLCHANGE + m_iChannel), 
//		(BYTE)10,
//		(BYTE)m_iPan
//	};

//	fwrite( a_bPanEvent, 4, 1, fp );

	// write notes
	int iDelay = 0;
	for (i = 0; i < m_iNumberNotes; i++)
	{
		m_ppNotes[ i ]->Write( buffer, iDelay );
	}

	// write footer
	for (i=0; i<MT_FOOTERSIZE; i++) buffer.push_back( m_pucFooter[i] );
//	fwrite( m_pucFooter, MT_FOOTERSIZE, 1, fp );

	// calculate track body size in bytes
//	long endpos = ftell( fp );
	int endpos = buffer.size();
	m_iLengthInBytes = (endpos - startpos) - MT_HEADERSIZE;
	m_pucHeader[6] = (BYTE)(m_iLengthInBytes / 0x100);
	m_pucHeader[7] = (BYTE)(m_iLengthInBytes % 0x100);

	// rewrite header with track size
//	fseek( fp, startpos, SEEK_SET );
//	fwrite( m_pucHeader, MT_HEADERSIZE, 1, fp );
	for (i=0; i<MT_HEADERSIZE; i++) buffer[startpos+i] = m_pucHeader[i];

	// move to end of stream
//	fseek( fp, endpos, SEEK_SET );
}
/*
BOOL CMidiTrack::Read( PBYTE pbData, int& riIndex )
{
//	fin.read( m_pucHeader, MT_HEADERSIZE );
	::CopyMemory( m_pucHeader, pbData + riIndex, MT_HEADERSIZE );
	riIndex += MT_HEADERSIZE;

	// check for magic
	if (m_pucHeader[ 0 ] != 'M') return FALSE;
	if (m_pucHeader[ 1 ] != 'T') return FALSE;
	if (m_pucHeader[ 2 ] != 'r') return FALSE;
	if (m_pucHeader[ 3 ] != 'k') return FALSE;

	// check for track size in bytes
	m_iLengthInBytes = (int)ntohl( *(DWORD*)(&m_pucHeader[ 4 ]) );

	// read track data into buffer
	unsigned char* pucData = new unsigned char[ m_iLengthInBytes ];
//	fin.read( pucData, m_iLengthInBytes );
	::CopyMemory( pucData, pbData + riIndex, m_iLengthInBytes );
	riIndex += m_iLengthInBytes;
	
	// find events
	int i = 0;
	while (i < m_iLengthInBytes)
	{
		// find delta time
		DWORD dwDeltaTime = pucData[ i ];
		while (pucData[ i ] & 0x80)
		{
			i++;
		}

		// find if meta event
		if (pucData[ i ] == MN_METAEVENT)
		{
			unsigned char ucCommand = pucData[ ++i ];
			unsigned char ucSize = pucData[ ++i ];

			switch (ucCommand)
			{
				default :
				{
					i += ucSize;
				}
			}
		}
		else
		{
			unsigned char ucEvent = pucData[ i ] & 0xF0;
			unsigned char ucChannel = pucData[ i ] & 0x0F;

			if (ucEvent == MN_PATCHCHANGE || ucEvent == MN_CHANNELAFTERTOUCH)
			{
				i++;
			}
			else
			{
				i++;
				i++;
			}
		}

		i++;
	}

	return TRUE;
}
*/


