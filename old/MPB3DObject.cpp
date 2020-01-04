
#include "MPB3DObject.h"
#include "MPBPhysics.h"
#include "MPBFrameCounter.h"
#include "MPBSoundManager.h"
#include "MPBTexturedPlane.h"
#include "MPBOpenGL.h"
#include "MPBStreamingSound.h"
#include "MPBGameEntities.h"
#include "mmgr.h"

const bool USE_ODE = true;

MPB3DObject::MPB3DObject():
	m_red( 255 ),
	m_blue( 255 ),
	m_green( 255 ),
	m_mass( 100 ),
	m_collided( false ),
	m_gravity( MPBPhysics::gravity ),
	m_planeCollisionSound( NULL ),
	m_objectCollisionSound( NULL ),
	m_ininiteMass( false ),
	m_networkId( 0 ),
	m_sound( 0 ),
	m_streamingSound( NULL )
{
	if (USE_ODE) m_bodyID = dBodyCreate( MPBGameEntities::worldID );

//	m_red = 255.0 * RANDOM_FRACTION;
//	m_green = 255.0 * RANDOM_FRACTION;
//	m_blue = 255.0 * RANDOM_FRACTION;
}

MPB3DObject::~MPB3DObject()
{
	if (USE_ODE) dBodyDestroy( m_bodyID );

	MPB_LIST_DELETE( m_shapes, MPBShape* );

	stopSound();
	killStreamingSound();
}

void MPB3DObject::setMass( MPBFLOAT d )
{ 
	m_mass = d;
}

void MPB3DObject::updateFrame()
{
	if (USE_ODE)
	{
		const dReal *pos = dBodyGetPosition( m_bodyID );
		m_position.set( pos[0], pos[1], pos[2] );
	}

	if (m_sound)
	{
		float alPos[3];
		alPos[0] = (float)m_position.x / MPBSoundManager::soundPositionDivisor;
		alPos[1] = (float)m_position.y / MPBSoundManager::soundPositionDivisor;
		alPos[2] = (float)m_position.z / MPBSoundManager::soundPositionDivisor;
		alSourcefv( m_sound, AL_POSITION, alPos );
		MPB_ASSERT( alGetError() == AL_NO_ERROR );

		float alVel[3];
		alVel[0] = (float)m_velocity.x / MPBSoundManager::soundPositionDivisor;
		alVel[1] = (float)m_velocity.y / MPBSoundManager::soundPositionDivisor;
		alVel[2] = (float)m_velocity.z / MPBSoundManager::soundPositionDivisor;
		alSourcefv( m_sound, AL_VELOCITY, alVel );
		MPB_ASSERT( alGetError() == AL_NO_ERROR );
	}

	if (m_streamingSound)
	{
		m_streamingSound->updatePosition( m_position, m_velocity );
		m_streamingSound->start();
	}
}

void MPB3DObject::addShape( MPBShape* s ) 
{ 
	s->setFrustumCull( false ); 
	m_shapes.push_back( s ); 

	MPB_LIST_LOOP( s->getPlanes(), p, MPBTexturedPlane* )
	{
		if (MPBOpenGL::getPointLight( 0 ))
		{
			p->addTexture( "generatedLightmap" );
		}
	}
}

void MPB3DObject::inheritVelocity( MPB3DObject* o )
{
	setPosition( o->getPosition() );
	setPrevPosition( o->getPrevPosition() );
	setVelocity( o->getVelocity() );
}

void MPB3DObject::setPosition( MPBVector pos ) 
{ 
	if (USE_ODE)
	{
		dBodySetPosition( m_bodyID, pos.x, pos.y, pos.z );
		dBodySetLinearVel( m_bodyID, 0, 0, 0 );
		dBodySetAngularVel( m_bodyID, 0, 0, 0 );
	}

	m_position = pos;
	m_prevPosition = pos; 
}

void MPB3DObject::setPrevPosition( MPBVector pos ) 
{ 
	m_prevPosition = pos; 
}

void MPB3DObject::changePosition( MPBVector v )
{
	if (!m_ininiteMass) m_position = m_position + v;
}

void MPB3DObject::forceStop()
{
	m_velocity.reset();
}

// force is Newtons per second
void MPB3DObject::applyForce( const MPBVector& force )
{
	if (USE_ODE) dBodyAddForce( m_bodyID, force.x, force.y, force.z );
	else m_acceleration = m_acceleration + (force * MPBFrameCounter::secondsElapsed / m_mass);
}

// force is Newtons
void MPB3DObject::applyImpulse( const MPBVector& force )
{
	if (USE_ODE) 
	{
		dVector3 f;
		dWorldImpulseToForce( MPBGameEntities::worldID, MPBFrameCounter::secondsElapsed, force.x, force.y, force.z, f );
		dBodyAddForce( m_bodyID, f[0], f[1], f[2] );
	}
	else m_acceleration = m_acceleration + (force / m_mass);
}

MPBVector MPB3DObject::getMomentum() const
{
	return m_velocity * m_mass;
}

MPBVector MPB3DObject::getLateralVelocity() const
{
	MPBVector lateralVelocity;
	MPBVector xzVelocity( m_velocity.x, 0, m_velocity.z );

	if (xzVelocity.getLength() != 0)
	{
		xzVelocity.normalize();

		// forwardslip angle = difference between velocity and right arm
		MPBVector rightArm = getAxisVector( DIR_RIGHT );
		MPBFLOAT forwardslipAngleCosine = rightArm.dotProduct( xzVelocity );
		
		MPBFLOAT lateralSpeed = m_velocity.getLength() * forwardslipAngleCosine;
		lateralVelocity = rightArm * lateralSpeed;
	}

	return lateralVelocity;
}

MPBVector MPB3DObject::getLongitudinalVelocity() const
{
	MPBVector longitudinalVelocity;
	MPBVector xzVelocity( m_velocity.x, 0, m_velocity.z );

	if (xzVelocity.getLength() != 0)
	{
		xzVelocity.normalize();

		// forwardslip angle = difference between velocity and nose
		MPBVector nose = getAxisVector( DIR_FORWARDS );
		MPBFLOAT forwardslipAngleCosine = nose.dotProduct( xzVelocity );
		
		MPBFLOAT longitudinalSpeed = m_velocity.getLength() * forwardslipAngleCosine;
		longitudinalVelocity = nose * longitudinalSpeed;
	}

	return longitudinalVelocity;
}


bool MPB3DObject::pointIsPointedTo( const MPBVector& point, DirectionType moveType, MPBFLOAT thresholdRadians ) const
{
//	const MPBFLOAT THRESHOLD = .1f;
	if (point == getPosition()) return true;
	MPBVector vectorToPoint( point - getPosition() );
	vectorToPoint.normalize();
	if (getAxisVector( moveType ).dotProduct( vectorToPoint ) > thresholdRadians) return true;
	return false;
}

bool MPB3DObject::pointIsInFront( const MPBVector& point, MPBFLOAT thresholdRadians ) const 
{ return pointIsPointedTo( point, DIR_FORWARDS, thresholdRadians ); }

bool MPB3DObject::pointIsBehind( const MPBVector& point, MPBFLOAT thresholdRadians ) const 
{ return pointIsPointedTo( point, DIR_BACKWARDS, thresholdRadians ); }

bool MPB3DObject::pointIsToLeft( const MPBVector& point, MPBFLOAT thresholdRadians ) const 
{ return pointIsPointedTo( point, DIR_LEFT, thresholdRadians ); }

bool MPB3DObject::pointIsToRight( const MPBVector& point, MPBFLOAT thresholdRadians ) const 
{ return pointIsPointedTo( point, DIR_RIGHT, thresholdRadians ); }

bool MPB3DObject::pointIsAbove( const MPBVector& point, MPBFLOAT thresholdRadians ) const 
{ return pointIsPointedTo( point, DIR_UP, thresholdRadians ); }

bool MPB3DObject::pointIsBelow( const MPBVector& point, MPBFLOAT thresholdRadians ) const 
{ return pointIsPointedTo( point, DIR_DOWN, thresholdRadians ); }


void MPB3DObject::setStreamingSound( MPBStreamingSound* s )
{
	m_streamingSound = s;
	MPBSoundManager::addStreamingSound( s );
}

void MPB3DObject::killStreamingSound()
{
	if (m_streamingSound)
	{
		MPBSoundManager::removeStreamingSound( m_streamingSound );
		NULLIFY( m_streamingSound );
	}
}

void MPB3DObject::setSound( char* filename )
{
	m_sound = MPBSoundManager::createSoundSource( filename );

	if (m_sound)
	{
		alSourcef( m_sound, AL_GAIN, 1.0 );
		alSourcei( m_sound, AL_LOOPING, true );

		alSourcePlay( m_sound );
		MPB_ASSERT( alGetError() == AL_NO_ERROR );
	}
}

void MPB3DObject::stopSound()
{
	if (m_sound) MPBSoundManager::destroySoundSource( m_sound );
	m_sound = 0;
}








