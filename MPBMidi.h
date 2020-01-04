#ifndef __MPBMIDI_H__
#define __MPBMIDI_H__


// midi events
const BYTE MN_NOTEOFF	=			0x80;
const BYTE MN_NOTEON	=			0x90;
const BYTE MN_KEYAFTERTOUCH	=		0xA0;
const BYTE MN_CONTROLCHANGE	=		0xB0;
const BYTE MN_PATCHCHANGE =			0xC0;
const BYTE MN_CHANNELAFTERTOUCH	=	0xD0;
const BYTE MN_PITCHWHEELCHANGE	=	0xE0;
const BYTE MN_METAEVENT	=			0xFF;




void mpbMidiInit();
void mpbMidiClose();
void mpbMidiMessage( BYTE status, BYTE data1, BYTE data2 );
void mpbMidiNoteOff( BYTE channel, BYTE note, BYTE velocity );
void mpbMidiNoteOn( BYTE channel, BYTE note, BYTE velocity );
void mpbMidiPatchChange( BYTE channel, BYTE newProgram );



#endif
