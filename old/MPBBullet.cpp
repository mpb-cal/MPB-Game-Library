
#include "MPBBullet.h"
#include "MPBCountdown.h"
#include "MPBCollision.h"
#include "MPBSoundManager.h"
#include "mmgr.h"


#define PLUS_MINUS	(rand() % 2 == 0 ? -1 : 1)

MPBBullet::MPBBullet( MPBFLOAT radius ):
	MPBPhysicsObject( POT_SPHERE ),
	m_countdownAlive( NULL )
{
	setRadius( radius );
	setGravity( m_gravity / 4.0 );
	setElasticity( 0 );

	m_countdownAlive = new MPBCountdown( 5000, false );
	m_countdownAlive->start();

	setTexture( "solidWhite" );

	setStreamingSound( new MPBStreamingSound2 );
	((MPBStreamingSound2*)getStreamingSound())->m_amplitude = .05f;
}

MPBBullet::~MPBBullet()
{
	delete m_countdownAlive;
}

void MPBBullet::updateFrame()
{
	MPBPhysicsObject::updateFrame();

	if (m_countdownAlive->isDone())
	{
		die();
	}

	MPBFLOAT speed = getVelocity().getLength();
	if (getStreamingSound()) 
		((MPBStreamingSound2*)getStreamingSound())->m_highpassFrequency = 500 + speed * 100.0;
}


void MPBBullet::handleCollision1( Collision* c )
{
	if (c->m_targetObject)
	{
		MPBVector force( getVelocity() * 1000.0 );

		c->m_targetObject->applyForce( force );
		c->m_targetObject->addDamage( getVelocity().getLength() * 200.0 );
	}
}

void MPBBullet::handleCollision2( Collision* c )
{
	die();

//	MPBSoundManager::playSound( "Bexpl2.wav", m_position, m_velocity, 1 );


	MPBParticleSystem* ps;

	if (getVelocity().x || getVelocity().y || getVelocity().z)
	{
		// fast explosion
		ps = new MPBParticleSystem( 20 );
		ps->setPosition( c->m_newDestination );
	//	ps->inheritVelocity( this );
		ps->setAlive( true );
//		ps->setImmovable( true );
		ps->setCollidable( false );
		ps->setCollisionTest( false );
		ps->setPlaneCollisionTest( false );
		ps->setCastsShadow( false );
		MPBPhysics::addObject( ps );
		ps->startEmittingParticlesCone(	-1, 
													true, 
													true, 
													false,
													5, 
													300,
													100,
													200,
													MPBVector(),
													MPBVector( 1, .9, .5 ), 
													.06,
													false,
													false,
													getVelocity(),
													10	);
	}
}

