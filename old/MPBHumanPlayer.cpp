
#include "MPBHumanPlayer.h"
#include "MPBHumanoid.h"
#include "MPBMissile.h"
#include "MPBBullet.h"
#include "MPBBall.h"
#include "MPBCrate.h"
#include "MPBSoundManager.h"
#include "MPBUsableObject.h"
#include "mmgr.h"

const MPBFLOAT MISSILE_RADIUS = .02f;
const MPBFLOAT BULLET_RADIUS = .01f;


MPBHumanPlayer::MPBHumanPlayer( MPBVector radiusVector ):
	MPBHumanoid( radiusVector )
{
	m_countdownMissile = new MPBCountdown( 500, true );
}

MPBHumanPlayer::~MPBHumanPlayer()
{
	NULLIFY( m_countdownMissile );
}

void MPBHumanPlayer::fireMissile()
{
	if (m_countdownMissile == NULL || m_countdownMissile->isDone())
	{
		m_countdownMissile->start();

		const bool USE_BULLET = false;
		const bool USE_BALL = false;
		const bool USE_CRATE = true;

		MPBPhysicsObject* missile;

		if (USE_BULLET)
		{
			missile = new MPBBullet( BULLET_RADIUS );
			MPBSoundManager::playSound( "Gun1.wav", getPosition(), getVelocity(), 1 );
		}
		else if (USE_BALL)
		{
			missile = new MPBBall( .5 );
		}
		else if (USE_CRATE)
		{
			missile = new MPBCrate( .5 );
		}
		else
		{
			missile = new MPBMissile( MISSILE_RADIUS );
		}

		missile->setPosition(	MPBVector(	getPosition().x, 
														getPosition().y + getRadius() * .8f,
														getPosition().z ) +
															getAxisVector( DIR_FORWARDS ) * getRadius() );

		missile->setPrevPosition(	MPBVector(	getPrevPosition().x, 
															getPrevPosition().y + getRadius() * .8f,
															getPrevPosition().z ) +
																getAxisVector( DIR_FORWARDS ) * getRadius() );

		missile->setVelocity( getVelocity() );

		MPBQuaternion q;
		q.fromEuler( getLookRotation() );
		missile->setOrientationQuaternion( q );

		if (USE_BULLET)
		{
			missile->applyImpulse( missile->getAxisVector( DIR_FORWARDS ) * missile->getRadius() * 1000000 );
		}
		else if (USE_BALL)
		{
			missile->applyImpulse( missile->getAxisVector( DIR_FORWARDS ) * missile->getRadius() * 10000 );
		}
		else if (USE_CRATE)
		{
			missile->applyImpulse( missile->getAxisVector( DIR_FORWARDS ) * missile->getRadius() * 10000 );
		}
		else
		{
			missile->applyImpulse( missile->getAxisVector( DIR_FORWARDS ) * missile->getRadius() * 100000 );

			((MPBMissile*)missile)->startEmittingParticles(	-1,
																			false, 
																			false, 
																			false,
																			.1f, 
																			50,
																			50,
																			10,
																			MPBVector(),
																			MPBVector( .5f, 0, 0 ), 
																			.5f,
																			false	);
		}

		MPBPhysics::addObject( missile );

		missile->updateFrame();
	}
}

void MPBHumanPlayer::takeUsableObject( MPBUsableObject* o )
{
	m_usableObjects.push_back( o );
}

bool MPBHumanPlayer::hasUsableObject( MPBUsableObject* o )
{
	MPB_LIST_LOOP( m_usableObjects, uo, MPBUsableObject* )
	{
		if (uo == o) return true;
	}

	return false;
}










