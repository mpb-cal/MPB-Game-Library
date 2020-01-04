
#include "MPBBall.h"
#include "MPBCountdown.h"
#include "MPBCollision.h"
#include "MPBSoundManager.h"
#include "mmgr.h"


#define PLUS_MINUS	(rand() % 2 == 0 ? -1 : 1)

MPBBall::MPBBall( MPBFLOAT radius ):
	MPBPhysicsObject( POT_SPHERE ),
	m_countdownAlive( NULL )
{
	setRadius( radius );
	setGravity( 0 );
	setElasticity( 1.0 );
	setRollingResistance( 0 );

	m_countdownAlive = new MPBCountdown( 5000, false );
	m_countdownAlive->start();

	setTexture( "solidWhite" );
}

MPBBall::~MPBBall()
{
	delete m_countdownAlive;
}

void MPBBall::updateFrame()
{
	MPBPhysicsObject::updateFrame();

	if (m_countdownAlive->isDone())
	{
		die();
	}
}


void MPBBall::handleCollision1( Collision* c )
{
}

void MPBBall::handleCollision2( Collision* c )
{
	MPBSoundManager::playSound( "drip.wav", m_position, m_velocity, 1 );
}

