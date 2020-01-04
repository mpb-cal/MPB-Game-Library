
#include "MPBLibrary.h"
#include "MPBMidi.h"


HMIDIOUT m_hmo = 0;


void mpbMidiInit()
{
	MMRESULT mmr;

	mmr = midiOutOpen( &m_hmo, MIDI_MAPPER, NULL, 0, 0 );
	if (mmr != MMSYSERR_NOERROR) return;
}

void mpbMidiClose()
{
	MMRESULT mmr;

	mmr = midiOutClose( m_hmo );
	if (mmr != MMSYSERR_NOERROR) return;
}

void mpbMidiMessage( BYTE status, BYTE data1, BYTE data2 )
{
	MMRESULT mmr;

	union {
		BYTE b[4];
		DWORD dw;
	} u;

	u.b[0] = status;
	u.b[1] = data1;
	u.b[2] = data2;
	u.b[3] = 0; 

	mmr = midiOutShortMsg( m_hmo, u.dw );
	if (mmr != MMSYSERR_NOERROR) return;
}

void mpbMidiNoteOff( BYTE channel, BYTE note, BYTE velocity )
{
	mpbMidiMessage( MN_NOTEOFF + channel, note, velocity );
}

void mpbMidiNoteOn( BYTE channel, BYTE note, BYTE velocity )
{
	mpbMidiMessage( MN_NOTEON + channel, note, velocity );
}

void mpbMidiPatchChange( BYTE channel, BYTE newProgram )
{
	mpbMidiMessage( MN_PATCHCHANGE + channel, newProgram, 0 );
}

