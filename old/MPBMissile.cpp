
#include "MPBMissile.h"
#include "MPBCountdown.h"
#include "MPBCollision.h"
#include "MPBSoundManager.h"
#include "mmgr.h"


#define PLUS_MINUS	(rand() % 2 == 0 ? -1 : 1)

MPBMissile::MPBMissile( MPBFLOAT radius ):
	MPBParticleSystem( 100 ),
	m_countdownAlive( NULL ),
	m_grounded( false ),
	m_exploded( false )
{
	setRadius( radius );
	setGravity( m_gravity / 4.0 );
	setElasticity( 0 );
	setDrawSolid( true );

	m_countdownAlive = new MPBCountdown( 3000, false );
	m_countdownAlive->start();

//	setSound( "buzz.wav" );
	setTexture( "solidWhite" );

	setStreamingSound( new MPBStreamingSound3 );
	((MPBStreamingSound3*)getStreamingSound())->m_amplitude = .1f;
	((MPBStreamingSound3*)getStreamingSound())->m_resonance = 16;
	((MPBStreamingSound3*)getStreamingSound())->m_lowpassFrequency = 1200;
}

MPBMissile::~MPBMissile()
{
	delete m_countdownAlive;
}

void MPBMissile::updateFrame()
{
	MPBParticleSystem::updateFrame();

	if (m_countdownAlive->isDone())
	{
		explode( true );
	}

//	MPBFLOAT speed = getVelocity().getLength();
//	if (getStreamingSound()) 
//		((MPBStreamingSound2*)getStreamingSound())->m_highpassFrequency = 500 + speed * 100.0;
}


void MPBMissile::handleCollision1( Collision* c )
{
	explode( false );

	if (c->m_targetObject)
	{
//		MPBVector force( getVelocity() * 10000.0 );

//		c->m_targetObject->applyForce( force );
		c->m_targetObject->addDamage( getVelocity().getLength() * 2000.0 );
	}
}

void MPBMissile::explode( bool causedByTimeout )
{
	if (m_exploded) return;
	m_exploded = true;

	setImmovable( true );
	setCollidable( false );

	//////////////////////////////////////
	// hit objects with a force

/*	VECTOR_LOOP( MPBPhysics::objectVector, MPBPhysicsObject* )
	{
		MPBPhysicsObject* obj = *itr;
		if (obj == this) continue;
		if (!obj->getCollidable()) continue;

		const MPBFLOAT lineOfSightRadius = .1;

		MPBVector force( obj->getPosition() - getPosition() );
		MPBFLOAT distance = force.getLength();
		if (distance < 10)
		{
			if (pathIsClear(
					obj->getPosition(),
					lineOfSightRadius,
					true,
					obj
				))
			{
				MPBFLOAT damage = 3000000.0 / (distance * distance);
				force.setLength( damage );
				obj->applyForce( force );
				obj->addDamage( damage );
			}
		}
	}}
*/

	stopEmittingParticles();
	setDieWhenDoneEmitting( true );
	stopSound();
	killStreamingSound();
	setDrawSolid( false );

	MPBSoundManager::playSound( "Bexpl2.wav", m_position, m_velocity, 1 );

	// explosion
	MPBParticleSystem* ps = new MPBParticleSystem( 20 );
	ps->inheritVelocity( this );
	ps->setAlive( true );
	ps->setCollidable( false );
	ps->setCollisionTest( false );
	ps->setPlaneCollisionTest( false );
	ps->setCastsShadow( false );
	MPBPhysics::addObject( ps );
	ps->startEmittingParticles(	-1, 
											true, 
											true, 
											causedByTimeout,
											2, 
											100,
											100,
											100,
											MPBVector(),
											MPBVector( 1, .7, 0 ), 
											.4,
											false	);

	// fast explosion
	ps = new MPBParticleSystem( 20 );
	ps->inheritVelocity( this );
	ps->setAlive( true );
	ps->setCollidable( false );
	ps->setCollisionTest( false );
	ps->setPlaneCollisionTest( false );
	ps->setCastsShadow( false );
	MPBPhysics::addObject( ps );
	ps->startEmittingParticles(	-1, 
											true, 
											true, 
											causedByTimeout,
											4, 
											200,
											200,
											100,
											MPBVector(),
											MPBVector( 1, .9, .5 ), 
											.4,
											false	);

	// smoke
	ps = new MPBParticleSystem( 20 );
	ps->inheritVelocity( this );
	ps->setAlive( true );
	ps->setCollidable( false );
	ps->setCollisionTest( false );
	ps->setPlaneCollisionTest( false );
	ps->setCastsShadow( false );
	MPBPhysics::addObject( ps );
	ps->startEmittingParticles(	-1, 
											true, 
											true, 
											causedByTimeout,
											.4, 
											30,
											40,
											0,
											MPBVector( 0, .01, 0 ),
											MPBVector( .8, .5, 0 ), 
											.5,
											false	);
}

