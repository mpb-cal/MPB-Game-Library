
#include "MPBCrate.h"
#include "MPBCountdown.h"
#include "MPBCollision.h"
#include "MPBSoundManager.h"
#include "MPBShapeBox.h"
#include "mmgr.h"


#define PLUS_MINUS	(rand() % 2 == 0 ? -1 : 1)

MPBCrate::MPBCrate( MPBFLOAT radius ):
	MPBPhysicsObject( POT_BODY ),
	m_countdownAlive( NULL )
{
	setRadius( radius );
	setGravity( 1 );
	setElasticity( 1 );
	setRollingResistance( 0 );

	m_countdownAlive = new MPBCountdown( 5000, false );
	m_countdownAlive->start();

	setTexture( "solidWhite" );

//	addShape( new MPBShapeBox( radius * 2, radius * 2, radius * 2, "solidWhite", false ) );
}

MPBCrate::~MPBCrate()
{
	delete m_countdownAlive;
}

void MPBCrate::updateFrame()
{
	MPBPhysicsObject::updateFrame();

	if (m_countdownAlive->isDone())
	{
		die();
	}
}


void MPBCrate::handleCollision1( Collision* c )
{
}

void MPBCrate::handleCollision2( Collision* c )
{
//	MPBSoundManager::playSound( "drip.wav", m_position, m_velocity, 1 );
}

