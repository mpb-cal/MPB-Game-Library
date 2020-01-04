
#include "MPBCountdown.h"
#include "MPBGlobal.h"
#include "MPBGameEntities.h"
#include "mmgr.h"

// all times are in milliseconds

MPBCountdown::MPBCountdown( int length, bool shouldRandomize ) : 
	m_elapsedTime( length ), 
	m_length( length ),
	m_shouldRandomize( shouldRandomize )
{ 
	MPBGameEntities::countdowns.push_back( this ); 
}

MPBCountdown::~MPBCountdown() 
{
	if (MPBGameEntities::countdowns.size())
		MPBGameEntities::countdowns.remove( this );
}

// called once per frame from game loop
void MPBCountdown::update( int ms ) 
{ 
	if (m_elapsedTime > 0) m_elapsedTime -= ms; 
}

void MPBCountdown::start() 
{ 
	m_elapsedTime = m_length; 
}

void MPBCountdown::start( int length ) 
{ 
	m_length = length; 
	m_elapsedTime = m_length; 
}

