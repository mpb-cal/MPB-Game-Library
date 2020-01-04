
#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#include "OpenAL/alu.h"
#include "OpenAL/alut.h"

#include "MPBSound.h"
#include "MPBVector.h"
#include "mmgr.h"


MPBSound::MPBSound( ALuint buffer, char* filename ):
	m_buffer( buffer )
{
	strcpy( m_filename, filename );
}

MPBSound::~MPBSound()
{
	alDeleteBuffers( 1, &m_buffer );
}


